#include "recipe_data_convert.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include "model_data_convert_helper.h"
#include "recipe_data_model.h"
#include "exception_code.h"

void putFem(
    QJsonObject& obj, const FemMeasureModel& param) {
    if (param.foucsStart != nullptr) {
        obj.insert("focus_start", param.foucsStart.toQStringAndKeepFullDecimal());
    }
    if (param.foucsStep != nullptr) {
        obj.insert("focus_step", param.foucsStep.toQStringAndKeepFullDecimal());
    }
    if (param.exposureStart != nullptr) {
        obj.insert("exposure_start", param.exposureStart.toQStringAndKeepFullDecimal());
    }
    if (param.exposureStep != nullptr) {
        obj.insert("exposure_step", param.exposureStep.toQStringAndKeepFullDecimal());
    }
    obj.insert("fem_type", param.femType);
    obj.insert("start_chip", param.startChip);
}
int fromFem(
    const QJsonValue& val, FemMeasureModel& param) {
    QJsonObject obj;
    int errCode = jValToObject(obj, val);
    if (errCode) {
        return errCode;
    }
    if (obj.isEmpty()) {
        return 0;
    }
    errCode = jValToNumber(param.foucsStart, obj["focus_start"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToNumber(param.foucsStep, obj["focus_step"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToNumber(param.exposureStart, obj["exposure_start"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToNumber(param.exposureStep, obj["exposure_step"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.femType, obj["fem_type"]);
    if (errCode) {
        return errCode;
    }
    return jValToInt(param.startChip, obj["start_chip"]);
}

void putOmSemScopeCorrectionModel(
    QJsonObject& obj, const OmSemScopeCorrectionModel& param) {
    if (param.targetRangeX != nullptr)
        obj.insert("target_range_x", param.targetRangeX.toQStringAndKeepFullDecimal());
    if (param.targetRangeY != nullptr)
        obj.insert("target_range_y", param.targetRangeY.toQStringAndKeepFullDecimal());
    if (param.targetRangeDiffX != nullptr)
        obj.insert("target_range_diff_x", param.targetRangeDiffX.toQStringAndKeepFullDecimal());
    if (param.targetRangeDiffY != nullptr)
        obj.insert("target_range_diff_y", param.targetRangeDiffY.toQStringAndKeepFullDecimal());
    obj.insert("allow_adjustment", param.allowAdjustment);
    obj.insert("enalbe_om_sem_scope_correction", param.enableOmSemScopeCorrection);
}
int fromOmSemScopeCorrectionModel(
    const QJsonValue& val, OmSemScopeCorrectionModel& param) {
    QJsonObject obj;
    int errCode = jValToObject(obj, val);
    if (errCode) {
        return errCode;
    }
    if (obj.isEmpty()) {
        return 0;
    }
    errCode = jValToNumber(param.targetRangeX, obj["target_range_x"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToNumber(param.targetRangeY, obj["target_range_y"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToNumber(param.targetRangeDiffX, obj["target_range_diff_x"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToNumber(param.targetRangeDiffY, obj["target_range_diff_y"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToBool(param.allowAdjustment, obj["allow_adjustment"]);
    if (errCode) {
        return errCode;
    }
    return jValToBool(param.enableOmSemScopeCorrection, obj["enalbe_om_sem_scope_correction"]);
}

void putZSensorOffsetModel(
    QJsonObject& obj, const ZSensorOffsetModel& param) {
    if (param.offset != nullptr) {
        obj.insert("offset", param.offset.value());
    }
    if (param.lsb != nullptr) {
        obj.insert("lsb", param.lsb.value());
    }
    obj.insert("allow_adjustment", param.allowAdjustment);
    obj.insert("enable_z_sensor_offset", param.enableZSensorOffset);
}
int fromZSensorOffsetModel(
    const QJsonValue& val, ZSensorOffsetModel& param) {
    QJsonObject obj;
    int errCode = jValToObject(obj, val);
    if (errCode) {
        return errCode;
    }
    if (obj.isEmpty()) {
        return 0;
    }
    errCode = jValToPack(param.offset, obj["offset"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToPack(param.lsb, obj["lsb"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToBool(param.allowAdjustment, obj["allow_adjustment"]);
    if (errCode) {
        return errCode;
    }
    return jValToBool(param.enableZSensorOffset, obj["enable_z_sensor_offset"]);
}

void putCDCheckModel(
    QJsonObject& obj, const CDCheckModel& param) {
    obj.insert("ref_msr_id", qint64(param.refMsrId));
    if (param.nextMsrNo != nullptr) {
        obj.insert("next_msr_no", param.nextMsrNo.value());
    }
    //TODO 其他参数未知
    obj.insert("enable_cd_check", param.enableCDCheck);
}
int fromCDCheckModel(
    const QJsonValue& val, CDCheckModel& param) {
    QJsonObject obj;
    int errCode = jValToObject(obj, val);
    if (errCode) {
        return errCode;
    }
    if (obj.isEmpty()) {
        return 0;
    }
    errCode = jValToInt(param.refMsrId, obj["ref_msr_id"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToPack(param.nextMsrNo, obj["next_msr_no"]);
    if (errCode) {
        return errCode;
    }
    return jValToBool(param.enableCDCheck, obj["enable_cd_check"]);
}

void putStageMonitorModel(
    QJsonObject& obj, const StageMonitorModel& param) {
    if (param.range != nullptr)
        obj.insert("range", param.range.toQStringAndKeepFullDecimal());
    obj.insert("enable_stage_monitor", param.enableStageMonitor);
}
int fromStageMonitorModel(
    const QJsonValue& val, StageMonitorModel& param) {
    QJsonObject obj;
    int errCode = jValToObject(obj, val);
    if (errCode) {
        return errCode;
    }
    if (obj.isEmpty()) {
        return 0;
    }
    errCode = jValToNumber(param.range, obj["range"]);
    if (errCode) {
        return errCode;
    }
    return jValToBool(param.enableStageMonitor, obj["enable_stage_monitor"]);
}

void putPgMonitorModel(
    QJsonObject& obj, const PgMonitorModel& param) {
    obj.insert("enable_pg_monitor", param.enablePgMonitor);
}
int fromPgMonitorModel(
    const QJsonValue& val, PgMonitorModel& param) {
    QJsonObject obj;
    int errCode = jValToObject(obj, val);
    if (errCode) {
        return errCode;
    }
    if (obj.isEmpty()) {
        return 0;
    }
    return jValToBool(param.enablePgMonitor, obj["enable_pg_monitor"]);
}

void putMaintenance(
    QJsonObject& obj, const MaintenanceDataModel& param) {
    QJsonObject oObj;
    QJsonObject zObj;
    QJsonObject cObj;
    QJsonObject sObj;
    QJsonObject pObj;
    putOmSemScopeCorrectionModel(oObj, param.omSemScopeCorrection);
    putZSensorOffsetModel(zObj, param.zSensorOffset);
    putCDCheckModel(cObj, param.cdCheck);
    putStageMonitorModel(sObj, param.stageMonitor);
    putPgMonitorModel(pObj, param.pgMonitor);
    obj.insert("om_sem_scope_correction", oObj);
    obj.insert("z_sensor_offset", zObj);
    obj.insert("cd_check", cObj);
    obj.insert("stage_monitor", sObj);
    obj.insert("pg_monitor", pObj);
}
int fromMaintenance(
    const QJsonValue& val, MaintenanceDataModel& param) {
    QJsonObject obj;
    int errCode = jValToObject(obj, val);
    if (errCode) {
        return errCode;
    }
    if (obj.isEmpty()) {
        return 0;
    }
    errCode = fromOmSemScopeCorrectionModel(obj["om_sem_scope_correction"],
                                            param.omSemScopeCorrection);
    if (errCode) {
        return errCode;
    }
    errCode = fromZSensorOffsetModel(obj["z_sensor_offset"], param.zSensorOffset);
    if (errCode) {
        return errCode;
    }
    errCode = fromCDCheckModel(obj["cd_check"], param.cdCheck);
    if (errCode) {
        return errCode;
    }
    errCode = fromStageMonitorModel(obj["stage_monitor"], param.stageMonitor);
    if (errCode) {
        return errCode;
    }
    return fromPgMonitorModel(obj["pg_monitor"], param.pgMonitor);
}

void putRecipeMeasureResultModel(
    QJsonObject& obj, const RecipeMeasureResultModel& param) {
    obj.insert("disk_save", param.diskSave);
    obj.insert("send_to_host", param.sendToHost);
    obj.insert("print_out", param.printOut);
    obj.insert("network_transfer", param.networkTransfer);
    obj.insert("confirm", param.confirm);
    if (param.succRate != nullptr) {
        obj.insert("succ_rate", param.succRate.value());
    }
    obj.insert("method", param.method);
    obj.insert("limit_check", param.limitCheck);
}
int fromRecipeMeasureResultModel(
    const QJsonValue& val, RecipeMeasureResultModel& param) {
    QJsonObject obj;
    int errCode = jValToObject(obj, val);
    if (errCode) {
        return errCode;
    }
    if (obj.isEmpty()) {
        return 0;
    }
    errCode = jValToBool(param.diskSave, obj["disk_save"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToBool(param.sendToHost, obj["send_to_host"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToBool(param.printOut, obj["print_out"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToBool(param.networkTransfer, obj["network_transfer"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.confirm, obj["confirm"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToPack(param.succRate, obj["succ_rate"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.method, obj["method"]);
    if (errCode) {
        return errCode;
    }
    return jValToInt(param.limitCheck, obj["limit_check"]);
}

void putRecipeProcessModel(
    QJsonObject& obj, const RecipeProcessModel& param) {
    obj.insert("execute_mode", param.executeMode);
    obj.insert("manual_assist", param.manualAssist);
    obj.insert("image_save", param.imageSave);
}
int fromRecipeProcessModel(
    const QJsonValue& val, RecipeProcessModel& param) {
    QJsonObject obj;
    int errCode = jValToObject(obj, val);
    if (errCode) {
        return errCode;
    }
    if (obj.isEmpty()) {
        return 0;
    }
    errCode = jValToInt(param.executeMode, obj["execute_mode"]);
    errCode = jValToInt(param.manualAssist, obj["manual_assist"]);
    return jValToInt(param.imageSave, obj["image_save"]);
}

void putRecipeEnvAdditionSettingModel(
    QJsonObject& obj, const RecipeEnvAdditionSettingModel& param) {
    obj.insert("edit_idp_after_wafer_align", param.editIdpAfterWaferAlign);
    obj.insert("preset_bc", param.presetBC);
    obj.insert("retry_sem_align_with_afcc", param.retrySemAlignWithAFCC);
    obj.insert("retry_address_with_afcc", param.retryAddressWithAFCC);
    obj.insert("retry_pattern_reg_with_afcc", param.retryPatternRegWithAFCC);
    obj.insert("re_measure_error_site", param.reMeasureErrorSite);
    obj.insert("periodic_wafer_align", param.periodicWaferAlign);
    if (param.ionizerIrradition != nullptr) {
        obj.insert("ionizer_irradition", param.ionizerIrradition.value());
    }
}
int fromRecipeEnvAdditionSettingModel(
    const QJsonValue& val, RecipeEnvAdditionSettingModel& param) {
    QJsonObject obj;
    int errCode = jValToObject(obj, val);
    if (errCode) {
        return errCode;
    }
    if (obj.isEmpty()) {
        return 0;
    }
    errCode = jValToBool(param.editIdpAfterWaferAlign, obj["edit_idp_after_wafer_align"]);
    errCode = jValToBool(param.presetBC, obj["preset_bc"]);
    errCode = jValToBool(param.retrySemAlignWithAFCC, obj["retry_sem_align_with_afcc"]);
    errCode = jValToBool(param.retryAddressWithAFCC, obj["retry_address_with_afcc"]);
    errCode = jValToBool(param.retryPatternRegWithAFCC, obj["retry_pattern_reg_with_afcc"]);
    errCode = jValToBool(param.reMeasureErrorSite, obj["re_measure_error_site"]);
    errCode = jValToBool(param.periodicWaferAlign, obj["periodic_wafer_align"]);
    return jValToPack(param.ionizerIrradition, obj["ionizer_irradition"]);
}

void putRecipeEnvironmentModel(
    QJsonObject& obj, const RecipeEnvironmentModel& param) {
    QJsonObject resObj;
    QJsonObject waferObj;
    QJsonObject addressObj;
    QJsonObject exeObj;
    QJsonObject addObj;
    putRecipeMeasureResultModel(resObj, param.measureResult);
    putRecipeProcessModel(waferObj, param.waferAlign);
    putRecipeProcessModel(addressObj, param.measureAdderss);
    putRecipeProcessModel(exeObj, param.measureExe);
    putRecipeEnvAdditionSettingModel(addObj, param.additionSetting);
    obj.insert("measure_result", resObj);
    obj.insert("wafer_align", waferObj);
    obj.insert("measure_address", addressObj);
    obj.insert("measure_exe", exeObj);
    obj.insert("addition_setting", addObj);
}
int fromRecipeEnvironmentModel(
    const QJsonValue& val, RecipeEnvironmentModel& param) {
    QJsonObject obj;
    int errCode = jValToObject(obj, val);
    if (errCode) {
        return errCode;
    }
    if (obj.isEmpty()) {
        return 0;
    }
    errCode = fromRecipeMeasureResultModel(obj["measure_result"], param.measureResult);
    if (errCode) {
        return errCode;
    }
    errCode = fromRecipeProcessModel(obj["wafer_align"], param.waferAlign);
    if (errCode) {
        return errCode;
    }
    errCode = fromRecipeProcessModel(obj["measure_address"], param.measureAdderss);
    if (errCode) {
        return errCode;
    }
    errCode = fromRecipeProcessModel(obj["measure_exe"], param.measureExe);
    if (errCode) {
        return errCode;
    }
    return fromRecipeEnvAdditionSettingModel(obj["addition_setting"], param.additionSetting);
}

void putRecipeDataModel(
    QJsonObject& obj, const RecipeDataModel& param) {
    QJsonObject fObj;
    QJsonObject mainObj;
    QJsonObject envObj;
    putFem(fObj, param.fem);
    putMaintenance(mainObj, param.maintenance);
    putRecipeEnvironmentModel(envObj, param.env);
    obj.insert("fem", fObj);
    obj.insert("maintenance", mainObj);
    obj.insert("env", envObj);
    if (param.selectedSlots.size() > 0) {
        QJsonArray slotList;
        for (const int& s : param.selectedSlots) {
            slotList.append(s);
        }
        obj.insert("selected_slots", slotList);
    }

    //这4个ID在表中有独立字段，无需放入到json中保存
    // obj.insert("class_id", qint64(param.classId));
    // obj.insert("recipe_id", qint64(param.recipeId));
    // obj.insert("idw_id", qint64(param.idwId));
    // obj.insert("idp_id", qint64(param.idpId));
    if (param.semConditionNum != nullptr) {
        obj.insert("sem_condition_num", param.semConditionNum.value());
    }
    obj.insert("wafer_property", param.waferProperty);
    obj.insert("special", param.special);
    obj.insert("auto_calibration", param.autoCalibration);
}
int fromRecipeDataModel(
    const QJsonDocument& obj, RecipeDataModel& param) {
    int errCode;
    if (!obj.isObject()) {
        return JSON_CONVERT_TYPE_ERR_CODE;
    }
    if (obj.isEmpty()) {
        return 0;
    }
    errCode = fromFem(obj["fem"], param.fem);
    if (errCode) {
        return errCode;
    }
    errCode = fromMaintenance(obj["maintenance"], param.maintenance);
    if (errCode) {
        return errCode;
    }
    errCode = fromRecipeEnvironmentModel(obj["env"], param.env);
    if (errCode) {
        return errCode;
    }
    QJsonArray slotList;
    jValToArray(slotList, obj["selected_slots"]);
    param.selectedSlots.clear();
    if (slotList.size() > 0) {
        param.selectedSlots.reserve(slotList.size());
        int v;
        for (auto it = slotList.begin(); it != slotList.end(); ++it) {
            errCode = jValToInt(v, *it);
            if (errCode) {
                return errCode;
            }
            param.selectedSlots.append(v);
        }
    }
    errCode = jValToPack(param.semConditionNum, obj["sem_condition_num"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.waferProperty, obj["wafer_property"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.special, obj["special"]);
    if (errCode) {
        return errCode;
    }
    return jValToBool(param.autoCalibration, obj["auto_calibration"]);
}

void recipeDataToJson(
    QString& json, const RecipeDataModel& param) {
    QJsonDocument doc;
    QJsonObject obj;
    putRecipeDataModel(obj, param);
    doc.setObject(obj);
    json = doc.toJson(QJsonDocument::Compact);
}

int recipeDataFromJson(
    const QString& json, RecipeDataModel& param) {
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8(), &err);
    if (err.error != QJsonParseError::NoError) {
        qCritical() << "Recipe data convert from json error: " << err.errorString();
        return JSON_CONVERT_ERR_CODE;
    }
    return fromRecipeDataModel(doc, param);
}

void recipeEnvironmentToJson(
    QString& json, const RecipeEnvironmentModel& env) {
    QJsonDocument doc;
    QJsonObject obj;
    putRecipeEnvironmentModel(obj, env);
    doc.setObject(obj);
    json = doc.toJson(QJsonDocument::Compact);
}
int recipeEnvironmentFromJson(
    const QString& json, RecipeEnvironmentModel& env) {
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8(), &err);
    if (err.error != QJsonParseError::NoError) {
        qCritical() << "Measurement Environment data convert from json error: "
                    << err.errorString();
        return JSON_CONVERT_ERR_CODE;
    }
    if (!doc.isObject()) {
        return JSON_CONVERT_TYPE_ERR_CODE;
    }
    return fromRecipeEnvironmentModel(QJsonValue(doc.object()), env);
}
