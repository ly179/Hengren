#ifndef MEMORY_CONDITION_ENTITY_H
#define MEMORY_CONDITION_ENTITY_H

#include "model_setget.h"
#include "cdsem_pack_base.h"
#include <QDateTime>

class MemoryConditionEntity {
    CDSEMPSTRING(ConditionDetail, conditionDetail)
    CDSEMPDATETIME(CreateTime, createTime)
    CDSEMPDATETIME(EditTime, editTime)
    CDSEMPULONG64(Id, id)
};

#endif // MEMORY_CONDITION_ENTITY_H
