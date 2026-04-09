#ifndef LOG_FILE_SCAN_ENTITY_H
#define LOG_FILE_SCAN_ENTITY_H
#include <QDateTime>
#include <QString>
#include "entity_common_global.h"

//! 日志文件扫描记录表Entity
class ENTITY_COMMON_EXPORT LogFileScanEntity {
private:
    quint64 id = 0;
    //! 已扫描的文件长度
    qint64 scanedLength = 0;
    QDateTime createTime;
    QDateTime editTime;
    //! 文件路径，最好使用绝对路径
    QString filePath;
    //! 对应的日志表名
    QString recordTable;
    //! 对应的日志表中该文件上次扫描完毕时的最后一条记录的ID
    quint64 lastRecordId = 0;

public:
    LogFileScanEntity();
    quint64 getId() const;
    void setId(quint64 newId);
    qint64 getScanedLength() const;
    void setScanedLength(qint64 newScanedLength);
    QDateTime getCreateTime() const;
    void setCreateTime(const QDateTime &newCreateTime);
    QDateTime getEditTime() const;
    void setEditTime(const QDateTime &newEditTime);
    QString getRecordTable() const;
    void setRecordTable(const QString &newRecordTable);
    quint64 getLastRecordId() const;
    void setLastRecordId(quint64 newLastRecordId);
    QString getFilePath() const;
    void setFilePath(const QString &newFilePath);
};

#endif // LOG_FILE_SCAN_ENTITY_H
