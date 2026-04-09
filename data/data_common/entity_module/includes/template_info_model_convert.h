#ifndef TEMPLATE_INFO_MODEL_CONVERT_H
#define TEMPLATE_INFO_MODEL_CONVERT_H

#include "entity_module_global.h"
#include "template_info_entity.h"
#include "template_info_model.h"

class TemplateInfoEntity;
template<class T>
void templateInfoModelToEnity(
    TemplateInfoEntity& entity, const TemplateInfoModel<T>& data) {
    entity.setTemplateName(data.name);
    entity.setTemplateComment(data.comment);
    entity.setId(data.id);
    templateInfoModelDataToEntity(entity, data);
}

template<class T>
int templateInfoModelFromEntity(
    const TemplateInfoEntity& entity, TemplateInfoModel<T>& data) {
    data.name = entity.getTemplateName();
    data.comment = entity.getTemplateComment();
    data.id = entity.getId();
    return templateInfoModelDataFromEntity(entity, data);
}

void ENTITY_MODULE_EXPORT templateInfoModelDataToEntity(TemplateInfoEntity& entity,
                                                        const ApTemplateInfoModel& param);
void ENTITY_MODULE_EXPORT templateInfoModelDataToEntity(TemplateInfoEntity& entity,
                                                        const MultiAddressTemplateInfoModel& param);
void ENTITY_MODULE_EXPORT templateInfoModelDataToEntity(TemplateInfoEntity& entity,
                                                        const MpTemplateInfoModel& param);
void ENTITY_MODULE_EXPORT templateInfoModelDataToEntity(TemplateInfoEntity& entity,
                                                        const MPIPTemplateInfoModel& param);

int ENTITY_MODULE_EXPORT templateInfoModelDataFromEntity(const TemplateInfoEntity& entity,
                                                         ApTemplateInfoModel& param);
int ENTITY_MODULE_EXPORT templateInfoModelDataFromEntity(const TemplateInfoEntity& entity,
                                                         MultiAddressTemplateInfoModel& param);
int ENTITY_MODULE_EXPORT templateInfoModelDataFromEntity(const TemplateInfoEntity& entity,
                                                         MpTemplateInfoModel& param);
int ENTITY_MODULE_EXPORT templateInfoModelDataFromEntity(const TemplateInfoEntity& entity,
                                                         MPIPTemplateInfoModel& param);

#endif // TEMPLATE_INFO_MODEL_CONVERT_H
