#ifndef IMAGE_RELATION_RECORD_ENTITY_H
#define IMAGE_RELATION_RECORD_ENTITY_H
#include "model_setget.h"
#include <QDateTime>

enum ImageRelationDataType {
    //与IDP文件关联
    IRDT_IDP = 0,
    //与模板文件关联
    IRDT_TEMPLATE = 1,
    //与测量结果文件关联
    IRDT_MSR = 2
};

class ImageRelationRecordEntity {
    CDSEMPDATETIME(CreateTime, createTime)
    CDSEMPDATETIME(EditTime, editTime)
    CDSEMPUINT64(Id, id)
    CDSEMPUINT64(ImageId, imageId)
    CDSEMPUINT64(DataId, dataId)
    //see ImageRelationDataType枚举
    CDSEMPINT32(DataType, dataType)
    CDSEMPCHAR(Deleted, deleted)
};
#endif // IMAGE_RELATION_RECORD_ENTITY_H
