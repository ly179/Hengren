#include "template_info_dao.h"
#include "exception_code.h"
#include "sem_common.h"

void fullColumnEntityFromQuery(
    TemplateInfoEntity &entity, const SqlQueryHolder &query) {
    entity.setId(query.value(0).toULongLong());
    entity.setTemplateType(query.value(1).toInt());
    entity.setTemplateName(query.value(2).toString());
    entity.setTemplateData(query.value(3).toString());
    entity.setTemplateComment(query.value(4).toString());
    entity.setVersion(query.value(5).toInt());
    entity.setTopId(query.value(6).toULongLong());
    entity.setInvalid(query.value(7).toInt());
    entity.setDeleted(query.value(8).toBool());
    entity.setCreateTime(query.value(9).toDateTime());
    entity.setEditTime(query.value(10).toDateTime());
}

int TemplateInfoDao::save(
    TemplateInfoEntity &entity) {
    SqlConnectionHolder holder = getConnectionHolder();
    if (holder.openError()) {
        qCritical() << "sql connection open err on TemplateInfoDao::save: " << holder.getErrMsg();
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    sem::AfterGc gc([this]() { close(); });

    //这个version应该不会升级到几千万上亿吧，所以应该不用考虑越界问题
    qint32 version = 0;
    quint64 topId = 0;
    if (entity.getId() > 0) {
        QString sql = "select top_id from sem_template_info where id = ?";
        SqlQueryHolder query = holder.exec(sql);
        query.bindValue(0, entity.getId());
        if (!query.exec()) {
            qCritical() << "sql query err in TemplateInfoDao::save when query top_id by id: "
                        << query.lastError();
            return SQL_QUERY_ERR_CODE;
        }
        if (!query.next()) {
            qCritical() << "sql query err in TemplateInfoDao::save when query top_id by "
                           "id[data not found]: "
                        << query.lastError();
            return SQL_QUERY_DATA_NOT_EXISTS;
        }
        topId = query.value(0).toULongLong();
        if (topId <= 0) {
            topId = entity.getId();
        }
        query.clear();

        sql = "update sem_template_info set invalid = 1 where top_id = ? and invalid = 0 and "
              "deleted "
              "= 0";
        query = holder.exec(sql);
        query.addBindValue(topId);
        if (!query.exec()) {
            qCritical() << "sql query err in TemplateInfoDao::save when update invalid by id: "
                        << query.lastError();
            return SQL_UPDATE_ERR_CODE;
        }
        query.clear();
        sql = "select max(version) from sem_template_info where top_id = ? ";
        query = holder.exec(sql);
        query.bindValue(0, topId);
        if (!query.exec()) {
            qCritical() << "sql query err in TemplateInfoDao::save when check max version: "
                        << query.lastError();
            return SQL_CONNECTION_OPEN_ERR_CODE;
        }
        if (query.next()) {
            version = query.value(0).toInt() + 1;
        }
        query.clear();
    }

    QString sql = "insert into sem_template_info (template_type, template_name, template_data, "
                  "template_comment,"
                  "version, top_id, invalid)"
                  " values (? ,? ,? ,? ,? ,?, ?)";
    SqlQueryHolder query = holder.exec(sql);
    query.bindValue(0, entity.getTemplateType());
    query.bindValue(1, entity.getTemplateName());
    query.bindValue(2, entity.getTemplateData());
    query.bindValue(3, entity.getTemplateComment());
    query.bindValue(4, version);
    query.bindValue(5, topId);
    query.bindValue(6, entity.getInvalid());
    if (!query.exec()) {
        qCritical() << "sql query err in TemplateInfoDao::save when save: " << query.lastError();
        return SQL_UPDATE_ERR_CODE;
    }
    quint64 id = query.lastInsertId().toULongLong();
    if (entity.getId() == 0) {
        query = holder.exec("update sem_template_info set top_id = id where id = ?");
        query.addBindValue(id);
        if (!query.exec()) {
            qCritical() << "sql query err in TemplateInfoDao::save when update top_id: "
                        << query.lastError();
            return SQL_UPDATE_ERR_CODE;
        }
    }
    entity.setId(id);
    entity.setTopId(topId);
    entity.setVersion(version);
    return 0;
}

qint32 TemplateInfoDao::saveWithTransaction(
    TemplateInfoEntity &entity) {
    bool hasTran = hasTransaction();
    bool succ = beginTransaction(true);
    if (!succ) {
        qCritical() << "Open transaction fail when TemplateInfoDao::save";
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    sem::AfterGc gc([this, &succ, hasTran]() {
        if (!hasTran) {
            if (succ) {
                commit();
            } else {
                rollback();
            }
        }
    });
    qint32 errCode = save(entity);
    if (errCode) {
        succ = false;
    }
    return errCode;
}

TemplateInfoEntity TemplateInfoDao::getById(
    quint64 id, int *errCode) {
    SqlConnectionHolder holder = getConnectionHolder();
    sem::AfterGc gc([this]() { close(); });
    if (holder.openError()) {
        qCritical() << "sql connection open err on TemplateInfoDao::getById: "
                    << holder.getErrMsg();
        if (errCode) {
            *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        }
        return TemplateInfoEntity();
    }
    QString sql = "select * from sem_template_info where id = ? and deleted = 0";
    SqlQueryHolder query = holder.exec(sql);
    query.bindValue(0, id);
    if (!query.exec()) {
        qCritical() << "query err at TemplateInfoDao::getById: " << query.lastError();
        if (errCode) {
            *errCode = SQL_QUERY_ERR_CODE;
        }
        return TemplateInfoEntity();
    }
    TemplateInfoEntity entity;
    if (!query.next()) {
        return entity;
    }
    fullColumnEntityFromQuery(entity, query);
    if (errCode) {
        *errCode = 0;
    }
    return entity;
}

QList<TemplateInfoEntity> TemplateInfoDao::getByIds(
    const QList<quint64> &ids, int *errCode) {
    QList<TemplateInfoEntity> list;
    if (ids.isEmpty()) {
        if (errCode) {
            *errCode = 0;
        }
        return list;
    }
    SqlConnectionHolder holder = getConnectionHolder();
    sem::AfterGc gc([this]() { close(); });

    if (holder.openError()) {
        qCritical() << "sql connection open err on TemplateInfoDao::getByIds: "
                    << holder.getErrMsg();
        if (errCode) {
            *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        }
        return list;
    }
    QString sql = "select * from "
                  "sem_template_info where deleted = 0 and id in (";
    for (int i = 0; i < ids.size(); ++i) {
        if (i > 0) {
            sql.append(",");
        }
        sql.append(" ? ");
    }
    sql.append(") order by id asc");
    SqlQueryHolder query = holder.exec(sql);
    for (quint64 id : ids) {
        query.addBindValue(id);
    }
    if (!query.exec()) {
        qCritical() << "query error at TemplateInfoDao::getByIds: " << query.lastError();
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
        list.reserve(size);
    }
    while (query.next()) {
        TemplateInfoEntity entity;
        fullColumnEntityFromQuery(entity, query);
        list.append(entity);
    }
    return list;
}

TemplateInfoEntity TemplateInfoDao::getDataByIdForNewVersion(
    quint64 id, int *errCode) {
    SqlConnectionHolder holder = getConnectionHolder();
    sem::AfterGc gc([this]() { close(); });
    if (holder.openError()) {
        qCritical() << "sql connection open err on TemplateInfoDao::getDataByTopIdForNewVersion: "
                    << holder.getErrMsg();
        if (errCode) {
            *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        }
        return TemplateInfoEntity();
    }
    QString sql = "select * from sem_template_info where top_id = ("
                  "select top_id from sem_template_info where id = ? ) and deleted = 0  and "
                  "invalid = 0 limit 1";
    SqlQueryHolder query = holder.exec(sql);
    query.bindValue(0, id);
    if (!query.exec()) {
        qCritical() << "query err at TemplateInfoDao::getDataByTopIdForNewVersion: "
                    << query.lastError();
        if (errCode) {
            *errCode = SQL_QUERY_ERR_CODE;
        }
        return TemplateInfoEntity();
    }
    TemplateInfoEntity entity;
    if (!query.next()) {
        return entity;
    }
    fullColumnEntityFromQuery(entity, query);
    if (errCode) {
        *errCode = 0;
    }
    return entity;
}

QList<TemplateInfoEntity> TemplateInfoDao::getDatasByIdsForNewVersion(
    const QList<quint64> &ids, int *errCode) {
    QList<TemplateInfoEntity> list;
    if (ids.isEmpty()) {
        if (errCode) {
            *errCode = 0;
        }
        return list;
    }
    SqlConnectionHolder holder = getConnectionHolder();
    sem::AfterGc gc([this]() { close(); });

    if (holder.openError()) {
        qCritical() << "sql connection open err on TemplateInfoDao::getDatasByIdsForNewVersion: "
                    << holder.getErrMsg();
        if (errCode) {
            *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        }
        return list;
    }
    QString sql
        = "select * from sem_template_info where deleted = 0  and invalid = 0 and top_id in ("
          "select top_id from sem_template_info where id in (";
    for (int i = 0; i < ids.size(); ++i) {
        if (i > 0) {
            sql.append(",");
        }
        sql.append(" ? ");
    }
    sql.append(")) order by id asc");
    SqlQueryHolder query = holder.exec(sql);
    for (quint64 id : ids) {
        query.addBindValue(id);
    }
    if (!query.exec()) {
        qCritical() << "query error at TemplateInfoDao::getDatasByIdsForNewVersion: "
                    << query.lastError();
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
        list.reserve(size);
    }
    while (query.next()) {
        TemplateInfoEntity entity;
        fullColumnEntityFromQuery(entity, query);
        list.append(entity);
    }
    return list;
}

TemplateInfoEntity TemplateInfoDao::getSimpleInfoById(
    quint64 id, int *errCode) {
    SqlConnectionHolder holder = getConnectionHolder();
    sem::AfterGc gc([this]() { close(); });
    if (holder.openError()) {
        qCritical() << "sql connection open err on TemplateInfoDao::getSimpleInfoById: "
                    << holder.getErrMsg();
        if (errCode) {
            *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        }
        return TemplateInfoEntity();
    }
    QString sql = "select id, template_type, template_name, template_comment, create_time from "
                  "sem_template_info where id = ? and deleted = 0";
    SqlQueryHolder query = holder.exec(sql);
    query.bindValue(0, id);
    if (!query.exec()) {
        qCritical() << "query err at TemplateInfoDao::getSimpleInfoById: " << query.lastError();
        if (errCode) {
            *errCode = SQL_QUERY_ERR_CODE;
        }
        return TemplateInfoEntity();
    }
    if (errCode) *errCode = 0;
    TemplateInfoEntity entity;
    if (!query.next()) {
        return entity;
    }
    entity.setId(query.value(0).toULongLong());
    entity.setTemplateType(query.value(1).toInt());
    entity.setTemplateName(query.value(2).toString());
    entity.setTemplateComment(query.value(3).toString());
    entity.setCreateTime(query.value(4).toDateTime());
    return entity;
}

QList<TemplateInfoEntity> TemplateInfoDao::getSimpleInfoByIds(
    const QList<quint64> &ids, int *errCode) {
    QList<TemplateInfoEntity> list;
    if (ids.isEmpty()) {
        if (errCode) {
            *errCode = 0;
        }
        return list;
    }
    SqlConnectionHolder holder = getConnectionHolder();
    sem::AfterGc gc([this]() { close(); });

    if (holder.openError()) {
        qCritical() << "sql connection open err on TemplateInfoDao::getSimpleInfoByIds: "
                    << holder.getErrMsg();
        if (errCode) {
            *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        }
        return list;
    }
    QString sql
        = "select id, top_id, template_type, template_name, template_comment, create_time from "
          "sem_template_info where deleted = 0 and id in (";
    for (int i = 0; i < ids.size(); ++i) {
        if (i > 0) {
            sql.append(",");
        }
        sql.append(" ? ");
    }
    sql.append(") order by id asc");
    SqlQueryHolder query = holder.exec(sql);
    for (quint64 id : ids) {
        query.addBindValue(id);
    }
    if (!query.exec()) {
        qCritical() << "query error at TemplateInfoDao::getSimpleInfoByIds: " << query.lastError();
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
        list.reserve(size);
    }
    while (query.next()) {
        TemplateInfoEntity entity;
        entity.setId(query.value(0).toULongLong());
        entity.setTopId(query.value(1).toULongLong());
        entity.setTemplateType(query.value(2).toInt());
        entity.setTemplateName(query.value(3).toString());
        entity.setTemplateComment(query.value(4).toString());
        entity.setCreateTime(query.value(10).toDateTime());
        list.append(entity);
    }
    return list;
}

TemplateInfoEntity TemplateInfoDao::getSimpleInfoByIdForNewVersion(
    quint64 id, int *errCode) {
    SqlConnectionHolder holder = getConnectionHolder();
    sem::AfterGc gc([this]() { close(); });
    if (holder.openError()) {
        qCritical() << "sql connection open err on TemplateInfoDao::getSimpleInfoById: "
                    << holder.getErrMsg();
        if (errCode) {
            *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        }
        return TemplateInfoEntity();
    }
    QString sql = "select id, template_type, template_name, template_comment, create_time from "
                  "sem_template_info where top_id = ("
                  "select top_id from sem_template_info where id = ? ) and deleted = 0  and "
                  "invalid = 0 limit 1";
    SqlQueryHolder query = holder.exec(sql);
    query.bindValue(0, id);
    if (!query.exec()) {
        qCritical() << "query err at TemplateInfoDao::getSimpleInfoById: " << query.lastError();
        if (errCode) {
            *errCode = SQL_QUERY_ERR_CODE;
        }
        return TemplateInfoEntity();
    }
    TemplateInfoEntity entity;
    if (!query.next()) {
        return entity;
    }
    entity.setId(query.value(0).toULongLong());
    entity.setTemplateType(query.value(1).toInt());
    entity.setTemplateName(query.value(2).toString());
    entity.setTemplateComment(query.value(3).toString());
    entity.setCreateTime(query.value(4).toDateTime());
    if (errCode) {
        *errCode = 0;
    }
    return entity;
}

QList<TemplateInfoEntity> TemplateInfoDao::searchList(
    const TemplateInfoSearchDTO &param, qint32 *errCode) {
    QList<TemplateInfoEntity> list;
    SqlConnectionHolder holder = getConnectionHolder();
    sem::AfterGc gc([this]() { close(); });
    if (holder.openError()) {
        qCritical() << "sql connection open err on TemplateInfoDao::searchList: "
                    << holder.getErrMsg();
        if (errCode) {
            *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        }
        return list;
    }
    QString sql
        = "select id, top_id, template_type, template_name, template_comment, create_time from "
          "sem_template_info where deleted = 0";
    if (!param.getTemplateName().isEmpty()) {
        sql.append(" and template_name like concat('%', ?, '%')");
    }
    if (!param.getTemplateComment().isEmpty()) {
        sql.append(" and template_comment like concat('%', ?, '%')");
    }
    if (param.getTopId() != nullptr) {
        sql.append(" and top_id = ?");
    }
    if (param.getTemplateType() != nullptr) {
        sql.append(" and template_type = ?");
    }
    if (param.getInvalid() != nullptr) {
        sql.append(" and invalid = ?");
    }
    if (param.getIds().size() > 0) {
        sql.append(" and id in (");
        for (int i = 0; i < param.getIds().size(); ++i) {
            if (i > 0) {
                sql.append(",");
            }
            sql.append(" ? ");
        }
        sql.append(")");
    }
    sql.append(" order by id asc");
    SqlQueryHolder query = holder.exec(sql);
    if (!param.getTemplateName().isEmpty()) {
        query.addBindValue(param.getTemplateName());
    }
    if (!param.getTemplateComment().isEmpty()) {
        query.addBindValue(param.getTemplateComment());
    }
    if (param.getTopId() != nullptr) {
        query.addBindValue(param.getTopId().value());
    }
    if (param.getTemplateType() != nullptr) {
        query.addBindValue(param.getTemplateType().value());
    }
    if (param.getInvalid() != nullptr) {
        query.addBindValue(param.getInvalid().value());
    }

    if (param.getIds().size() > 0) {
        for (quint64 id : param.getIds()) {
            query.addBindValue(id);
        }
    }

    if (!query.exec()) {
        qCritical() << "query error at TemplateInfoDao::searchList: " << query.lastError();
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
        list.reserve(size);
    }
    while (query.next()) {
        TemplateInfoEntity entity;
        entity.setId(query.value(0).toULongLong());
        entity.setTopId(query.value(1).toULongLong());
        entity.setTemplateType(query.value(2).toInt());
        entity.setTemplateName(query.value(3).toString());
        entity.setTemplateComment(query.value(4).toString());
        entity.setCreateTime(query.value(5).toDateTime());
        list.append(entity);
    }
    return list;
}

qint32 TemplateInfoDao::deleteTemplate(
    quint64 id, qint32 *errCode) {
    SqlConnectionHolder holder = getConnectionHolder();
    sem::AfterGc gc([this]() { close(); });
    if (holder.openError()) {
        qCritical() << "sql connection open err on TemplateInfoDao::deleteClass: "
                    << holder.getErrMsg();
        if (errCode) {
            *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        }
        return 0;
    }
    QString sql = "update sem_template_info set deleted = 1 where id = ? and deleted = 0";
    SqlQueryHolder query = holder.exec(sql);
    query.bindValue(0, id);
    if (!query.exec()) {
        qCritical() << "query err at TemplateInfoDao::deleteClass: " << query.lastError();
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

qint32 TemplateInfoDao::updateNameOnly(
    quint64 id, const QString &title) {
    SqlConnectionHolder holder = getConnectionHolder();
    sem::AfterGc gc([this]() { close(); });
    if (holder.openError()) {
        qCritical() << "sql connection open err on TemplateInfoDao::updateIdpNameOnly: "
                    << holder.getErrMsg();
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    QString sql = "update sem_template_info set template_name = ? where id = ?";
    SqlQueryHolder query = holder.exec(sql);
    query.bindValue(0, title);
    query.bindValue(1, id);
    if (!query.exec()) {
        qCritical() << "update error at TemplateInfoDao::updateIdpNameOnly: " << query.lastError();
        return SQL_UPDATE_ERR_CODE;
    }
    return 0;
}

quint64 TemplateInfoDao::getIdByName(
    const QString &name, const Integer32 &type, qint32 *errCode) {
    SqlConnectionHolder holder = getConnectionHolder();
    sem::AfterGc gc([this]() { close(); });
    if (holder.openError()) {
        qCritical() << "sql connection open err on TemplateInfoDao::getIdByName: "
                    << holder.getErrMsg();
        if (errCode) {
            *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        }
        return 0;
    }
    QString sql = "select id from sem_template_info where template_name = ? ";
    if (type != nullptr) {
        sql.append(" and template_type = ? ");
    }
    sql.append("and deleted = 0 and invalid "
               "= 0 limit 1");
    SqlQueryHolder query = holder.exec(sql);
    query.bindValue(0, name);
    if (type != nullptr) {
        query.bindValue(1, type.value());
    }
    if (!query.exec()) {
        qCritical() << "query err at TemplateInfoDao::getIdByName: " << query.lastError();
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

TemplateInfoEntity TemplateInfoDao::getSimpleInfoByName(const QString &name, const Integer32 &type, qint32 *errCode) {
    SqlConnectionHolder holder = getConnectionHolder();
    sem::AfterGc gc([this]() { close(); });
    TemplateInfoEntity entity;
    if (holder.openError()) {
        qCritical() << "sql connection open err on TemplateInfoDao::getSimpleInfoByName: "
                    << holder.getErrMsg();
        if (errCode) {
            *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        }
        return entity;
    }
    QString sql = "select id, template_type, template_name, template_comment, create_time from sem_template_info where template_name = ? ";
    if (type != nullptr) {
        sql.append(" and template_type = ? ");
    }
    sql.append("and deleted = 0 and invalid "
               "= 0 limit 1");
    SqlQueryHolder query = holder.exec(sql);
    query.bindValue(0, name);
    if (type != nullptr) {
        query.bindValue(1, type.value());
    }
    if (!query.exec()) {
        qCritical() << "query err at TemplateInfoDao::getSimpleInfoByName: " << query.lastError();
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
    entity.setTemplateType(query.value(1).toInt());
    entity.setTemplateName(query.value(2).toString());
    entity.setTemplateComment(query.value(3).toString());
    entity.setCreateTime(query.value(4).toDateTime());
    return entity;
}
