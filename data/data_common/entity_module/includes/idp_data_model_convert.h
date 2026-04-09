#ifndef IDP_DATA_MODEL_CONVERT_H
#define IDP_DATA_MODEL_CONVERT_H
#include "entity_module_global.h"

struct IdpApDetailData;
struct IdpMpDetailData;
void ENTITY_MODULE_EXPORT idpApModelToJson(QString& json, const IdpApDetailData& apData);
int ENTITY_MODULE_EXPORT idpApModelFromJson(const QString& json, IdpApDetailData& apData);

void ENTITY_MODULE_EXPORT idpMpModelToJson(QString& json, const IdpMpDetailData& mpData);
int ENTITY_MODULE_EXPORT idpMpModelFromJson(const QString& json, IdpMpDetailData& mpData);

#endif // IDP_DATA_MODEL_CONVERT_H
