#include "memory_condition_model_convert.h"
#include <QJsonObject>
#include <QJsonValue>
#include "model_data_convert_helper.h"
#include "memory_condition_model.h"
#include <QJsonParseError>

void putMemoryModelSignalData(QJsonObject& obj, const MemoryModelSignalData& param) {
    if (param.signalPort == nullptr || param.signalPort < 0) {
        if (obj.size() > 0) {
            obj = QJsonObject();
        }
        return;
    }
    obj.insert("signal_port", param.signalPort.value());
    obj.insert("signal_name", param.signalName);
    obj.insert("brightness", param.brightness.toMinString());
    obj.insert("contrast", param.contrast.toMinString());
}

int fromMemoryModelSignalData(const QJsonValue& val, MemoryModelSignalData& param) {
    QJsonObject obj;
    int errCode = jValToObject(obj, val);
    if (errCode) return errCode;
    if (obj.isEmpty()) {
        param = MemoryModelSignalData{};
        return 0;
    }
    errCode = jValToPack<int>(param.signalPort, obj["signal_port"]);
    if (errCode) return errCode;
    errCode = jValToString(param.signalName, obj["signal_name"]);
    if (errCode) return errCode;
    errCode = jValToNumber(param.brightness, obj["brightness"]);
    if (errCode) return errCode;
    return jValToNumber(param.contrast, obj["contrast"]);
}

void toMemoryConditionDataModelJson(QString& json, const MemoryModelData& param) {
    QJsonDocument doc;
    QJsonObject obj;

    if (param.ip != nullptr)
        obj.insert("ip", param.ip.toMinString());
    if (param.eht != nullptr)
        obj.insert("eht", param.eht.toMinString());
    if (param.stageZ != nullptr)
        obj.insert("stage_z", param.stageZ.toMinString());
    if (param.apertureX != nullptr)
        obj.insert("apt_x", param.apertureX.toMinString());
    if (param.apertureY != nullptr)
        obj.insert("apt_y", param.apertureY.toMinString());
    if (param.workDistance != nullptr)
        obj.insert("wd", param.workDistance.toMinString());
    if (param.stigX != nullptr)
        obj.insert("stig_x", param.stigX.toMinString());
    if (param.stigY != nullptr)
        obj.insert("stig_y", param.stigY.toMinString());
    if (param.brightness != nullptr)
        obj.insert("brightness", param.brightness.toMinString());
    if (param.contrast != nullptr)
        obj.insert("contrast", param.contrast.toMinString());
    if (param.mixSignal != nullptr)
        obj.insert("mix_signal", param.mixSignal.toMinString());
    if (!param.dateTime.isNull()) {
        qint64 v;
        dateTimeToNumber(v, param.dateTime);
        obj.insert("date_time", v);
    }
    if (param.aptNum != nullptr)
        obj.insert("apt_num", param.aptNum.toString());
    if (param.mixing != nullptr)
        obj.insert("mixing", param.mixing.isTrue());
    obj.insert("auto_update", param.autoUpdate);
    obj.insert("is_proble", param.isProble);
    QJsonObject sObj;
    putMemoryModelSignalData(sObj, param.signalA);
    if (sObj.size() > 0) {
        obj.insert("signal_a", sObj);
    }
    if (param.mixing != nullptr && param.mixing.isTrue()) {
        putMemoryModelSignalData(sObj, param.signalB);
        if (sObj.size() > 0) {
            obj.insert("signal_b", sObj);
        }
    }

    doc.setObject(obj);
    json = doc.toJson(QJsonDocument::Compact);
}

int fromMemoryConditionDataModelJson(const QString& json, MemoryModelData& param) {
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8(), &err);
    if (err.error != QJsonParseError::NoError) {
        qCritical() << "covert memory condition data model from entity err: " << err.error << ", "
                    << err.errorString();
        return JSON_CONVERT_ERR_CODE;
    }
    if (!doc.isObject()) {
        return JSON_CONVERT_TYPE_ERR_CODE;
    }
    int errCode = jValToNumber(param.ip, doc["ip"]);
    if (errCode) return errCode;
    errCode = jValToNumber(param.eht, doc["eht"]);
    if (errCode) return errCode;
    errCode = jValToNumber(param.stageZ, doc["stage_z"]);
    if (errCode) return errCode;
    errCode = jValToNumber(param.apertureX, doc["apt_x"]);
    if (errCode) return errCode;
    errCode = jValToNumber(param.apertureY, doc["apt_y"]);
    if (errCode) return errCode;
    errCode = jValToNumber(param.workDistance, doc["wd"]);
    if (errCode) return errCode;
    errCode = jValToNumber(param.stigX, doc["stig_x"]);
    if (errCode) return errCode;
    errCode = jValToNumber(param.stigY, doc["stig_y"]);
    if (errCode) return errCode;
    errCode = jValToNumber(param.brightness, doc["brightness"]);
    if (errCode) return errCode;
    errCode = jValToNumber(param.contrast, doc["contrast"]);
    if (errCode) return errCode;
    errCode = jValToNumber(param.mixSignal, doc["mix_signal"]);
    if (errCode) return errCode;
    errCode = numberToDateTime(param.dateTime, doc["date_time"]);
    if (errCode) return errCode;
    errCode =  jValToPack(param.aptNum, doc["apt_num"]);
    if (errCode) return errCode;
    errCode = jValToBoolean(param.mixing, doc["mixing"]);
    if (errCode) return errCode;
    errCode = jValToBool(param.autoUpdate, doc["auto_update"]);
    if (errCode) return errCode;
    errCode = jValToBool(param.isProble, doc["is_proble"]);
    if (errCode) return errCode;
    errCode = fromMemoryModelSignalData(doc["signal_a"], param.signalA);
    if (errCode) return errCode;
    if (param.mixing != nullptr && param.mixing.isTrue())
        return fromMemoryModelSignalData(doc["signal_b"], param.signalB);
    return 0;
}
