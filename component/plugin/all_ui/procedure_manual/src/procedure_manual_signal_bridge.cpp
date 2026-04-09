#include "procedure_manual_signal_bridge.h"
#ifdef __UI_PROCEDURE_MANUAL_TARGET
#include "plugin_signal_hub.h"
#include "signal_hub_plugin_ids.h"
#include "sem_global.h"

#include "exception_code.h"
#include "plugin_version_util.h"
#include "procedure_manual_normal_window.h"
#include "sem_common_data_model.h"

RelatoinProcedureManualNormal* openNormalWindow() {
    RelatoinProcedureManualNormal* normalWindow = new ProcedureManualNormalWindow;
    normalWindow->setAttribute(Qt::WA_DeleteOnClose);
    normalWindow->setWindowTitle("Wait Input");
    normalWindow->hideFullSizeBtn();
    normalWindow->setResizable(false);
    normalWindow->show();
    MainThreadInvoker::getInvoker().moveToScreenCenter(normalWindow);
    return normalWindow;
}

SIGNAL_CHECK(__UI_PROCEDURE_MANUAL_TARGET) {
    Q_UNUSED(sysVersion)
#ifdef _UI_PROCEDURE_MANUAL_VERSION_
    bool succ = readVersion(_UI_PROCEDURE_MANUAL_VERSION_, *libVersion);
    if (succ) {
        result->errCode = 0;
        result->errMsg.clear();
        //TODO 未来可添加版本号处理机制
    } else {
        result->errCode = LIBRARY_VERSION_READ_ERROR_CODE;
        result->errMsg = "Optics controller version is not defined.";
    }
#else
#warning "Procedure manual component version is not defined."
    result->errCode = 0;
    result->errMsg.clear();
#endif
}

SIGNAL_REG(__UI_PROCEDURE_MANUAL_TARGET) {
    Q_UNUSED(sysVersion)
    PluginSignalHub::instance()->registSlot(OPEN_PROCEDURE_MANUAL_NORMAL, PluginHubAnyFunc<RelatoinProcedureManualNormal*>([](){
        return openNormalWindow();
    }));
    if (result) {
        result->errCode = 0;
        result->errMsg.clear();
    }
}

SIGNAL_UNREG(__UI_PROCEDURE_MANUAL_TARGET) {
    QString pid = QString::fromLatin1(pluginId);
    PluginSignalHub::instance()->unRegistAll(OPEN_PROCEDURE_MANUAL_NORMAL, [pid, callback](){
        callback(pid.toLatin1());
    });
}

PROCEDURE_MANUAL_SIGNAL_BRIDGE_H
#endif //
