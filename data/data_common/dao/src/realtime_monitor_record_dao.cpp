//
// Created by Shisan on 2026/4/2.
//

#include "realtime_monitor_record_dao.h"
#include "global_connection_factory.h"
#include "cas_lock.h"

inline QString allColumns() {
    return "id, group_id, data_index, data_value, data_split, create_time";
}

void copyColumnToEntity(SqlQueryHolder& query, QList<RealTimeMonitorRecordEntity>& list) {
    if (query.numRowsAffected() > 0) {
        list.reserve(query.numRowsAffected());
    }
    RealTimeMonitorRecordEntity entity;
    while (query.next()) {
        entity.setId(query.value(0).toULongLong());
        entity.setGroupId(query.value(1).toString());
        entity.setDataIndex(query.value(2).toULongLong());
        entity.setDataValue(query.value(3).toString());
        entity.setDataSplit(query.value(4).toBool());
        entity.setCreateTime(query.value(5).toDateTime());
        list.append(entity);
    }
}

struct RealTimeMonitorRecordDaoDataHelper {
    CasLock lock;
    RealTimeMonitorRecordDao* d;
    quint64 idpId = 0;

    int appendData(SqlConnectionHolder& connection, const QString& groupId, const FloatNumber2& value, quint64 index, bool newData) {
        QString sql = "insert into sem_realtime_monitor_record (group_id, data_index, data_value, data_split) values(?, ?, ?, ?)";
        SqlQueryHolder query = connection.exec(sql);
        query.addBindValue(groupId);
        query.addBindValue(value.toMinString());
        query.addBindValue(index);
        query.addBindValue(newData ? 1 : 0);
        if (!query.exec()) {
            qCritical() << "RealTimeMonitorRecordDao::appendData eror: " << query.lastError();
            return SQL_UPDATE_ERR_CODE;
        }
        return 0;
    }

    int clearAllData(SqlConnectionHolder& connection) {
        QString sql = "TRUNCATE TABLE sem_realtime_monitor_record";
        SqlQueryHolder query = connection.exec(sql);
        if (!query.exec()) {
            qCritical() << "RealTimeMonitorRecordDao::clearAllData error: " << query.lastError().text();
            return SQL_UPDATE_ERR_CODE;
        }
        return 0;
    }

    int initIdpId(quint64 currId, SqlConnectionHolder& connection) {
        CasLocker locker(lock);
        bool needInitCurrId = false;
        if (idpId == 0) {
            QString sql = "select group_id from sem_realtime_monitor_record order by id asc limit 1";
            SqlQueryHolder query = connection.exec(sql);
            if (!query.exec()) {
                qCritical() << "RealTimeMonitorRecordDao::initIdpId error: " << query.lastError();
                return SQL_QUERY_ERR_CODE;
            }
            if (query.next()) {
                bool ok;
                idpId = query.value(0).toULongLong(&ok);
                if (!ok) {
                    qCritical() << "RealTimeMonitorRecordDao::initIdpId error: IDP ID is invalid, all data will be clear";
                    int errCode = clearAllData(connection);
                    if (errCode) return errCode;
                    needInitCurrId = true;
                }
            } else {
                needInitCurrId = true;
            }
        }
        if (idpId > 0 && idpId != currId) {
            int errCode = clearAllData(connection);
            if (errCode) return errCode;
            needInitCurrId = true;
        }
        if (needInitCurrId) {
            QString sql = "insert into sem_realtime_monitor_record (group_id, data_index, data_value, data_split) values (?, 0, '', 0)";
            SqlQueryHolder query = connection.exec(sql);
            query.addBindValue(QString::number(currId));
            if (!query.exec()) {
                qCritical() << "RealTimeMonitorRecordDao::initIdpId error: " << query.lastError();
                return SQL_UPDATE_ERR_CODE;
            }
        }
        return 0;
    }
};

RealTimeMonitorRecordDao::RealTimeMonitorRecordDao() : helper(new RealTimeMonitorRecordDaoDataHelper) {
    helper->d = this;
}

RealTimeMonitorRecordDao::~RealTimeMonitorRecordDao() {}

QSharedPointer<SqlConnectionFactory> RealTimeMonitorRecordDao::getConnectionFactory() {
    return GlobalConnectionFactory::instance()->getFileSqlConnectionFactory();
}

int RealTimeMonitorRecordDao::appendDataValue(quint64 idpId, const QString& groupId, const FloatNumber2& value, bool firstData) {
    SqlConnectionHolder holder = getConnectionHolder();
    if (holder.openError()) {
        qCritical() << "sql connection open err on RealTimeMonitorRecordDao::appendDataValue: "
                    << holder.getErrMsg();
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    sem::AfterGc gc([this]() { close(); });
    int errCode = helper->initIdpId(idpId, holder);
    if (errCode) return errCode;
    CasLocker locker(helper->lock);
    QString sql = "select data_index from sem_realtime_monitor_record where group_id = ? and (id > select min(id) from sem_realtime_monitor_record) order by data_index desc limit 1";
    SqlQueryHolder query = holder.exec(sql);
    query.addBindValue(groupId);
    if (!query.exec()) {
        qCritical() << "RealTimeMonitorRecordDao::appendDataValue query last index error: " << query.lastError();
        return SQL_QUERY_ERR_CODE;
    }
    quint64 index;
    if (query.next()) index = query.value(0).toULongLong();
    else index = -1;
    ++ index;
    return helper->appendData(holder, groupId, value, index, firstData);
}

int RealTimeMonitorRecordDao::appendDataValues(quint64 idpId, const QList<std::pair<QString, FloatNumber2>>& values, bool firstData) {
    if (values.isEmpty()) return 0;
    SqlConnectionHolder holder = getConnectionHolder();
    if (holder.openError()) {
        qCritical() << "sql connection open err on RealTimeMonitorRecordDao::appendDataValues: "
                    << holder.getErrMsg();
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    bool succ = false;
    bool hasTran = false;
    sem::AfterGc gc([this, &succ, hasTran]() {
        if (!succ && !hasTran && !rollback()) {
            qCritical() << "RealTimeMonitorRecordDao::appendDataValues rollback transaction error";
        }
        close();
    });
    hasTran = hasTransaction();
    succ = beginTransaction(true);
    if (!succ) {
        qCritical() << "sql connection open err on RealTimeMonitorRecordDao::appendDataValue: open transaction error";
        return SQL_OPEN_TRAN_ERROR_CODE;
    }
    succ = false;
    int errCode = helper->initIdpId(idpId, holder);
    if (errCode) return errCode;
    CasLocker locker(helper->lock);
    QHash<QString, quint64> groupIndices;
    {
        QString sql = "select max(data_index), group_id from sem_realtime_monitor_record where group_id in (%1) and (id > select min(id) from sem_realtime_monitor_record)  group by group_id";
        QString groups;
        groups.reserve(values.size()* 2 - 1);
        for (int i = 0; i < values.size(); ++i) {
            if (i > 0) groups.append(",");
            groups.append("?");
        }
        sql = sql.arg(groups);
        SqlQueryHolder query = holder.exec(sql);
        for (auto &pair : values) {
            query.addBindValue(pair.first);
        }
        if (!query.exec()) {
            qCritical() << "RealTimeMonitorRecordDao::appendDataValues query last index error: " << query.lastError();
            return SQL_QUERY_ERR_CODE;
        }
        while (query.next()) {
            groupIndices.insert(query.value(1).toString(), query.value(0).toULongLong());
        }
    }
    QString sql = "insert into sem_realtime_monitor_record (group_id, data_index, data_value, data_split) values ";
    for (int i = 0; i < values.size(); ++i) {
        sql.append("(?, ?, ?, ?)");
    }
    SqlQueryHolder query = holder.exec(sql);
    QSet<QString> firstCheck;
    bool split;
    quint64 index;
    for (auto& pair : values) {
        if (firstData) split = !firstCheck.contains(pair.first);
        else split = false;
        index = groupIndices.value(pair.first, -1) + 1;
        query.addBindValue(pair.first);
        query.addBindValue(index);
        query.addBindValue(pair.second.toMinString());
        query.addBindValue(split ? 1 : 0);
        if (firstData) firstCheck.insert(pair.first);
    }
    if (!query.exec()) {
        qCritical() << "RealTimeMonitorRecordDao::appendDataValues error: " << query.lastError();
        return SQL_QUERY_ERR_CODE;
    }
    if (!hasTran && !commit()) {
        qCritical() << "RealTimeMonitorRecordDao::appendDataValues commit transaction error";
        return SQL_COMMIT_ERROR_CODE;
    }
    succ = true;
    return 0;
}

int RealTimeMonitorRecordDao::clearAllData() {
    SqlConnectionHolder holder = getConnectionHolder();
    if (holder.openError()) {
        qCritical() << "sql connection open err on RealTimeMonitorRecordDao::clearAllData: "
                    << holder.getErrMsg();
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    sem::AfterGc gc([this]() { close(); });
    helper->clearAllData(holder);
    return 0;
}

QList<RealTimeMonitorRecordEntity> RealTimeMonitorRecordDao::searchData(const QString& groupId, quint64 startIndex, int limit, int* errCode) {
    SqlConnectionHolder holder = getConnectionHolder();
    QList<RealTimeMonitorRecordEntity> list;
    if (holder.openError()) {
        qCritical() << "sql connection open err on RealTimeMonitorRecordDao::appendDataValue: "
                    << holder.getErrMsg();
        if (errCode) *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        return list;
    }
    sem::AfterGc gc([this]() { close(); });
    QString sql = "select %1 from sem_realtime_monitor_record where group_id = ? and data_index >= ? and (id > select min(id) from sem_realtime_monitor_record)  order by data_index asc limit ?";
    sql = sql.arg(allColumns());
    SqlQueryHolder query = holder.exec(sql);
    query.addBindValue(groupId);
    query.addBindValue(startIndex);
    query.addBindValue(limit);
    if (!query.exec()) {
        qCritical() << "RealTimeMonitorRecordDao::searchData error: " << query.lastError();
        if (errCode) *errCode = SQL_QUERY_ERR_CODE;
        return list;
    }
    if (errCode) *errCode = 0;
    copyColumnToEntity(query, list);
    return list;
}

QList<RealTimeMonitorRecordEntity> RealTimeMonitorRecordDao::searchData(const QString& groupId, const QList<quint64>& indices, int* errCode) {
    QList<RealTimeMonitorRecordEntity> list;
    if (indices.isEmpty()) {
        if (errCode) *errCode = 0;
        return list;
    }
    SqlConnectionHolder holder = getConnectionHolder();
    if (holder.openError()) {
        qCritical() << "sql connection open err on RealTimeMonitorRecordDao::appendDataValue: "
                    << holder.getErrMsg();
        if (errCode) *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        return list;
    }
    sem::AfterGc gc([this]() { close(); });
    QString sql = "select %1 from sem_realtime_monitor_record where group_id = ? and data_index in (%2) and (id > select min(id) from sem_realtime_monitor_record)  order by data_index asc";
    QString params;
    params.reserve(indices.size() * 2 - 1);
    for (int i = 0; i < indices.size(); ++ i) {
        if (i > 0) params.append(",");
        params.append("?");
    }
    sql = sql.arg(allColumns(), params);
    SqlQueryHolder query = holder.exec(sql);
    query.addBindValue(groupId);
    for (auto& i : indices) {
        query.addBindValue(i);
    }
    if (!query.exec()) {
        qCritical() << "RealTimeMonitorRecordDao::searchData error: " << query.lastError();
        if (errCode) *errCode = SQL_QUERY_ERR_CODE;
        return list;
    }
    if (errCode) *errCode = 0;
    copyColumnToEntity(query, list);
    return list;
}