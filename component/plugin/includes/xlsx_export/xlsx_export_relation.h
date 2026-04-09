#ifndef XLSX_EXPORT_RELATION_H
#define XLSX_EXPORT_RELATION_H

#include "msr_data_model.h"
#include "plugin_signal_hub.h"
#include "signal_hub_plugin_ids.h"

namespace xlsx_export {
bool xlsxExportAvailable() {
    return PluginSignalHub::instance()->testSlot<void, const MSR_Chip&, const QString&>(MSR_XLSX_EXPORT);
}

MsgReceiveStatus<void> exportMSR(const MSR_Chip& data, const QString& path) {
    return PluginSignalHub::instance()->sendMsg<void, const MSR_Chip&, const QString&>(MSR_XLSX_EXPORT, data, path);
}
}

#endif // XLSX_EXPORT_RELATION_H
