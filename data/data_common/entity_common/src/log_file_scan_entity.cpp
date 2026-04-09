#include "log_file_scan_entity.h"

quint64 LogFileScanEntity::getId() const {
    return id;
}

void LogFileScanEntity::setId(quint64 newId) {
    id = newId;
}

qint64 LogFileScanEntity::getScanedLength() const {
    return scanedLength;
}

void LogFileScanEntity::setScanedLength(qint64 newScanedLength) {
    scanedLength = newScanedLength;
}

QDateTime LogFileScanEntity::getCreateTime() const {
    return createTime;
}

void LogFileScanEntity::setCreateTime(const QDateTime &newCreateTime) {
    createTime = newCreateTime;
}

QDateTime LogFileScanEntity::getEditTime() const {
    return editTime;
}

void LogFileScanEntity::setEditTime(const QDateTime &newEditTime) {
    editTime = newEditTime;
}

QString LogFileScanEntity::getRecordTable() const {
    return recordTable;
}

void LogFileScanEntity::setRecordTable(const QString &newRecordTable) {
    recordTable = newRecordTable;
}

quint64 LogFileScanEntity::getLastRecordId() const {
    return lastRecordId;
}

void LogFileScanEntity::setLastRecordId(quint64 newLastRecordId) {
    lastRecordId = newLastRecordId;
}

QString LogFileScanEntity::getFilePath() const {
    return filePath;
}

void LogFileScanEntity::setFilePath(const QString &newFilePath) {
    filePath = newFilePath;
}

LogFileScanEntity::LogFileScanEntity() {}
