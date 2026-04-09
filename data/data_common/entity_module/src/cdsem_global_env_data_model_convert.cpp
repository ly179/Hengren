#include "cdsem_global_env_data_model_convert.h"
#include "model_data_convert_helper.h"
#include "cdsem_global_env_data_model.h"
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

void calibrationPyParamToJson(QString& json, const CDSEM_GlobalCalibrationPyParamModel& param) {
    QJsonObject obj;
    obj.insert("view_stage_angle", FloatNumber10(param.getViewStageAngle()).toMinString());
    obj.insert("view_stage_scale", FloatNumber10(param.getViewStageScale()).toMinString());
    obj.insert("view_stage_scale_amp", FloatNumber10(param.getViewStageScaleAmp()).toMinString());
    obj.insert("view_stage_angle_amp", FloatNumber10(param.getViewStageAngleAmp()).toMinString());
    obj.insert("view_stage_phase", FloatNumber10(param.getViewStagePhase()).toMinString());

    QJsonArray array;
    simpleListToJArray<10>(param.getViewStageTransform(), array);
    obj.insert("view_stage_transform", array);
    simpleListToJArray<10>(param.getViewOMStageTransform(), array);
    obj.insert("view_om_stage_transform", array);

    simpleListToJArray<10>(param.getWaferTransformMatrix(), array);
    obj.insert("wafer_transform_matrix", array);
    simpleListToJArray<10>(param.getWaferUnTransformMatrix(), array);
    obj.insert("wafer_unTransform_matrix", array);
    simpleListToJArray<10>(param.getWdTrance(), array);
    obj.insert("workdistance_Transform_matrix", array);

    obj.insert("view_stage_scale_x", FloatNumber10(param.getViewStageScaleX()).toMinString());
    obj.insert("view_stage_scale_y", FloatNumber10(param.getViewStageScaleY()).toMinString());

    obj.insert("view_stage_angle_x", FloatNumber10(param.getViewStageAngleX()).toMinString());
    obj.insert("view_stage_angle_y", FloatNumber10(param.getViewStageAngleY()).toMinString());

    QString str;
    floatPointToJValStr<10>(str, param.getRotationCenter());
    obj.insert("rotation_center", str);

    obj.insert("wafer_angle", FloatNumber10(param.getWaferAngle()).toMinString());

    floatPointToJValStr<10>(str, param.getWaferCenter());
    obj.insert("wafer_center", str);
    floatPointToJValStr<10>(str, param.getFlatCenter());
    obj.insert("flat_center", str);

    obj.insert("core_angle", FloatNumber10(param.getCoreAngle()).toMinString());
    obj.insert("stage_limit_range", FloatNumber10(param.getStageLimitRange()).toMinString());

    floatPointToJValStr<10>(str, param.getStageLimit());
    obj.insert("stage_limit", str);

    QJsonDocument doc;
    doc.setObject(obj);
    json = doc.toJson(QJsonDocument::Compact);
}

int calibrationPyParamFromJson(const QString& json, CDSEM_GlobalCalibrationPyParamModel& param) {
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8(), &err);
    if (err.error != QJsonParseError::NoError) {
        qCritical() << "covert calibration py param from json err: " << err.error << ", "
                    << err.errorString();
        return JSON_CONVERT_ERR_CODE;
    }
    QJsonObject obj = doc.object();
    FloatNumber10 f;
    int errCode = jValToNumber(f, obj["view_stage_angle"]);
    if (errCode) return errCode;
    param.setViewStageAngle(f == nullptr ? 0 : f.toDouble());
    errCode = jValToNumber(f, obj["view_stage_scale"]);
    if (errCode) return errCode;
    param.setViewStageScale(f == nullptr ? 0 : f.toDouble());
    errCode = jValToNumber(f, obj["view_stage_scale_amp"]);
    if (errCode) return errCode;
    param.setViewStageScaleAmp(f == nullptr ? 0 : f.toDouble());
    errCode = jValToNumber(f, obj["view_stage_angle_amp"]);
    if (errCode) return errCode;
    param.setViewStageAngleAmp(f == nullptr ? 0 : f.toDouble());
    errCode = jValToNumber(f, obj["view_stage_phase"]);
    if (errCode) return errCode;
    param.setViewStagePhase(f == nullptr ? 0 : f.toDouble());

    QList<double> list;
    errCode = jArrayToSimpleList<10, double>(list, obj["view_stage_transform"]);
    if (errCode) return errCode;
    param.setViewStageTransform(list);
    errCode = jArrayToSimpleList<10, double>(list, obj["view_om_stage_transform"]);
    if (!errCode) {
        param.setViewOMStageTransform(list);
    }

    errCode = jArrayToSimpleList<10, double>(list, obj["wafer_transform_matrix"]);
    if (errCode) return errCode;
    param.setWaferTransformMatrix(list);
    errCode = jArrayToSimpleList<10, double>(list, obj["wafer_unTransform_matrix"]);
    if (errCode) return errCode;
    param.setWaferUnTransformMatrix(list);
    QList<double> listDouble;
    errCode = jArrayToSimpleList<10, double>(listDouble, obj["workdistance_Transform_matrix"]);
    if (!errCode)
        param.setWdTrance(listDouble);

    errCode = jValToNumber(f, obj["view_stage_scale_x"]);
    if (errCode) return errCode;
    param.setViewStageScaleX(f == nullptr ? 0 : f.toDouble());
    errCode = jValToNumber(f, obj["view_stage_scale_y"]);
    if (errCode) return errCode;
    param.setViewStageScaleY(f == nullptr ? 0 : f.toDouble());
    errCode = jValToNumber(f, obj["view_stage_angle_x"]);
    if (errCode) return errCode;
    param.setViewStageAngleX(f == nullptr ? 0 : f.toDouble());
    errCode = jValToNumber(f, obj["view_stage_angle_y"]);
    if (errCode) return errCode;
    param.setViewStageAngleY(f == nullptr ? 0 : f.toDouble());

    QPointF pos;
    errCode = jValStrToFloatPoint<10>(pos, obj["rotation_center"]);
    if (errCode) return errCode;
    param.setRotationCenter(pos);

    errCode = jValToNumber(f, obj["wafer_angle"]);
    if (errCode) return errCode;
    param.setWaferAngle(f == nullptr ? 0 : f.toDouble());

    errCode = jValStrToFloatPoint<10>(pos, obj["wafer_center"]);
    if (errCode) return errCode;
    param.setWaferCenter(pos);
    errCode = jValStrToFloatPoint<10>(pos, obj["flat_center"]);
    if (errCode) return errCode;
    param.setFlatCenter(pos);

    errCode = jValToNumber(f, obj["core_angle"]);
    if (errCode) return errCode;
    param.setCoreAngle(f == nullptr ? 0 : f.toDouble());
    errCode = jValToNumber(f, obj["stage_limit_range"]);
    if (errCode) return errCode;
    param.setStageLimitRange(f == nullptr ? 0 : f.toDouble());

    errCode = jValStrToFloatPoint<10>(pos, obj["stage_limit"]);
    if (errCode) return errCode;
    param.setStageLimit(pos);
    return 0;
}
