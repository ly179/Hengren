#ifndef DATA_MANAGER_RELATION_H
#define DATA_MANAGER_RELATION_H

#include "relation_file_manager.h"
#include "relation_image_manager.h"
#include "relation_msr_manager.h"
#include "relation_template_manager.h"
#include "plugin_signal_hub.h"
#include "signal_hub_plugin_ids.h"

namespace data_manager {

bool fileManagerAvailable() {
    return PluginSignalHub::instance()->testSlot<RelationFileManager*>(OPEN_SINGLE_FILE_MANAGER);
}
bool imageManagerAvailable() {
    return PluginSignalHub::instance()->testSlot<RelationImageManager*>(OPEN_SINGLE_IMAGE_MANAGER);
}
bool templateManagerAvailable() {
    return PluginSignalHub::instance()->testSlot<RelationTemplateManager*>(OPEN_SINGLE_TEMPLATE_MANAGER);
}
bool msrManagerAvailable() {
    return PluginSignalHub::instance()->testSlot<RelationMSRManager*>(OPEN_SINGLE_MSR_MANAGER);
}

inline bool isSingleFileManager(const QString& id) {
    return OPEN_SINGLE_FILE_MANAGER == id;
}

inline bool isSingleImageManager(const QString& id) {
    return OPEN_SINGLE_IMAGE_MANAGER == id;
}

inline bool isSingleTemplateManager(const QString& id) {
    return OPEN_SINGLE_TEMPLATE_MANAGER == id;
}

inline bool isSingleMSRManager(const QString& id) {
    return OPEN_SINGLE_MSR_MANAGER == id;
}

inline MsgReceiveStatus<RelationFileManager*> openFileManagerController() {
    return PluginSignalHub::instance()->sendCreateObjMsg<RelationFileManager*>(OPEN_SINGLE_FILE_MANAGER);
}

inline MsgReceiveStatus<RelationTemplateManager*> openTemplateManagerController() {
    return PluginSignalHub::instance()->sendCreateObjMsg<RelationTemplateManager*>(OPEN_SINGLE_TEMPLATE_MANAGER);
}

inline MsgReceiveStatus<RelationImageManager*> openImageManagerController() {
    return PluginSignalHub::instance()->sendCreateObjMsg<RelationImageManager*>(OPEN_SINGLE_IMAGE_MANAGER);
}

inline MsgReceiveStatus<RelationMSRManager*> openMSRManagerController() {
    return PluginSignalHub::instance()->sendCreateObjMsg<RelationMSRManager*>(OPEN_SINGLE_MSR_MANAGER);
}
}

#endif // DATA_MANAGER_RELATION_H
