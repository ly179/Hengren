#include "amp_signal_bridge.h"
#ifdef _AMP_TARGET
#include "plugin_signal_hub.h"
#include "signal_hub_plugin_ids.h"
#include "signal_hub_single_object_manager.h"
#include "amp_edit_controller.h"
#include "sem_global.h"

#include "plugin_version_util.h"
#include "exception_code.h"
#include "sem_common_data_model.h"

#include "cde90_layout_helper.h"
#include "detection_common_setting_controller.h"

#define TAMPDS "ampds"

RelatoinAmpEditController* openSignalAMPWindow() {
    AmpEditController* editor = SignalHubSingleObjectManager::instance()->getObject<AmpEditController*>(OPEN_SINGLE_AMP_EDITOR);
    if (editor) {
        if (!editor->isActiveWindow()) editor->activateWindow();
        return editor;
    }
    editor = new AmpEditController;
    editor->setWindowTitle("AMP");
    editor->setAttribute(Qt::WA_DeleteOnClose);
    editor->initAsyncCaller(
        []() {
            SignalHubSingleObjectManager::instance()->removeObject(OPEN_SINGLE_AMP_EDITOR);
        }, true);
    editor->show();
    MainThreadInvoker::getInvoker().moveToScreenCenter(editor);
    SignalHubSingleObjectManager::instance()->setObject(OPEN_SINGLE_AMP_EDITOR, editor);
    return editor;
}

RelatoinAmpEditController* openSignalAMPWindow(int id, const QString& json) {
    RelatoinAmpEditController* editor = openSignalAMPWindow();
    if (id >= 0 && json.size() > 0) {
        editor->loadAmp(cdsem::MeasureType(id), json);
    } else {
        editor->loadAmp();
    }
    return editor;
}

SIGNAL_CHECK(_AMP_TARGET) {
    Q_UNUSED(sysVersion)
#ifdef _AMP_VERSION_
    bool succ = readVersion(_AMP_VERSION_, *libVersion);
    if (succ) {
        result->errCode = 0;
        result->errMsg.clear();
        //TODO 未来可添加版本号处理机制
    } else {
        result->errCode = LIBRARY_VERSION_READ_ERROR_CODE;
        result->errMsg = "AMP version is not defined.";
    }
#else
#warning "AMP component version is not defined."
    result->errCode = 0;
    result->errMsg.clear();
#endif
}

SIGNAL_REG(_AMP_TARGET) {
    Q_UNUSED(sysVersion)
    registTypeHandler(TAMPDS,
        [](CDE90LayoutNode& current,
            const QHash<QString, QObject*>&,
            QHash<QString, QList<QStringList>>& allProperties) -> CDE90EngineObjectPtr {
                Q_UNUSED(allProperties)
                CDE90LayoutNode* parent = current.getParent();
                QWidget* pWidget = parent->getQObject<QWidget>();
                if (!pWidget) {
                    return CDE90EngineObjectPtr();
                }
                return {new DetectionCommonSettingController(pWidget), HE_QObject, true};
        });
    PluginSignalHub::instance()->registSlot(OPEN_SINGLE_AMP_EDITOR, PluginHubAnyFunc<RelatoinAmpEditController*>([](){
        return openSignalAMPWindow(-1, "");
    }));
    PluginSignalHub::instance()->registSlot(OPEN_SINGLE_AMP_EDITOR, PluginHubAnyFunc<RelatoinAmpEditController*, int, const QString&>([](int id, const QString& json){
        return openSignalAMPWindow(id, json);
    }));
    if (result) {
        result->errCode = 0;
        result->errMsg.clear();
    }
}

SIGNAL_UNREG(_AMP_TARGET){
    QString pid = QString::fromLatin1(pluginId);
    PluginSignalHub::instance()->unRegistAll(OPEN_SINGLE_AMP_EDITOR, [pid, callback](){
        unRegistTypeHandler(TAMPDS);
        callback(pid.toLatin1());
    });
}
AMP_SIGNAL_BRIDGE_H
#endif

