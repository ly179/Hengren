#include "template_info_model_convert.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include "model_data_convert_helper.h"
#include "template_data_model_convert.h"

void templateInfoModelDataToEntity(
    TemplateInfoEntity& entity, const ApTemplateInfoModel& param) {
    QJsonDocument doc;
    QJsonObject obj;
    idpApTemplateToJsonObject(obj, param.data);
    doc.setObject(obj);
    entity.setTemplateData(doc.toJson(QJsonDocument::Compact));
    entity.setTemplateType(TEMP_ALIGN);
}

void templateInfoModelDataToEntity(TemplateInfoEntity& entity,
                                                        const MultiAddressTemplateInfoModel& param) {
    QJsonDocument doc;
    QJsonObject obj;
    idpAddressTemplateToJsonObject(obj, param.data);
    doc.setObject(obj);
    entity.setTemplateData(doc.toJson(QJsonDocument::Compact));
    entity.setTemplateType(TEMP_MS);
}

void templateInfoModelDataToEntity(
    TemplateInfoEntity& entity, const MpTemplateInfoModel& param) {
    QJsonDocument doc;
    QJsonObject obj;
    idpMpTemplateToJsonObject(obj, param.data);
    doc.setObject(obj);
    entity.setTemplateData(doc.toJson(QJsonDocument::Compact));
    entity.setTemplateType(TEMP_MP);
}

void templateInfoModelDataToEntity(
    TemplateInfoEntity& entity, const MPIPTemplateInfoModel& param) {
    entity.setTemplateComment(param.comment);
    entity.setId(param.id);
    entity.setTemplateName(param.name);
    QJsonDocument doc;
    QJsonObject obj;
    if (param.dx != nullptr) {
        obj.insert("dx", param.dx.toQStringAndKeepFullDecimal());
    }
    if (param.dy != nullptr) {
        obj.insert("dy", param.dy.toQStringAndKeepFullDecimal());
    }
    obj.insert("ms_id", qint64(param.msId));
    obj.insert("mp_id", qint64(param.mpId));
    doc.setObject(obj);
    entity.setTemplateData(doc.toJson(QJsonDocument::Compact));
    entity.setTemplateType(TEMP_MPIP);
}

int templateInfoModelDataFromEntity(
    const TemplateInfoEntity& entity, ApTemplateInfoModel& param) {
    if (entity.getTemplateType() != TEMP_ALIGN) {
        qCritical() << "转换ap的template错误，要转换的目标ApTemplateInfoModel类不适合该类型: "
                    << entity.getTemplateType();
        return JSON_CONVERT_TYPE_ERR_CODE;
    }
    param.type = entity.getTemplateType();
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(entity.getTemplateData().toUtf8(), &err);
    if (err.error != QJsonParseError::NoError) {
        qCritical() << "covert ap template model info from json err: " << err.error << ", "
                    << err.errorString();
        return JSON_CONVERT_ERR_CODE;
    }
    if (!doc.isObject()) {
        return JSON_CONVERT_TYPE_ERR_CODE;
    }
    return idpApTemplateFromJsonValue(QJsonValue(doc.object()), param.data);
}

int ENTITY_MODULE_EXPORT templateInfoModelDataFromEntity(const TemplateInfoEntity& entity,
                                                         MultiAddressTemplateInfoModel& param) {
    if (entity.getTemplateType() != TEMP_MS) {
        qCritical() << "转换ms的template错误，要转换的目标AddressTemplateInfoModel类不适合该类型: "
                    << entity.getTemplateType();
        return JSON_CONVERT_TYPE_ERR_CODE;
    }
    param.type = entity.getTemplateType();
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(entity.getTemplateData().toUtf8(), &err);
    if (err.error != QJsonParseError::NoError) {
        qCritical() << "covert ap template model info from json err: " << err.error << ", "
                    << err.errorString();
        return JSON_CONVERT_ERR_CODE;
    }
    if (!doc.isObject()) {
        return JSON_CONVERT_TYPE_ERR_CODE;
    }
    return idpAddressTemplateFromJsonValue(QJsonValue(doc.object()), param.data);
}

int templateInfoModelDataFromEntity(
    const TemplateInfoEntity& entity, MpTemplateInfoModel& param) {
    if (entity.getTemplateType() != TEMP_MP) {
        qCritical() << "转换mp的template错误，要转换的目标MPTemplateInfoModel类不适合该类型: "
                    << entity.getTemplateType();
        return JSON_CONVERT_TYPE_ERR_CODE;
    }
    param.type = entity.getTemplateType();
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(entity.getTemplateData().toUtf8(), &err);
    if (err.error != QJsonParseError::NoError) {
        qCritical() << "covert ap template model info from json err: " << err.error << ", "
                    << err.errorString();
        return JSON_CONVERT_ERR_CODE;
    }
    if (!doc.isObject()) {
        return JSON_CONVERT_TYPE_ERR_CODE;
    }
    return idpMpTemplateFromJsonValue(QJsonValue(doc.object()), param.data);
}

int templateInfoModelDataFromEntity(
    const TemplateInfoEntity& entity, MPIPTemplateInfoModel& param) {
    if (entity.getTemplateType() != TEMP_MPIP) {
        qCritical() << "转换mpip的template错误，要转换的目标MPIPTemplateInfoModel类不适合该类型: "
                    << entity.getTemplateType();
        return JSON_CONVERT_TYPE_ERR_CODE;
    }
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(entity.getTemplateData().toUtf8(), &err);
    if (err.error != QJsonParseError::NoError) {
        qCritical() << "covert ap template model info from json err: " << err.error << ", "
                    << err.errorString();
        return JSON_CONVERT_ERR_CODE;
    }
    if (!doc.isObject()) {
        return JSON_CONVERT_TYPE_ERR_CODE;
    }
    int errCode;
    errCode = jValToNumber(param.dx, doc["dx"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToNumber(param.dy, doc["dy"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.msId, doc["ms_id"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.mpId, doc["mp_id"]);
    if (errCode) {
        return errCode;
    }
    param.comment = entity.getTemplateComment();
    param.id = entity.getId();
    param.name = entity.getTemplateName();
    return 0;
}
