#ifndef WORKSHEET_RELATION_H
#define WORKSHEET_RELATION_H

#include "signal_hub_plugin_ids.h"
#include "plugin_signal_hub.h"
#include "relation_worksheet_controller.h"
#include "msr_data_model.h"

namespace worksheet {
inline bool worksheetAvaliable() {
    return PluginSignalHub::instance()->testSlot<RelationWorksheetController*>(OPEN_SINGLE_WORKSHEET_CONTROLLER);
}

inline bool isSingleWorksheetController(const QString& id) {
    return OPEN_SINGLE_WORKSHEET_CONTROLLER == id;
}

inline MsgReceiveStatus<RelationWorksheetController*> openSingleWorksheetController() {
    return PluginSignalHub::instance()->sendCreateObjMsg<RelationWorksheetController*>(OPEN_SINGLE_WORKSHEET_CONTROLLER);
}
inline MsgReceiveStatus<RelationWorksheetController*> openSingleWorksheetController(const MSR_Chip& c) {
    return PluginSignalHub::instance()->sendCreateObjMsg<RelationWorksheetController*, const MSR_Chip&>(OPEN_SINGLE_WORKSHEET_CONTROLLER, c);
}

inline MsgReceiveStatus<RelationWorksheetController*> openSingleWorksheetController(quint64 msrId) {
    return PluginSignalHub::instance()->sendCreateObjMsg<RelationWorksheetController*, quint64>(OPEN_SINGLE_WORKSHEET_CONTROLLER, msrId);
}

inline MsgReceiveStatus<void> appendDataToSingleWorksheetController(const MSR_MP_Chip_MetaData& metaData, const MSR_MP_Data& d) {
    return PluginSignalHub::instance()->sendMsg<void, const MSR_MP_Chip_MetaData&, const MSR_MP_Data&>(APPEND_WORKSHEET_DATA, metaData, d);
}


}

#endif // WORKSHEET_RELATION_H
