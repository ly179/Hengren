#include "auth_dao.h"
#include "global_connection_factory.h"
#include "exception_code.h"
#include "sem_common.h"

int copyAuthDataToEntity(
    AuthEntity& entity, SqlQueryHolder& query, int start = 0) {
    entity.setId(query.value(start + 0).toULongLong());
    entity.setAuthType(query.value(start + 1).toInt());
    entity.setInvalid(query.value(start + 2).toInt());
    entity.setAuthName(query.value(start + 3).toString());
    entity.setAuthKey(query.value(start + 4).toString());
    entity.setAuthDescription(query.value(start + 5).toString());
    entity.setReadable(query.value(start + 6).toInt());
    entity.setWritable(query.value(start + 7).toInt());
    entity.setExecutable(query.value(start + 8).toInt());
    entity.setCreateTime(query.value(start + 9).toDateTime());
    entity.setEditTime(query.value(start + 10).toDateTime());
    entity.setDeleted(query.value(start + 11).toInt());
    return start + 11;
}

extern int copyRoleAuthDataToEntity(RoleAuthEntity& entity, SqlQueryHolder& query, int start = 0);

QSharedPointer<SqlConnectionFactory> AuthDao::getConnectionFactory() {
    return GlobalConnectionFactory::instance()->getAuthSqlConnectionFactory();
}

QList<AuthEntity> AuthDao::getByIds(
    const QList<quint64>& ids, qint32* errCode) {
    if (ids.isEmpty()) {
        if (errCode) {
            *errCode = 0;
        }
        return QList<AuthEntity>();
    }
    SqlConnectionHolder holder = getConnectionHolder();
    if (holder.openError()) {
        qCritical() << "sql connection open err on AuthDao::getByIds: " << holder.getErrMsg();
        if (errCode) {
            *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        }
        return QList<AuthEntity>();
    }
    sem::AfterGc gc([this]() { close(); });
    QString sql = "select * from cdsem_auth where deleted = 0 and id in (";
    for (int i = 0; i < ids.size(); ++i) {
        if (i > 0) {
            sql.append(",");
        }
        sql.append("?");
    }
    sql.append(")");
    SqlQueryHolder query = holder.exec(sql);
    for (const quint64& id : ids) {
        query.addBindValue(id);
    }
    if (!query.exec()) {
        qCritical() << "sql query err in AuthDao::getByIds: " << query.lastError();
        if (errCode) {
            *errCode = SQL_QUERY_ERR_CODE;
        }
        return QList<AuthEntity>();
    }
    if (errCode) {
        *errCode = 0;
    }
    QList<AuthEntity> list;
    if (query.size() > 0) {
        list.reserve(query.size());
    }
    AuthEntity entity;
    while (query.next()) {
        copyAuthDataToEntity(entity, query);
        list.append(entity);
    }
    return list;
}

RoleAuthResultDTO AuthDao::authList(
    const SearchAuthDTO& param, qint32* errCode) {
    SqlConnectionHolder holder = getConnectionHolder();
    if (holder.openError()) {
        qCritical() << "sql connection open err on AuthDao::authList: " << holder.getErrMsg();
        if (errCode) {
            *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        }
        return {};
    }
    sem::AfterGc gc([this]() { close(); });
    QString sql = "select a.*";
    if (param.roleId > 0) {
        sql.append(", r.*");
    }
    sql.append(" from cdsem_auth a");
    if (param.roleId > 0) {
        sql.append(" join cdsem_role_auth r on r.auth_id = a.id");
    }
    sql.append(" where a.deleted = 0");
    if (param.roleId > 0) {
        sql.append(" and r.deleted = 0 and r.invalid = 0 and r.role_id = ?");
    }
    if (param.authName.length() > 0) {
        sql.append(" and a.auth_name like concat('%', ?, '%')");
    }
    SqlQueryHolder query = holder.exec(sql);
    if (param.roleId > 0) {
        query.addBindValue(param.roleId);
    }
    if (param.authName.length() > 0) {
        query.addBindValue(param.authName);
    }
    if (!query.exec()) {
        qCritical() << "sql query err in AuthDao::authList: " << query.lastError();
        if (errCode) {
            *errCode = SQL_QUERY_ERR_CODE;
        }
        return {};
    }
    if (errCode) {
        *errCode = 0;
    }
    RoleAuthResultDTO result;
    AuthEntity entity;
    QSet<quint64> repeatChecker;
    while (query.next()) {
        if (!repeatChecker.contains(query.value(0).toULongLong())) {
            copyAuthDataToEntity(entity, query);
            result.auths.append(entity);
            repeatChecker.insert(entity.getId());
        }
        if (param.roleId > 0) {
            RoleAuthEntity roleAuthEntity;
            copyRoleAuthDataToEntity(roleAuthEntity, query, 12);
            result.roleAuths.append(roleAuthEntity);
        }
    }
    return result;
}
