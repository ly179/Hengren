#ifndef LOG_RECORD_ENTITY_H
#define LOG_RECORD_ENTITY_H

#include <QDateTime>
#include <QString>
#include "entity_common_global.h"
#include "model_setget.h"

class LogRecordSearchDTO {
    CDSEMPSTRING(Level, level)
    CDSEMPSTRING(Catalog, catalog)
    CDSEMPSTRING(File, file)
    CDSEMPSTRING(Content, content)
    CDSEMPDATETIME(StartTime, startTime)
    CDSEMPDATETIME(EndTime, endTime)
    CDSEMPDATETIME(PageTime, pageTime)
    CDSEMPUINT64(PageId, pageId)
    CDSEMPINT32(Limit, limit, 50)
};

class LogRecordEntity;

struct LogRecordSearchResult {
    QList<LogRecordEntity> list;
    QDateTime startTime;
    quint64 startId = 0;
};

//! 日志记录表Entity
class ENTITY_COMMON_EXPORT LogRecordEntity {
private:
    //! 输出日志时间
    QDateTime logCreateTime;
    //! 保存到数据库时间
    QDateTime createTime;
    //日志分类
    QString catalog;
    //! 日志等级
    QString logLevel;
    //! 日志内容
    QString logContent;
    //! 输入日志的源码文件
    QString fileName;
    qint32 fileLine = -1;
    quint64 id;

public:
    LogRecordEntity();
    QDateTime getLogCreateTime() const;
    void setLogCreateTime(const QDateTime &newLogCreateTime);
    QDateTime getCreateTime() const;
    void setCreateTime(const QDateTime &newCreateTime);
    QString getLogLevel() const;
    void setLogLevel(const QString &newLogLevel);
    QString getLogContent() const;
    void setLogContent(const QString &newLogContent);
    QString getFileName() const;
    void setFileName(const QString &newFileName);
    quint64 getId() const;
    void setId(quint64 newId);

    qint32 getFileLine() const;
    void setFileLine(qint32 newFileLine);

    const QString& getCatalog() const;
    void setCatalog(const QString &newCatalog);
};

#endif // LOG_RECORD_ENTITY_H
