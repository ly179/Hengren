#include "template_data_model_convert.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include "image_save_model_convert.h"
#include "model_data_convert_helper.h"
#include "exception_code.h"
#include "template_data_model.h"

void putIdpMExeParamDetailToJson(QJsonObject& obj, const IdpMExeParamDetail& param) {
    if (param.rot != nullptr)
        obj.insert("rot", param.rot.toQStringAndKeepFullDecimal());
    if (param.designValue != nullptr)
        obj.insert("design_value", param.designValue.toQStringAndKeepFullDecimal());
    if (param.low1 != nullptr)
        obj.insert("low1", param.low1.toQStringAndKeepFullDecimal());
    if (param.low2 != nullptr)
        obj.insert("low2", param.low2.toQStringAndKeepFullDecimal());
    if (param.up1 != nullptr)
        obj.insert("up1", param.up1.toQStringAndKeepFullDecimal());
    if (param.up2 != nullptr)
        obj.insert("up2", param.up2.toQStringAndKeepFullDecimal());
    obj.insert("mp_name", param.mpName);
    obj.insert("detail", param.detail);
    obj.insert("data_measure", param.dataMeasure);
    obj.insert("output_data", param.outputData);
    obj.insert("formula", param.formula);
    obj.insert("function", param.function);
    obj.insert("amp_type", param.ampType);
    obj.insert("seq_num", param.seqNum);
    obj.insert("method", param.method);
    obj.insert("object", param.object);
    obj.insert("b", param.b);
    obj.insert("c", param.c);
    obj.insert("frame", param.frame);
    obj.insert("mag", param.mag);
    obj.insert("axb_num", param.axbNum);
    obj.insert("scan_view", param.scanView);
    obj.insert("scan", param.scan);
    if (param.dNumbers.size() > 0) {
        QJsonArray arr;
        for (const int& d : param.dNumbers) {
            arr.append(d);
        }
        obj.insert("d_numbers", arr);
    }
}

int fromIdpMExeParamDetailJson(const QJsonValue& val, IdpMExeParamDetail& param) {
    int errCode;
    QJsonObject obj;
    errCode = jValToObject(obj, val);
    if (errCode) {
        return errCode;
    }
    if (obj.isEmpty()) {
        return 0;
    }
    errCode = jValToNumber(param.rot, obj["rot"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToNumber(param.designValue, obj["design_value"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToNumber(param.low1, obj["low1"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToNumber(param.low2, obj["low2"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToNumber(param.up1, obj["up1"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToNumber(param.up2, obj["up2"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToString(param.mpName, obj["mp_name"]);
    if (errCode) return errCode;
    errCode = jValToString(param.detail, obj["detail"]);
    if (errCode) return errCode;
    errCode = jValToString(param.dataMeasure, obj["data_measure"]);
    if (errCode) return errCode;
    errCode = jValToString(param.outputData, obj["output_data"]);
    if (errCode) return errCode;
    errCode = jValToString(param.formula, obj["formula"]);
    if (errCode) return errCode;
    errCode = jValToString(param.function, obj["function"]);
    if (errCode) return errCode;
    errCode = jValToInt(param.ampType, obj["amp_type"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.seqNum, obj["seq_num"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.method, obj["method"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.object, obj["object"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.b, obj["b"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.c, obj["c"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.frame, obj["frame"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.mag, obj["mag"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.axbNum, obj["axb_num"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.scanView, obj["scan_view"]);
    if (errCode) {
        return errCode;
    }
    int v;
    errCode = jValToInt(v, obj["scan"]);
    param.scan = v;

    param.dNumbers.clear();
    QJsonValueRef dNumObj = obj["d_numbers"];
    if (dNumObj.isArray()) {
        QJsonArray array = dNumObj.toArray();
        for (const QJsonValue& v : array) {
            param.dNumbers.append(v.toInt());
        }
    }
    return errCode;
}

void putIdpMExeParamToJson(QJsonObject& obj, const IdpMExeParam& param) {
    if (param.list.size() > 0) {
        QJsonArray array;
        for (const IdpMExeParamDetail& detail : param.list) {
            QJsonObject obj;
            putIdpMExeParamDetailToJson(obj, detail);
            array.append(obj);
        }
        obj.insert("list", array);
    }
    QJsonObject sObj;
    putImageScanModelToJson(sObj, param.scanParam);
    obj.insert("scan", sObj);
    obj.insert("enable_scan", param.enableScan);
    obj.insert("enable_measure_exe", param.enableMeasureExe);
    obj.insert("image_id", qint64(param.imageId));
}

int fromIdpMExeParamJson(const QJsonValue& val, IdpMExeParam& param) {
    QJsonObject obj;
    int errCode = jValToObject(obj, val);
    if (errCode) {
        return errCode;
    }
    if (obj.isEmpty()) {
        return 0;
    }
    const QJsonValue& listValue = obj["list"];
    if (!listValue.isUndefined() && !listValue.isNull()) {
        if (!listValue.isArray()) {
            return JSON_CONVERT_TYPE_ERR_CODE;
        }
        QJsonArray array = listValue.toArray();
        if (array.size() > 0) {
            param.list.reserve(array.size());
            for (const QJsonValue& v : array) {
                IdpMExeParamDetail d;
                errCode = fromIdpMExeParamDetailJson(v, d);
                if (errCode) {
                    return errCode;
                }
                param.list.append(d);
            }
        }
    }
    errCode = fromImageScanModelJson(obj["scan"], param.scanParam);
    if (errCode) {
        return errCode;
    }
    errCode = jValToBool(param.enableScan, obj["enable_scan"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.imageId, obj["image_id"]);
    if (errCode) return errCode;
    return jValToBool(param.enableMeasureExe, obj["enable_measure_exe"]);
}

void putAutoFocusSettingParam(QJsonObject& obj, const AutoFocusSettingParam& param) {
    if (param.relativePos.x != nullptr)
        obj.insert("relative_pos_x", param.relativePos.x.toQStringAndKeepFullDecimal());
    if (param.relativePos.y != nullptr)
        obj.insert("relative_pos_y", param.relativePos.y.toQStringAndKeepFullDecimal());
    if (param.waitSeconds != nullptr)
        obj.insert("wait_seconds", param.waitSeconds.toQStringAndKeepFullDecimal());
    obj.insert("method", param.method);
    obj.insert("pixel_xy", param.pixelXY);
    obj.insert("frame", param.frame);
    obj.insert("scan_rate", param.scanRate);
    if (param.mag != nullptr)
        obj.insert("mag", param.mag.value());
    if (param.offset != nullptr)
        obj.insert("offset", param.offset.toMinString());
    if (param.threshold != nullptr)
        obj.insert("threshold", param.threshold.value());
    if (param.lowChargVolt != nullptr)
        obj.insert("low_charg_volt", param.lowChargVolt.value());
    if (param.highChargVolt != nullptr)
        obj.insert("high_charg_volt", param.highChargVolt.value());
    obj.insert("enbale", param.enable);
}

int fromAutoFocusSettingParam(const QJsonValue& val, AutoFocusSettingParam& param) {
    int errCode;
    QJsonObject obj;
    errCode = jValToObject(obj, val);
    if (errCode) {
        return errCode;
    }
    if (obj.isEmpty()) {
        return 0;
    }
    errCode = jValToNumber(param.relativePos.x, obj["relative_pos_x"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToNumber(param.relativePos.y, obj["relative_pos_y"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToNumber(param.waitSeconds, obj["wait_seconds"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.method, obj["method"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.pixelXY, obj["pixel_xy"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.frame, obj["frame"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.scanRate, obj["scan_rate"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToPack(param.mag, obj["mag"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToNumber(param.offset, obj["offset"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToPack(param.threshold, obj["threshold"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToPack(param.lowChargVolt, obj["low_charg_volt"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToPack(param.highChargVolt, obj["high_charg_volt"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToBool(param.enable, obj["enbale"]);
    return errCode;
}

void putAutoStigmaSettingParam(QJsonObject& obj, const AutoStigmaSettingParam& param) {
    if (param.relativePos.x != nullptr)
        obj.insert("relative_pos_x", param.relativePos.x.toQStringAndKeepFullDecimal());
    if (param.relativePos.y != nullptr)
        obj.insert("relative_pos_y", param.relativePos.y.toQStringAndKeepFullDecimal());
    if (param.waitSeconds != nullptr)
        obj.insert("wait_seconds", param.waitSeconds.toQStringAndKeepFullDecimal());
    obj.insert("mode", param.mode);
    if (param.mag != nullptr)
        obj.insert("mag", param.mag.value());
    if (param.offset != nullptr)
        obj.insert("offset", param.offset.toMinString());
    obj.insert("enable", param.enable);
}

int fromAutoStigmaSettingParam(const QJsonValue& val, AutoStigmaSettingParam& param) {
    int errCode;
    QJsonObject obj;
    errCode = jValToObject(obj, val);
    if (errCode) {
        return errCode;
    }
    if (obj.isEmpty()) {
        return 0;
    }
    errCode = jValToNumber(param.relativePos.x, obj["relative_pos_x"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToNumber(param.relativePos.y, obj["relative_pos_y"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToNumber(param.waitSeconds, obj["wait_seconds"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.mode, obj["mode"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToPack(param.mag, obj["mag"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToNumber(param.offset, obj["offset"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToBool(param.enable, obj["enable"]);
    return errCode;
}

void putAutoFocusParam(QJsonObject& obj, const AutoFocusParam& param) {
    QJsonObject fObj;
    QJsonObject sObj;
    putAutoFocusSettingParam(fObj, param.focusParam);
    putAutoStigmaSettingParam(sObj, param.stigmaParam);
    obj.insert("focus_param", fObj);
    obj.insert("stigma_param", sObj);
    obj.insert("focus_type", param.focusType);
    obj.insert("offset", param.offset.toMinString());
    obj.insert("enable", param.enable);
}

int fromAutoFocusParam(const QJsonValue& val, AutoFocusParam& param) {
    int errCode;
    QJsonObject obj;
    errCode = jValToObject(obj, val);
    if (errCode) {
        return errCode;
    }
    if (obj.isEmpty()) {
        return 0;
    }
    errCode = fromAutoFocusSettingParam(obj["focus_param"], param.focusParam);
    if (errCode) {
        return errCode;
    }
    errCode = fromAutoStigmaSettingParam(obj["stigma_param"], param.stigmaParam);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.focusType, obj["focus_type"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToNumber(param.offset, obj["offset"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToBool(param.enable, obj["enable"]);
    return errCode;
}

void putPatternRecognitionParamSettingAutoParam(
    QJsonObject& obj, const PatternRecognitionParamSettingAutoParam& param) {
    obj.insert("offset_x", param.offset.x());
    obj.insert("offset_y", param.offset.y());
    if (param.waitSeconds != nullptr)
        obj.insert("wait_seconds", param.waitSeconds.toQStringAndKeepFullDecimal());
    obj.insert("rotation", param.rotation.toQStringAndKeepFullDecimal());
    obj.insert("centering", param.centering);
    if (param.minNumber1 != nullptr)
        obj.insert("min_num1", param.minNumber1.value());
    if (param.minNumber2 != nullptr)
        obj.insert("min_num2", param.minNumber2.value());
    obj.insert("scan_rate", param.scanRate);
    obj.insert("detection_frame", param.detectionFrame);
    obj.insert("measurement_frame", param.measurementFrame);
    obj.insert("filter", param.filter);
    obj.insert("evaluation_value", param.evaluationValue);
    obj.insert("entire_mode", param.entireMode);
    obj.insert("dc_mode", param.dcMode);
    obj.insert("brightness", param.brightness);
    obj.insert("contrast", param.contrast);
    if (param.acceptance != nullptr)
        obj.insert("acceptance", param.acceptance.value());
    obj.insert("ls_detection", param.lsDetection);
    obj.insert("method", param.method);
    obj.insert("edge_detection", param.edgeDetection);
    obj.insert("image_quality", param.imageQuality);
    if (param.bsePercent != nullptr)
        obj.insert("bse_percent", param.bsePercent.value());
    if (param.energyFilterValue != nullptr)
        obj.insert("energy_filter_value", param.energyFilterValue.value());
    obj.insert("enable_energy_filter", param.enableEnergyFilter);
    obj.insert("enable_flat_mode", param.enableFlatMode);
    obj.insert("enable_evaluation", param.enableEvaluation);
    obj.insert("enable_entire_mode_contrast_mode", param.enableEntireModeContrastMode);
    obj.insert("enable_ls_detection_contrast_mode", param.enableLsDetectionContrastMode);
    obj.insert("enable_search_around", param.enableSearchAround);
}

int fromPatternRecognitionParamSettingAutoParam(const QJsonValue& val,
                                                PatternRecognitionParamSettingAutoParam& param) {
    int errCode;
    QJsonObject obj;
    errCode = jValToObject(obj, val);
    if (errCode) {
        return errCode;
    }
    if (obj.isEmpty()) {
        return 0;
    }
    int n;
    errCode = jValToInt(n, obj["offset_x"]);
    if (errCode) {
        return errCode;
    }
    param.offset.setX(n);
    errCode = jValToInt(n, obj["offset_y"]);
    if (errCode) {
        return errCode;
    }
    param.offset.setY(n);
    errCode = jValToNumber(param.waitSeconds, obj["wait_seconds"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToNumber(param.rotation, obj["rotation"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.centering, obj["centering"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToPack(param.minNumber1, obj["min_num1"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToPack(param.minNumber2, obj["min_num2"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.scanRate, obj["scan_rate"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.detectionFrame, obj["detection_frame"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.measurementFrame, obj["measurement_frame"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.filter, obj["filter"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.evaluationValue, obj["evaluation_value"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.entireMode, obj["entire_mode"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.dcMode, obj["dc_mode"]);
    if (errCode) {
        return errCode;
    }
    FloatNumber9 num;
    errCode = jValToNumber(num, obj["brightness"]);
    if (errCode) {
        return errCode;
    }
    param.brightness = num.toDouble();
    errCode = jValToNumber(num, obj["contrast"]);
    if (errCode) {
        return errCode;
    }
    param.contrast = num.toDouble();
    errCode = jValToPack(param.acceptance, obj["acceptance"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.lsDetection, obj["ls_detection"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.method, obj["method"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.edgeDetection, obj["edge_detection"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.imageQuality, obj["image_quality"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToPack(param.bsePercent, obj["bse_percent"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToPack(param.energyFilterValue, obj["energy_filter_value"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToBool(param.enableEnergyFilter, obj["enable_energy_filter"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToBool(param.enableFlatMode, obj["enable_flat_mode"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToBool(param.enableEvaluation, obj["enable_evaluation"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToBool(param.enableEntireModeContrastMode,
                         obj["enable_entire_mode_contrast_mode"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToBool(param.enableLsDetectionContrastMode,
                         obj["enable_ls_detection_contrast_mode"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToBool(param.enableSearchAround, obj["enable_search_around"]);
    return errCode;
}

void putPatternRecognitionParamSettingManualParam(
    QJsonObject& obj, const PatternRecognitionParamSettingManualParam& param) {
    if (param.relativePos.x != nullptr)
        obj.insert("relative_pos_x", param.relativePos.x.toQStringAndKeepFullDecimal());
    if (param.relativePos.y != nullptr)
        obj.insert("relative_pos_y", param.relativePos.y.toQStringAndKeepFullDecimal());
    if  (param.rot != nullptr)
        obj.insert("rot", param.rot.toQStringAndKeepFullDecimal());
    obj.insert("brightness", param.brightness.toMinString());
    obj.insert("contrast", param.contrast.toMinString());
    if (param.mag != nullptr)
        obj.insert("mag", param.mag.value());
    if (param.acceptance != nullptr)
        obj.insert("acceptance", param.acceptance.value());
    obj.insert("frame", param.frame);
    obj.insert("filter", param.filter);
    obj.insert("scan_mode", param.scanMode);
    obj.insert("view_scan", param.viewScan);
}

int fromPatternRecognitionParamSettingManualParam(const QJsonValue& val,
                                                  PatternRecognitionParamSettingManualParam& param) {
    int errCode;
    QJsonObject obj;
    errCode = jValToObject(obj, val);
    if (errCode) {
        return errCode;
    }
    if (obj.isEmpty()) {
        return 0;
    }
    errCode = jValToNumber(param.relativePos.x, obj["relative_pos_x"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToNumber(param.relativePos.y, obj["relative_pos_y"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToNumber(param.rot, obj["rot"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToNumber(param.brightness, obj["brightness"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToNumber(param.contrast, obj["contrast"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToPack(param.mag, obj["mag"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToPack(param.acceptance, obj["acceptance"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.frame, obj["frame"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.filter, obj["filter"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.scanMode, obj["scan_mode"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.viewScan, obj["view_scan"]);
    return errCode;
}

//imageAdditionData字段不跟随存入数据库，它会转向存入到image库
void putPatternRegParamImageEnv(QJsonObject& obj, const PatternRegInfoParam& param) {
    obj.insert("name", param.name);
    QString str;
    rectToJValStr(str, param.modelArea);
    obj.insert("model_area", str);
    //该字段意义不大 0 - AP OM 1 - AP SEM 2 - Address 3 - MP
    obj.insert("model", param.model);
    obj.insert("kind", param.kind);
    obj.insert("score", param.score);
    obj.insert("quality", param.quality);
    obj.insert("detected_patterns", param.detectedPatterns);
    obj.insert("line_space_detection", param.lineSpaceDetection);
    obj.insert("processor", param.processor);
    obj.insert("alg", param.alg);
}

int fromPatternRegParamImageEnv(const QJsonValue& val, PatternRegInfoParam& param) {
    int errCode;
    QJsonObject obj;
    errCode = jValToObject(obj, val);
    if (errCode) {
        return errCode;
    }
    if (obj.isEmpty()) {
        return 0;
    }
    errCode = jValStrToRect(param.modelArea, obj["model_area"]);
    if (errCode) return errCode;
    errCode = jValToInt(param.model, obj["model"]);
    if (errCode) return errCode;
    errCode = jValToInt(param.kind, obj["kind"]);
    if (errCode) return errCode;
    errCode = jValToInt(param.score, obj["score"]);
    if (errCode) return errCode;
    errCode = jValToInt(param.quality, obj["quality"]);
    if (errCode) return errCode;
    errCode = jValToInt(param.detectedPatterns, obj["detected_patterns"]);
    if (errCode) return errCode;
    errCode = jValToInt(param.lineSpaceDetection, obj["line_space_detection"]);
    if (errCode) return errCode;
    errCode = jValToInt(param.processor, obj["processor"]);
    if (errCode) return errCode;
    return jValToInt(param.alg, obj["alg"]);
}

void putPatternRecognitionParamSettingParam(QJsonObject& obj,
                                            const PatternRecognitionParamSettingParam& param) {
    QJsonObject aObj;
    QJsonObject mObj;
    QJsonObject iObj;
    QJsonObject sObj;
    putPatternRecognitionParamSettingAutoParam(aObj, param.autoParam);
    putPatternRecognitionParamSettingManualParam(mObj, param.manualParam);
    putPatternRegParamImageEnv(iObj, param.infoParam);
    putPatternRecognitionAlgorithmSettingParam(sObj, param.algSettingParam);
    obj.insert("auto_param", aObj);
    obj.insert("manual_param", mObj);
    obj.insert("pattern_image", iObj);
    obj.insert("alg_setting_param", sObj);
    obj.insert("enable", param.enable);
    obj.insert("use_auto", param.useAuto);
    obj.insert("img_type", param.imgType);
    obj.insert("image_id", qint64(param.imageId));
}

int fromPatternRecognitionParamSettingParam(
    const QJsonValue& val, PatternRecognitionParamSettingParam& param) {
    int errCode;
    QJsonObject obj;
    errCode = jValToObject(obj, val);
    if (errCode) {
        return errCode;
    }
    if (obj.isEmpty()) {
        return 0;
    }
    errCode = fromPatternRecognitionParamSettingAutoParam(obj["auto_param"], param.autoParam);
    if (errCode) {
        return errCode;
    }
    errCode = fromPatternRecognitionParamSettingManualParam(obj["manual_param"], param.manualParam);
    if (errCode) {
        return errCode;
    }
    errCode = fromPatternRegParamImageEnv(obj["pattern_image"], param.infoParam);
    if (errCode) {
        return errCode;
    }
    errCode = fromPatternRecognitionAlgorithmSettingParam(obj["alg_setting_param"], param.algSettingParam);
    if (errCode) return errCode;
    errCode = jValToBool(param.enable, obj["enable"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToBool(param.useAuto, obj["use_auto"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.imageId, obj["image_id"]);
    if (errCode) {
        return errCode;
    }
    return jValToInt(param.imgType, obj["img_type"]);
}

void putIfemParam(
    QJsonObject& obj, const IfemParam& param) {
    if (param.kind < 0) {
        return;
    }
    if (param.width1 != nullptr) {
        obj.insert("width1", param.width1.toQStringAndKeepFullDecimal());
    }
    if (param.diameter != nullptr) {
        obj.insert("diameter", param.diameter.toQStringAndKeepFullDecimal());
    }
    if (param.pitchX != nullptr) {
        obj.insert("pitch_x", param.pitchX.toQStringAndKeepFullDecimal());
    }
    if (param.pitchY != nullptr) {
        obj.insert("pitch_y", param.pitchY.toQStringAndKeepFullDecimal());
    }
    if (param.lengthY != nullptr) {
        obj.insert("length_y", param.lengthY.toQStringAndKeepFullDecimal());
    }
    if (param.numberX != nullptr) {
        obj.insert("number_x", param.numberX.value());
    }
    if (param.numberY != nullptr) {
        obj.insert("number_y", param.numberY.value());
    }
    if (param.targetX != nullptr) {
        obj.insert("target_x", param.targetX.value());
    }
    if (param.targetY != nullptr) {
        obj.insert("target_y", param.targetY.value());
    }
    obj.insert("target_type", param.targetType);
    obj.insert("kind", param.kind);
};
int fromIfemParam(
    const QJsonValue& val, IfemParam& param) {
    QJsonObject obj;
    int errCode = jValToObject(obj, val);
    if (errCode) {
        return errCode;
    }
    if (obj.isEmpty()) {
        return 0;
    }
    errCode = jValToNumber(param.width1, obj["width1"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToNumber(param.diameter, obj["diameter"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToNumber(param.pitchX, obj["pitch_x"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToNumber(param.pitchY, obj["pitch_y"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToNumber(param.lengthY, obj["length_y"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToPack(param.numberX, obj["number_x"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToPack(param.numberY, obj["number_y"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToPack(param.targetX, obj["target_x"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToPack(param.targetY, obj["target_y"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.targetType, obj["target_type"]);
    if (errCode) {
        return errCode;
    }
    return jValToInt(param.kind, obj["kind"]);
}

void putPatternRecognitionAlgorithmSettingParam(
    QJsonObject& obj, const PatternRecognitionAlgorithmSettingParam& param) {
    QJsonObject iObj;
    putIfemParam(iObj, param.ifemParam);
    if (iObj.size() > 0)
        obj.insert("ifem", iObj);
    obj.insert("kind", param.kind);
    obj.insert("processor", param.processor);
    obj.insert("algorithm", param.algorithm);
    obj.insert("low_limit", param.lowLimit);
    obj.insert("enable_abc", param.enableABC);
    obj.insert("enable_auto_focus", param.enableAutoFocus);
    obj.insert("line_space_detection", param.lineSpaceDetection);
    obj.insert("macro_alignment", param.macroAlignment);
    obj.insert("load_a_plus_b", param.loadAPlusB);
    obj.insert("load_amp_cursor", param.loadAMPCursor);
    obj.insert("alg_mode", int(param.algMode));
}

int fromPatternRecognitionAlgorithmSettingParam(
    const QJsonValue& val, PatternRecognitionAlgorithmSettingParam& param) {
    int errCode;
    QJsonObject obj;
    errCode = jValToObject(obj, val);
    if (errCode) {
        return errCode;
    }
    if (obj.isEmpty()) {
        return 0;
    }
    errCode = fromIfemParam(obj["ifem"], param.ifemParam);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.kind, obj["kind"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.processor, obj["processor"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.algorithm, obj["algorithm"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.lowLimit, obj["low_limit"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToBool(param.enableABC, obj["enable_abc"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToBool(param.enableAutoFocus, obj["enable_auto_focus"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.lineSpaceDetection, obj["line_space_detection"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.macroAlignment, obj["macro_alignment"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToBool(param.loadAPlusB, obj["load_a_plus_b"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToBool(param.loadAMPCursor, obj["load_amp_cursor"]);
    if (errCode) return errCode;
    int num;
    errCode = jValToInt<int>(num, obj["alg_mode"]);
    if (errCode) {
        return errCode;
    }
    param.algMode = cdsem::PatternRegParamOpenMode(num);
    return 0;
}

void putPreDoseSettingParam(QJsonObject& obj, const PreDoseSettingParam& param) {
    obj.insert("mag", param.mag);
    if (param.waitSeconds != nullptr)
        obj.insert("wait_seconds", param.waitSeconds.toQStringAndKeepFullDecimal());
    obj.insert("vacc", param.vacc);
    obj.insert("vacc_mode", param.vaccMode);
    obj.insert("memory_number", param.memoryNumber);
    obj.insert("enable", param.enable);
    obj.insert("custom_vacc", param.customVacc);
    obj.insert("un_change_mode", param.unChangeMode);
    obj.insert("high_mode", param.highMode);
}

int fromPreDoseSettingParam(const QJsonValue& val, PreDoseSettingParam& param) {
    int errCode;
    QJsonObject obj;
    errCode = jValToObject(obj, val);
    if (errCode) {
        return errCode;
    }
    if (obj.isEmpty()) {
        return 0;
    }
    errCode = jValToInt(param.mag, obj["mag"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToNumber(param.waitSeconds, obj["wait_seconds"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.vacc, obj["vacc"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.vaccMode, obj["vacc_mode"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.memoryNumber, obj["memory_number"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToBool(param.enable, obj["enable"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToBool(param.customVacc, obj["custom_vacc"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToBool(param.unChangeMode, obj["un_change_mode"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToBool(param.highMode, obj["high_mode"]);
    return errCode;
}

void putZSensorSettingParam(QJsonObject& obj, const ZSensorSettingParam& param) {
    obj.insert("enable", param.enable);
    obj.insert("offset", param.offset);
}

int fromZSensorSettingParam(const QJsonValue& val, ZSensorSettingParam& param) {
    int errCode;
    QJsonObject obj;
    errCode = jValToObject(obj, val);
    if (errCode) {
        return errCode;
    }
    if (obj.isEmpty()) {
        return 0;
    }
    errCode = jValToBool(param.enable, obj["enable"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.offset, obj["offset"]);
    return errCode;
}

void idpApTemplateToJsonObject(QJsonObject& obj, const ApTemplateParam& param) {
    QJsonObject aObj;
    QJsonObject pRegObj;
    QJsonObject preObj;
    QJsonObject zObj;
    putAutoFocusParam(aObj, param.autoFoucsParam);
    putPatternRecognitionParamSettingParam(pRegObj, param.patternRegParam);
    putPreDoseSettingParam(preObj, param.preDoesParam);
    putZSensorSettingParam(zObj, param.zParam);
    obj.insert("auto_focus", aObj);
    obj.insert("pattern_reg_param", pRegObj);
    obj.insert("pre_dose_param", preObj);
    obj.insert("z_param", zObj);
    QString str;
    floatPointToJValStr(str, param.originOffset);
    obj.insert("origin_offset", str);
}
int idpApTemplateFromJsonValue(const QJsonValue& val, ApTemplateParam& param) {
    int errCode;
    QJsonObject obj;
    errCode = jValToObject(obj, val);
    if (errCode) {
        return errCode;
    }
    if (obj.isEmpty()) {
        return 0;
    }
    errCode = fromAutoFocusParam(obj["auto_focus"], param.autoFoucsParam);
    if (errCode) {
        return errCode;
    }
    errCode = fromPatternRecognitionParamSettingParam(obj["pattern_reg_param"],
                                                      param.patternRegParam);
    if (errCode) {
        return errCode;
    }
    errCode = fromPreDoseSettingParam(obj["pre_dose_param"], param.preDoesParam);
    if (errCode) {
        return errCode;
    }
    errCode = fromZSensorSettingParam(obj["z_param"], param.zParam);
    if (errCode) return errCode;
    return jValStrToFloatPoint(param.originOffset, obj["origin_offset"]);
}

void putMultiAddressDetailTemplateParam(QJsonObject& obj, const MultiAddressDetailTemplateParam& param ) {
    QJsonObject aObj;
    QJsonObject algObj;
    QJsonObject pObj;
    putAutoFocusParam(aObj, param.focusParam);
    putPatternRecognitionParamSettingParam(pObj, param.patternRegParam);
    QString str;
    floatPointToJValStr(str, param.relativePos);
    obj.insert("relative_pos", str);
    obj.insert("focus_param", aObj);
    obj.insert("pattern_reg_alg_param", algObj);
    obj.insert("pattern_reg_param", pObj);
    obj.insert("relative_pos", str);
}

int fromMultiAddressDetailTemplateParam(const QJsonValue& val, MultiAddressDetailTemplateParam& param ) {
    QJsonObject obj;
    int errCode = jValToObject(obj, val);
    if (errCode) return errCode;
    if (obj.isEmpty()) {
        param = MultiAddressDetailTemplateParam {};
        return 0;
    }
    errCode = fromAutoFocusParam(obj["focus_param"], param.focusParam);
    if (errCode) return errCode;
    errCode = errCode = fromPatternRecognitionParamSettingParam(obj["pattern_reg_param"], param.patternRegParam);
    if (errCode) return errCode;
    return jValStrToFloatPoint(param.relativePos, obj["relative_pos"]);
}

void idpAddressTemplateToJsonObject(QJsonObject& obj, const MultiAddressTemplateParam& param) {
    QJsonObject pObj;
    QJsonObject zObj;
    putPreDoseSettingParam(pObj, param.preDoseParam);
    putZSensorSettingParam(zObj, param.zParam);
    if (param.addressList.size() > 0) {
        QJsonArray array;
        simpleListToJArray<MultiAddressDetailTemplateParam>(param.addressList, array, putMultiAddressDetailTemplateParam);
        obj.insert("address_list", array);
    }
    obj.insert("pre_dose_param", pObj);
    obj.insert("z_param", zObj);
}

int idpAddressTemplateFromJsonValue(const QJsonValue& val,
                                                         MultiAddressTemplateParam& param) {
    QJsonObject obj;
    int errCode = jValToObject(obj, val);
    if (errCode) return errCode;
    if (obj.isEmpty()) {
        param = MultiAddressTemplateParam {};
        return 0;
    }
    errCode = fromZSensorSettingParam(obj["z_param"], param.zParam);
    if (errCode) return errCode;
    errCode = fromPreDoseSettingParam(obj["pre_dose_param"], param.preDoseParam);
    if (errCode) return errCode;
    return jArrayToSimpleList<MultiAddressDetailTemplateParam>(param.addressList, obj["address_list"], fromMultiAddressDetailTemplateParam);
}

void idpMpTemplateToJsonObject(QJsonObject& obj, const MpTemplateParam& param) {
    QJsonObject mfObj;
    QJsonObject mpRegObj;
    QJsonObject iObj;
    QJsonObject mObj;
    putAutoFocusParam(mfObj, param.autoFocusParam);
    putPatternRecognitionParamSettingParam(mpRegObj, param.patternRegParam);
    putIdpMExeParamToJson(mObj, param.exeParam);
    putImageSaveModelToJson(iObj, param.imageSaveParam);

    obj.insert("measurement_focus_param", mfObj);
    obj.insert("measurement_pattern_reg_param", mpRegObj);
    obj.insert("measure_exe_param", mObj);
    obj.insert("img_save_param", iObj);
    QString str;
    floatPointToJValStr<10>(str, param.diffShift);
    obj.insert("diff_shift", str);
}
int idpMpTemplateFromJsonValue(const QJsonValue& val, MpTemplateParam& param) {
    int errCode;
    QJsonObject obj;
    errCode = jValToObject(obj, val);
    if (errCode) {
        return errCode;
    }
    if (obj.isEmpty()) {
        return 0;
    }
    errCode = fromAutoFocusParam(obj["measurement_focus_param"], param.autoFocusParam);
    if (errCode) {
        return errCode;
    }
    errCode = fromPatternRecognitionParamSettingParam(obj["measurement_pattern_reg_param"],
                                                      param.patternRegParam);
    if (errCode) {
        return errCode;
    }
    errCode = fromIdpMExeParamJson(obj["measure_exe_param"], param.exeParam);
    if (errCode) {
        return errCode;
    }
    errCode = fromImageSaveModelJson(obj["img_save_param"], param.imageSaveParam);
    if (errCode) return errCode;
    errCode = jValStrToFloatPoint<10>(param.diffShift, obj["diff_shift"]);
    if (errCode) return errCode;
    if (param.diffShift.x == nullptr) param.diffShift.x = 0;
    if (param.diffShift.y == nullptr) param.diffShift.y = 0;
    return 0;
}

void idpApTemplateToJson(QString& json, const ApTemplateDataParam& apData) {
    QJsonDocument doc;
    QJsonObject obj;
    QJsonObject tObj;
    idpApTemplateToJsonObject(tObj, apData.templateParam);
    obj.insert("template", tObj);
    obj.insert("name", apData.name);
    obj.insert("comment", apData.comment);
    doc.setObject(obj);
    json = doc.toJson(QJsonDocument::Compact);
}
int idpApTemplateFromJson(const QString& json, ApTemplateDataParam& apData) {
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8(), &err);
    if (err.error != QJsonParseError::NoError) {
        qCritical() << "covert idp ap template data from json err: " << err.error << ", "
                    << err.errorString();
        return JSON_CONVERT_ERR_CODE;
    }
    int errCode = idpApTemplateFromJsonValue(doc["template"], apData.templateParam);
    if (errCode) {
        return errCode;
    }
    jValToString(apData.name, doc["name"]);
    jValToString(apData.comment, doc["comment"]);
    return 0;
}
void idpAddressTemplateToJson(QString& json, const AddressTemplateDataParam& addressData) {
    QJsonDocument doc;
    QJsonObject obj;
    QJsonObject tObj;
    idpAddressTemplateToJsonObject(tObj, addressData.templateParam);
    obj.insert("template", tObj);
    obj.insert("name", addressData.name);
    obj.insert("comment", addressData.comment);
    doc.setObject(obj);
    json = doc.toJson(QJsonDocument::Compact);
}
int idpAddressTemplateFromJson(const QString& json, AddressTemplateDataParam& addressData) {
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8(), &err);
    if (err.error != QJsonParseError::NoError) {
        qCritical() << "covert idp address template data from json err: " << err.error << ", "
                    << err.errorString();
        return JSON_CONVERT_ERR_CODE;
    }
    int errCode = idpAddressTemplateFromJsonValue(doc["template"], addressData.templateParam);
    if (errCode) {
        return errCode;
    }
    jValToString(addressData.name, doc["name"]);
    jValToString(addressData.comment, doc["comment"]);
    return 0;
}
void idpMpTemplateToJson(QString& json, const MpTemplateDataParam& mpData) {
    QJsonDocument doc;
    QJsonObject obj;
    QJsonObject tObj;
    idpMpTemplateToJsonObject(tObj, mpData.templateParam);
    obj.insert("template", tObj);
    obj.insert("name", mpData.name);
    obj.insert("comment", mpData.comment);
    doc.setObject(obj);
    json = doc.toJson(QJsonDocument::Compact);
}
int idpMpTemplateFromJson(const QString& json, MpTemplateDataParam& mpData) {
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8(), &err);
    if (err.error != QJsonParseError::NoError) {
        qCritical() << "covert idp mp template data from json err: " << err.error << ", "
                    << err.errorString();
        return JSON_CONVERT_ERR_CODE;
    }
    int errCode = idpMpTemplateFromJsonValue(doc["template"], mpData.templateParam);
    if (errCode) {
        return errCode;
    }
    jValToString(mpData.name, doc["name"]);
    jValToString(mpData.comment, doc["comment"]);
    return 0;
}

int patternRegAlgJsonToStruct(const QString& json,
                              PatternRecognitionAlgorithmSettingParam& param,
                              int mode) {
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8(), &err);
    if (err.error != QJsonParseError::NoError) {
        qCritical() << "covert pattern reg alg data from entity err: " << err.error << ", "
                    << err.errorString();
        return JSON_CONVERT_ERR_CODE;
    }
    QJsonValue value = doc["kind"];
    int errCode;
    int val;
    errCode = jValToInt(val, value);
    if (errCode) {
        return errCode;
    }
    param.kind = val;
    value = doc["processor"];
    errCode = jValToInt(val, value);
    if (errCode) {
        return errCode;
    }
    param.processor = val;
    value = doc["algorithm"];
    errCode = jValToInt(val, value);
    if (errCode) {
        return errCode;
    }
    param.algorithm = val;
    value = doc["low_limit"];
    errCode = jValToInt(val, value);
    if (errCode) {
        return errCode;
    }
    param.lowLimit = val;
    if (mode == 2) {
        value = doc["line_space_detection"];
        errCode = jValToInt(val, value);
        if (errCode) {
            return errCode;
        }
        param.lineSpaceDetection = val;
        value = doc["macro_alignment"];
        errCode = jValToInt(val, value);
        if (errCode) {
            return errCode;
        }
        param.macroAlignment = val;
        value = doc["load_a_plus_b"];
        if (!value.isBool()) {
            return JSON_CONVERT_TYPE_ERR_CODE;
        }
        param.loadAPlusB = value.toBool();
        value = doc["load_amp_cursor"];
        if (!value.isBool()) {
            return JSON_CONVERT_TYPE_ERR_CODE;
        }
        param.loadAMPCursor = value.toBool();
    } else if (mode == 0) {
        value = doc["enable_abc"];
        if (!value.isBool()) {
            return JSON_CONVERT_TYPE_ERR_CODE;
        }
        param.enableABC = value.toBool();
        value = doc["enable_auto_focus"];
        if (!value.isBool()) {
            return JSON_CONVERT_TYPE_ERR_CODE;
        }
        if (value.isBool()) {
            param.enableAutoFocus = value.toBool();
        }
    }
    return 0;
}
void patternRegAlgStructToJson(QString& json, const PatternRecognitionAlgorithmSettingParam& param) {
    QJsonDocument doc;
    QJsonObject obj;
    putPatternRecognitionAlgorithmSettingParam(obj, param);
    doc.setObject(obj);
    json = doc.toJson(QJsonDocument::Compact);
}
