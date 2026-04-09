//
// Created by Shisan on 2026/1/14.
//
#ifndef HENGREN_CD_SEM_STAGE_MEMORY_ENTITY_H
#define HENGREN_CD_SEM_STAGE_MEMORY_ENTITY_H

#include  "model_setget.h"
#include  <QDateTime>

class StageMemoryEntity {
    CDSEMPSTRING(MemName, memName)
    CDSEMPSTRING(MemCoordinate, memCoordinate)
    ENTITY_EDIT_TIME
    ENTITY_CREATE_TIME
    ENTITY_ID
    ENTITY_DELETED
};


#endif //HENGREN_CD_SEM_STAGE_MEMORY_ENTITY_H