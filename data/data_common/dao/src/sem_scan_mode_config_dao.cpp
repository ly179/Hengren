#include "sem_scan_mode_config_dao.h"
#include "exception_code.h"
#include "sem_common.h"

qint32 SemScanModeConfigDao::save(
    QList<ScanModeEntity>& list) {
    SqlConnectionHolder holder = getConnectionHolder();
    if (holder.openError()) {
        qCritical() << "sql connection open err on SemScanModeConfigDao::save: "
                    << holder.getErrMsg();
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    sem::AfterGc gc([&holder]() mutable { holder.close(); });
    if (list.size() > 0) {
        for (qint32 i = 0; i < list.size(); ++ i) {
            ScanModeEntity& entity = list[i];
            if (entity.getId() <= 0) {
                continue;
            } else {
                QString sql = "update sem_scan_mode_config set ";
                if (entity.getDetail().length() > 0) {
                    sql.append("detail = ? ");
                }
                sql.append("where id = ? and deleted = 0");
                SqlQueryHolder query = holder.exec(sql);
                if (entity.getDetail().length() > 0) {
                    query.addBindValue(entity.getDetail());
                }
                query.addBindValue(entity.getId());
                if (!query.exec()) {
                    qCritical() << "update err at SemScanModeConfigDao::save: "
                                << query.lastError();
                    return SQL_UPDATE_ERR_CODE;
                }
            }
        }
    }
    return 0;
}

qint32 SemScanModeConfigDao::saveWithTransaction(
    QList<ScanModeEntity>& list) {
    bool hasTran = hasTransaction();
    bool succ = beginTransaction(true);
    if (!succ) {
        qCritical() << "Open transaction fail when SemScanModeConfigDao::save";
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    qint32 errCode = save(list);
    if (!hasTran) {
        if (errCode) {
            rollback();
        } else {
            commit();
        }
    }
    return errCode;
}

QList<ScanModeEntity> SemScanModeConfigDao::get(
    qint32* errCode) {
    QList<ScanModeEntity> list;
    SqlConnectionHolder holder = getConnectionHolder();
    if (holder.openError()) {
        qCritical() << "sql connection open err on SemScanModeConfigDao::get: "
                    << holder.getErrMsg();
        if (errCode) {
            *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        }
        return list;
    }
    sem::AfterGc gc([this]() mutable { close(); });

    QString sql = "select id, detail, create_time, edit_time, deleted from sem_scan_mode_config where deleted = 0 order by id asc";
    SqlQueryHolder query = holder.exec(sql);
    if (!query.exec()) {
        qCritical() << "query err at SemScanModeConfigDao::get: " << query.lastError();
        if (errCode) {
            *errCode = SQL_QUERY_ERR_CODE;
        }
        return list;
    }
    while (query.next()) {
        ScanModeEntity entity;
        entity.setId(query.value(0).toULongLong());
        entity.setDetail(query.value(1).toString());
        entity.setCreateTime(query.value(2).toDateTime());
        entity.setEditTime(query.value(3).toDateTime());
        entity.setDeleted(char(query.value(4).toInt()));
        list.append(entity);
    }
    if (errCode) {
        *errCode = 0;
    }
    return list;
}
