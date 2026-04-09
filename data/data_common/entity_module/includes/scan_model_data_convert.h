#ifndef SCAN_MODEL_DATA_CONVERT_H
#define SCAN_MODEL_DATA_CONVERT_H

#include "entity_module_global.h"

class ScanModeDataModel;
void ENTITY_MODULE_EXPORT scanModeDataToEntity(QString &, const ScanModeDataModel &);
int ENTITY_MODULE_EXPORT scanModeDataFromEntity(const QString &, ScanModeDataModel &);
#endif // SCAN_MODEL_DATA_CONVERT_H
