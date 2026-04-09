#ifndef CDSEM_GLOBAL_ENV_DATA_MODEL_CONVERT_H
#define CDSEM_GLOBAL_ENV_DATA_MODEL_CONVERT_H

#include "entity_module_global.h"

class CDSEM_GlobalCalibrationPyParamModel;
class QString;

void ENTITY_MODULE_EXPORT calibrationPyParamToJson(QString& json, const CDSEM_GlobalCalibrationPyParamModel&);
int ENTITY_MODULE_EXPORT calibrationPyParamFromJson(const QString& json, CDSEM_GlobalCalibrationPyParamModel&);

#endif // CDSEM_GLOBAL_ENV_DATA_MODEL_CONVERT_H
