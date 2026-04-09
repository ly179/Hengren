#ifndef IMAGE_RELATION_RECORD_DAO_H
#define IMAGE_RELATION_RECORD_DAO_H

#include "abstract_dao.h"
#include "general_database_dao_global.h"
#include "image_relation_record_entity.h"

class SEM_DAO_EXPORT ImageRelationRecordDao : public AbstractDao{
private:
    QSharedPointer<SqlConnectionFactory> getConnectionFactory() override;

public:
    qint32 save(const QList<ImageRelationRecordEntity>&);
    QList<ImageRelationRecordEntity> getByDataId(quint64 dataId, ImageRelationDataType dataType, qint32 *errCode = nullptr);
    qint32 removeByIds(const QList<quint64>& ids);
    qint32 removeByDataImage(quint64 dataId, ImageRelationDataType dataType, const QList<quint64>& imageIds);
    qint32 deleteByDataImage(quint64 dataId, ImageRelationDataType dataType, const QList<quint64>& imageIds);
};

#endif // IMAGE_RELATION_RECORD_DAO_H
