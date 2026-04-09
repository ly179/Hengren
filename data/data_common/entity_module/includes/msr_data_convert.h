#ifndef MSR_DATA_CONVERT_H
#define MSR_DATA_CONVERT_H
#include "entity_module_global.h"

struct MSR_Chip;
void ENTITY_MODULE_EXPORT msrModeDataToEntity(QString &, const MSR_Chip &);
int ENTITY_MODULE_EXPORT msrModeDataFromEntity(const QString &, MSR_Chip &);

#endif // MSR_DATA_CONVERT_H
