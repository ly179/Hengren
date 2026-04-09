#include "idw_wafer_dao.h"
#include "sem_common.h"
#include "sql_query_holder.h"
#include "exception_code.h"

qint32 IDW_WaferDao::checkAndSave(
    IDW_WaferInfoEntity& entity) {
    SqlConnectionHolder holder = getConnectionHolder();
    if (holder.openError()) {
        qCritical() << "sql connection open err on IDW_WaferDao::checkAndSave: "
                    << holder.getErrMsg();
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    sem::AfterGc gc([this]() { close(); });
    //这个version应该不会升级到几千万上亿吧，所以应该不用考虑越界问题
    qint32 version = 0;
    quint64 topId = 0;
    if (entity.getId() > 0) {
        QString sql = "select top_id, class_id from sem_idw_wafer_info where id = ?";
        SqlQueryHolder query = holder.exec(sql);
        query.bindValue(0, entity.getId());
        if (!query.exec()) {
            qCritical() << "sql query err in IDW_WaferDao::checkAndSave when query top_id by id: "
                        << query.lastError();
            return SQL_QUERY_ERR_CODE;
        }
        if (!query.next()) {
            qCritical() << "sql query err in IDW_WaferDao::checkAndSave when query top_id by "
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

        sql = "update sem_idw_wafer_info set invalid = 1 where id = ? and invalid = 0 and deleted = 0";
        query = holder.exec(sql);
        query.bindValue(0, entity.getId());
        if (!query.exec()) {
            qCritical()
                << "sql query err in IDW_WaferDao::checkAndSave when update invalid by id: "
                << query.lastError();
            return SQL_UPDATE_ERR_CODE;
        }
        query.clear();
        sql = "select max(version) from sem_idw_wafer_info where top_id = ? ";
        query = holder.exec(sql);
        query.bindValue(0, topId);
        if (!query.exec()) {
            qCritical() << "sql query err in IDW_WaferDao::checkAndSave when check max version: "
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

qint32 IDW_WaferDao::save(IDW_WaferInfoEntity & entity) {
    SqlConnectionHolder holder = getConnectionHolder();
    if (holder.openError()) {
        qCritical() << "sql connection open err on IDW_WaferDao::saveWaferInfo: "
                    << holder.getErrMsg();
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    sem::AfterGc gc([this]() { close(); });
    QString sql = "insert into sem_idw_wafer_info (class_id, wafer_diameter, wafer_name, items, detail, version, top_id, invalid)"
                  " values (? ,? ,? ,? ,? ,? ,? ,?)";
    SqlQueryHolder query = holder.exec(sql);
    query.bindValue(0, entity.getClassId());
    query.bindValue(1, entity.getWaferDiameter().toQString());
    query.bindValue(2, entity.getWaferName());
    query.bindValue(3, entity.getItems());
    query.bindValue(4, entity.getDetail());
    query.bindValue(5, entity.getVersion());
    query.bindValue(6, entity.getTopId());
    query.bindValue(7, entity.getInvalid());
    if (!query.exec()) {
        qCritical() << "sql query err in IDW_WaferDao::saveWaferInfo when save: "
                    << query.lastError();
        return SQL_UPDATE_ERR_CODE;
    }
    quint64 id = query.lastInsertId().toULongLong();
    entity.setId(id);
    return 0;
}

qint32 IDW_WaferDao::updateTopId(quint64 id, quint64 topId, int *errCode) {
    SqlConnectionHolder holder = getConnectionHolder();
    if (holder.openError()) {
        qCritical() << "sql connection open err on IDW_WaferDao::updateTopId: "
                    << holder.getErrMsg();
        if (errCode) {
            *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        }
        return 0;
    }
    sem::AfterGc gc([this]() { close(); });
    SqlQueryHolder query = holder.exec("update sem_idw_wafer_info set top_id = ? where id = ?");
    query.addBindValue(topId);
    query.addBindValue(id);
    if (!query.exec()) {
        qCritical() << "sql query err in IDW_WaferDao::updateTopId when update top_id: "
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

qint32 IDW_WaferDao::saveWithTransaction(
    IDW_WaferInfoEntity& entity) {
    bool hasTran = hasTransaction();
    bool succ = beginTransaction(true);
    if (!succ) {
        qCritical() << "Open transaction fail when IDW_WaferDao::save";
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

IDW_WaferInfoEntity IDW_WaferDao::getWaferInfoById(
    quint64 id, qint32* errCode) {
    SqlConnectionHolder holder = getConnectionHolder();
    sem::AfterGc gc([this](){
        close();
    });
    if (holder.openError()) {
        qCritical() << "sql connection open err on IDW_WaferDao::getWaferInfoById: "
                    << holder.getErrMsg();
        if (errCode) {
            *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        }
        return IDW_WaferInfoEntity();
    }
    QString sql = "select * from sem_idw_wafer_info where id = ? and deleted = 0";
    SqlQueryHolder query = holder.exec(sql);
    query.bindValue(0, id);
    if (!query.exec()) {
        qCritical() << "query err at IDW_WaferDao::getWaferInfoById: " << query.lastError();
        if (errCode) {
            *errCode = SQL_QUERY_ERR_CODE;
        }
        return IDW_WaferInfoEntity();
    }
    IDW_WaferInfoEntity entity;
    if (!query.next()) {
        return entity;
    }
    entity.setId(query.value(0).toULongLong());
    entity.setClassId(query.value(1).toULongLong());
    entity.setWaferDiameter(query.value(2).toString());
    entity.setWaferName(query.value(3).toString());
    entity.setItems(query.value(4).toString());
    entity.setDetail(query.value(5).toString());
    entity.setVersion(query.value(6).toInt());
    entity.setTopId(query.value(7).toULongLong());
    entity.setInvalid(query.value(8).toInt());
    entity.setDeleted(query.value(9).toBool());
    entity.setCreateTime(query.value(10).toDateTime());
    entity.setEditTime(query.value(11).toDateTime());
    if (errCode) {
        *errCode = 0;
    }
    return entity;
}

IDW_WaferInfoEntity IDW_WaferDao::getWaferInfoByTopId(quint64 topId, qint32 *errCode) {
    SqlConnectionHolder holder = getConnectionHolder();
    sem::AfterGc gc([this](){
        close();
    });
    if (holder.openError()) {
        qCritical() << "sql connection open err on IDW_WaferDao::getWaferInfoByTopId: "
                    << holder.getErrMsg();
        if (errCode) {
            *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        }
        return IDW_WaferInfoEntity();
    }
    QString sql = "select * from sem_idw_wafer_info where top_id = ? and invalid = 0 and deleted = 0 order by id desc limit 1";
    SqlQueryHolder query = holder.exec(sql);
    query.bindValue(0, topId);
    if (!query.exec()) {
        qCritical() << "query err at IDW_WaferDao::getWaferInfoByTopId: " << query.lastError();
        if (errCode) {
            *errCode = SQL_QUERY_ERR_CODE;
        }
        return IDW_WaferInfoEntity();
    }
    IDW_WaferInfoEntity entity;
    if (!query.next()) {
        return entity;
    }
    entity.setId(query.value(0).toULongLong());
    entity.setClassId(query.value(1).toULongLong());
    entity.setWaferDiameter(query.value(2).toString());
    entity.setWaferName(query.value(3).toString());
    entity.setItems(query.value(4).toString());
    entity.setDetail(query.value(5).toString());
    entity.setVersion(query.value(6).toInt());
    entity.setTopId(query.value(7).toULongLong());
    entity.setInvalid(query.value(8).toInt());
    entity.setDeleted(query.value(9).toBool());
    entity.setCreateTime(query.value(10).toDateTime());
    entity.setEditTime(query.value(11).toDateTime());
    if (errCode) {
        *errCode = 0;
    }
    return entity;
}

IDW_WaferInfoEntity IDW_WaferDao::getSimpleInfoById(quint64 id, qint32 *errCode) {
    SqlConnectionHolder holder = getConnectionHolder();
    sem::AfterGc gc([this](){
        close();
    });
    if (holder.openError()) {
        qCritical() << "sql connection open err on IDW_WaferDao::getSimpleInfoById: "
                    << holder.getErrMsg();
        if (errCode) {
            *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        }
        return IDW_WaferInfoEntity();
    }
    QString sql = "select id, class_id, wafer_diameter, wafer_name, version, top_id, invalid, create_time, edit_time from sem_idw_wafer_info where id = ? and deleted = 0";
    SqlQueryHolder query = holder.exec(sql);
    query.bindValue(0, id);
    if (!query.exec()) {
        qCritical() << "query err at IDW_WaferDao::getSimpleInfoById: " << query.lastError();
        if (errCode) {
            *errCode = SQL_QUERY_ERR_CODE;
        }
        return IDW_WaferInfoEntity();
    }
    IDW_WaferInfoEntity entity;
    if (!query.next()) {
        return entity;
    }
    entity.setId(query.value(0).toULongLong());
    entity.setClassId(query.value(1).toULongLong());
    entity.setWaferDiameter(query.value(2).toString());
    entity.setWaferName(query.value(3).toString());
    entity.setVersion(query.value(4).toInt());
    entity.setTopId(query.value(5).toULongLong());
    entity.setInvalid(query.value(6).toInt());
    entity.setCreateTime(query.value(7).toDateTime());
    entity.setEditTime(query.value(8).toDateTime());
    if (errCode) {
        *errCode = 0;
    }
    return entity;
}

IDW_WaferInfoEntity IDW_WaferDao::getSimpleInfoByTopId(quint64 topId, qint32 *errCode) {
    SqlConnectionHolder holder = getConnectionHolder();
    sem::AfterGc gc([this](){
        close();
    });
    if (holder.openError()) {
        qCritical() << "sql connection open err on IDW_WaferDao::getSimpleInfoByTopId: "
                    << holder.getErrMsg();
        if (errCode) {
            *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        }
        return IDW_WaferInfoEntity();
    }
    QString sql = "select id, class_id, wafer_diameter, wafer_name, version, top_id, invalid, create_time, edit_time from sem_idw_wafer_info where top_id = ? and invalid = 0  and deleted = 0 order by id desc limit 1";
    SqlQueryHolder query = holder.exec(sql);
    query.bindValue(0, topId);
    if (!query.exec()) {
        qCritical() << "query err at IDW_WaferDao::getSimpleInfoByTopId: " << query.lastError();
        if (errCode) {
            *errCode = SQL_QUERY_ERR_CODE;
        }
        return IDW_WaferInfoEntity();
    }
    IDW_WaferInfoEntity entity;
    if (!query.next()) {
        return entity;
    }
    entity.setId(query.value(0).toULongLong());
    entity.setClassId(query.value(1).toULongLong());
    entity.setWaferDiameter(query.value(2).toString());
    entity.setWaferName(query.value(3).toString());
    entity.setVersion(query.value(4).toInt());
    entity.setTopId(query.value(5).toULongLong());
    entity.setInvalid(query.value(6).toInt());
    entity.setCreateTime(query.value(7).toDateTime());
    entity.setEditTime(query.value(8).toDateTime());
    if (errCode) {
        *errCode = 0;
    }
    return entity;
}

QString IDW_WaferDao::getWaferName(
    quint64 id, qint32* errCode) {
    SqlConnectionHolder holder = getConnectionHolder();
    sem::AfterGc gc([this]() { close(); });
    if (holder.openError()) {
        qCritical() << "sql connection open err on IDW_WaferDao::getWaferName: "
                    << holder.getErrMsg();
        if (errCode) {
            *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        }
        return "";
    }
    QString sql = "select wafer_name from sem_idw_wafer_info where id = ? and deleted = 0";
    SqlQueryHolder query = holder.exec(sql);
    query.bindValue(0, id);
    if (!query.exec()) {
        qCritical() << "query err at IDW_WaferDao::getWaferInfoById: " << query.lastError();
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

IDW_WaferInfoEntity IDW_WaferDao::getSimpleWaferIdByName(
    const QString& name, quint64 classId, int* errCode) {
    IDW_WaferInfoEntity entity;
    SqlConnectionHolder holder = getConnectionHolder();
    sem::AfterGc gc([this]() { close(); });
    if (holder.openError()) {
        qCritical() << "sql connection open err on IDW_WaferDao::getWaferIdByName: "
                    << holder.getErrMsg();
        if (errCode) {
            *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        }
        return entity;
    }
    QString sql = "select id, class_id, wafer_diameter, wafer_name, version, top_id, invalid, create_time, edit_time from sem_idw_wafer_info where wafer_name = ? and class_id = ? and "
                  "deleted = 0 and invalid = 0 limit 1";
    SqlQueryHolder query = holder.exec(sql);
    query.addBindValue(name);
    query.addBindValue(classId);
    if (!query.exec()) {
        qCritical() << "query err at IDW_WaferDao::getWaferIdByName: " << query.lastError();
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
    entity.setClassId(query.value(1).toULongLong());
    entity.setWaferDiameter(query.value(2).toString());
    entity.setWaferName(query.value(3).toString());
    entity.setVersion(query.value(4).toInt());
    entity.setTopId(query.value(5).toULongLong());
    entity.setInvalid(query.value(6).toInt());
    entity.setCreateTime(query.value(7).toDateTime());
    entity.setEditTime(query.value(8).toDateTime());
    return entity;
}

quint64 IDW_WaferDao::getClassIdByWaferId(quint64 id, qint32* errCode) {
    SqlConnectionHolder holder = getConnectionHolder();
    sem::AfterGc gc([this]() { close(); });
    if (holder.openError()) {
        qCritical() << "sql connection open err on IDW_WaferDao::getClassIdByWaferId: "
                    << holder.getErrMsg();
        if (errCode) {
            *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        }
        return 0;
    }
    QString sql = "select class_id from sem_idw_wafer_info where id = ? and "
                  "deleted = 0";
    SqlQueryHolder query = holder.exec(sql);
    query.addBindValue(id);
    if (!query.exec()) {
        qCritical() << "query err at IDW_WaferDao::getClassIdByWaferId: " << query.lastError();
        if (errCode) {
            *errCode = SQL_QUERY_ERR_CODE;
        }
        return 0;
    }
    if (errCode) {
        *errCode = 0;
    }
    if (!query.next()) {
        return 0;
    }
    return query.value(0).toULongLong();
}

QList<IDW_WaferInfoEntity> IDW_WaferDao::searchList(
    const IDW_WaferSearchDTO& param, qint32* errCode) {
    QList<IDW_WaferInfoEntity> list;
    SqlConnectionHolder holder = getConnectionHolder();
    sem::AfterGc gc([this](){
        close();
    });
    if (holder.openError()) {
        qCritical() << "sql connection open err on IDW_WaferDao::searchList: "
                    << holder.getErrMsg();
        if (errCode) {
            *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        }
        return list;
    }
    QString sql = "select id, class_id, wafer_diameter, wafer_name, version, top_id, invalid, create_time, edit_time from sem_idw_wafer_info where deleted = 0";
    const QList<quint64>& ids = param.getIds();
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
    if (!param.getWaferName().isEmpty()) {
        sql.append(" and wafer_name like concat('%', ?, '%')");
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
    sql.append(" order by id asc");
    SqlQueryHolder query = holder.exec(sql);
    if (ids.size() > 0) {
        for (const quint64& id : ids) {
            query.addBindValue(id);
        }
    }
    if (!param.getWaferName().isEmpty()) {
        query.addBindValue(param.getWaferName());
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

    if (!query.exec()) {
        qCritical() << "query error at IDW_WaferDao::searchList: " << query.lastError();
        if (errCode) {
            *errCode = SQL_QUERY_ERR_CODE;
        }
        return list;
    }
    if (errCode) {
        *errCode = 0;
    }
    qint32 size = query.size();
    if (size>= 0) {
        if (size == 0) {
            return list;
        }
        list.resize(size);
    }
    qint32 index = 0;
    IDW_WaferInfoEntity entity;
    while (query.next()) {
        entity.setId(query.value(0).toULongLong());
        entity.setClassId(query.value(1).toULongLong());
        entity.setWaferDiameter(query.value(2).toString());
        entity.setWaferName(query.value(3).toString());
        entity.setVersion(query.value(4).toInt());
        entity.setTopId(query.value(5).toULongLong());
        entity.setInvalid(query.value(6).toInt());
        entity.setCreateTime(query.value(7).toDateTime());
        entity.setEditTime(query.value(8).toDateTime());
        if (index >= list.size()) {
            list.append(entity);
        } else {
            list.replace(index, entity);
        }
        ++ index;
    }
    return list;
}

qint32 IDW_WaferDao::updateWaferNameOnly(
    quint64 id, const QString& title) {
    SqlConnectionHolder holder = getConnectionHolder();
    sem::AfterGc gc([this](){
        close();
    });
    if (holder.openError()) {
        qCritical() << "sql connection open err on IDW_WaferDao::updateWaferNameOnly: "
                    << holder.getErrMsg();
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    QString sql = "update sem_idw_wafer_info set wafer_name = ? where id = ?";
    SqlQueryHolder query = holder.exec(sql);
    query.bindValue(0, title);
    query.bindValue(1, id);
    if (!query.exec()) {
        qCritical() << "update error at IDW_WaferDao::updateWaferNameOnly: " << query.lastError();
        return SQL_UPDATE_ERR_CODE;
    }
    return 0;
}

qint32 IDW_WaferDao::deleteWaferById(
    quint64 id) {
    SqlConnectionHolder holder = getConnectionHolder();
    sem::AfterGc gc([this](){
        close();
    });
    if (holder.openError()) {
        qCritical() << "sql connection open err on IDW_WaferDao::deleteWaferById: "
                    << holder.getErrMsg();
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    QString sql = "update sem_idw_wafer_info set deleted = 1 where id = ?";
    SqlQueryHolder query = holder.exec(sql);
    query.bindValue(0, id);
    if (!query.exec()) {
        qCritical() << "update error at IDW_WaferDao::deleteWaferById: " << query.lastError();
        return SQL_UPDATE_ERR_CODE;
    }
    return 0;
}

qint32 IDW_WaferDao::deleteWaferByClass(
    quint64 classId) {
    SqlConnectionHolder holder = getConnectionHolder();
    sem::AfterGc gc([this](){
        close();
    });
    if (holder.openError()) {
        qCritical() << "sql connection open err on IDW_WaferDao::deleteWaferByClass: "
                    << holder.getErrMsg();
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    QString sql = "update sem_idw_wafer_info set deleted = 1 where class_id = ?";
    SqlQueryHolder query = holder.exec(sql);
    query.bindValue(0, classId);
    if (!query.exec()) {
        qCritical() << "update error at IDW_WaferDao::deleteWaferByClass: " << query.lastError();
        return SQL_UPDATE_ERR_CODE;
    }
    return 0;
}
