#include "log_scanner.h"
#include <QDir>
void scanWork(const QDir& dir, const LogScannerFileCallback& work) {
    QFileInfoList fileList = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs);
    if (fileList.isEmpty()) {
        return;
    }
    QString path;
    for (QFileInfo& fileInfo : fileList) {
        //优先处理.tmp文件
        if (fileInfo.isFile()) {
            path = fileInfo.absoluteFilePath();
            if (!path.endsWith(".tmp", Qt::CaseInsensitive)) {
                path = path.append(".tmp");
                if (QFile(path).exists()) {
                    work(path);
                    continue;
                }
                work(fileInfo.absoluteFilePath());
                continue;
            }
            work(path);
            continue;
        }
        scanWork(QDir(fileInfo.absoluteFilePath()), work);
    }
}

LogScanner::LogScanner(const QString& logDir, const QString& fileNameReg, qint64 waitTime) : logDir(logDir), waitTime(waitTime) {
    this->fileNameReg.setPattern(fileNameReg);
}

void LogScanner::setCallback(const LogScannerFileCallback& callback) {
    this->callback = callback;
}

void LogScanner::beginScan() {
    if (!callback) {
        return;
    }
    QDir dir(logDir);
    scanWork(dir, callback);
}
