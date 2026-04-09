#include "recipe_data_dao.h"
#include "exception_code.h"
#include "sem_common.h"

qint32 RecipeDataDao::checkAndSave(
    RecipeDataEntity &entity) {
    SqlConnectionHolder holder = getConnectionHolder();
    if (holder.openError()) {
        qCritical() << "sql connection open err on RecipeDataDao::checkAndSave: " << holder.getErrMsg();
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    sem::AfterGc gc([this]() { close(); });

    qint32 version = 0;
    quint64 topId = 0;
    if (entity.getId() > 0) {
        QString sql = "select top_id, class_id from sem_recipe_info where id = ?";
        SqlQueryHolder query = holder.exec(sql);
        query.bindValue(0, entity.getId());
        if (!query.exec()) {
            qCritical() << "sql query err in RecipeDataDao::checkAndSave when query top_id by id: "
                        << query.lastError();
            return SQL_QUERY_ERR_CODE;
        }
        if (!query.next()) {
            qCritical() << "sql query err in RecipeDataDao::checkAndSave when query top_id by "
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

        sql = "update sem_recipe_info set invalid = 1 where id = ? and invalid = 0 and deleted "
              "= 0";
        query = holder.exec(sql);
        query.bindValue(0, entity.getId());
        if (!query.exec()) {
            qCritical() << "sql query err in RecipeDataDao::checkAndSave when update invalid by id: "
                        << query.lastError();
            return SQL_UPDATE_ERR_CODE;
        }
        query.clear();
        sql = "select max(version) from sem_recipe_info where top_id = ? ";
        query = holder.exec(sql);
        query.bindValue(0, topId);
        if (!query.exec()) {
            qCritical() << "sql query err in RecipeDataDao::checkAndSave when check max version: "
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

qint32 RecipeDataDao::save(RecipeDataEntity & entity) {
    SqlConnectionHolder holder = getConnectionHolder();
    if (holder.openError()) {
        qCritical() << "sql connection open err on RecipeDataDao::save: " << holder.getErrMsg();
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    sem::AfterGc gc([this]() { close(); });
    QString sql = "insert into sem_recipe_info (recipe_name, class_id, idw_id, idw_top_id, idp_id, idp_top_id, detail, "
                  "version, top_id, invalid)"
                  " values (? ,? ,? ,? ,? ,? ,? ,?, ?, ?)";
    SqlQueryHolder query = holder.exec(sql);
    query.addBindValue(entity.getRecipeName());
    query.addBindValue(entity.getClassId());
    query.addBindValue(entity.getIdwId());
    query.addBindValue(entity.getIdwTopId());
    query.addBindValue(entity.getIdpId());
    query.addBindValue(entity.getIdpTopId());
    query.addBindValue(entity.getDetail());
    query.addBindValue(entity.getVersion());
    query.addBindValue(entity.getTopId());
    query.addBindValue(entity.getInvalid());
    if (!query.exec()) {
        qCritical() << "sql query err in RecipeDataDao::save when save: " << query.lastError();
        return SQL_UPDATE_ERR_CODE;
    }
    quint64 id = query.lastInsertId().toULongLong();
    entity.setId(id);
    return 0;
}

qint32 RecipeDataDao::updateTopId(quint64 id, quint64 topId, int* errCode) {
    SqlConnectionHolder holder = getConnectionHolder();
    if (holder.openError()) {
        qCritical() << "sql connection open err on RecipeDataDao::save: " << holder.getErrMsg();
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    sem::AfterGc gc([this]() { close(); });
    SqlQueryHolder query = holder.exec("update sem_recipe_info set top_id = ? where id = ?");
    query.addBindValue(topId);
    query.addBindValue(id);
    if (!query.exec()) {
        qCritical() << "sql query err in RecipeDataDao::save when update top_id: "
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

qint32 RecipeDataDao::saveWithTransaction(
    RecipeDataEntity &entity) {
    bool hasTran = hasTransaction();
    bool succ = beginTransaction(true);
    if (!succ) {
        qCritical() << "Open transaction fail when RecipeDataDao::save";
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

RecipeDataEntity RecipeDataDao::get(
    quint64 id, qint32 *errCode) {
    SqlConnectionHolder holder = getConnectionHolder();
    sem::AfterGc gc([this]() { close(); });
    if (holder.openError()) {
        qCritical() << "sql connection open err on RecipeDataDao::get: " << holder.getErrMsg();
        if (errCode) {
            *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        }
        return RecipeDataEntity();
    }
    QString sql = "select * from sem_recipe_info where id = ? and deleted = 0";
    SqlQueryHolder query = holder.exec(sql);
    query.bindValue(0, id);
    if (!query.exec()) {
        qCritical() << "query err at RecipeDataDao::get: " << query.lastError();
        if (errCode) {
            *errCode = SQL_QUERY_ERR_CODE;
        }
        return RecipeDataEntity();
    }
    RecipeDataEntity entity;
    if (!query.next()) {
        return entity;
    }
    entity.setId(query.value(0).toULongLong());
    entity.setRecipeName(query.value(1).toString());
    entity.setClassId(query.value(2).toULongLong());
    entity.setIdwId(query.value(3).toULongLong());
    entity.setIdwTopId(query.value(4).toULongLong());
    entity.setIdpId(query.value(5).toULongLong());
    entity.setIdpTopId(query.value(6).toULongLong());
    entity.setDetail(query.value(7).toString());

    entity.setVersion(query.value(8).toInt());
    entity.setTopId(query.value(9).toULongLong());
    entity.setInvalid(query.value(10).toInt());
    entity.setDeleted(query.value(11).toBool());
    entity.setCreateTime(query.value(12).toDateTime());
    entity.setEditTime(query.value(13).toDateTime());
    if (errCode) {
        *errCode = 0;
    }
    return entity;
}

RecipeDataEntity RecipeDataDao::getSimpleInfo(
    quint64 id, qint32 *errCode) {
    SqlConnectionHolder holder = getConnectionHolder();
    sem::AfterGc gc([this]() { close(); });
    if (holder.openError()) {
        qCritical() << "sql connection open err on RecipeDataDao::get: " << holder.getErrMsg();
        if (errCode) {
            *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        }
        return RecipeDataEntity();
    }
    QString sql
        = "select id, recipe_name, class_id, idw_id, idw_top_id, idp_id, idp_top_id, version, top_id, invalid, create_time, edit_time from sem_recipe_info "
          "where id = ? and deleted = 0";
    SqlQueryHolder query = holder.exec(sql);
    query.bindValue(0, id);
    if (!query.exec()) {
        qCritical() << "query err at RecipeDataDao::get: " << query.lastError();
        if (errCode) {
            *errCode = SQL_QUERY_ERR_CODE;
        }
        return RecipeDataEntity();
    }
    RecipeDataEntity entity;
    if (!query.next()) {
        return entity;
    }
    entity.setId(query.value(0).toULongLong());
    entity.setRecipeName(query.value(1).toString());
    entity.setClassId(query.value(2).toULongLong());
    entity.setIdwId(query.value(3).toULongLong());
    entity.setIdwTopId(query.value(4).toULongLong());
    entity.setIdpId(query.value(5).toULongLong());
    entity.setIdpTopId(query.value(6).toULongLong());
    entity.setVersion(query.value(7).toInt());
    entity.setTopId(query.value(8).toULongLong());
    entity.setInvalid(query.value(9).toInt());
    entity.setCreateTime(query.value(10).toDateTime());
    entity.setEditTime(query.value(11).toDateTime());
    if (errCode) {
        *errCode = 0;
    }
    return entity;
}

quint64 RecipeDataDao::getRecipeIdByName(quint64 classId, const QString& name, qint32* errCode) {
    SqlConnectionHolder holder = getConnectionHolder();
    sem::AfterGc gc([this]() { close(); });
    if (holder.openError()) {
        qCritical() << "sql connection open err on RecipeDataDao::getRecipeIdByName: " << holder.getErrMsg();
        if (errCode) {
            *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        }
        return 0;
    }
    QString sql = "select id from sem_recipe_info where class_id = ? and recipe_name = ? and deleted = 0 and invalid = 0 limit 1";
    SqlQueryHolder query = holder.exec(sql);
    query.addBindValue(classId);
    query.addBindValue(name);
    if (!query.exec()) {
        qCritical() << "query err at RecipeDataDao::getRecipeIdByName: " << query.lastError();
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

RecipeDataEntity RecipeDataDao::getSimpleRecipeByName(quint64 classId, const QString& name, qint32* errCode) {
    SqlConnectionHolder holder = getConnectionHolder();
    sem::AfterGc gc([this]() { close(); });
    RecipeDataEntity entity;
    if (holder.openError()) {
        qCritical() << "sql connection open err on RecipeDataDao::getSimpleRecipeByName: " << holder.getErrMsg();
        if (errCode) {
            *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        }
        return entity;
    }
    QString sql = "select id, recipe_name, class_id, idw_id, idw_top_id, idp_id, idp_top_id, version, top_id, invalid, create_time, edit_time from sem_recipe_info where class_id = ? and recipe_name = ? and deleted = 0 and invalid = 0 limit 1";
    SqlQueryHolder query = holder.exec(sql);
    query.addBindValue(classId);
    query.addBindValue(name);
    if (!query.exec()) {
        qCritical() << "query err at RecipeDataDao::getSimpleRecipeByName: " << query.lastError();
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
    entity.setRecipeName(query.value(1).toString());
    entity.setClassId(query.value(2).toULongLong());
    entity.setIdwId(query.value(3).toULongLong());
    entity.setIdwTopId(query.value(4).toULongLong());
    entity.setIdpId(query.value(5).toULongLong());
    entity.setIdpTopId(query.value(6).toULongLong());
    entity.setVersion(query.value(7).toInt());
    entity.setTopId(query.value(8).toULongLong());
    entity.setInvalid(query.value(9).toInt());
    entity.setCreateTime(query.value(10).toDateTime());
    entity.setEditTime(query.value(11).toDateTime());
    return entity;
}

QList<RecipeDataEntity> RecipeDataDao::searchList(
    const RecipeSearchDTO &param, qint32 *errCode) {
    QList<RecipeDataEntity> list;
    SqlConnectionHolder holder = getConnectionHolder();
    sem::AfterGc gc([this]() { close(); });
    if (holder.openError()) {
        qCritical() << "sql connection open err on RecipeDataDao::searchList: "
                    << holder.getErrMsg();
        if (errCode) {
            *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        }
        return list;
    }
    QString sql = "select id, recipe_name, class_id, idw_id, idw_top_id, idp_id, idp_top_id, version, top_id, invalid, create_time, edit_time from "
                  "sem_recipe_info where deleted = 0";
    if (!param.getRecipeName().isEmpty()) {
        sql.append(" and recipe_name like concat('%', ?, '%')");
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
    if (param.getIdpId() != nullptr) {
        sql.append(" and idp_id = ?");
    }
    sql.append(" order by id asc");
    SqlQueryHolder query = holder.exec(sql);
    if (!param.getRecipeName().isEmpty()) {
        query.addBindValue(param.getRecipeName());
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
    if (param.getIdpId() != nullptr) {
        query.addBindValue(param.getIdpId().value());
    }
    if (!query.exec()) {
        qCritical() << "query error at RecipeDataDao::searchList: " << query.lastError();
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
    RecipeDataEntity entity;
    while (query.next()) {
        entity.setId(query.value(0).toULongLong());
        entity.setRecipeName(query.value(1).toString());
        entity.setClassId(query.value(2).toULongLong());
        entity.setIdwId(query.value(3).toULongLong());
        entity.setIdwTopId(query.value(4).toULongLong());
        entity.setIdpId(query.value(5).toULongLong());
        entity.setIdpTopId(query.value(6).toULongLong());
        entity.setVersion(query.value(7).toInt());
        entity.setTopId(query.value(8).toULongLong());
        entity.setInvalid(query.value(9).toInt());
        entity.setCreateTime(query.value(10).toDateTime());
        entity.setEditTime(query.value(11).toDateTime());
        if (index >= list.size()) {
            list.append(entity);
        } else {
            list.replace(index, entity);
        }
        ++index;
    }
    return list;
}

qint32 RecipeDataDao::updateRecipeNameOnly(
    quint64 id, const QString &title) {
    SqlConnectionHolder holder = getConnectionHolder();
    sem::AfterGc gc([this]() { close(); });
    if (holder.openError()) {
        qCritical() << "sql connection open err on RecipeDataDao::updateRecipeNameOnly: "
                    << holder.getErrMsg();
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    QString sql = "update sem_recipe_info set recipe_name = ? where id = ?";
    SqlQueryHolder query = holder.exec(sql);
    query.bindValue(0, title);
    query.bindValue(1, id);
    if (!query.exec()) {
        qCritical() << "update error at RecipeDataDao::updateRecipeNameOnly: " << query.lastError();
        return SQL_UPDATE_ERR_CODE;
    }
    return 0;
}

qint32 RecipeDataDao::deleteRecipeById(
    quint64 id) {
    SqlConnectionHolder holder = getConnectionHolder();
    sem::AfterGc gc([this]() { close(); });
    if (holder.openError()) {
        qCritical() << "sql connection open err on RecipeDataDao::deleteRecipeById: "
                    << holder.getErrMsg();
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    QString sql = "update sem_recipe_info set deleted = 1 where id = ?";
    SqlQueryHolder query = holder.exec(sql);
    query.bindValue(0, id);
    if (!query.exec()) {
        qCritical() << "update error at RecipeDataDao::deleteRecipeById: " << query.lastError();
        return SQL_UPDATE_ERR_CODE;
    }
    return 0;
}

qint32 RecipeDataDao::deleteRecipeByClass(
    quint64 classId) {
    SqlConnectionHolder holder = getConnectionHolder();
    sem::AfterGc gc([this]() { close(); });
    if (holder.openError()) {
        qCritical() << "sql connection open err on RecipeDataDao::deleteRecipeByClass: "
                    << holder.getErrMsg();
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    QString sql = "update sem_recipe_info set deleted = 1 where class_id = ?";
    SqlQueryHolder query = holder.exec(sql);
    query.bindValue(0, classId);
    if (!query.exec()) {
        qCritical() << "update error at RecipeDataDao::deleteRecipeByClass: " << query.lastError();
        return SQL_UPDATE_ERR_CODE;
    }
    return 0;
}
