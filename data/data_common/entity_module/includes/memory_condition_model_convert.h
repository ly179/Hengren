#ifndef MEMORY_CONDITION_MODEL_CONVERT_H
#define MEMORY_CONDITION_MODEL_CONVERT_H

#include "entity_module_global.h"

struct MemoryModelData;

void ENTITY_MODULE_EXPORT toMemoryConditionDataModelJson(QString& json, const MemoryModelData& param);
int ENTITY_MODULE_EXPORT fromMemoryConditionDataModelJson(const QString& json, MemoryModelData& param);

#endif // MEMORY_CONDITION_MODEL_CONVERT_H
