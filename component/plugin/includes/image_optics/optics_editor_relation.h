#ifndef OPTICS_EDITOR_RELATION_H
#define OPTICS_EDITOR_RELATION_H


#include "plugin_signal_hub.h"
#include "signal_hub_plugin_ids.h"
#include "relation_optics_edit_controller.h"
#include "signal_hub_single_object_manager.h"

namespace image_optics {

inline bool imageOpticsAvailable() {
    return PluginSignalHub::instance()->testSlot<RelationOpticsEditController*, AsyncCaller*, QWidget*>(OPEN_SINGLE_OPTICS_CONTROLLER);
}

inline MsgReceiveStatus<RelationOpticsEditController*> openSingleOpticsController(AsyncCaller* caller, QWidget* parent) {
    return PluginSignalHub::instance()->sendCreateObjMsg<RelationOpticsEditController*, AsyncCaller*, QWidget*>(OPEN_SINGLE_OPTICS_CONTROLLER, caller, parent);
}

inline bool singleOpticsWindowOpened() {
    return SignalHubSingleObjectManager::instance()->getObject<RelationOpticsEditController*>(OPEN_SINGLE_OPTICS_CONTROLLER);
}

inline bool isSignalOpticsWindow(const QString& id) {
    return id == OPEN_SINGLE_OPTICS_CONTROLLER;
}

}

#endif //OPTICS_EDITOR_RELATION_H
