#ifndef WAFER_TRANSFER_RELATION_H
#define WAFER_TRANSFER_RELATION_H
#include "signal_hub_plugin_ids.h"
#include "plugin_signal_hub.h"
#include "relation_stage_controller.h"
#include "relation_manual_controller.h"

namespace wafer_transfer {
inline bool stageControllerAvaliable() {
    return PluginSignalHub::instance()->testSlot<RelationStageController*>(OPEN_SINGLE_STAGE_CONTROLLER);
}
inline bool manualControllerAvaliable() {
    return PluginSignalHub::instance()->testSlot<RelationManualController*>(OPEN_SINGLE_MANUAL_CONTROLLER);
}

inline bool isSingleStageController(const QString& id) {
    return OPEN_SINGLE_STAGE_CONTROLLER == id;
}

inline bool isSingleManualController(const QString& id) {
    return OPEN_SINGLE_MANUAL_CONTROLLER == id;
}

inline MsgReceiveStatus<RelationStageController*> openSingleStageController() {
    return PluginSignalHub::instance()->sendCreateObjMsg<RelationStageController*>(OPEN_SINGLE_STAGE_CONTROLLER);
}
inline MsgReceiveStatus<RelationManualController*> openSingleManualController() {
    return PluginSignalHub::instance()->sendCreateObjMsg<RelationManualController*>(OPEN_SINGLE_MANUAL_CONTROLLER);
}
}


#endif // WAFER_TRANSFER_RELATION_H
