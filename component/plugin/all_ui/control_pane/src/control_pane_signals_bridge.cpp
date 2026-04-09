#include "control_pane_signals_bridge.h"
#ifdef __CONTROL_PANE_TARGET
#include "plugin_signal_hub.h"
#include "signal_hub_plugin_ids.h"
#include "main_controller.h"

#include "exception_code.h"
#include "plugin_version_util.h"
#include "sem_common_data_model.h"

SIGNAL_CHECK(__CONTROL_PANE_TARGET) {
    Q_UNUSED(sysVersion)
#ifdef __CONTROL_PANE_TARGET
    bool succ = readVersion(_CONTROL_PANE_VERSION_, *libVersion);
    if (succ) {
        result->errCode = 0;
        result->errMsg.clear();
        //TODO 未来可添加版本号处理机制
    } else {
        result->errCode = LIBRARY_VERSION_READ_ERROR_CODE;
        result->errMsg = "Control pane version is not defined.";
    }
#else
#warning "Control Pane component version is not defined."
    result->errCode = 0;
    result->errMsg.clear();
#endif
}

SIGNAL_REG(__CONTROL_PANE_TARGET) {
    Q_UNUSED(sysVersion)
    PluginSignalHub::instance()->registSlot(APP_MAIN_CONTROL_PANE, PluginHubAnyFunc<RelationMainController*, QWidget*>([](QWidget* parent)->RelationMainController*{
        return new MainController(parent);
    }));
    if (result) {
        result->errCode = 0;
        result->errMsg.clear();
    }
}

SIGNAL_UNREG(__CONTROL_PANE_TARGET) {
    QString pid = QString::fromLatin1(pluginId);
    PluginSignalHub::instance()->unRegistAll(APP_MAIN_CONTROL_PANE, [pid, callback](){
        callback(pid.toLatin1());
    });
}

CONTROL_PANE_SIGNALS_BRIDGE_H
#endif //
