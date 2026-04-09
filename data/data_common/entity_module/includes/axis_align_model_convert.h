#ifndef AXIS_ALIGN_MODEL_CONVERT_H
#define AXIS_ALIGN_MODEL_CONVERT_H

#include "entity_module_global.h"

struct AxisAlignModel;
class QJsonObject;
class QJsonValue;

void putAxisAlignData(QJsonObject& obj, const AxisAlignModel& param);
int fromAxisAlignData(const QJsonValue& val, AxisAlignModel& param);

void ENTITY_MODULE_EXPORT axisAlignModelToJson(QString& json, const AxisAlignModel& data);
int ENTITY_MODULE_EXPORT axisAlignModelFromJson(const QString& json, AxisAlignModel& data);

#endif // AXIS_ALIGN_MODEL_CONVERT_H
