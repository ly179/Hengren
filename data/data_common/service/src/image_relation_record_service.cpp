#include "image_relation_record_service.h"
#include "dao_manager.h"
#include "image_relation_record_dao.h"
#include "exception_code.h"
#include "sem_common.h"

ImageRelationRecordService::ImageRelationRecordService() {
    relationDao = DaoManager::instance()->getFileFactoryDao<ImageRelationRecordDao>();
}

ImageRelationRecordService::~ImageRelationRecordService() noexcept {}

qint32 ImageRelationRecordService::updateRelation(quint64 dataId, ImageRelationDataType dataType, const QList<quint64>& allImageIds) {
    bool hasTran = relationDao->hasTransaction();
    bool succ = relationDao->beginTransaction(true);
    if (!succ) {
        qCritical() << "ImageRelationRecordService::updateRelation open transaction error.";
        return SQL_OPEN_TRAN_ERROR_CODE;
    }
    int errCode;
    sem::AfterGc gc([&errCode, this, &hasTran](){
        if (hasTran) return;
        if (errCode) relationDao->rollback();
        else if (!relationDao->commit()) {
            qCritical() << "ImageRelationRecordService::updateRelation commit transaction error.";
            errCode = SQL_COMMIT_ERROR_CODE;
        }
    });
    QList<ImageRelationRecordEntity> oldList = relationDao->getByDataId(dataId, dataType, &errCode);
    if (errCode) return errCode;
    QSet<quint64> oldIds;
    for (ImageRelationRecordEntity& entity : oldList) {
        oldIds.insert(entity.getImageId());
    }
    QSet<quint64> repeatChecker;
    QList<ImageRelationRecordEntity> newImageDatas;
    newImageDatas.reserve(allImageIds.size());
    ImageRelationRecordEntity entity;
    entity.setDataId(dataId);
    entity.setDataType(int(dataType));
    for (const quint64& imageId : allImageIds) {
        if (oldIds.contains(imageId)) {
            oldIds.remove(imageId);
            continue;
        }
        if (repeatChecker.contains(imageId)) continue;
        repeatChecker.insert(imageId);
        entity.setImageId(imageId);
        newImageDatas.append(entity);
    }
    if (oldIds.size() > 0) {
        QList<quint64> deleteImageIds;
        deleteImageIds.reserve(allImageIds.size());
        for (const quint64& imageId : oldIds) {
            deleteImageIds.append(imageId);
        }
        errCode = relationDao->removeByDataImage(dataId, dataType, deleteImageIds);
        if (errCode) return errCode;
    }
    if (newImageDatas.size() > 0) {
        errCode = relationDao->save(newImageDatas);
    }
    return errCode;
}

qint32 ImageRelationRecordService::realDeleteImageRelation(quint64 dataId, ImageRelationDataType dataType, const QList<quint64>& imageIds) {
    return relationDao->deleteByDataImage(dataId, dataType, imageIds);
}
