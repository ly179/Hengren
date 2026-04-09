#include "idw_signals_bridge.h"
#ifdef __IDW__TARGET
#include "plugin_signal_hub.h"
#include "signal_hub_plugin_ids.h"
#include "signal_hub_single_object_manager.h"
#include "idw_wafer_editor.h"
#include "sem_global.h"

#include "plugin_version_util.h"
#include "exception_code.h"
#include "sem_common_data_model.h"

IdwWaferEditor* openSignalIDWWafer(const sem::ONE_PARAM_ARG_FUNC<quint64>& exitCallback) {
    IdwWaferEditor* idwWaferEditor = SignalHubSingleObjectManager::instance()->getObject<IdwWaferEditor*>(OPEN_SINGLE_IDW_WAFER);
    if (idwWaferEditor) {
        if (!idwWaferEditor->isActiveWindow()) idwWaferEditor->activateWindow();
        return idwWaferEditor;
    }
    idwWaferEditor = new IdwWaferEditor;
    idwWaferEditor->setWindowTitle("IDW Editor");
    idwWaferEditor->setAttribute(Qt::WA_DeleteOnClose);
    // idwWaferEditor->setResizable(false);
    idwWaferEditor->show();
    idwWaferEditor->resize(QSize(1115, idwWaferEditor->height()));
    idwWaferEditor->setExitCallback([idwWaferEditor, exitCallback](){
        if (exitCallback) exitCallback(idwWaferEditor->getIdwId());
        SignalHubSingleObjectManager::instance()->removeObject(OPEN_SINGLE_IDW_WAFER);
    });
    MainThreadInvoker::getInvoker().moveToScreenTopCenter(idwWaferEditor);
    SignalHubSingleObjectManager::instance()->setObject(OPEN_SINGLE_IDW_WAFER, idwWaferEditor);
    return idwWaferEditor;
}

SIGNAL_CHECK(__IDW__TARGET) {
    Q_UNUSED(sysVersion)
#ifdef _IDW_VERSION_
    bool succ = readVersion(_IDW_VERSION_, *libVersion);
    if (succ) {
        result->errCode = 0;
        result->errMsg.clear();
        //TODO 未来可添加版本号处理机制
    } else {
        result->errCode = LIBRARY_VERSION_READ_ERROR_CODE;
        result->errMsg = "IDW wafer version is not defined.";
    }
#else
#warning "IDW component version is not defined."
    result->errCode = 0;
    result->errMsg.clear();
#endif
}

SIGNAL_REG(__IDW__TARGET) {
    Q_UNUSED(sysVersion)
    PluginSignalHub::instance()->registSlot(OPEN_SINGLE_IDW_WAFER, PluginHubAnyFunc<RelationIDWWaferEditor*, const sem::ONE_PARAM_ARG_FUNC<quint64>&>([](const sem::ONE_PARAM_ARG_FUNC<quint64>& callback){
        return openSignalIDWWafer(callback);
    }));
    if (result) {
        result->errCode = 0;
        result->errMsg.clear();
    }
}

SIGNAL_UNREG(__IDW__TARGET){
    QString pid = QString::fromLatin1(pluginId);
    PluginSignalHub::instance()->unRegistAll(OPEN_SINGLE_IDW_WAFER, [pid, callback](){
        callback(pid.toLatin1());
    });
}
IDW_SIGNALS_BRIDGE_H
#endif

