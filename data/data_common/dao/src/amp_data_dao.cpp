#include "amp_data_dao.h"
#include "exception_code.h"
#include "sem_common.h"

qint32 AmpDataDao::save(
    AmpDataEntity &entity) {
    SqlConnectionHolder holder = getConnectionHolder();
    if (holder.openError()) {
        qCritical() << "sql connection open err on AmpDataDao::save: " << holder.getErrMsg();
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    sem::AfterGc gc([this]() { close(); });

    //这个version应该不会升级到几千万上亿吧，所以应该不用考虑越界问题
    qint32 version = 0;
    quint64 topId = 0;
    if (entity.getId() > 0) {
        QString sql = "select top_id from sem_amp_data_info where id = ?";
        SqlQueryHolder query = holder.exec(sql);
        query.bindValue(0, entity.getId());
        if (!query.exec()) {
            qCritical() << "sql query err in AmpDataDao::save when query top_id by id: "
                        << query.lastError();
            return SQL_QUERY_ERR_CODE;
        }
        if (!query.next()) {
            qCritical() << "sql query err in AmpDataDao::save when query top_id by "
                           "id[data not found]: "
                        << query.lastError();
            return SQL_QUERY_DATA_NOT_EXISTS;
        }
        topId = query.value(0).toULongLong();
        if (topId <= 0) {
            topId = entity.getId();
        }
        query.clear();

        sql = "update sem_amp_data_info set invalid = 1 where id = ? and invalid = 0 and deleted "
              "= 0";
        query = holder.exec(sql);
        query.bindValue(0, entity.getId());
        if (!query.exec()) {
            qCritical() << "sql query err in AmpDataDao::save when update invalid by id: "
                        << query.lastError();
            return SQL_UPDATE_ERR_CODE;
        }
        query.clear();
        sql = "select max(version) from sem_amp_data_info where top_id = ? ";
        query = holder.exec(sql);
        query.bindValue(0, topId);
        if (!query.exec()) {
            qCritical() << "sql query err in AmpDataDao::save when check max version: "
                        << query.lastError();
            return SQL_CONNECTION_OPEN_ERR_CODE;
        }
        if (query.next()) {
            version = query.value(0).toInt() + 1;
        }
        query.clear();
    }

    QString sql = "insert into sem_amp_data_info (amp_name, amp_type, detail_content, "
                  "version, top_id, invalid)"
                  " values (? ,? ,? ,? ,? ,?)";
    SqlQueryHolder query = holder.exec(sql);
    query.bindValue(0, entity.getAmpName());
    query.bindValue(1, entity.getAmpType());
    query.bindValue(2, entity.getDetailContent());
    query.bindValue(3, version);
    query.bindValue(4, topId);
    query.bindValue(5, entity.getInvalid());
    if (!query.exec()) {
        qCritical() << "sql query err in AmpDataDao::save when save: " << query.lastError();
        return SQL_UPDATE_ERR_CODE;
    }
    quint64 id = query.lastInsertId().toULongLong();
    if (entity.getId() == 0) {
        query = holder.exec("update sem_amp_data_info set top_id = id where id = ?");
        query.addBindValue(id);
        if (!query.exec()) {
            qCritical() << "sql query err in AmpDataDao::save when update top_id: "
                        << query.lastError();
            return SQL_UPDATE_ERR_CODE;
        }
    }
    entity.setId(id);
    return 0;
}

qint32 AmpDataDao::saveWithTransaction(
    AmpDataEntity &entity) {
    bool hasTran = hasTransaction();
    bool succ = beginTransaction(true);
    if (!succ) {
        qCritical() << "Open transaction fail when AmpDataDao::save";
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

AmpDataEntity AmpDataDao::get(
    quint64 id, qint32 *errCode) {
    SqlConnectionHolder holder = getConnectionHolder();
    sem::AfterGc gc([this]() { close(); });
    if (holder.openError()) {
        qCritical() << "sql connection open err on AmpDataDao::get: " << holder.getErrMsg();
        if (errCode) {
            *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        }
        return AmpDataEntity();
    }
    QString sql = "select * from sem_amp_data_info where id = ? and deleted = 0";
    SqlQueryHolder query = holder.exec(sql);
    query.bindValue(0, id);
    if (!query.exec()) {
        qCritical() << "query err at AmpDataDao::get: " << query.lastError();
        if (errCode) {
            *errCode = SQL_QUERY_ERR_CODE;
        }
        return AmpDataEntity();
    }
    AmpDataEntity entity;
    if (errCode) {
        *errCode = 0;
    }
    if (!query.next()) {
        return entity;
    }
    entity.setId(query.value(0).toULongLong());
    entity.setAmpName(query.value(1).toString());
    entity.setAmpType(query.value(2).toInt());
    entity.setDetailContent(query.value(3).toString());
    entity.setDeleted(query.value(4).toBool());
    entity.setCreateTime(query.value(5).toDateTime());
    entity.setEditTime(query.value(6).toDateTime());
    entity.setVersion(query.value(7).toInt());
    entity.setTopId(query.value(8).toULongLong());
    entity.setInvalid(query.value(9).toInt());
    return entity;
}

quint64 AmpDataDao::checkName(
    const QString &ampName, qint32 *errCode) {
    SqlConnectionHolder holder = getConnectionHolder();
    sem::AfterGc gc([this]() { close(); });
    if (holder.openError()) {
        qCritical() << "sql connection open err on AmpDataDao::checkName: " << holder.getErrMsg();
        if (errCode) {
            *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        }
        return 0;
    }
    QString sql = "select id from sem_amp_data_info where amp_name = ? and deleted = 0 and invalid "
                  "= 0 limit 1";
    SqlQueryHolder query = holder.exec(sql);
    query.bindValue(0, ampName);
    if (!query.exec()) {
        qCritical() << "query err at AmpDataDao::checkName: " << query.lastError();
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

QList<AmpDataEntity> AmpDataDao::searchList(
    const AmpDataSearchDTO &param, qint32 *errCode) {
    QList<AmpDataEntity> list;
    SqlConnectionHolder holder = getConnectionHolder();
    sem::AfterGc gc([this]() { close(); });
    if (holder.openError()) {
        qCritical() << "sql connection open err on AmpDataDao::searchList: " << holder.getErrMsg();
        if (errCode) {
            *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        }
        return list;
    }
    QString sql = "select id, amp_name, amp_type, create_time from "
                  "sem_amp_data_info where deleted = 0";
    if (!param.getAmpName().isEmpty()) {
        sql.append(" and amp_name like concat('%', ?, '%')");
    }
    if (param.getTopId() != nullptr) {
        sql.append(" and top_id = ?");
    }
    if (param.getInvalid() != nullptr) {
        sql.append(" and invalid = ?");
    }
    sql.append(" order by id asc");
    SqlQueryHolder query = holder.exec(sql);
    int bindIndex = 0;
    if (!param.getAmpName().isEmpty()) {
        query.bindValue(bindIndex, param.getAmpName());
        ++bindIndex;
    }
    if (param.getTopId() != nullptr) {
        query.bindValue(bindIndex, param.getTopId().value());
        ++bindIndex;
    }
    if (param.getInvalid() != nullptr) {
        query.bindValue(bindIndex, param.getInvalid().value() ? 1 : 0);
        ++bindIndex;
    }

    if (!query.exec()) {
        qCritical() << "query error at AmpDataDao::searchList: " << query.lastError();
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
    while (query.next()) {
        AmpDataEntity entity;
        entity.setId(query.value(0).toULongLong());
        entity.setAmpName(query.value(1).toString());
        entity.setAmpType(query.value(2).toInt());
        entity.setCreateTime(query.value(3).toDateTime());
        if (index >= list.size()) {
            list.append(entity);
        } else {
            list.replace(index, entity);
        }
        ++index;
    }
    return list;
}
