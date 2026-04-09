#include "scan_model_data_convert.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include "model_data_convert_helper.h"
#include "exception_code.h"
#include "scan_mode_data_model.h"

void scanModeDataToEntity(QString& entity, const ScanModeDataModel& data) {
    QJsonDocument doc;
    QJsonObject obj;
    obj.insert("scan_rate", data.getScanRate());
    obj.insert("frame", data.getFrame());
    obj.insert("pixel_x", data.getPixelX());
    obj.insert("pixel_y", data.getPixelY());
    obj.insert("x_per_nm", data.getXPerNm().toQString());
    obj.insert("y_per_nm", data.getYPerNm().toQString());
    obj.insert("filter_type", data.getFilterType());
    obj.insert("flat", data.getFlat());
    obj.insert("dc_type", data.getDcType());
    doc.setObject(obj);
    entity = doc.toJson(QJsonDocument::Compact);
}
int scanModeDataFromEntity(const QString& entity, ScanModeDataModel& data) {
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(entity.toUtf8(), &err);
    if (err.error != QJsonParseError::NoError) {
        qCritical() << "covert scan mode data from entity err: " << err.error << ", "
                    << err.errorString();
        return JSON_CONVERT_ERR_CODE;
    }
    int ival;
    int errCode;
    errCode = jValToInt(ival, doc["scan_rate"]);
    if (errCode) {
        return JSON_CONVERT_TYPE_ERR_CODE;
    }
    data.setScanRate(ival);
    errCode = jValToInt(ival, doc["frame"]);
    if (errCode) {
        return JSON_CONVERT_TYPE_ERR_CODE;
    }
    data.setFrame(ival);
    errCode = jValToInt(ival, doc["pixel_x"]);
    if (errCode) {
        return JSON_CONVERT_TYPE_ERR_CODE;
    }
    data.setPixelX(ival);
    errCode = jValToInt(ival, doc["pixel_y"]);
    if (errCode) {
        return JSON_CONVERT_TYPE_ERR_CODE;
    }
    data.setPixelY(ival);
    errCode = jValToNumber(data.getXPerNm(), doc["x_per_nm"], "0");
    if (errCode) {
        return JSON_CONVERT_TYPE_ERR_CODE;
    }
    errCode = jValToNumber(data.getYPerNm(), doc["y_per_nm"], "0");
    if (errCode) {
        return JSON_CONVERT_TYPE_ERR_CODE;
    }
    errCode = jValToInt(ival, doc["filter_type"]);
    if (errCode) {
        return JSON_CONVERT_TYPE_ERR_CODE;
    }
    data.setFilterType(ival);
    errCode = jValToInt(ival, doc["flat"]);
    if (errCode) {
        return JSON_CONVERT_TYPE_ERR_CODE;
    }
    data.setFlat(ival);
    errCode = jValToInt(ival, doc["dc_type"]);
    if (errCode) {
        return JSON_CONVERT_TYPE_ERR_CODE;
    }
    data.setDcType(ival);
    return 0;
}
