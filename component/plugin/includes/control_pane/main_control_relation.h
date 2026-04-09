#ifndef MAIN_CONTROL_RELATION_H
#define MAIN_CONTROL_RELATION_H

#include "signal_hub_plugin_ids.h"
#include "plugin_signal_hub.h"
#include "relation_main_controller.h"
namespace main_control {

MsgReceiveStatus<RelationMainController*> createMainControl(QWidget* parent = nullptr) {
    return PluginSignalHub::instance()->sendCreateObjMsg<RelationMainController*, QWidget*>(APP_MAIN_CONTROL_PANE, parent);
}

}
#endif // MAIN_CONTROL_RELATION_H
