//
// Created by Shisan on 2026/3/17.
//

#ifndef HENGREN_CD_SEM_LOG_VIEWER_RELATION_H
#define HENGREN_CD_SEM_LOG_VIEWER_RELATION_H


#include "plugin_signal_hub.h"
#include "signal_hub_plugin_ids.h"
#include "relation_log_viewer_controller.h"
#include "signal_hub_single_object_manager.h"

namespace log_viewer {
    inline bool logViewerAlivable() {
        return PluginSignalHub::instance()->testSlot<RelationLogViewerController*>(OPEN_SINGLE_LOG_VIEWER);
    }

    inline MsgReceiveStatus<RelationLogViewerController*> openSingleLogViewer() {
        return PluginSignalHub::instance()->sendCreateObjMsg<RelationLogViewerController*>(OPEN_SINGLE_LOG_VIEWER);
    }
    inline bool singleLogViewerOpened() {
        return SignalHubSingleObjectManager::instance()->getObject<RelationLogViewerController*>(OPEN_SINGLE_LOG_VIEWER);
    }

    inline void prepareLogViewInstance() {
        PluginSignalHub::instance()->sendMsg<void>(CREATE_FULL_LEVEL_LOG_INS);
    }

    inline bool isSingleLogViewer(const QString& id) {
        return OPEN_SINGLE_LOG_VIEWER == id;
    }

}

#endif //HENGREN_CD_SEM_LOG_VIEWER_RELATION_H