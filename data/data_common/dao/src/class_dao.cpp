#include "class_dao.h"
#include "sem_common.h"
#include "exception_code.h"

qint32 ClassDao::create(
    ClassEntity& entity) {
    SqlConnectionHolder holder = getConnectionHolder();
    sem::AfterGc gc([this](){
        close();
    });
    if (holder.openError()) {
        qCritical() << "sql connection open err on ClassDao::create: " << holder.getErrMsg();
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    QString sql = "insert into sem_class_info (class_name) values (?)";
    SqlQueryHolder query = holder.exec(sql);
    query.bindValue(0, entity.getClassName());
    if (!query.exec()) {
        qCritical() << "query err at ClassDao::create: " << query.lastError();
        return SQL_UPDATE_ERR_CODE;
    }
    entity.setId(query.lastInsertId().toULongLong());
    return 0;
}

qint32 ClassDao::update(
    const ClassEntity& entity, qint32* errCode) {
    SqlConnectionHolder holder = getConnectionHolder();
    sem::AfterGc gc([this](){
        close();
    });
    if (holder.openError()) {
        qCritical() << "sql connection open err on ClassDao::update: " << holder.getErrMsg();
        if (errCode) {
            *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        }
        return 0;
    }
    QString sql = "update sem_class_info set class_name = ? where id = ? and deleted = 0";
    SqlQueryHolder query = holder.exec(sql);
    query.bindValue(0, entity.getClassName());
    query.bindValue(1, entity.getId());
    if (!query.exec()) {
        qCritical() << "query err at ClassDao::update: " << query.lastError();
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

qint32 ClassDao::deleteClass(
    quint64 id, qint32* errCode) {
    SqlConnectionHolder holder = getConnectionHolder();
    sem::AfterGc gc([this](){
        close();
    });
    if (holder.openError()) {
        qCritical() << "sql connection open err on ClassDao::deleteClass: " << holder.getErrMsg();
        if (errCode) {
            *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        }
        return 0;
    }
    QString sql = "update sem_class_info set deleted = 1 where id = ? and deleted = 0";
    SqlQueryHolder query = holder.exec(sql);
    query.bindValue(0, id);
    if (!query.exec()) {
        qCritical() << "query err at ClassDao::deleteClass: " << query.lastError();
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

ClassEntity ClassDao::getById(
    quint64 id, qint32* errCode) {
    ClassEntity entity;
    SqlConnectionHolder holder = getConnectionHolder();
    sem::AfterGc gc([this](){
        close();
    });
    if (holder.openError()) {
        qCritical() << "sql connection open err on ClassDao::getById: " << holder.getErrMsg();
        if (errCode) {
            *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        }
        return entity;
    }
    QString sql = "select id, class_name from sem_class_info where id = ? and deleted = 0";
    SqlQueryHolder query = holder.exec(sql);
    query.bindValue(0, id);
    if (!query.exec()) {
        qCritical() << "query err at ClassDao::getById: " << query.lastError();
        if (errCode) {
            *errCode = SQL_QUERY_ERR_CODE;
        }
        return entity;
    }

    if (query.next()) {
        if (errCode) {
            *errCode = 0;
        }
        entity.setId(query.value(0).toULongLong());
        entity.setClassName(query.value(1).toString());
    } else {
        if (errCode) {
            *errCode = SQL_QUERY_DATA_NOT_EXISTS;
        }
    }
    return entity;
}

QList<ClassEntity> ClassDao::searchList(
    const ClassSearchDTO& param, qint32* errCode) {
    SqlConnectionHolder holder = getConnectionHolder();
    if (holder.openError()) {
        qCritical() << "sql connection open err on ClassDao::searchList: " << holder.getErrMsg();
        if (errCode) {
            *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        }
        return QList<ClassEntity>(0);
    }
    sem::AfterGc gc([this](){
        close();
    });
    QString sql = "select id, class_name from sem_class_info where deleted = 0";
    if (param.getClassName().length() > 0) {
        sql.append(" and class_name like concat('%', ?, '%')");
    }
    sql.append(" order by id asc");
    SqlQueryHolder query = holder.exec(sql);
    if (param.getClassName().length() > 0) {
        query.bindValue(0, param.getClassName());
    }
    if (!query.exec()) {
        qCritical() << "query err at ClassDao::searchList: " << query.lastError();
        if (errCode) {
            *errCode = SQL_QUERY_ERR_CODE;
        }
        return QList<ClassEntity>(0);
    }
    if (errCode) {
        *errCode = 0;
    }
    QList<ClassEntity> list;
    while (query.next()) {
        ClassEntity entity;
        entity.setId(query.value(0).toULongLong());
        entity.setClassName(query.value(1).toString());
        list.append(entity);
    }
    return list;
}

quint64 ClassDao::getByName(const QString& name, int* errCode) {
    SqlConnectionHolder holder = getConnectionHolder();
    if (holder.openError()) {
        qCritical() << "sql connection open err on ClassDao::getByName: " << holder.getErrMsg();
        if (errCode) {
            *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        }
        return 0;
    }
    sem::AfterGc gc([this](){
        close();
    });
    QString sql = "select id from sem_class_info where deleted = 0 and class_name = ? limit 1";
    SqlQueryHolder query = holder.exec(sql);
    query.addBindValue(name);
    if (!query.exec()) {
        qCritical() << "query err at ClassDao::searchList: " << query.lastError();
        if (errCode) {
            *errCode = SQL_QUERY_ERR_CODE;
        }
        return 0;
    }
    if (errCode) {
        *errCode = 0;
    }
    if (query.next()) {
        return query.value(0).toULongLong();
    }
    return 0;
}
