//
// Created by Shisan on 2026/1/14.
//
#include "stage_memory_dao.h"
#include  "stage_memory_entity.h"
#include  "exception_code.h"
#include "sem_common.h"
#include  <QList>

int StageMemoryDao::save(StageMemoryEntity& entity) {
    SqlConnectionHolder holder = getConnectionHolder();
    if (holder.openError()) {
        qCritical() << "sql connection open err on StageMemoryDao::save: " << holder.getErrMsg();
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    sem::AfterGc gc([this]() { close(); });
    QString sql;
    if (entity.getId() > 0) {
        sql = "update sem_stage_memory set mem_name = ?, mem_coordinate = ? where id = ?";
    } else {
        sql = "insert into sem_stage_memory(mem_name, mem_coordinate) values (?, ?)";
    }
    SqlQueryHolder query = holder.exec(sql);
    query.addBindValue(entity.getMemName());
    query.addBindValue(entity.getMemCoordinate());
    if (entity.getId() > 0) {
        query.addBindValue(entity.getId());
    }
    if (!query.exec()) {
        qCritical() << "sql update error on StageMemoryDao::save: " << holder.getErrMsg();
        return SQL_UPDATE_ERR_CODE;
    }
    return 0;
}

QList<StageMemoryEntity> StageMemoryDao::allAvailableMems(int* errCode) {
    QList<StageMemoryEntity> list;
    SqlConnectionHolder holder = getConnectionHolder();
    if (holder.openError()) {
        qCritical() << "sql connection open err on StageMemoryDao::save: " << holder.getErrMsg();
        if (errCode) {
            *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        }
        return list;
    }
    sem::AfterGc gc([this]() { close(); });
    QString sql = "select id, mem_name, mem_coordinate, create_time, edit_time, deleted from sem_stage_memory where deleted = 0 order by id asc";
    SqlQueryHolder query = holder.exec(sql);
    if (!query.exec()) {
        if (errCode) {
            *errCode = SQL_QUERY_ERR_CODE;
        }
        qCritical() << "sql query error on StageMemoryDao::allAvailableMems: " << holder.getErrMsg();
        return list;
    }
    if (errCode) *errCode = 0;
    if (query.size() > 0) {
        list.reserve(query.size());
    }
    StageMemoryEntity entity;
    while (query.next()) {
        entity.setId(query.value(0).toULongLong());
        entity.setMemName(query.value(1).toString());
        entity.setMemCoordinate(query.value(2).toString());
        entity.setCreateTime(query.value(3).toDateTime());
        entity.setEditTime(query.value(4).toDateTime());
        entity.setDeleted(char(query.value(5).toInt()));
        list.append(entity);
    }
    return list;
}

int StageMemoryDao::updateToDelete(quint64 id) {
    SqlConnectionHolder holder = getConnectionHolder();
    if (holder.openError()) {
        qCritical() << "sql connection open err on StageMemoryDao::save: " << holder.getErrMsg();
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    sem::AfterGc gc([this]() { close(); });
    QString sql = "update sem_stage_memory set deleted = 1 where id = ?";
    SqlQueryHolder query = holder.exec(sql);
    query.addBindValue(id);
    if (!query.exec()) {
        return SQL_UPDATE_ERR_CODE;
    }
    return 0;
}