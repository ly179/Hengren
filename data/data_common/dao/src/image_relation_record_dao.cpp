#include "image_relation_record_dao.h"
#include "global_connection_factory.h"
#include "exception_code.h"
#include "sem_common.h"

QSharedPointer<SqlConnectionFactory> ImageRelationRecordDao::getConnectionFactory() {
    return GlobalConnectionFactory::instance()->getFileSqlConnectionFactory();
}

qint32 ImageRelationRecordDao::save(const QList<ImageRelationRecordEntity>& list) {
    if (list.isEmpty()) return 0;
    SqlConnectionHolder holder = getConnectionHolder();
    if (holder.openError()) {
        qCritical() << "sql connection open err on ImageRelationRecordDao::save: " << holder.getErrMsg();
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    sem::AfterGc gc([this]() { close(); });
    QString sql = "insert into sem_image_relation_record (image_id, data_id, data_type) values ";
    for (int i = 0; i < list.size(); ++ i) {
        if (i > 0) sql.append(",");
        sql.append("(?, ?, ?)");
    }
    SqlQueryHolder query = holder.exec(sql);
    for (const ImageRelationRecordEntity& entity : list) {
        query.addBindValue(entity.getImageId());
        query.addBindValue(entity.getDataId());
        query.addBindValue(entity.getDataType());
    }
    if (!query.exec()) {
        qCritical() << "sql query err in ImageRelationRecordDao::save when save: " << query.lastError();
        return SQL_UPDATE_ERR_CODE;
    }
    return 0;
}

QList<ImageRelationRecordEntity> ImageRelationRecordDao::getByDataId(quint64 dataId, ImageRelationDataType dataType, qint32 *errCode) {
    QList<ImageRelationRecordEntity> list;
    SqlConnectionHolder holder = getConnectionHolder();
    if (holder.openError()) {
        qCritical() << "sql connection open err on ImageRelationRecordDao::getByDataId: " << holder.getErrMsg();
        if (errCode) *errCode = SQL_CONNECTION_OPEN_ERR_CODE;
        return list;
    }
    sem::AfterGc gc([this]() { close(); });
    QString sql = "select * from sem_image_relation_record where data_id = ? and data_type = ? and deleted = 0";
    SqlQueryHolder query = holder.exec(sql);
    query.addBindValue(dataId);
    query.addBindValue(int(dataType));
    if (!query.exec()) {
        qCritical() << "sql query err in ImageRelationRecordDao::getByDataId when query: " << query.lastError();
        if (errCode) *errCode = SQL_QUERY_ERR_CODE;
        return list;
    }
    int size = query.size();
    if (size > 0) {
        list.reserve(size);
    }
    ImageRelationRecordEntity entity;
    while (query.next()) {
        entity.setId(query.value(0).toULongLong());
        entity.setImageId(query.value(1).toULongLong());
        entity.setDataId(query.value(2).toULongLong());
        entity.setDataType(query.value(3).toInt());
        entity.setCreateTime(query.value(4).toDateTime());
        entity.setEditTime(query.value(5).toDateTime());
        entity.setDeleted(query.value(6).toInt());
        list.append(entity);
    }
    if (errCode) *errCode = 0;
    return list;
}

qint32 ImageRelationRecordDao::removeByIds(const QList<quint64>& ids) {
    if (ids.isEmpty()) return 0;
    SqlConnectionHolder holder = getConnectionHolder();
    if (holder.openError()) {
        qCritical() << "sql connection open err on ImageRelationRecordDao::removeByIds: " << holder.getErrMsg();
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    sem::AfterGc gc([this]() { close(); });
    QString sql = "update sem_image_relation_record set deleted = 1 where deleted = 0 and id in (";
    for (int i = 0; i < ids.size(); ++ i) {
        if (i > 0) sql.append(",");
        sql.append("?");
    }
    sql.append(")");
    SqlQueryHolder query = holder.exec(sql);
    for (const quint64& id : ids) {
        query.addBindValue(id);
    }
    if (!query.exec()) {
        qCritical() << "sql query err in ImageRelationRecordDao::removeByIds when remove: " << query.lastError();
        return SQL_UPDATE_ERR_CODE;
    }
    return 0;
}

qint32 ImageRelationRecordDao::removeByDataImage(quint64 dataId, ImageRelationDataType dataType, const QList<quint64>& imageIds) {
    if (imageIds.isEmpty()) return 0;
    SqlConnectionHolder holder = getConnectionHolder();
    if (holder.openError()) {
        qCritical() << "sql connection open err on ImageRelationRecordDao::removeByDataImage: " << holder.getErrMsg();
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    sem::AfterGc gc([this]() { close(); });
    QString sql = "update sem_image_relation_record set deleted = 1 where deleted = 0 and data_id = ? and data_type = ? and image_id in (";
    for (int i = 0; i < imageIds.size(); ++ i) {
        if (i > 0) sql.append(",");
        sql.append("?");
    }
    sql.append(")");
    SqlQueryHolder query = holder.exec(sql);
    query.addBindValue(dataId);
    query.addBindValue(int(dataType));
    for (const quint64& imageId : imageIds) {
        query.addBindValue(imageId);
    }
    if (!query.exec()) {
        qCritical() << "sql query err in ImageRelationRecordDao::removeByDataImage when remove: " << query.lastError();
        return SQL_UPDATE_ERR_CODE;
    }
    return 0;
}

qint32 ImageRelationRecordDao::deleteByDataImage(quint64 dataId, ImageRelationDataType dataType, const QList<quint64>& imageIds) {
    if (imageIds.isEmpty()) return 0;
    SqlConnectionHolder holder = getConnectionHolder();
    if (holder.openError()) {
        qCritical() << "sql connection open err on ImageRelationRecordDao::deleteByDataImage: " << holder.getErrMsg();
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    sem::AfterGc gc([this]() { close(); });
    QString sql = "delete from sem_image_relation_record where deleted = 0 and data_id = ? and data_type = ? and image_id in (";
    for (int i = 0; i < imageIds.size(); ++ i) {
        if (i > 0) sql.append(",");
        sql.append("?");
    }
    sql.append(")");
    SqlQueryHolder query = holder.exec(sql);
    query.addBindValue(dataId);
    query.addBindValue(int(dataType));
    for (const quint64& imageId : imageIds) {
        query.addBindValue(imageId);
    }
    if (!query.exec()) {
        qCritical() << "sql query err in ImageRelationRecordDao::deleteByDataImage when remove: " << query.lastError();
        return SQL_UPDATE_ERR_CODE;
    }
    return 0;
}
