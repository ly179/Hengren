#include "memory_condition_dao.h"
#include "sem_common.h"

int MemoryConditionDao::save(const MemoryConditionEntity& entity) {
    SqlConnectionHolder holder = getConnectionHolder();
    if (holder.openError()) {
        qCritical() << "sql connection open err on MemoryConditionDao::save: " << holder.getErrMsg();
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    sem::AfterGc gc([this]() { close(); });

    QString sql = "insert into sem_memory_condition (id, condition_detail)"
                  " values (:id , :detail) on duplicate key update condition_detail = :detail";
    SqlQueryHolder query = holder.exec(sql);
    query.bindValue(":id", entity.getId().value());
    query.bindValue(":detail", entity.getConditionDetail());
    if (!query.exec()) {
        qCritical() << "sql query err in MemoryConditionDao::save when save: " << query.lastError();
        return SQL_UPDATE_ERR_CODE;
    }
    return 0;
}

MemoryConditionEntity MemoryConditionDao::getById(quint64 id, int * errCode) {
    SqlConnectionHolder holder = getConnectionHolder();
    MemoryConditionEntity entity;
    if (holder.openError()) {
        qCritical() << "sql connection open err on MemoryConditionDao::getById: " << holder.getErrMsg();
        if (errCode) *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        return entity;
    }
    sem::AfterGc gc([this]() { close(); });
    QString sql = "select id, condition_detail, create_time, edit_time from sem_memory_condition where id = ?";
    SqlQueryHolder query = holder.exec(sql);
    query.addBindValue(id);
    if (!query.exec()) {
        qCritical() << "sql query err in MemoryConditionDao::getById when query: " << query.lastError();
        if (errCode) *errCode = SQL_QUERY_ERR_CODE;
        return entity;
    }
    if (errCode) *errCode = 0;
    if (query.next()) {
        entity.setId(query.value(0).toULongLong());
        entity.setConditionDetail(query.value(1).toString());
        entity.setCreateTime(query.value(2).toDateTime());
        entity.setEditTime(query.value(3).toDateTime());
    }
    return entity;
}

QList<MemoryConditionEntity> MemoryConditionDao::list(int * errCode) {
    SqlConnectionHolder holder = getConnectionHolder();
    QList<MemoryConditionEntity> list;
    if (holder.openError()) {
        qCritical() << "sql connection open err on MemoryConditionDao::list: " << holder.getErrMsg();
        if (errCode) *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        return list;
    }
    sem::AfterGc gc([this]() { close(); });
    QString sql = "select id, condition_detail, create_time, edit_time from sem_memory_condition order by id asc limit 100";
    SqlQueryHolder query = holder.exec(sql);
    if (!query.exec()) {
        qCritical() << "sql query err in MemoryConditionDao::list when query: " << query.lastError();
        if (errCode) *errCode = SQL_QUERY_ERR_CODE;
        return list;
    }
    if (errCode) *errCode = 0;
    if (query.numRowsAffected() > 0) {
        list.reserve(query.numRowsAffected());
    }
    MemoryConditionEntity entity;
    while (query.next()) {
        entity.setId(query.value(0).toULongLong());
        entity.setConditionDetail(query.value(1).toString());
        entity.setCreateTime(query.value(2).toDateTime());
        entity.setEditTime(query.value(3).toDateTime());
        list.append(entity);
    }
    return list;
}

int MemoryConditionDao::deleteById(quint64 id) {
    SqlConnectionHolder holder = getConnectionHolder();
    if (holder.openError()) {
        qCritical() << "sql connection open err on MemoryConditionDao::deleteById: " << holder.getErrMsg();
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    sem::AfterGc gc([this]() { close(); });
    QString sql = "delete from sem_memory_condition where id = ?";
    SqlQueryHolder query = holder.exec(sql);
    query.addBindValue(id);
    if (!query.exec()) {
        qCritical() << "sql delete err in MemoryConditionDao::deleteById when query: " << query.lastError();
        return SQL_QUERY_ERR_CODE;
    }
    return 0;
}
