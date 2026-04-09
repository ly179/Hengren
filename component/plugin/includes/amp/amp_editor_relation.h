#ifndef AMP_EDITOR_RELATION_H
#define AMP_EDITOR_RELATION_H

#include "plugin_signal_hub.h"
#include "signal_hub_plugin_ids.h"
#include "relation_amp_edit_controller.h"
#include "signal_hub_single_object_manager.h"

namespace amp_editor {
inline bool ampEditorAlivable() {
    return PluginSignalHub::instance()->testSlot<RelatoinAmpEditController*>(OPEN_SINGLE_AMP_EDITOR);
}

inline MsgReceiveStatus<RelatoinAmpEditController*> openSingleAMPEditor() {
    return PluginSignalHub::instance()->sendCreateObjMsg<RelatoinAmpEditController*>(OPEN_SINGLE_AMP_EDITOR);
}
inline MsgReceiveStatus<RelatoinAmpEditController*> openSingleAMPEditor(int id, const QString& json) {
    return PluginSignalHub::instance()->sendCreateObjMsg<RelatoinAmpEditController*, int, const QString&>(OPEN_SINGLE_AMP_EDITOR, id, json);
}

inline bool singleAMPWindowOpened() {
    return SignalHubSingleObjectManager::instance()->getObject<RelatoinAmpEditController*>(OPEN_SINGLE_AMP_EDITOR);
}

inline bool isSingleAMP(const QString& id) {
    return OPEN_SINGLE_AMP_EDITOR == id;
}

}

#endif // AMP_EDITOR_RELATION_H
