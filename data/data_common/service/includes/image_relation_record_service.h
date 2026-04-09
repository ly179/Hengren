#ifndef IMAGE_RELATION_RECORD_SERVICE_H
#define IMAGE_RELATION_RECORD_SERVICE_H

#include "general_database_service_global.h"
#include "image_relation_record_entity.h"
#include "iservice.h"

class ImageRelationRecordDao;
class SEM_SERVICE_EXPORT ImageRelationRecordService : public IService{

private:
    ImageRelationRecordDao* relationDao;

public:
    ImageRelationRecordService();
    ~ImageRelationRecordService() noexcept;

    qint32 updateRelation(quint64 dataId, ImageRelationDataType dataType, const QList<quint64>& allImageIds);

    qint32 realDeleteImageRelation(quint64 dataId, ImageRelationDataType dataType, const QList<quint64>& imageIds);
};

#endif // IMAGE_RELATION_RECORD_SERVICE_H
