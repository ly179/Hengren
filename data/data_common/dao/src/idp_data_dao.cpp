#include "idp_data_dao.h"
#include "exception_code.h"
#include "sem_common.h"

qint32 IdpDataDao::checkAndSave(
    IdpDataEntity &entity) {
    SqlConnectionHolder holder = getConnectionHolder();
    if (holder.openError()) {
        qCritical() << "sql connection open err on IdpDataDao::checkAndSave: " << holder.getErrMsg();
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    sem::AfterGc gc([this]() { close(); });

    //这个version应该不会升级到几千万上亿吧，所以应该不用考虑越界问题
    qint32 version = 0;
    quint64 topId = 0;
    if (entity.getId() > 0) {
        QString sql = "select top_id, class_id from sem_idp_data_info where id = ?";
        SqlQueryHolder query = holder.exec(sql);
        query.bindValue(0, entity.getId());
        if (!query.exec()) {
            qCritical() << "sql query err in IdpDataDao::checkAndSave when query top_id by id: "
                        << query.lastError();
            return SQL_QUERY_ERR_CODE;
        }
        if (!query.next()) {
            qCritical() << "sql query err in IdpDataDao::checkAndSave when query top_id by "
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

        sql = "update sem_idp_data_info set invalid = 1 where id = ? and invalid = 0 and deleted "
              "= 0";
        query = holder.exec(sql);
        query.bindValue(0, entity.getId());
        if (!query.exec()) {
            qCritical() << "sql query err in IdpDataDao::checkAndSave when update invalid by id: "
                        << query.lastError();
            return SQL_UPDATE_ERR_CODE;
        }
        query.clear();
        sql = "select max(version) from sem_idp_data_info where top_id = ? ";
        query = holder.exec(sql);
        query.bindValue(0, topId);
        if (!query.exec()) {
            qCritical() << "sql query err in IdpDataDao::checkAndSave when check max version: "
                        << query.lastError();
            return SQL_CONNECTION_OPEN_ERR_CODE;
        }
        if (query.next()) {
            version = query.value(0).toInt() + 1;
        }
        query.clear();
    }
    entity.setVersion(version);
    entity.setTopId(topId);
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

qint32 IdpDataDao::save(IdpDataEntity & entity) {
    SqlConnectionHolder holder = getConnectionHolder();
    if (holder.openError()) {
        qCritical() << "sql connection open err on IdpDataDao::save: " << holder.getErrMsg();
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    sem::AfterGc gc([this]() { close(); });
    QString sql = "insert into sem_idp_data_info (idp_name, class_id, idw_id, idw_top_id, idp_ap_detail, "
                  "idp_mp_detail, version, top_id, invalid)"
                  " values (? ,? ,? ,? ,? ,? ,? ,?, ?)";
    SqlQueryHolder query = holder.exec(sql);
    query.addBindValue(entity.getIdpName());
    query.addBindValue(entity.getClassId());
    query.addBindValue(entity.getIdwId());
    query.addBindValue(entity.getIdwTopId());
    query.addBindValue(entity.getIdpApDetail());
    query.addBindValue(entity.getIdpMpDetail());
    query.addBindValue(entity.getVersion());
    query.addBindValue(entity.getTopId());
    query.addBindValue(entity.getInvalid());
    if (!query.exec()) {
        qCritical() << "sql query err in IdpDataDao::save when save: " << query.lastError();
        return SQL_UPDATE_ERR_CODE;
    }
    quint64 id = query.lastInsertId().toULongLong();
    entity.setId(id);
    return 0;
}

qint32 IdpDataDao::updateTopId(quint64 id, quint64 topId, int *errCode) {
    SqlConnectionHolder holder = getConnectionHolder();
    if (holder.openError()) {
        qCritical() << "sql connection open err on IdpDataDao::updateTopId: " << holder.getErrMsg();
        if (errCode) {
            *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        }
        return 0;
    }
    SqlQueryHolder query = holder.exec("update sem_idp_data_info set top_id = ? where id = ?");
    query.addBindValue(topId);
    query.addBindValue(id);
    if (!query.exec()) {
        qCritical() << "sql query err in IdpDataDao::updateTopId when update top_id: "
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

qint32 IdpDataDao::saveWithTransaction(
    IdpDataEntity &entity) {
    bool hasTran = hasTransaction();
    bool succ = beginTransaction(true);
    if (!succ) {
        qCritical() << "Open transaction fail when IdpDataDao::save";
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    qint32 errCode = checkAndSave(entity);
    if (!hasTran) {
        if (errCode) {
            rollback();
        } else {
            commit();
        }
    }
    return errCode;
}

IdpDataEntity IdpDataDao::get(
    quint64 id, qint32 *errCode) {
    SqlConnectionHolder holder = getConnectionHolder();
    sem::AfterGc gc([this]() { close(); });
    if (holder.openError()) {
        qCritical() << "sql connection open err on IdpDataDao::get: " << holder.getErrMsg();
        if (errCode) {
            *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        }
        return IdpDataEntity();
    }
    QString sql = "select * from sem_idp_data_info where id = ? and deleted = 0";
    SqlQueryHolder query = holder.exec(sql);
    query.bindValue(0, id);
    if (!query.exec()) {
        qCritical() << "query err at IdpDataDao::get: " << query.lastError();
        if (errCode) {
            *errCode = SQL_QUERY_ERR_CODE;
        }
        return IdpDataEntity();
    }
    IdpDataEntity entity;
    if (!query.next()) {
        return entity;
    }
    entity.setId(query.value(0).toULongLong());
    entity.setIdpName(query.value(1).toString());
    entity.setClassId(query.value(2).toULongLong());
    entity.setIdwId(query.value(3).toULongLong());
    entity.setIdwTopId(query.value(4).toULongLong());
    entity.setIdpApDetail(query.value(5).toString());
    entity.setIdpMpDetail(query.value(6).toString());

    entity.setVersion(query.value(7).toInt());
    entity.setTopId(query.value(8).toULongLong());
    entity.setInvalid(query.value(9).toInt());
    entity.setDeleted(query.value(10).toBool());
    entity.setCreateTime(query.value(11).toDateTime());
    entity.setEditTime(query.value(12).toDateTime());
    if (errCode) {
        *errCode = 0;
    }
    return entity;
}

IdpDataEntity IdpDataDao::getByTopId(quint64 topId, qint32 *errCode) {
    SqlConnectionHolder holder = getConnectionHolder();
    sem::AfterGc gc([this]() { close(); });
    if (holder.openError()) {
        qCritical() << "sql connection open err on IdpDataDao::getByTopId: " << holder.getErrMsg();
        if (errCode) {
            *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        }
        return IdpDataEntity();
    }
    QString sql = "select * from sem_idp_data_info where top_id = ? and deleted = 0 and invalid = 0 order by id desc limit 1";
    SqlQueryHolder query = holder.exec(sql);
    query.addBindValue(topId);
    if (!query.exec()) {
        qCritical() << "query err at IdpDataDao::getByTopId: " << query.lastError();
        if (errCode) {
            *errCode = SQL_QUERY_ERR_CODE;
        }
        return IdpDataEntity();
    }
    IdpDataEntity entity;
    if (!query.next()) {
        return entity;
    }
    entity.setId(query.value(0).toULongLong());
    entity.setIdpName(query.value(1).toString());
    entity.setClassId(query.value(2).toULongLong());
    entity.setIdwId(query.value(3).toULongLong());
    entity.setIdwTopId(query.value(4).toULongLong());
    entity.setIdpApDetail(query.value(5).toString());
    entity.setIdpMpDetail(query.value(6).toString());

    entity.setVersion(query.value(7).toInt());
    entity.setTopId(query.value(8).toULongLong());
    entity.setInvalid(query.value(9).toInt());
    entity.setDeleted(query.value(10).toBool());
    entity.setCreateTime(query.value(11).toDateTime());
    entity.setEditTime(query.value(12).toDateTime());
    if (errCode) {
        *errCode = 0;
    }
    return entity;
}

IdpDataEntity IdpDataDao::getSimpleInfo(
    quint64 id, qint32 *errCode) {
    SqlConnectionHolder holder = getConnectionHolder();
    sem::AfterGc gc([this]() { close(); });
    if (holder.openError()) {
        qCritical() << "sql connection open err on IdpDataDao::getSimpleInfo: " << holder.getErrMsg();
        if (errCode) {
            *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        }
        return IdpDataEntity();
    }
    QString sql = "select id, idp_name, class_id, idw_id, idw_top_id, version, top_id, invalid, create_time, edit_time from sem_idp_data_info where id = ? and deleted = 0";
    SqlQueryHolder query = holder.exec(sql);
    query.bindValue(0, id);
    if (!query.exec()) {
        qCritical() << "query err at IdpDataDao::getSimpleInfo: " << query.lastError();
        if (errCode) {
            *errCode = SQL_QUERY_ERR_CODE;
        }
        return IdpDataEntity();
    }
    IdpDataEntity entity;
    if (!query.next()) {
        return entity;
    }
    entity.setId(query.value(0).toULongLong());
    entity.setIdpName(query.value(1).toString());
    entity.setClassId(query.value(2).toULongLong());
    entity.setIdwId(query.value(3).toULongLong());
    entity.setIdwTopId(query.value(4).toULongLong());

    entity.setVersion(query.value(5).toInt());
    entity.setTopId(query.value(6).toULongLong());
    entity.setInvalid(query.value(7).toInt());
    entity.setCreateTime(query.value(8).toDateTime());
    entity.setEditTime(query.value(9).toDateTime());
    if (errCode) {
        *errCode = 0;
    }
    return entity;
}

QString IdpDataDao::getIdpName(
    qint32 id, qint32 *errCode) {
    SqlConnectionHolder holder = getConnectionHolder();
    sem::AfterGc gc([this]() { close(); });
    if (holder.openError()) {
        qCritical() << "sql connection open err on IdpDataDao::getIdpName: " << holder.getErrMsg();
        if (errCode) {
            *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        }
        return "";
    }
    QString sql = "select idp_name from sem_idp_data_info where id = ? and deleted = 0";
    SqlQueryHolder query = holder.exec(sql);
    query.bindValue(0, id);
    if (!query.exec()) {
        qCritical() << "query err at IdpDataDao::getIdpName: " << query.lastError();
        if (errCode) {
            *errCode = SQL_QUERY_ERR_CODE;
        }
        return "";
    }
    if (errCode) {
        *errCode = 0;
    }
    if (!query.next()) {
        return "";
    }
    return query.value(0).toString();
}

IdpDataEntity IdpDataDao::getSimpleIdpByName(quint64 classId, const QString& name, qint32* errCode) {
    SqlConnectionHolder holder = getConnectionHolder();
    sem::AfterGc gc([this]() { close(); });
    IdpDataEntity entity;
    if (holder.openError()) {
        qCritical() << "sql connection open err on IdpDataDao::getSimpleIdpByName: " << holder.getErrMsg();
        if (errCode) {
            *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        }
        return entity;
    }
    QString sql = "select id, idp_name, class_id, idw_id, idw_top_id, version, top_id, invalid, create_time, edit_time from sem_idp_data_info where class_id = ? and idp_name = ? and deleted = 0 and invalid = 0 limit 1";
    SqlQueryHolder query = holder.exec(sql);
    query.addBindValue(classId);
    query.addBindValue(name);
    if (!query.exec()) {
        qCritical() << "query err at IdpDataDao::getSimpleIdpByName: " << query.lastError();
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
    entity.setIdpName(query.value(1).toString());
    entity.setClassId(query.value(2).toULongLong());
    entity.setIdwId(query.value(3).toULongLong());
    entity.setIdwTopId(query.value(4).toULongLong());

    entity.setVersion(query.value(5).toInt());
    entity.setTopId(query.value(6).toULongLong());
    entity.setInvalid(query.value(7).toInt());
    entity.setCreateTime(query.value(8).toDateTime());
    entity.setEditTime(query.value(9).toDateTime());
    return entity;
}

QList<IdpDataEntity> IdpDataDao::searchList(
    const IdpSearchDTO &param, qint32 *errCode) {
    QList<IdpDataEntity> list;
    SqlConnectionHolder holder = getConnectionHolder();
    sem::AfterGc gc([this]() { close(); });
    if (holder.openError()) {
        qCritical() << "sql connection open err on IdpDataDao::searchList: " << holder.getErrMsg();
        if (errCode) {
            *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        }
        return list;
    }
    QString sql = "select id, idp_name, class_id, idw_id, idw_top_id, version, top_id, invalid, create_time, edit_time from "
                  "sem_idp_data_info where deleted = 0";
    const QList<quint64> &ids = param.getIds();
    if (ids.size() > 0) {
        sql.append(" and id in (");
        for (int i = 0; i < ids.size(); ++i) {
            if (i > 0) {
                sql.append(",");
            }
            sql.append("?");
        }
        sql.append(")");
    }
    if (!param.getIdpName().isEmpty()) {
        sql.append(" and idp_name like concat('%', ?, '%')");
    }
    if (param.getTopId() != nullptr) {
        sql.append(" and top_id = ?");
    }
    if (param.getInvalid() != nullptr) {
        sql.append(" and invalid = ?");
    }
    if (param.getClassId() != nullptr) {
        sql.append(" and class_id = ?");
    }
    if (param.getIdwId() != nullptr) {
        sql.append(" and idw_id = ?");
    }
    if (param.getIdwTopId() != nullptr) {
        sql.append(" and idw_top_id = ?");
    }
    sql.append(" order by id asc");
    SqlQueryHolder query = holder.exec(sql);
    if (ids.size() > 0) {
        for (const quint64 &id : ids) {
            query.addBindValue(id);
        }
    }
    if (!param.getIdpName().isEmpty()) {
        query.addBindValue(param.getIdpName());
    }
    if (param.getTopId() != nullptr) {
        query.addBindValue(param.getTopId().value());
    }
    if (param.getInvalid() != nullptr) {
        query.addBindValue(param.getInvalid().value() ? 1 : 0);
    }
    if (param.getClassId() != nullptr) {
        query.addBindValue(param.getClassId().value());
    }
    if (param.getIdwId() != nullptr) {
        query.addBindValue(param.getIdwId().value());
    }
    if (param.getIdwTopId() != nullptr) {
        query.addBindValue(param.getIdwTopId().value());
    }

    if (!query.exec()) {
        qCritical() << "query error at IdpDataDao::searchList: " << query.lastError();
        if (errCode) {
            *errCode = SQL_QUERY_ERR_CODE;
        }
        return list;
    }
    if (errCode) {
        *errCode = 0;
    }
    qint32 size = query.size();
    if (size >= 0) {
        if (size == 0) {
            return list;
        }
        list.resize(size);
    }
    qint32 index = 0;
    IdpDataEntity entity;
    while (query.next()) {
        entity.setId(query.value(0).toULongLong());
        entity.setIdpName(query.value(1).toString());
        entity.setClassId(query.value(2).toULongLong());
        entity.setIdwId(query.value(3).toULongLong());
        entity.setIdwTopId(query.value(4).toULongLong());

        entity.setVersion(query.value(5).toInt());
        entity.setTopId(query.value(6).toULongLong());
        entity.setInvalid(query.value(7).toInt());
        entity.setCreateTime(query.value(8).toDateTime());
        entity.setEditTime(query.value(9).toDateTime());
        if (index >= list.size()) {
            list.append(entity);
        } else {
            list.replace(index, entity);
        }
        ++index;
    }
    return list;
}

quint64 IdpDataDao::checkIdwHasIdp(
    quint64 idwId, quint64 skipIdpId, int *errCode) {
    SqlConnectionHolder holder = getConnectionHolder();
    sem::AfterGc gc([this]() { close(); });
    if (holder.openError()) {
        qCritical() << "sql connection open err on IdpDataDao::checkIdwHasIdp: "
                    << holder.getErrMsg();
        if (errCode) {
            *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        }
        return 0;
    }
    QString sql
        = "select id from sem_idp_data_info where deleted = 0 and invalid = 0 and idw_id = ? ";
    if (skipIdpId > 0) {
        sql.append(" and id != ? ");
    }
    sql.append(" limit 1");
    SqlQueryHolder query = holder.exec(sql);
    query.addBindValue(idwId);
    if (skipIdpId > 0) {
        query.addBindValue(skipIdpId);
    }
    if (!query.exec()) {
        qCritical() << "update error at IdpDataDao::checkIdwHasIdp: " << query.lastError();
        if (errCode) {
            *errCode = SQL_UPDATE_ERR_CODE;
        }
        return 0;
    }
    if (errCode) {
        *errCode = 0;
    }
    return query.next() ? query.value(0).toUInt() : 0;
}

qint32 IdpDataDao::updateIdpNameOnly(
    quint64 id, const QString &title) {
    SqlConnectionHolder holder = getConnectionHolder();
    sem::AfterGc gc([this]() { close(); });
    if (holder.openError()) {
        qCritical() << "sql connection open err on IdpDataDao::updateIdpNameOnly: "
                    << holder.getErrMsg();
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    QString sql = "update sem_idp_data_info set idp_name = ? where id = ?";
    SqlQueryHolder query = holder.exec(sql);
    query.bindValue(0, title);
    query.bindValue(1, id);
    if (!query.exec()) {
        qCritical() << "update error at IdpDataDao::updateIdpNameOnly: " << query.lastError();
        return SQL_UPDATE_ERR_CODE;
    }
    return 0;
}

qint32 IdpDataDao::deleteIDPById(
    quint64 id) {
    SqlConnectionHolder holder = getConnectionHolder();
    sem::AfterGc gc([this]() { close(); });
    if (holder.openError()) {
        qCritical() << "sql connection open err on IdpDataDao::deleteIDPById: "
                    << holder.getErrMsg();
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    QString sql = "update sem_idp_data_info set deleted = 1 where id = ?";
    SqlQueryHolder query = holder.exec(sql);
    query.bindValue(0, id);
    if (!query.exec()) {
        qCritical() << "update error at IdpDataDao::deleteIDPById: " << query.lastError();
        return SQL_UPDATE_ERR_CODE;
    }
    return 0;
}

qint32 IdpDataDao::deleteIDPByClass(
    quint64 classId) {
    SqlConnectionHolder holder = getConnectionHolder();
    sem::AfterGc gc([this]() { close(); });
    if (holder.openError()) {
        qCritical() << "sql connection open err on IdpDataDao::deleteIDPByClass: "
                    << holder.getErrMsg();
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    QString sql = "update sem_idp_data_info set deleted = 1 where class_id = ?";
    SqlQueryHolder query = holder.exec(sql);
    query.bindValue(0, classId);
    if (!query.exec()) {
        qCritical() << "update error at IdpDataDao::deleteIDPByClass: " << query.lastError();
        return SQL_UPDATE_ERR_CODE;
    }
    return 0;
}
