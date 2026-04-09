#ifndef AMP_DATA_ENTITY_H
#define AMP_DATA_ENTITY_H
#include <QDateTime>
#include <QString>
#include "cdsem_pack_base.h"
#include "model_setget.h"

class AmpDataSearchDTO {
    CDSEMPSTRING(AmpName, ampName)
    CDSEMPULONG64(
        TopId, topId)
    CDSEMPINTEGER32(
        Invalid, invalid, 0)
};

class AmpDataEntity {
    CDSEMPDATETIME(CreateTime, createTime)
    CDSEMPDATETIME(EditTime, editTime)
    CDSEMPSTRING(AmpName, ampName)
    CDSEMPSTRING(DetailContent, detailContent)
    CDSEMPUINT64(
        Id, id, 0)
    CDSEMPUINT64(TopId, topId)
    CDSEMPINT(
        Version, version, 0)
    CDSEMPCHAR(
        AmpType, ampType, 0)
    CDSEMPCHAR(
        Invalid, invalid, 0)
    CDSEMPCHAR(
        Deleted, deleted, 0)
};

#endif // AMP_DATA_ENTITY_H
