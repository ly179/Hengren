#ifndef IMAGE_OPERATION_RELATION_H
#define IMAGE_OPERATION_RELATION_H

#include "relation_image_operation_controller.h"
#include "plugin_signal_hub.h"
#include "signal_hub_plugin_ids.h"
#include "signal_hub_single_object_manager.h"

namespace image_operation {

inline bool imageOperationAvailable() {
    return PluginSignalHub::instance()->testSlot<RelationImageOperationController*, bool, bool>(OPEN_SINGLE_IMAGE_OPERATION_WINDOW);
}

inline MsgReceiveStatus<RelationImageOperationController*> openSingleImageOperation(bool notExistsCreate = false, bool activeWindow = true) {
    return PluginSignalHub::instance()->sendCreateObjMsg<RelationImageOperationController*, bool, bool>(OPEN_SINGLE_IMAGE_OPERATION_WINDOW, notExistsCreate, activeWindow);
}

inline bool singleOperationWindowOpened() {
    return SignalHubSingleObjectManager::instance()->getObject<RelationImageOperationController*>(OPEN_SINGLE_IMAGE_OPERATION_WINDOW);
}

inline bool isSignalOperatinWidonwId(const QString& id) {
    return id ==  OPEN_SINGLE_IMAGE_OPERATION_WINDOW;
}

}

#endif //IMAGE_OPERATION_RELATION_H
