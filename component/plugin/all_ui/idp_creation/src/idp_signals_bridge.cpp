#include "idp_signals_bridge.h"
#ifdef __IDP__TARGET
#include "plugin_signal_hub.h"
#include "signal_hub_plugin_ids.h"
#include "idp_data_editor_controller.h"
#include "signal_hub_single_object_manager.h"
#include "plugin_version_util.h"
#include "sem_global.h"

RelationIdpEditController* openSingleIdpEditWindow(const sem::TWO_PARAM_ARG_FUNC<quint64, quint64>& callback) {
    RelationIdpEditController* idpDataEditor = SignalHubSingleObjectManager::instance()->getObject<IdpDataEditorController*>(OPEN_SINGLE_IDP_EDITOR);
    if (idpDataEditor) {
        if (!idpDataEditor->isActiveWindow()) idpDataEditor->activateWindow();
        return idpDataEditor;
    }
    idpDataEditor = new IdpDataEditorController;
    idpDataEditor->setWindowTitle("IDP Editor");
    idpDataEditor->setAttribute(Qt::WA_DeleteOnClose);
    idpDataEditor->initAsyncCaller();
    idpDataEditor->setCloseCall(
        [callback, idpDataEditor](CDE90WindowCloseStatus) -> bool {
            if (callback)
                callback(idpDataEditor->getIdwTopId(), idpDataEditor->getIdpId());
            SignalHubSingleObjectManager::instance()->removeObject(OPEN_SINGLE_IDP_EDITOR);
            return true;
        });
    idpDataEditor->show();
    MainThreadInvoker::getInvoker().moveToScreenTopCenter(idpDataEditor);
    SignalHubSingleObjectManager::instance()->setObject(OPEN_SINGLE_IDP_EDITOR, idpDataEditor);
    return idpDataEditor;
}

SIGNAL_CHECK(__IDP__TARGET) {
    Q_UNUSED(sysVersion)
#ifdef _IDP_VERSION_
    bool succ = readVersion(_IDP_VERSION_, *libVersion);
    if (succ) {
        result->errCode = 0;
        result->errMsg.clear();
        //TODO 未来可添加版本号处理机制
    } else {
        result->errCode = LIBRARY_VERSION_READ_ERROR_CODE;
        result->errMsg = "IDP Editor version is not defined.";
    }
#else
#warning "IDP component version is not defined."
    result->errCode = 0;
    result->errMsg.clear();
#endif
}

SIGNAL_REG(__IDP__TARGET) {
    Q_UNUSED(sysVersion)
    PluginSignalHub::instance()->registSlot(OPEN_SINGLE_IDP_EDITOR, PluginHubAnyFunc<RelationIdpEditController*, const sem::TWO_PARAM_ARG_FUNC<quint64, quint64>&>([](const sem::TWO_PARAM_ARG_FUNC<quint64, quint64>& callback){
        return openSingleIdpEditWindow(callback);
    }));

    if (result) {
        result->errCode = 0;
        result->errMsg.clear();
    }
}

SIGNAL_UNREG(__IDP__TARGET){
    QString pid = QString::fromLatin1(pluginId);
    PluginSignalHub::instance()->unRegistAll(OPEN_SINGLE_IDP_EDITOR, [pid, callback](){
        callback(pid.toLatin1());
    });
}

IDP_SIGNALS_BRIDGE_H
#endif
