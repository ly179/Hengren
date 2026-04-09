#include "log_record_dao.h"
#include "global_connection_factory.h"
#include "exception_code.h"
#include "sem_common.h"

QSharedPointer<SqlConnectionFactory> LogRecordDao::getConnectionFactory() {
    return GlobalConnectionFactory::instance()->getLogSqlConnectionFactory();
}

LogRecordSearchResult LogRecordDao::searchLogs(const LogRecordSearchDTO& param, int* errCode) {
    LogRecordSearchResult result;
    SqlConnectionHolder holder = getConnectionHolder();
    if (holder.openError()) {
        qCritical() << "sql connection open err on LogRecordDao::searchLogs: " << holder.getErrMsg();
        if (errCode) {
            *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        }
        return result;
    }
    sem::AfterGc gc([this]() { close(); });
    QString tableName;
    if (param.getStartTime().isValid() || param.getEndTime().isValid()) {
        if (param.getStartTime().isValid()) {
            tableName = param.getStartTime().toString("yyyyMM");
        } else {
            tableName = param.getEndTime().toString("yyyyMM");
        }
    } else {
        tableName = QDate::currentDate().toString("yyyyMM");
    }
    QString sql = "select id, log_level, file_name, file_line, log_content, log_create_time, catalog from hengren_log_record_%1";
    sql = sql.arg(tableName);
    QString condition;
    if (param.getLevel().length() > 0) {
        if (condition.length() > 0) {
            condition.append(" and ");
        }
        condition.append(" log_level = ?");
    }
    if (param.getCatalog().length() > 0) {
        if (condition.length() > 0) {
            condition.append(" and ");
        }
        condition.append(" catalog like concat('%', ?, '%')");
    }
    if (param.getContent().length() > 0) {
        if (condition.length() > 0) {
            condition.append(" and ");
        }
        condition.append(" log_content like concat('%', ?, '%')");
    }
    if (param.getFile().length() > 0) {
        if (condition.length() > 0) {
            condition.append(" and ");
        }
        condition.append(" file_name like concat('%', ?, '%')");
    }
    if (param.getStartTime().isValid()) {
        if (condition.length() > 0) {
            condition.append(" and ");
        }
        condition.append(" log_create_time >= ?");
    }
    if (param.getEndTime().isValid()) {
        if (condition.length() > 0) {
            condition.append(" and ");
        }
        condition.append(" log_create_time < ?");
    }
    if (param.getPageId() > 0 && param.getPageTime().isValid()) {
        if (condition.length() > 0) {
            condition.append(" and ");
        }
        condition.append(" (log_create_time < ? or (log_create_time = ? and id < ?)) ");
    }
    if (condition.length() > 0) {
        sql.append(" where ").append(condition);
    }
    int limit = param.getLimit();
    if (limit < 1) {
        limit = 50;
    } else if (limit > 200) {
        limit = 200;
    }
    sql.append(" order by log_create_time desc limit ?");
    SqlQueryHolder query = holder.exec(sql);
    if (param.getLevel().length() > 0) {
        query.addBindValue(param.getLevel());
    }
    if (param.getCatalog().length() > 0) {
        query.addBindValue(param.getCatalog());
    }
    if (param.getContent().length() > 0) {
        query.addBindValue(param.getContent());
    }
    if (param.getFile().length() > 0) {
        query.addBindValue(param.getFile());
    }
    if (param.getStartTime().isValid()) {
        query.addBindValue(param.getStartTime());
    }
    if (param.getEndTime().isValid()) {
        query.addBindValue(param.getEndTime());
    }
    if (param.getPageId() > 0 && param.getPageTime().isValid()) {
        query.addBindValue(param.getPageTime());
        query.addBindValue(param.getPageTime());
        query.addBindValue(param.getPageId());
    }
    query.addBindValue(limit);
    if (!query.exec()) {
        qCritical() << "query error at LogRecordDao::searchLogs: " << query.lastError();
        if (errCode) {
            *errCode = SQL_QUERY_ERR_CODE;
        }
        return result;
    }
    if (query.numRowsAffected() > 0) {
        result.list.reserve(query.numRowsAffected());
    }
    if (errCode) {
        *errCode = 0;
    }
    LogRecordEntity entity;
    while (query.next()) {
        entity.setId(query.value(0).toULongLong());
        entity.setLogLevel(query.value(1).toString());
        entity.setFileName(query.value(2).toString());
        entity.setFileLine(query.value(3).toInt());
        entity.setLogContent(query.value(4).toString());
        entity.setLogCreateTime(query.value(5).toDateTime());
        entity.setCatalog(query.value(6).toString());
        result.list.append(entity);
    }
    if (result.list.size() >= limit) {
        LogRecordEntity& last = result.list.last();
        result.startId = last.getId();
        result.startTime = last.getLogCreateTime();
    } else {
        result.startId = 0;
        result.startTime = QDateTime();
    }
    return result;
}
