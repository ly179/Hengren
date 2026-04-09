#ifndef PROCEDURE_MANUAL_RELATION_H
#define PROCEDURE_MANUAL_RELATION_H

#include "plugin_signal_hub.h"
#include "signal_hub_plugin_ids.h"
#include "relation_procedure_manual_normal.h"

namespace procedure_manual {

inline bool procedureUiManualAvailable() {
    return PluginSignalHub::instance()->testSlot<RelatoinProcedureManualNormal*>(OPEN_PROCEDURE_MANUAL_NORMAL);
}

inline MsgReceiveStatus<RelatoinProcedureManualNormal*> openProcedureManualNormalWindow() {
    return PluginSignalHub::instance()->sendCreateObjMsg<RelatoinProcedureManualNormal*>(OPEN_PROCEDURE_MANUAL_NORMAL);
}

}

#endif // PROCEDURE_MANUAL_RELATION_H
