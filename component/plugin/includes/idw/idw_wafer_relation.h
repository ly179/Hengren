#ifndef IDW_WAFER_RELATION_H
#define IDW_WAFER_RELATION_H

#include "plugin_signal_hub.h"
#include "signal_hub_plugin_ids.h"
#include "relation_idw_wafer_editor.h"
#include "signal_hub_single_object_manager.h"

namespace idw_wafer {

inline bool idwWaferAvailable() {
    return PluginSignalHub::instance()->testSlot<RelationIDWWaferEditor*, const sem::ONE_PARAM_ARG_FUNC<quint64>&>(OPEN_SINGLE_IDW_WAFER);
}

inline MsgReceiveStatus<RelationIDWWaferEditor*> openSingleIdwWaferEditor(const sem::ONE_PARAM_ARG_FUNC<quint64>& callback) {
    return PluginSignalHub::instance()->sendCreateObjMsg<RelationIDWWaferEditor*, const sem::ONE_PARAM_ARG_FUNC<quint64>&>(OPEN_SINGLE_IDW_WAFER, callback);
}

inline bool singleIDWWindowOpened() {
    return SignalHubSingleObjectManager::instance()->getObject<RelationIDWWaferEditor*>(OPEN_SINGLE_IDW_WAFER);
}

}

#endif // IDW_WAFER_RELATION_H
