#include "worksheet_signal_bridge.h"
#ifdef __WORKSHEET_TARGET
#include "plugin_signal_hub.h"
#include "signal_hub_plugin_ids.h"
#include "worksheet_controller.h"
#include "msr_data_model.h"
#include "signal_hub_single_object_manager.h"
#include "sem_global.h"

#include "plugin_version_util.h"
#include "exception_code.h"
#include "sem_common_data_model.h"

WorksheetController* openSingleWorksheet() {
    WorksheetController* workSheetController = SignalHubSingleObjectManager::instance()->getObject<WorksheetController*>(OPEN_SINGLE_WORKSHEET_CONTROLLER);
    if (workSheetController) {
        if (!workSheetController->isActiveWindow()) workSheetController->activateWindow();
        return workSheetController;
    }
    workSheetController = new WorksheetController;
    workSheetController->setAttribute(Qt::WA_DeleteOnClose);
    // workSheetController->setResizable(false);
    workSheetController->setWindowTitle("Work Sheet");
    workSheetController->setCloseCall([](CDE90WindowCloseStatus){
        SignalHubSingleObjectManager::instance()->removeObject(OPEN_SINGLE_WORKSHEET_CONTROLLER);
        return true;
    });
    workSheetController->initAsyncCaller();
    workSheetController->show();
    MainThreadInvoker::getInvoker().moveToScreenCenter(workSheetController);
    SignalHubSingleObjectManager::instance()->setObject(OPEN_SINGLE_WORKSHEET_CONTROLLER, workSheetController);
    return workSheetController;
}

WorksheetController* openSingleWorksheet(const MSR_Chip& c) {
    WorksheetController* w = openSingleWorksheet();
    w->resetData(c);
    return w;
}
WorksheetController* openSingleWorksheet(quint64 msrId) {
    WorksheetController* w = openSingleWorksheet();
    w->resetData(msrId);
    return w;
}

void appendDataToWorkSheet(const MSR_MP_Chip_MetaData& metaData, const MSR_MP_Data& d) {
    if (!MainThreadInvoker::getInvoker().isMainThread()) {
        MainThreadInvoker::getInvoker().runLater([metaData, d](){
            appendDataToWorkSheet(metaData, d);
        });
        return;
    }
    WorksheetController* workSheetController = SignalHubSingleObjectManager::instance()->getObject<WorksheetController*>(OPEN_SINGLE_WORKSHEET_CONTROLLER);
    if (!workSheetController) return;
    workSheetController->appendData(metaData, d);
}

SIGNAL_CHECK(__WORKSHEET_TARGET) {
    Q_UNUSED(sysVersion)
#ifdef _WORKSHEET_VERSION_
    bool succ = readVersion(_WORKSHEET_VERSION_, *libVersion);
    if (succ) {
        result->errCode = 0;
        result->errMsg.clear();
        //TODO 未来可添加版本号处理机制
    } else {
        result->errCode = LIBRARY_VERSION_READ_ERROR_CODE;
        result->errMsg = "Worksheet version is not defined.";
    }
#else
#warning "Worksheet component version is not defined."
    result->errCode = 0;
    result->errMsg.clear();
#endif
}

SIGNAL_REG(__WORKSHEET_TARGET) {
    Q_UNUSED(sysVersion)
    PluginSignalHub::instance()->registSlot(OPEN_SINGLE_WORKSHEET_CONTROLLER, PluginHubAnyFunc<RelationWorksheetController*>([](){
        return openSingleWorksheet();
    }));
    PluginSignalHub::instance()->registSlot(OPEN_SINGLE_WORKSHEET_CONTROLLER, PluginHubAnyFunc<RelationWorksheetController*, const MSR_Chip&>([](const MSR_Chip& c){
        return openSingleWorksheet(c);
    }));
    PluginSignalHub::instance()->registSlot(OPEN_SINGLE_WORKSHEET_CONTROLLER, PluginHubAnyFunc<RelationWorksheetController*, quint64>([](quint64 msrId){
        return openSingleWorksheet(msrId);
    }));
    PluginSignalHub::instance()->registSlot(APPEND_WORKSHEET_DATA, PluginHubAnyFunc<void, const MSR_MP_Chip_MetaData&, const MSR_MP_Data&>([](const MSR_MP_Chip_MetaData& metaData, const MSR_MP_Data& d){
        appendDataToWorkSheet(metaData, d);
    }));
    if (result) {
        result->errCode = 0;
        result->errMsg.clear();
    }
}

SIGNAL_UNREG(__WORKSHEET_TARGET) {
    QList<QString> list;
    list.reserve(2);
    list << OPEN_SINGLE_WORKSHEET_CONTROLLER << APPEND_WORKSHEET_DATA;
    QString pid = QString::fromLatin1(pluginId);
    PluginSignalHub::instance()->unRegistAll(list, [pid, callback](){
        callback(pid.toLatin1());
    });
}

WORKSHEET_SIGNAL_BRIDGE_H
#endif
