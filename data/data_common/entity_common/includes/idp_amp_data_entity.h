#ifndef IDP_AMP_DATA_ENTITY_H
#define IDP_AMP_DATA_ENTITY_H
#include <QDateTime>
#include <QString>
#include <QtGlobal>
#include "model_setget.h"

class IdpAmpDataEntity {
    CDSEMPDATETIME(CreateTime, createTime)
    CDSEMPDATETIME(EditTime, editTime)
    CDSEMPSTRING(AmpDetail, ampDetail)
    CDSEMPSTRING(ExtraInfo, extraInfo)
    CDSEMPSTRING(AmpName, ampName)
    CDSEMPUINT64(
        Id, id, 0)
    CDSEMPUINT64(
        IdpId, idpId, 0)
    CDSEMPCHAR(
        AmpMethod, ampMethod, 0)
    CDSEMPCHAR(
        AmpObject, ampObject, 0)
    CDSEMPCHAR(
        Deleted, deleted, 0)
};

#endif // IDP_AMP_DATA_ENTITY_H
