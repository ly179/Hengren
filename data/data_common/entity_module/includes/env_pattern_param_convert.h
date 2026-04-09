#ifndef ENV_PARAM_TO_PATTERN_PARAM_H
#define ENV_PARAM_TO_PATTERN_PARAM_H
#include "entity_module_global.h"

struct PatternRecognitionParamSettingParam;
struct ImageAddtionDataModel;
void ENTITY_MODULE_EXPORT semEnvToPatternParam(PatternRecognitionParamSettingParam&, const ImageAddtionDataModel&);

#endif // ENV_PARAM_TO_PATTERN_PARAM_H
