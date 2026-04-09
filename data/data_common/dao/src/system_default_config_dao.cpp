#include "system_default_config_dao.h"
#include "exception_code.h"
#include "sem_common.h"

qint32 SystemDefaultConfigDao::save(
    const SystemDefaultConfigEntity& entity) {
    SqlConnectionHolder holder =  getConnectionHolder();
    if (holder.openError()) {
        qCritical() << "sql connection open err on SystemDefaultConfigDao::save: "
                    << holder.getErrMsg();
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    sem::AfterGc gc([this]() { close(); });
    SqlQueryHolder query = holder.exec("update sem_system_default_config set deleted = 1 where s_key = ? and deleted = 0");
    query.bindValue(0, entity.getSKey());
    if (!query.exec()) {
        qCritical() << "update old version err on SystemDefaultConfigDao::save: "
                    << query.lastError();
        return SQL_UPDATE_ERR_CODE;
    }
    query = holder.exec("select max(version) from sem_system_default_config where s_key = ?");
    query.bindValue(0, entity.getSKey());
    if (!query.exec()) {
        qCritical() << "get old version err on SystemDefaultConfigDao::save: " << query.lastError();
        return SQL_QUERY_ERR_CODE;
    }
    int version = 0;
    if (query.next()) {
        version = query.value(0).toInt() + 1;
    }
    query = holder.exec("insert into sem_system_default_config (s_key, s_value, version) values (?, ?, ?)");
    query.bindValue(0, entity.getSKey());
    query.bindValue(1, entity.getSValue());
    query.bindValue(2, version);
    if (!query.exec()) {
        qCritical() << "save new data err on SystemDefaultConfigDao::save: " << query.lastError();
        return SQL_UPDATE_ERR_CODE;
    }
    return 0;
}

qint32 SystemDefaultConfigDao::saveWithTransaction(
    const SystemDefaultConfigEntity& entity) {
    bool hasTran = hasTransaction();
    bool succ = beginTransaction(true);
    if (!succ) {
        qCritical() << "Open transaction fail when SystemDefaultConfigDao::save";
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    qint32 errCode = save(entity);
    if (!hasTran) {
        if (errCode) {
            rollback();
        } else {
            commit();
        }
    }
    return errCode;
}

SystemDefaultConfigEntity SystemDefaultConfigDao::getByKey(
    const QString& key, int* errCode) {
    SqlConnectionHolder holder =  getConnectionHolder();
    SystemDefaultConfigEntity entity;
    if (holder.openError()) {
        qCritical() << "sql connection open err on SystemDefaultConfigDao::getByKey: "
                    << holder.getErrMsg();
        if (errCode) {
            *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        }
        return entity;
    }
    sem::AfterGc gc([this](){
        close();
    });
    SqlQueryHolder query = holder.exec("select id, s_key, s_value, version, deleted, create_time, edit_time, version from sem_system_default_config where s_key = ? and deleted = 0 order by id desc limit 1");
    query.bindValue(0, key);
    if (!query.exec()) {
        qCritical() << "sql connection open err on SystemDefaultConfigDao::getByKey: "
                    << holder.getErrMsg();
        if (errCode) {
            *errCode = SQL_QUERY_ERR_CODE;
        }
        return entity;
    }
    if (errCode) {
        *errCode = 0;
    }
    if (!query.next()) {
        return entity;
    }
    entity.setId(query.value(0).toULongLong());
    entity.setSKey(query.value(1).toString());
    entity.setSValue(query.value(2).toString());
    entity.setDeleted(char(query.value(3).toInt()));
    entity.setCreateTime(query.value(4).toDateTime());
    entity.setEditTime(query.value(5).toDateTime());
    entity.setVersion(query.value(6).toInt());
    return entity;
}
