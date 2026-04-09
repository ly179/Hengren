#ifndef MSR_ENTITY_H
#define MSR_ENTITY_H

#include <QString>
#include <QDateTime>
#include "model_setget.h"
#include "cdsem_pack_base.h"

class MsrSearchDTO {
    CDSEMPSTRING(MsrName, msrName)
    CDSEMPULONG64(ClassId, classId)
    CDSEMPULONG64(TopId, topId)
    CDSEMPINTEGER32(Invalid, invalid)
};

// MSR文件实体类
class MsrEntity {
    ENTITY_CREATE_TIME
    ENTITY_EDIT_TIME
    CDSEMPSTRING(MsrDetail, msrDetail)
    CDSEMPSTRING(MsrName, msrName)
    ENTITY_ID
    CDSEMPUINT64(ClassId, classId)
    CDSEMPUINT64(TopId, topId)
    CDSEMPUINT64(RecipeId, recipeId, 0)
    CDSEMPUINT64(IdpId, idpId, 0)
    CDSEMPUINT64(IdwId, idwId, 0)
    CDSEMPINT32(Version, version)
    ENTITY_DELETED
    CDSEMPCHAR(Invalid, invalid)
};

#endif // MSR_ENTITY_H
