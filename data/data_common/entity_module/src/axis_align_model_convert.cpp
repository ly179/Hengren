#include "axis_align_model_convert.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include "axis_align_model.h"
#include "model_data_convert_helper.h"
#include "template_data_model.h"
#include "template_data_model_convert.h"

void putAxisAlignMoveParam(
    QJsonObject& obj, const AxisAlignMoveModel& param) {
    if (param.x != nullptr) {
        obj.insert("x", param.x.toQStringAndKeepFullDecimal());
    }
    if (param.y != nullptr) {
        obj.insert("y", param.y.toQStringAndKeepFullDecimal());
    }
    if (param.deviationX != nullptr) {
        obj.insert("deviation_x", param.deviationX.toQStringAndKeepFullDecimal());
    }
    if (param.deviationY != nullptr) {
        obj.insert("deviation_y", param.deviationY.toQStringAndKeepFullDecimal());
    }
    obj.insert("enable", param.enable);
}

int fromAxisAlignMoveParam(
    const QJsonValue& val, AxisAlignMoveModel& param) {
    QJsonObject obj;
    int errCode = jValToObject(obj, val);
    if (errCode) {
        return errCode;
    }
    if (obj.isEmpty()) {
        return 0;
    }
    errCode = jValToNumber(param.x, obj["x"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToNumber(param.y, obj["y"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToNumber(param.deviationX, obj["deviation_x"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToNumber(param.deviationY, obj["deviation_y"]);
    if (errCode) {
        return errCode;
    }
    return jValToBool(param.enable, obj["enable"]);
}

void putAxisAlignParam(
    QJsonObject& obj, const AxisAlignParamModel& param) {
    if (param.waitSeconds != nullptr) {
        obj.insert("wait_seconds", param.waitSeconds.toQStringAndKeepFullDecimal());
    }
    if (param.mag != nullptr) {
        obj.insert("mag", param.mag.value());
    }
    obj.insert("item", param.item);
    obj.insert("enable", param.enable);
}
int fromAxisAlignParam(
    const QJsonValue& val, AxisAlignParamModel& param) {
    QJsonObject obj;
    int errCode = jValToObject(obj, val);
    if (errCode) {
        return errCode;
    }
    if (obj.isEmpty()) {
        return 0;
    }
    errCode = jValToNumber(param.waitSeconds, obj["wait_seconds"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToPack(param.mag, obj["mag"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.item, obj["item"]);
    if (errCode) {
        return errCode;
    }
    return jValToBool(param.enable, obj["enable"]);
}

void putAxisChipParam(
    QJsonObject& obj, const AxisChipModel& param) {
    if (param.x != nullptr) {
        obj.insert("x", param.x.value());
    }
    if (param.y != nullptr) {
        obj.insert("y", param.y.value());
    }
    if (param.deviationX != nullptr) {
        obj.insert("deviation_x", param.deviationX.value());
    }
    if (param.deviationY != nullptr) {
        obj.insert("deviation_y", param.deviationY.value());
    }
}
int fromAxisChipParam(
    const QJsonValue& val, AxisChipModel& param) {
    QJsonObject obj;
    int errCode = jValToObject(obj, val);
    if (errCode) {
        return errCode;
    }
    if (obj.isEmpty()) {
        return 0;
    }
    errCode = jValToPack(param.x, obj["x"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToPack(param.y, obj["y"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToPack(param.deviationX, obj["deviation_x"]);
    if (errCode) {
        return errCode;
    }
    return jValToPack(param.deviationY, obj["deviation_y"]);
}

void putFocusParam(
    QJsonObject& obj, const AutoFocusSettingParam& param) {
    putAutoFocusSettingParam(obj, param);
}

void putFocusParam(
    QJsonObject& obj, const AutoStigmaSettingParam& param) {
    putAutoStigmaSettingParam(obj, param);
}

int fromFocusParam(
    const QJsonValue& val, AutoFocusSettingParam& param) {
    return fromAutoFocusSettingParam(val, param);
}

int fromFocusParam(
    const QJsonValue& val, AutoStigmaSettingParam& param) {
    return fromAutoStigmaSettingParam(val, param);
}

template<class T>
void putAxisAlignContent(
    QJsonObject& obj, const AxisAlignContentModel<T>& param) {
    QJsonObject rObj;
    QJsonObject aObj;
    QJsonObject fObj;
    QJsonObject zObj;
    QJsonObject mObj;
    QJsonObject axisObj;
    putPatternRecognitionParamSettingParam(rObj, param.patternParam);
    putFocusParam(fObj, param.focusParam);
    putAxisAlignMoveParam(mObj, param.moveParam);
    putAxisAlignParam(axisObj, param.axisParam);
    putZSensorSettingParam(zObj, param.zParam);
    obj.insert("pattern_param", rObj);
    obj.insert("alg_param", aObj);
    obj.insert("focus_param", fObj);
    obj.insert("move_param", mObj);
    obj.insert("axis_param", axisObj);
    obj.insert("z_param", zObj);
    obj.insert("image_id", qint64(param.imageId));
}

template<class T>
int fromAxisAlignContent(
    const QJsonValue& val, AxisAlignContentModel<T>& param) {
    QJsonObject obj;
    int errCode = jValToObject(obj, val);
    if (errCode) {
        return errCode;
    }
    if (obj.isEmpty()) {
        return 0;
    }
    errCode = fromPatternRecognitionParamSettingParam(obj["pattern_param"], param.patternParam);
    if (errCode) {
        return errCode;
    }
    errCode = fromFocusParam(obj["focus_param"], param.focusParam);
    if (errCode) {
        return errCode;
    }
    errCode = fromAxisAlignMoveParam(obj["move_param"], param.moveParam);
    if (errCode) {
        return errCode;
    }
    errCode = fromAxisAlignParam(obj["axis_param"], param.axisParam);
    if (errCode) {
        return errCode;
    }
    errCode = fromZSensorSettingParam(obj["z_param"], param.zParam);
    if (errCode) {
        return errCode;
    }
    return jValToInt(param.imageId, obj["image_id"]);
}

void putAxisAlignData(
    QJsonObject& obj, const AxisAlignModel& param) {
    QJsonObject alignObj;
    QJsonObject stigmaObj;
    QJsonObject chipObj;
    putAxisAlignContent(alignObj, param.alignContentParam);
    putAxisAlignContent(stigmaObj, param.stigmaContentParam);
    putAxisChipParam(chipObj, param.chipParam);
    obj.insert("align_focus", alignObj);
    obj.insert("stigma_focus", stigmaObj);
    if (chipObj.size() > 0) {
        obj.insert("chip", chipObj);
    }
    if (param.memNo1 != nullptr) {
        obj.insert("mem_1", param.memNo1.value());
    }
    if (param.memNo2 != nullptr) {
        obj.insert("mem_2", param.memNo2.value());
    }
    if (param.memNo3 != nullptr) {
        obj.insert("mem_3", param.memNo3.value());
    }
    obj.insert("manual_assist", param.manualAssist);
    obj.insert("enable_confirm", param.enableConfirm);
    obj.insert("enable_update_sem_condition", param.enableUpdateSemCondition);
    obj.insert("enable_axis", param.enableAxis);
}
int fromAxisAlignData(
    const QJsonValue& val, AxisAlignModel& param) {
    QJsonObject obj;
    int errCode = jValToObject(obj, val);
    if (errCode) {
        return errCode;
    }
    if (obj.isEmpty()) {
        return 0;
    }
    errCode = fromAxisAlignContent(obj["align_focus"], param.alignContentParam);
    if (errCode) {
        return errCode;
    }
    errCode = fromAxisAlignContent(obj["stigma_focus"], param.stigmaContentParam);
    if (errCode) {
        return errCode;
    }
    errCode = fromAxisChipParam(obj["chip"], param.chipParam);
    if (errCode) {
        return errCode;
    }
    errCode = jValToPack(param.memNo1, obj["mem_1"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToPack(param.memNo2, obj["mem_2"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToPack(param.memNo3, obj["mem_3"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.manualAssist, obj["manual_assist"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToBool(param.enableConfirm, obj["enable_confirm"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToBool(param.enableUpdateSemCondition, obj["enable_update_sem_condition"]);
    if (errCode) {
        return errCode;
    }
    return jValToBool(param.enableAxis, obj["enable_axis"]);
}

void axisAlignModelToJson(
    QString& json, const AxisAlignModel& data) {
    QJsonDocument doc;
    QJsonObject obj;
    putAxisAlignData(obj, data);
    doc.setObject(obj);
    json = doc.toJson(QJsonDocument::Compact);
}
int axisAlignModelFromJson(
    const QString& json, AxisAlignModel& data) {
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8(), &err);
    if (err.error != QJsonParseError::NoError) {
        qCritical() << "covert axis alignment on wafer data from json err: " << err.error << ", "
                    << err.errorString();
        return JSON_CONVERT_ERR_CODE;
    }
    return fromAxisAlignData(doc.object(), data);
}
