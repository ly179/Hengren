//
// Created by Shisan on 2026/3/31.
//

#ifndef HENGREN_CD_SEM_REALTIME_MONITOR_RELATION_H
#define HENGREN_CD_SEM_REALTIME_MONITOR_RELATION_H

#include "plugin_signal_hub.h"
#include "signal_hub_plugin_ids.h"
#include "signal_hub_single_object_manager.h"
#include "relation_realtime_monitor.h"

namespace realtime_monitor {
    inline bool realTimeMonitorAlivable() {
        return PluginSignalHub::instance()->testSlot<RelationRealTimeMonitor*>(CREATE_REALTIME_MONITOR);
    }

    inline MsgReceiveStatus<RelationRealTimeMonitor*> openSingleRealTimeMonitor() {
        return PluginSignalHub::instance()->sendCreateObjMsg<RelationRealTimeMonitor*>(CREATE_REALTIME_MONITOR);
    }
    inline bool singleRealTimeMonitorOpened() {
        return SignalHubSingleObjectManager::instance()->getObject<RelationRealTimeMonitor*>(CREATE_REALTIME_MONITOR);
    }

    inline bool isSingleRealTimeMonitor(const QString& id) {
        return CREATE_REALTIME_MONITOR == id;
    }
}

#endif //HENGREN_CD_SEM_REALTIME_MONITOR_RELATION_H