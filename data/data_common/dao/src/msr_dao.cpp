#include "msr_dao.h"
#include "global_connection_factory.h"
#include "exception_code.h"
#include "sem_common.h"

void copySimpleColumns(SqlQueryHolder& queryHolder, MsrEntity& entity) {
    entity.setId(queryHolder.value(0).toULongLong());
    entity.setMsrName(queryHolder.value(1).toString());
    entity.setClassId(queryHolder.value(2).toULongLong());
    entity.setRecipeId(queryHolder.value(3).toULongLong());
    entity.setIdpId(queryHolder.value(4).toULongLong());
    entity.setIdwId(queryHolder.value(5).toULongLong());
    entity.setTopId(queryHolder.value(6).toULongLong());
    entity.setInvalid(queryHolder.value(7).toInt());
    entity.setVersion(queryHolder.value(8).toInt());
    entity.setCreateTime(queryHolder.value(9).toDateTime());
    entity.setEditTime(queryHolder.value(10).toDateTime());
}

void copyFullColumns(SqlQueryHolder& queryHolder, MsrEntity& entity) {
    entity.setId(queryHolder.value(0).toULongLong());
    entity.setMsrName(queryHolder.value(1).toString());
    entity.setMsrDetail(queryHolder.value(2).toString());
    entity.setClassId(queryHolder.value(3).toULongLong());
    entity.setRecipeId(queryHolder.value(4).toULongLong());
    entity.setIdpId(queryHolder.value(5).toULongLong());
    entity.setIdwId(queryHolder.value(6).toULongLong());
    entity.setTopId(queryHolder.value(7).toULongLong());
    entity.setInvalid(queryHolder.value(8).toInt());
    entity.setVersion(queryHolder.value(9).toInt());
    entity.setCreateTime(queryHolder.value(10).toDateTime());
    entity.setEditTime(queryHolder.value(11).toDateTime());
}

inline QString simpleColumns() {
    return "id, msr_name, class_id, recipe_id, idp_id, idw_id, top_id, invalid, version, create_time, edit_time";
}

inline QString allColumns() {
    return "id, msr_name, msr_detail, class_id, recipe_id, idp_id, idw_id, top_id, invalid, version, create_time, edit_time";
}

QSharedPointer<SqlConnectionFactory> MsrDao::getConnectionFactory() {
    return GlobalConnectionFactory::instance()->getFileSqlConnectionFactory();
}

qint32 MsrDao::checkAndSave(
    MsrEntity& entity) {
    SqlConnectionHolder holder = getConnectionHolder();
    if (holder.openError()) {
        qCritical() << "sql connection open err on MsrDao::checkAndSave: "
                    << holder.getErrMsg();
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    sem::AfterGc gc([this]() { close(); });
    //这个version应该不会升级到几千万上亿吧，所以应该不用考虑越界问题
    qint32 version = 0;
    quint64 topId = 0;
    if (entity.getId() > 0) {
        QString sql = "select top_id, class_id from sem_msr_info where id = ?";
        SqlQueryHolder query = holder.exec(sql);
        query.bindValue(0, entity.getId());
        if (!query.exec()) {
            qCritical() << "sql query err in MsrDao::checkAndSave when query top_id by id: "
                        << query.lastError();
            return SQL_QUERY_ERR_CODE;
        }
        if (!query.next()) {
            qCritical() << "sql query err in MsrDao::checkAndSave when query top_id by "
                           "id[data not found]: "
                        << query.lastError();
            return SQL_QUERY_DATA_NOT_EXISTS;
        }
        topId = query.value(0).toULongLong();
        if (topId <= 0) {
            topId = entity.getId();
        }
        if (entity.getClassId() == 0) {
            entity.setClassId(query.value(1).toULongLong());
        }
        query.clear();

        sql = "update sem_msr_info set invalid = 1 where id = ? and invalid = 0 and deleted = 0";
        query = holder.exec(sql);
        query.bindValue(0, entity.getId());
        if (!query.exec()) {
            qCritical()
            << "sql query err in MsrDao::checkAndSave when update invalid by id: "
            << query.lastError();
            return SQL_UPDATE_ERR_CODE;
        }
        query.clear();
        sql = "select max(version) from sem_msr_info where top_id = ? ";
        query = holder.exec(sql);
        query.bindValue(0, topId);
        if (!query.exec()) {
            qCritical() << "sql query err in MsrDao::checkAndSave when check max version: "
                        << query.lastError();
            return SQL_CONNECTION_OPEN_ERR_CODE;
        }
        if (query.next()) {
            version = query.value(0).toInt() + 1;
        }
        query.clear();
    }
    entity.setTopId(topId);
    entity.setVersion(version);
    quint64 id = entity.getId();
    int errCode = save(entity);
    if (errCode) {
        return errCode;
    }
    if (id == 0) {
        updateTopId(entity.getId(), entity.getId(), &errCode);
    }
    return errCode;
}

int MsrDao::save(MsrEntity& entity) {
    SqlConnectionHolder holder = getConnectionHolder();
    if (holder.openError()) {
        qCritical() << "sql connection open err on MsrDao::save: " << holder.getErrMsg();
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    sem::AfterGc gc([this]() { close(); });
    QString sql = "insert into sem_msr_info (msr_name, class_id, recipe_id, idp_id, idw_id, msr_detail, version, invalid, top_id) values (?, ?, ?, ?, ?, ?, ?, ?, ?)";
    SqlQueryHolder sqlQuery = holder.exec(sql);
    sqlQuery.addBindValue(entity.getMsrName());
    sqlQuery.addBindValue(entity.getClassId());
    sqlQuery.addBindValue(entity.getRecipeId());
    sqlQuery.addBindValue(entity.getIdpId());
    sqlQuery.addBindValue(entity.getIdwId());
    sqlQuery.addBindValue(entity.getMsrDetail());
    sqlQuery.addBindValue(entity.getVersion());
    sqlQuery.addBindValue(entity.getInvalid());
    sqlQuery.addBindValue(entity.getTopId());
    if (!sqlQuery.exec()) {
        qCritical() << "MsrDao::save save error: " << sqlQuery.lastError();
        return SQL_UPDATE_ERR_CODE;
    }
    entity.setId(sqlQuery.lastInsertId().toULongLong());
    return 0;
}

qint32 MsrDao::saveWithTransaction(MsrEntity& entity) {
    bool hasTran = hasTransaction();
    bool succ = beginTransaction(true);
    if (!succ) {
        qCritical() << "Open transaction fail when MsrDao::save";
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    sem::AfterGc gc([&succ, &hasTran, this](){
        if (!hasTran && !succ) {
            succ = rollback();
            if (!succ) qCritical() << "Error to rollback MSR data.";
        }
    });
    qint32 errCode = checkAndSave(entity);
    if (!hasTran && !errCode) {
        succ = commit();
        if (!succ) {
            errCode = SQL_COMMIT_ERROR_CODE;
        }
    }
    return errCode;
}

qint32 MsrDao::updateTopId(quint64 id, quint64 topId, int *errCode) {
    SqlConnectionHolder holder = getConnectionHolder();
    if (holder.openError()) {
        qCritical() << "sql connection open err on MsrDao::updateTopId: "
                    << holder.getErrMsg();
        if (errCode) {
            *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        }
        return 0;
    }
    sem::AfterGc gc([this]() { close(); });
    SqlQueryHolder query = holder.exec("update sem_msr_info set top_id = ? where id = ?");
    query.addBindValue(topId);
    query.addBindValue(id);
    if (!query.exec()) {
        qCritical() << "sql query err in MsrDao::updateTopId when update top_id: "
                    << query.lastError();
        if (errCode) {
            *errCode = SQL_UPDATE_ERR_CODE;
        }
        return 0;
    }
    if (errCode) {
        *errCode = 0;
    }
    return query.numRowsAffected();
}

QList<MsrEntity> MsrDao::searchSimple(const MsrSearchDTO& param, int* errCode) {
    QList<MsrEntity> list;
    SqlConnectionHolder holder = getConnectionHolder();
    if (holder.openError()) {
        qCritical() << "sql connection open err on MsrDao::searchSimple: " << holder.getErrMsg();
        if (errCode) *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        return list;
    }
    sem::AfterGc gc([this]() { close(); });
    QString sql = QString("select %1 from sem_msr_info where deleted = 0").arg(simpleColumns());
    if (param.getClassId() != nullptr) {
        sql.append(" and class_id = ?");
    }
    if (param.getTopId() != nullptr) {
        sql.append(" and top_id = ?");
    }
    if (param.getInvalid() != nullptr) {
        sql.append(" and invalid = ?");
    }
    if (param.getMsrName().length() > 0) {
        sql.append(" and msr_name like concat('%', ?, '%') ");
    }
    SqlQueryHolder queryHolder = holder.exec(sql);
    if (param.getClassId() != nullptr) {
        queryHolder.addBindValue(param.getClassId().value());
    }
    if (param.getTopId() != nullptr) {
        queryHolder.addBindValue(param.getTopId().value());
    }
    if (param.getInvalid() != nullptr) {
        queryHolder.addBindValue(param.getInvalid().value());
    }
    if (param.getMsrName().length() > 0) {
        queryHolder.addBindValue(param.getMsrName());
    }
    if (!queryHolder.exec()) {
        qCritical() << "MsrDao::save query error: " << queryHolder.lastError();
        if (errCode) *errCode = SQL_QUERY_ERR_CODE;
        return list;
    }
    if (errCode) *errCode = 0;
    qint32 size = queryHolder.size();
    if (size>= 0) {
        if (size == 0) {
            return list;
        }
        list.reserve(size);
    }
    MsrEntity entity;
    while (queryHolder.next()) {
        copySimpleColumns(queryHolder, entity);
        list.append(entity);
    }
    return list;
}
MsrEntity MsrDao::getById(quint64 id, int* errCode) {
    MsrEntity entity;
    SqlConnectionHolder holder = getConnectionHolder();
    if (holder.openError()) {
        qCritical() << "sql connection open err on MsrDao::getById: " << holder.getErrMsg();
        if (errCode) *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        return entity;
    }
    sem::AfterGc gc([this]() { close(); });
    QString sql = QString("select %1 from sem_msr_info where deleted = 0 and id = ?").arg(allColumns());
    SqlQueryHolder queryHolder = holder.exec(sql);
    queryHolder.addBindValue(id);
    if (!queryHolder.exec()) {
        qCritical() << "MsrDao::getById error: " << queryHolder.lastError();
        if (errCode) *errCode = SQL_QUERY_ERR_CODE;
        return entity;
    }
    if (!queryHolder.next()) {
        return entity;
    }
    if (errCode) *errCode = 0;
    copyFullColumns(queryHolder, entity);
    return entity;
}

MsrEntity MsrDao::getByTopId(quint64 topId, int* errCode) {
    MsrEntity entity;
    SqlConnectionHolder holder = getConnectionHolder();
    if (holder.openError()) {
        qCritical() << "sql connection open err on MsrDao::getByTopId: " << holder.getErrMsg();
        if (errCode) *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        return entity;
    }
    sem::AfterGc gc([this]() { close(); });
    QString sql = QString("select %1 from sem_msr_info where deleted = 0 and invalid = 0 and top_id = ? order by id desc limit 1").arg(allColumns());
    SqlQueryHolder queryHolder = holder.exec(sql);
    queryHolder.addBindValue(topId);
    if (!queryHolder.exec()) {
        qCritical() << "MsrDao::getByTopId error: " << queryHolder.lastError();
        if (errCode) *errCode = SQL_QUERY_ERR_CODE;
        return entity;
    }
    if (errCode) *errCode = 0;
    if (!queryHolder.next()) {
        return entity;
    }
    copyFullColumns(queryHolder, entity);
    return entity;
}

quint64 MsrDao::getIdByName(quint64 classId, const QString& name, int* errCode) {
    SqlConnectionHolder holder = getConnectionHolder();
    if (holder.openError()) {
        qCritical() << "sql connection open err on MsrDao::getIdByName: " << holder.getErrMsg();
        if (errCode) *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        return 0;
    }
    sem::AfterGc gc([this]() { close(); });
    QString sql = QString("select id from sem_msr_info where deleted = 0 and invalid = 0 and class_id = ? and msr_name = ? limit 1");
    SqlQueryHolder queryHolder = holder.exec(sql);
    queryHolder.addBindValue(classId);
    queryHolder.addBindValue(name);
    if (!queryHolder.exec()) {
        qCritical() << "MsrDao::getIdByName error: " << queryHolder.lastError();
        if (errCode) *errCode = SQL_QUERY_ERR_CODE;
        return 0;
    }
    if (errCode) *errCode = 0;
    if (!queryHolder.next()) {
        return 0;
    }
    return queryHolder.value(0).toULongLong();
}

int MsrDao::updateNameOnly(quint64 id, const QString& name) {
    SqlConnectionHolder holder = getConnectionHolder();
    if (holder.openError()) {
        qCritical() << "sql connection open err on MsrDao::updateNameOnly: " << holder.getErrMsg();
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    sem::AfterGc gc([this]() { close(); });
    QString sql = "update sem_msr_info set msr_name = ? where id = ? and deleted = 0";
    SqlQueryHolder queryHolder = holder.exec(sql);
    queryHolder.addBindValue(name);
    queryHolder.addBindValue(id);
    if (!queryHolder.exec()) {
        return SQL_UPDATE_ERR_CODE;
    }
    return 0;
}

int MsrDao::updateNameById(quint64 id, const QString& name) {
    SqlConnectionHolder holder = getConnectionHolder();
    if (holder.openError()) {
        qCritical() << "sql connection open err on MsrDao::updateNameById: " << holder.getErrMsg();
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    sem::AfterGc gc([this]() { close(); });
    QString sql = "update sem_msr_info set msr_name = ? where id = ?";
    SqlQueryHolder queryHolder = holder.exec(sql);
    queryHolder.addBindValue(name);
    queryHolder.addBindValue(id);
    if (!queryHolder.exec()) {
        qCritical() << "MsrDao::updateNameById error: " << queryHolder.lastError();
        return SQL_UPDATE_ERR_CODE;
    }
    return 0;
}

//逻辑删除
int MsrDao::deleteDataById(quint64 id) {
    SqlConnectionHolder holder = getConnectionHolder();
    if (holder.openError()) {
        qCritical() << "sql connection open err on MsrDao::deleteDataById: " << holder.getErrMsg();
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    sem::AfterGc gc([this]() { close(); });
    QString sql = "update sem_msr_info set deleted = 1 where id = ?";
    SqlQueryHolder queryHolder = holder.exec(sql);
    queryHolder.addBindValue(id);
    if (!queryHolder.exec()) {
        qCritical() << "MsrDao::deleteDataById error: " << queryHolder.lastError();
        return SQL_UPDATE_ERR_CODE;
    }
    return 0;
}
//物理删除
int MsrDao::realDeleteDataById(quint64 id) {
    SqlConnectionHolder holder = getConnectionHolder();
    if (holder.openError()) {
        qCritical() << "sql connection open err on MsrDao::realDeleteDataById: " << holder.getErrMsg();
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    sem::AfterGc gc([this]() { close(); });
    QString sql = "delete from sem_msr_info where id = ?";
    SqlQueryHolder queryHolder = holder.exec(sql);
    queryHolder.addBindValue(id);
    if (!queryHolder.exec()) {
        qCritical() << "MsrDao::realDeleteDataById error: " << queryHolder.lastError();
        return SQL_UPDATE_ERR_CODE;
    }
    return 0;
}
