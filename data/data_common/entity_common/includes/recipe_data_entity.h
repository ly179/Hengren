#ifndef RECIPE_DATA_ENTITY_H
#define RECIPE_DATA_ENTITY_H
#include <QDateTime>
#include <QString>
#include "cdsem_pack_base.h"
#include "model_setget.h"

class RecipeSearchDTO {
    CDSEMPSTRING(RecipeName, recipeName)
    CDSEMPULONG64(
        ClassId, classId)
    CDSEMPULONG64(
        IdwId, idwId)
    CDSEMPULONG64(
        IdpId, idpId)
    CDSEMPULONG64(
        TopId, topId)
    CDSEMPINTEGER32(
        Invalid, invalid)
};

class RecipeDataEntity {
private:
    CDSEMPDATETIME(
        CreateTime, createTime)
    CDSEMPDATETIME(
        EditTime, editTime)
    CDSEMPSTRING(RecipeName, recipeName)
    CDSEMPSTRING(
        Detail, detail)
    CDSEMPUINT64(
        ClassId, classId, 0)
    CDSEMPUINT64(
        IdwId, idwId, 0)
    CDSEMPUINT64(
        IdwTopId, idwTopId, 0)
    CDSEMPUINT64(
        IdpId, idpId, 0)
    CDSEMPUINT64(
        IdpTopId, idpTopId, 0)
    CDSEMPUINT64(
        Id, id, 0)
    CDSEMPUINT64(
        TopId, topId, 0)
    CDSEMPUINT32(
        Version, version, 0)
    CDSEMPCHAR(
        Invalid, invalid, 0)
    CDSEMPCHAR(
        Deleted, deleted, 0)
};

#endif // RECIPE_DATA_ENTITY_H
