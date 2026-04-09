#include "log_file_reader.h"
#include <QDebug>
#include <QFile>
#include "sem_common.h"

LogFileReader::LogFileReader(const QString& path) : path(path) {}

void LogFileReader::setLineCallback(const LogLineCallback& callback) {
    this->callback = callback;
}

qint64 LogFileReader::beginRead(qint64 skip) {
    if (!callback) {
        return -1;
    }
    QFile file(path);
    if (!file.exists()) {
        return -2;
    }
    if (!file.open(QIODevice::Text | QIODevice::ReadOnly)) {
        return -1;
    }
    sem::AfterGc gc([&file](){
        file.close();
    });
    if (skip > 0) {
        if (!file.seek(skip)) {
            qCritical() << "对于文件[" << path << "]的seek操作失败";
            return -1;
        }
    }
    QString line;
    qint64 pos1, pos2;
    bool hasMoreData = false;
    while (!file.atEnd()) {
        pos1 = file.pos();
        line = file.readLine();
        pos2 = file.pos();
        if (line.isEmpty()) continue;
        if (!hasMoreData) hasMoreData = true;
        if (!callback(line, pos1, pos2)) {
            break;
        }
    }
    if (!hasMoreData) {
        qInfo() << "文件没有需要处理的行：" << path;
    }
    return file.pos();
}
