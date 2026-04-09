#include "env_pattern_param_convert.h"
#include "template_data_model.h"
#include "image_save_model.h"

void semEnvToPatternParam(PatternRecognitionParamSettingParam& param, const ImageAddtionDataModel& additionParam) {
    param.autoParam.brightness = additionParam.brightness;
    param.autoParam.contrast =additionParam.contrast;
    param.autoParam.detectionFrame = additionParam.frame;
    param.autoParam.scanRate = additionParam.scanRate;

    param.manualParam.brightness = additionParam.brightness;
    param.manualParam.contrast = additionParam.contrast;
    param.manualParam.mag = additionParam.mag;
    param.manualParam.rot = additionParam.imageRot;
    param.manualParam.frame = additionParam.frame;
    param.manualParam.scanMode = int(additionParam.imageType) - 1;
}
