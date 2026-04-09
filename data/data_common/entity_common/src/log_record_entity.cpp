#include "log_record_entity.h"

QDateTime LogRecordEntity::getLogCreateTime() const {
    return logCreateTime;
}

void LogRecordEntity::setLogCreateTime(const QDateTime &newLogCreateTime) {
    logCreateTime = newLogCreateTime;
}

QDateTime LogRecordEntity::getCreateTime() const {
    return createTime;
}

void LogRecordEntity::setCreateTime(const QDateTime &newCreateTime) {
    createTime = newCreateTime;
}

QString LogRecordEntity::getLogLevel() const {
    return logLevel;
}

void LogRecordEntity::setLogLevel(const QString &newLogLevel) {
    logLevel = newLogLevel;
}

QString LogRecordEntity::getLogContent() const {
    return logContent;
}

void LogRecordEntity::setLogContent(const QString &newLogContent) {
    logContent = newLogContent;
}

QString LogRecordEntity::getFileName() const {
    return fileName;
}

void LogRecordEntity::setFileName(const QString &newFileName) {
    fileName = newFileName;
}

quint64 LogRecordEntity::getId() const {
    return id;
}

void LogRecordEntity::setId(quint64 newId) {
    id = newId;
}

qint32 LogRecordEntity::getFileLine() const {
    return fileLine;
}

void LogRecordEntity::setFileLine(
    qint32 newFileLine) {
    fileLine = newFileLine;
}

const QString& LogRecordEntity::getCatalog() const {
    return catalog;
}

void LogRecordEntity::setCatalog(const QString &newCatalog) {
    catalog = newCatalog;
}

LogRecordEntity::LogRecordEntity() {}
