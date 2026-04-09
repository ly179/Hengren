#ifndef IDP_DATA_ENTITY_H
#define IDP_DATA_ENTITY_H

#include <QDateTime>
#include <QString>
#include "cdsem_pack_base.h"
#include "model_setget.h"

class IdpSearchDTO {
    CDSEMPC(
        QList<quint64>, Ids, ids)
    CDSEMPSTRING(IdpName, idpName)
    CDSEMPULONG64(
        ClassId, classId)
    CDSEMPULONG64(
        IdwId, idwId)
    CDSEMPULONG64(
        IdwTopId, idwTopId)
    CDSEMPULONG64(
        TopId, topId)
    CDSEMPINTEGER32(
        Invalid, invalid)
};

class IdpDataEntity {
    CDSEMPSTRING(IdpApDetail, idpApDetail)
    CDSEMPSTRING(IdpMpDetail, idpMpDetail)
    CDSEMPDATETIME(CreateTime, createTime)
    CDSEMPDATETIME(EditTime, editTime)
    CDSEMPSTRING(IdpName, idpName)
    CDSEMPUINT64(
        Id, id, 0)
    CDSEMPUINT64(
        TopId, topId, 0)
    CDSEMPUINT64(
        ClassId, classId, 0)
    CDSEMPUINT64(
        IdwId, idwId, 0)
    CDSEMPUINT64(
        IdwTopId, idwTopId, 0)
    CDSEMPINT32(
        Version, version, 0)
    CDSEMPCHAR(
        Invalid, invalid, 0)
    CDSEMPCHAR(
        Deleted, deleted, 0)
public:
    inline QString& getIdpApDetial() { return idpApDetail; }
    inline QString& getIdpMpDetial() { return idpMpDetail; }
};

#endif // IDP_DATA_ENTITY_H
