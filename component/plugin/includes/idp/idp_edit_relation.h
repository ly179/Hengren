#ifndef IDP_EDIT_RELATION_H
#define IDP_EDIT_RELATION_H
#include "plugin_signal_hub.h"
#include "signal_hub_plugin_ids.h"
#include "relation_idp_data_edit_controller.h"
#include "signal_hub_single_object_manager.h"

namespace idp_editor {

inline bool idpEditorAlivable() {
    return PluginSignalHub::instance()->testSlot<RelationIdpEditController*, const sem::TWO_PARAM_ARG_FUNC<quint64, quint64>&>(OPEN_SINGLE_IDP_EDITOR);
}

inline MsgReceiveStatus<RelationIdpEditController*> openSingleIdpEditor(const sem::TWO_PARAM_ARG_FUNC<quint64, quint64>& callback) {
    return PluginSignalHub::instance()->sendCreateObjMsg<RelationIdpEditController*, const sem::TWO_PARAM_ARG_FUNC<quint64, quint64>&>(OPEN_SINGLE_IDP_EDITOR, callback);
}

inline bool singleIDPWindowOpened() {
    return SignalHubSingleObjectManager::instance()->getObject<RelationIdpEditController*>(OPEN_SINGLE_IDP_EDITOR);
}

}

#endif //IDP_EDIT_RELATION_H
