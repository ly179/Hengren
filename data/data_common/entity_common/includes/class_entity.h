#ifndef CLASS_ENTITY_H
#define CLASS_ENTITY_H
#include <QDateTime>
#include "model_setget.h"

class ClassSearchDTO {
    CDSEMPSTRING(
        ClassName, className)
};

class ClassEntity {
    CDSEMPDATETIME(
        CreateTime, createTime)
    CDSEMPDATETIME(
        EditTime, editTime)
    CDSEMPSTRING(
        ClassName, className)
    CDSEMPUINT64(
        Id, id, 0)
    CDSEMPCHAR(
        Deleted, deleted, 0)
};

#endif // CLASS_ENTITY_H
