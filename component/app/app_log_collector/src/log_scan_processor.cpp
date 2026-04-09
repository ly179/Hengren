#include "log_scan_processor.h"
#include <QDate>
#include <QFile>
#include "log_content_format.h"
#include "log_file_reader.h"
#include "log_scanner.h"
#include "log_scanner_file.h"
#include "property_util.h"
#include "log_record_entity.h"
#include "log_file_scan_entity.h"
#include "sem_common.h"
#include "exception_code.h"

QRegularExpression LogScanProcessor::filePathReg = QRegularExpression(
    "^.+_([a-zA-Z0-9]+)_(\\d{8})_\\d+\\.log(\\..+)?$");

LogScanProcessor::LogScanProcessor() {
    PropertyUtil util;
    util.readFromFile(getLogScanConfig());
    const PropertyNodeData& node = util.getNode("log.scanner");
    if (node.type != PROPERTY_HASH) {
        return;
    }
    QString str = util.simpleGetConfig("wait-time", node);
    if (!str.isEmpty()) {
        qint64 time = str.toLongLong();
        if (time > 1000) {
            waitTime = time;
        }
    }
    str = util.simpleGetConfig("file-name", node);
    if (!str.isEmpty()) {
        fileNameReg = str;
    }
    logDir = util.simpleGetConfig("dir", node);
    saver = new LogSaver;
}

bool LogScanProcessor::doSave(QList<LogRecordEntity>& list,
                              LogFileScanEntity& posEntity,
                              const QDate& date) {
    saver->beginTransaction();
    int errCode = 0;
    sem::AfterGc gc([this, &errCode, &list]() mutable {
        list.clear();
        if (errCode) {
            saver->rollback();
        } else {
            saver->commit();
        }
    });
    LogRecordEntity entity;
    if (posEntity.getLastRecordId() > 0) {
        entity = list.takeFirst();
        entity.setId(posEntity.getLastRecordId());
        errCode = saver->update(entity, date);
        if (errCode) {
            qCritical() << "Log Scanner save database error at update lastRecord, error code: "
                        << errCode;
            return false;
        }
    }
    if (list.isEmpty()) {
        return true;
    }
    entity = list.takeLast();
    if (list.size() > 0) {
        errCode = saver->save(list, date);
        if (errCode == SQL_TABLE_NOT_EXISTS) {
            saver->generalTable(date);
            errCode = saver->save(list, date);
        }
        if (errCode) {
            qCritical() << "Log Scanner save database error at save list, error code: " << errCode;
            return false;
        }
    }
    errCode = saver->save(entity);
    if (errCode == SQL_TABLE_NOT_EXISTS) {
        saver->generalTable(date);
        errCode = saver->save(list, date);
    }
    if (errCode) {
        qCritical() << "Log Scanner save database error at save last, error code: " << errCode;
        return false;
    }
    posEntity.setLastRecordId(entity.getId());
    errCode = saver->saveFilePos(posEntity, date);
    return errCode == 0;
}

int LogScanProcessor::doRead(const QString& path, LogFileScanEntity& posRecorder) {
    QRegularExpressionMatch match = filePathReg.match(path);
    if (!match.hasMatch()) {
        return 1;
    }
    qInfo() << "正在扫描文件：" << path;
    QDate date = QDate::fromString(match.captured(2), "yyyyMMdd");
    QString level = match.captured(1);
    QList<LogRecordEntity> list;
    list.reserve(50);
    LogFileReader reader(path);
    bool success = true;
    reader.setLineCallback(
        [this, &level, &date, &list, &posRecorder, &path, &success](const QString& line,
                                                                    qint64 lPos,
                                                                    qint64 rPos) mutable {
            success = false;
            //使用lPos（即本行的起始偏移量）的原因：
            //防止最后一行的日志不是完整日志，因此特地让重试或再次扫描时能够重新扫描该行，到数据库后会做update操作而非insert。
            posRecorder.setScanedLength(lPos);
            bool ok;
            LogContentStruct data = contentParser.parseLogLine(line, &ok);
            if (!ok)
                qCritical() << "log scan format parse err at file: " << path << ", detail: ["
                            << lPos << "-" << rPos << "]" << line;
            LogRecordEntity entity;
            entity.setLogContent(data.content);
            entity.setFileName(data.classFile);
            entity.setLogLevel(level);
            if (data.time.isValid())
                entity.setLogCreateTime(QDateTime(date, data.time));
            else
                entity.setLogCreateTime(date.startOfDay());
            entity.setFileLine(data.line);
            entity.setCatalog(data.cat);
            list.append(entity);
            if (list.size() >= 50) {
                bool succ = doSave(list, posRecorder, date);
                posRecorder.setLastRecordId(0);
                success = succ;
                return succ;
            }
            success = true;
            return true;
        });
    int r = reader.beginRead(posRecorder.getScanedLength());
    if (r == -2) {
        return 2;
    }
    // 让出数据库资源，防止日志沾满数据库资源
    QThread::msleep(100);
    if (list.size() > 0) {
        if (!doSave(list, posRecorder, date)) {
            return false;
        }
    }
    return success ? 0 : 1;
}

void LogScanProcessor::tailHandle() {
    qint32 errCode;
    QList<LogFileScanEntity> list = saver->allFilePos(&errCode);
    if (list.size() > 0) {
        for (LogFileScanEntity& entity : list) {
            if (!QFile(entity.getFilePath()).exists()) {
                saver->deleteFilePosRecord(entity.getId());
            }
        }
    }
}


//扫描逻辑：
//1、优先检查.tmp结尾的文件，数据库扫描用于记录扫描pos的记录，path不包含tmp。（普通.log读取完毕后进行重命名为tmp，但是数据库的不做变更）
//2、读取数据库记录的path，先加上tmp处理，tmp处理完毕后再处理普通的.log文件
//3、处理tmp文件时，扫描完毕tmp后先删除数据库记录，删除成功后再删除tmp文件
//4、数据库不存在的tmp文件可直接删除
void LogScanProcessor::start() {
    LogScanner scanner(logDir, fileNameReg, waitTime);
    bool hasNewData = true;
    do {
        qInfo() << "开始新一轮日志文件扫描...";
        hasNewData = false;
        scanner.setCallback([this, &hasNewData](const QString& filePath) mutable {
            sem::AfterGc gc([this]() { saver->commit(); });
            hasNewData = true;
            int posErrCode;
            LogFileScanEntity posRecorder = saver->getFilePos(filePath, &posErrCode);
            if (posErrCode) {
                if (posErrCode == SQL_QUERY_DATA_NOT_EXISTS) {
                    posRecorder.setFilePath(filePath);
                } else {
                    qCritical() << "读取文件扫描记录失败: " << posErrCode;
                    return;
                }
            }
            int r = doRead(filePath, posRecorder);
            if (r) {
                //文件不存在直接删除记录即可。
                if (r == 2) {
                    r = saver->deleteFilePosRecord(posRecorder.getId());
                    if (r) {
                        qCritical() << "delete file record failed: " << r << ": " << filePath;
                    }
                }
                return;
            }
            if (filePath.endsWith(".tmp", Qt::CaseInsensitive)) {
                int errCode = saver->deleteFilePosRecord(posRecorder.getId());
                if (errCode) {
                    qCritical() << "delete file record failed: " << errCode << ": " << filePath;
                } else {
                    QFile(filePath).remove();
                }
            } else {
                QFile file(filePath);
                file.rename(file.fileName().append(".tmp"));
            }
        });
        scanner.beginScan();
        tailHandle();
        //休眠10秒再次执行
        QThread::sleep(10);
    } while (hasNewData);
    qInfo() << "本轮日志文件扫描结束";
}
