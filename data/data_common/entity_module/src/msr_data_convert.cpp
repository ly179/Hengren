#include "msr_data_convert.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include "model_data_convert_helper.h"
#include "msr_data_model.h"

void putOperationResultToJson(const OperationResult& result, QJsonObject& obj) {
    if (result.errCode != 0) {
        obj.insert("err_code", result.errCode);
        obj.insert("err_msg", result.errMsg);
    }
}

int operationResultFromJson(OperationResult& result, const QJsonValue& value, QJsonObject& obj) {
    int errCode = jValToObject(obj, value);
    if (errCode) {
        return errCode;
    }
    if (obj.isEmpty()) {
        return 0;
    }
    errCode = jValToInt(result.errCode, obj["err_code"]);
    if (errCode) return errCode;
    return jValToString(result.errMsg, obj["err_msg"]);
}

void putPatternResultToJson(QJsonObject& obj, const MSR_PatternResult& result) {
    obj.insert("image_id", qint64(result.imageId));
    putOperationResultToJson(result, obj);
}

int patternResultFromJson(const QJsonValue& value, MSR_PatternResult& result) {
    QJsonObject obj;
    int errCode = operationResultFromJson(result, value, obj);
    if (errCode) return errCode;
    if (obj.isEmpty()) {
        result = MSR_PatternResult{};
        return 0;
    }
    return jValToInt(result.imageId, obj["image_id"]);
}

void putExeResultToJson(const MSR_MP_ExeResult& result, QJsonObject& obj) {
    if (result.result != nullptr)
        obj.insert("result", result.result.toMinString());
    putOperationResultToJson(result, obj);
}

int exeResultFromJson(MSR_MP_ExeResult& result, const QJsonValue& value) {
    QJsonObject obj;
    int errCode = operationResultFromJson(result, value, obj);
    if (errCode) return errCode;
    if (obj.isEmpty()) {
        result = MSR_MP_ExeResult{};
        return 0;
    }
    return jValToNumber(result.result, obj["result"]);
}

void putMetaDataToJson(QJsonObject& obj, const MSR_MP_MetaData& result) {
    obj.insert("mp_name", result.mpName);
    QJsonArray array;
    simpleListToJArray(result.dNumbers, array);
    if (array.size() > 0) {
        obj.insert("d_numbers", array);
    }
}

int metaDataFromJson(const QJsonValue& value, MSR_MP_MetaData& result) {
    QJsonObject obj;
    int errCode = jValToObject(obj, value);
    if (errCode) return errCode;
    if (obj.isEmpty()) {
        result = MSR_MP_MetaData{};
        return 0;
    }
    errCode = jValToString(result.mpName, obj["mp_name"]);
    if (errCode) return errCode;
    return jArrayToSimpleList(result.dNumbers, obj["d_numbers"]);
}

void putMSR_AP_ChipToJson(const MSR_AP_Chip& result, QJsonObject& obj) {
    obj.insert("p_num", result.pNum);
    obj.insert("chip_number", result.chipNumber);
    QJsonObject rObj;
    putPatternResultToJson(rObj, result.apPatternResult);
    obj.insert("pattern", rObj);
}

int msr_AP_ChipFromJson(MSR_AP_Chip& result, const QJsonValue& value) {
    QJsonObject obj;
    int errCode = jValToObject(obj, value);
    if (errCode) return errCode;
    if (obj.isEmpty()) {
        result = MSR_AP_Chip{};
        return 0;
    }
    errCode = jValToInt(result.pNum, obj["p_num"]);
    if (errCode) return errCode;
    errCode = jValToInt(result.chipNumber, obj["chip_number"]);
    if (errCode) return errCode;
    return patternResultFromJson(obj["pattern"], result.apPatternResult);
}

void putMSR_MP_DataToJson(QJsonObject& obj, const MSR_MP_Data& result) {
    obj.insert("measure_image_id", qint64(result.measureImageId));
    QJsonObject mObj;
    if (result.addressPatternResults.size() > 0) {
        QJsonArray addressArray;
        simpleListToJArray<MSR_PatternResult>(result.addressPatternResults, addressArray, putPatternResultToJson);
        obj.insert("add_pattern_list", addressArray);
    }
    putPatternResultToJson(mObj, result.mpPatternResult);
    obj.insert("mp_pattern", mObj);
    QJsonObject results;
    QJsonObject rObj;
    for (auto it = result.mpResults.begin(); it != result.mpResults.end(); ++ it) {
        putExeResultToJson(*it, rObj);
        results.insert(QString::number(it.key()), rObj);
        rObj = QJsonObject {};
    }
    obj.insert("mp_results", results);
    if (result.createTime.isValid()) {
        obj.insert("create_time", result.createTime.toMSecsSinceEpoch());
    }
}

void putMSR_MP_ChipMetaDataToJson(const MSR_MP_Chip_MetaData& result, QJsonObject& obj) {
    obj.insert("chip_number", result.chipNumber);
    obj.insert("p_num", qint64(result.pNum));
    QJsonArray array;
    if (result.metaDatas.size() > 0) {
        simpleListToJArray<MSR_MP_MetaData>(result.metaDatas, array, putMetaDataToJson);
        obj.insert("mate_datas", array);
    }
}

void putMSR_MP_ChipToJson(const MSR_MP_Chip& result, QJsonObject& obj) {
    QString str;
    pointToJValStr(str, result.chipView);
    obj.insert("chip_view", str);
    QJsonObject metaObj;
    putMSR_MP_ChipMetaDataToJson(result.metaData, metaObj);
    obj.insert("meta_data", metaObj);
    QJsonArray array;
    if (result.list.size() > 0) {
        simpleListToJArray<MSR_MP_Data>(result.list, array, putMSR_MP_DataToJson);
        obj.insert("list", array);
    }
    if (result.validIndices.size() > 0) {
        QJsonObject vIndicesObj;
        for (auto it = result.validIndices.begin(); it != result.validIndices.end(); ++ it) {
            vIndicesObj.insert(QString::number(it.key()), it.value());
        }
        obj.insert("valid_indices", vIndicesObj);
    }
}

int msr_MP_DataFromJson(const QJsonValue& val, MSR_MP_Data& param) {
    QJsonObject obj;
    int errCode = jValToObject(obj, val);
    if (errCode) return errCode;
    if (obj.isEmpty()) {
        param = MSR_MP_Data{};
        return 0;
    }
    if (errCode) return errCode;
    errCode = jValToInt(param.measureImageId, obj["measure_image_id"]);
    if (errCode) return errCode;
    errCode = jArrayToSimpleList<MSR_PatternResult>(param.addressPatternResults, obj["add_pattern_list"], patternResultFromJson);
    if (errCode) return errCode;
    errCode = patternResultFromJson(obj["mp_pattern"], param.mpPatternResult);
    if (errCode) return errCode;
    param.mpResults.clear();
    QJsonValueConstRef mpResults = obj["mp_results"];
    if (mpResults.isNull() || mpResults.isUndefined()) {
        return 0;
    }
    if (!mpResults.isObject()) {
        return JSON_CONVERT_TYPE_ERR_CODE;
    }
    QJsonObject mpResultObj = mpResults.toObject();
    bool ok;
    int key;
    MSR_MP_ExeResult r;
    for (auto it = mpResultObj.begin(); it != mpResultObj.end(); ++ it) {
        key = it.key().toInt(&ok);
        if (!ok) {
            return JSON_CONVERT_TYPE_ERR_CODE;
        }
        errCode = exeResultFromJson(r, *it);
        if (errCode) return errCode;
        param.mpResults.insert(key, r);
    }
    qint64 millSec = 0;
    errCode = jValToInt<qint64>(millSec, obj["create_time"]);
    if (!errCode && millSec > 0) {
        param.createTime = QDateTime::fromMSecsSinceEpoch(millSec);
    } else if (param.createTime.isValid()){
        param.createTime = QDateTime();
    }
    return 0;
}

int msr_MP_ChipMetaDataFromJson(MSR_MP_Chip_MetaData& result, const QJsonValue& value) {
    QJsonObject obj;
    int errCode = jValToObject(obj, value);
    if (errCode) return errCode;
    if (obj.isEmpty()) {
        result = MSR_MP_Chip_MetaData{};
        return 0;
    }
    errCode = jValToInt(result.chipNumber, obj["chip_number"]);
    if (errCode) return errCode;
    errCode = jValToInt(result.pNum, obj["p_num"]);
    if (errCode) return errCode;

    return jArrayToSimpleList<MSR_MP_MetaData>(result.metaDatas, obj["mate_datas"], metaDataFromJson);
}

int msr_MP_ChipFromJson(MSR_MP_Chip& result, const QJsonValue& value) {
    QJsonObject obj;
    int errCode = jValToObject(obj, value);
    if (errCode) return errCode;
    if (obj.isEmpty()) {
        result = MSR_MP_Chip{};
        return 0;
    }
    errCode = msr_MP_ChipMetaDataFromJson(result.metaData, obj["meta_data"]);
    if (errCode) return errCode;
    errCode = jValStrToPoint(result.chipView, obj["chip_view"]);
    if (errCode) return errCode;
    errCode = jArrayToSimpleList<MSR_MP_Data>(result.list, obj["list"], msr_MP_DataFromJson);
    if (errCode) return errCode;
    QJsonObject vIndicesObj;
    errCode = jValToObject(vIndicesObj, obj["valid_indices"]);
    if (errCode) return errCode;
    if (result.validIndices.size() > 0) {
        result.validIndices.clear();
    }
    if (vIndicesObj.isEmpty()) {
        return 0;
    }
    int k, v;
    bool ok;
    for (auto it = vIndicesObj.begin(); it != vIndicesObj.end(); ++ it) {
        k = it.key().toInt(&ok);
        if (!ok) {
            return JSON_CONVERT_TYPE_ERR_CODE;
        }
        errCode = jValToInt(v, it.value());
        if (errCode) return errCode;
        result.validIndices.insert(k, v);
    }
    return 0;
}

void putMSR_ChipToJson(const MSR_Chip& result, QJsonObject& obj) {
    QJsonArray aArray;
    QJsonArray mArray;
    QJsonObject cObj;
    for (const MSR_AP_Chip& c : result.apList) {
        putMSR_AP_ChipToJson(c, cObj);
        aArray.append(cObj);
        cObj = QJsonObject {};
    }
    for (const MSR_MP_Chip& c : result.mpList) {
        putMSR_MP_ChipToJson(c, cObj);
        mArray.append(cObj);
        cObj = QJsonObject {};
    }
    obj.insert("ap_list", aArray);
    obj.insert("mp_list", mArray);
}

int msr_ChipFromJson(MSR_Chip& result, const QJsonValue& value) {
    QJsonObject obj;
    int errCode = jValToObject(obj, value);
    if (errCode) return errCode;
    if (obj.isEmpty()) {
        result = MSR_Chip{};
        return 0;
    }
    const QJsonValue& av = value["ap_list"];
    if (av.isNull() || av.isUndefined()) {
        result.apList.clear();
    } else if (!av.isArray()) {
        return JSON_CONVERT_TYPE_ERR_CODE;
    }
    QJsonArray array = av.toArray();
    MSR_AP_Chip ap;
    for (const QJsonValue& v : qAsConst(array)) {
        errCode = msr_AP_ChipFromJson(ap, v);
        if (errCode) return errCode;
        result.apList.append(ap);
        ap = MSR_AP_Chip {};
    }
    const QJsonValue& mv = value["mp_list"];
    if (mv.isNull() || mv.isUndefined()) {
        result.mpList.clear();
    } else if (!mv.isArray()) {
        return JSON_CONVERT_TYPE_ERR_CODE;
    }
    array = mv.toArray();
    MSR_MP_Chip mp;
    for (const QJsonValue& v : qAsConst(array)) {
        errCode = msr_MP_ChipFromJson(mp, v);
        if (errCode) return errCode;
        result.mpList.append(mp);
        mp = MSR_MP_Chip {};
    }
    return 0;
}

void msrModeDataToEntity(QString & json, const MSR_Chip & data) {
    QJsonDocument doc;
    QJsonObject obj;
    putMSR_ChipToJson(data, obj);
    doc.setObject(obj);
    json = doc.toJson(QJsonDocument::Compact);
}

int msrModeDataFromEntity(const QString & json, MSR_Chip & data) {
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8(), &err);
    if (err.error != QJsonParseError::NoError) {
        qCritical() << "MSR data convert from json error: " << err.errorString();
        return JSON_CONVERT_ERR_CODE;
    }
    return msr_ChipFromJson(data, doc.object());
}
