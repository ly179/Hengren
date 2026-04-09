#ifndef TEMPLATE_DATA_MODEL_CONVERT_H
#define TEMPLATE_DATA_MODEL_CONVERT_H

#include <QString>
#include "entity_module_global.h"

class QJsonObject;
class QJsonValue;
struct ApTemplateParam;
struct MultiAddressTemplateParam;
struct MpTemplateParam;
struct ApTemplateDataParam;
struct AddressTemplateDataParam;
struct MpTemplateDataParam;
struct PatternRecognitionAlgorithmSettingParam;
struct PatternRecognitionParamSettingParam;
struct AutoFocusSettingParam;
struct AutoStigmaSettingParam;
struct ZSensorSettingParam;

void ENTITY_MODULE_EXPORT idpApTemplateToJsonObject(QJsonObject& obj, const ApTemplateParam&);
int ENTITY_MODULE_EXPORT idpApTemplateFromJsonValue(const QJsonValue& val, ApTemplateParam&);
void ENTITY_MODULE_EXPORT idpAddressTemplateToJsonObject(QJsonObject& obj,
                                                         const MultiAddressTemplateParam&);
int ENTITY_MODULE_EXPORT idpAddressTemplateFromJsonValue(const QJsonValue& val,
                                                         MultiAddressTemplateParam&);
void ENTITY_MODULE_EXPORT idpMpTemplateToJsonObject(QJsonObject& obj, const MpTemplateParam&);
int ENTITY_MODULE_EXPORT idpMpTemplateFromJsonValue(const QJsonValue& val, MpTemplateParam&);
void ENTITY_MODULE_EXPORT idpApTemplateToJson(QString& json, const ApTemplateDataParam& apData);
int ENTITY_MODULE_EXPORT idpApTemplateFromJson(const QString& json, ApTemplateDataParam& apData);
void ENTITY_MODULE_EXPORT idpAddressTemplateToJson(QString& json,
                                                   const AddressTemplateDataParam& addressData);
int ENTITY_MODULE_EXPORT idpAddressTemplateFromJson(const QString& json,
                                                    AddressTemplateDataParam& addressData);
void ENTITY_MODULE_EXPORT idpMpTemplateToJson(QString& json, const MpTemplateDataParam& mpData);
int ENTITY_MODULE_EXPORT idpMpTemplateFromJson(const QString& json, MpTemplateDataParam& mpData);

int ENTITY_MODULE_EXPORT patternRegAlgJsonToStruct(const QString& json,
                                                   PatternRecognitionAlgorithmSettingParam&,
                                                   int mode);
void ENTITY_MODULE_EXPORT patternRegAlgStructToJson(QString& json,
                                                    const PatternRecognitionAlgorithmSettingParam&);

void ENTITY_MODULE_EXPORT putPatternRecognitionAlgorithmSettingParam(
    QJsonObject& obj, const PatternRecognitionAlgorithmSettingParam& param);
int ENTITY_MODULE_EXPORT fromPatternRecognitionAlgorithmSettingParam(
    const QJsonValue& val, PatternRecognitionAlgorithmSettingParam& param);

void ENTITY_MODULE_EXPORT putPatternRecognitionParamSettingParam(
    QJsonObject& obj, const PatternRecognitionParamSettingParam& param);
int ENTITY_MODULE_EXPORT fromPatternRecognitionParamSettingParam(
    const QJsonValue& val, PatternRecognitionParamSettingParam& param);

void ENTITY_MODULE_EXPORT putAutoFocusSettingParam(QJsonObject& obj,
                                                   const AutoFocusSettingParam& param);
int ENTITY_MODULE_EXPORT fromAutoFocusSettingParam(const QJsonValue& val,
                                                   AutoFocusSettingParam& param);

void ENTITY_MODULE_EXPORT putAutoStigmaSettingParam(QJsonObject& obj,
                                                    const AutoStigmaSettingParam& param);
int ENTITY_MODULE_EXPORT fromAutoStigmaSettingParam(const QJsonValue& val,
                                                    AutoStigmaSettingParam& param);

void ENTITY_MODULE_EXPORT putAutoStigmaSettingParam(QJsonObject& obj,
                                                    const AutoStigmaSettingParam& param);
int ENTITY_MODULE_EXPORT fromAutoStigmaSettingParam(const QJsonValue& val,
                                                    AutoStigmaSettingParam& param);

void ENTITY_MODULE_EXPORT putZSensorSettingParam(QJsonObject& obj, const ZSensorSettingParam& param);

int ENTITY_MODULE_EXPORT fromZSensorSettingParam(const QJsonValue& val, ZSensorSettingParam& param);
#endif // TEMPLATE_DATA_MODEL_CONVERT_H
