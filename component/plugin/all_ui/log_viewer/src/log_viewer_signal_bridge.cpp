//
// Created by Shisan on 2026/3/17.
//
#ifdef __LOG_VIEWER__TARGET
#include "log_viewer_signal_bridge.h"
#include "log_config_helper.h"

#include <cache_log_flusher.h>
#include <log_helper.h>

#include "plugin_signal_hub.h"
#include "signal_hub_plugin_ids.h"
#include "log_view_controller.h"
#include "signal_hub_single_object_manager.h"
#include "plugin_version_util.h"
#include "sem_global.h"
#include "exception_code.h"
#include "sem_common_data_model.h"

RelationLogViewerController* openSingleLogViewWindow() {
    RelationLogViewerController* logViewer = SignalHubSingleObjectManager::instance()->getObject<RelationLogViewerController*>(OPEN_SINGLE_LOG_VIEWER);
    if (logViewer) {
        if (!logViewer->isActiveWindow()) logViewer->activateWindow();
        return logViewer;
    }
    logViewer = new LogViewController;
    logViewer->setWindowTitle("Log Viewer");
    logViewer->setAttribute(Qt::WA_DeleteOnClose);
    logViewer->initAsyncCaller();
    logViewer->setCloseCall(
        [logViewer](CDE90WindowCloseStatus) -> bool {
            SignalHubSingleObjectManager::instance()->removeObject(OPEN_SINGLE_LOG_VIEWER);
            return true;
        });
    logViewer->show();
    MainThreadInvoker::getInvoker().moveToScreenTopCenter(logViewer);
    SignalHubSingleObjectManager::instance()->setObject(OPEN_SINGLE_LOG_VIEWER, logViewer);
    return logViewer;
}

SIGNAL_CHECK(__LOG_VIEWER__TARGET) {
    Q_UNUSED(sysVersion)
#ifdef _LOG_VIEWER_VERSION_
    bool succ = readVersion(_LOG_VIEWER_VERSION_, *libVersion);
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

SIGNAL_REG(__LOG_VIEWER__TARGET) {
    Q_UNUSED(sysVersion)
    PluginSignalHub::instance()->registSlot(OPEN_SINGLE_LOG_VIEWER, PluginHubAnyFunc<RelationLogViewerController*>([](){
        return openSingleLogViewWindow();
    }));
    PluginSignalHub::instance()->registSlot(CREATE_FULL_LEVEL_LOG_INS, PluginHubAnyFunc<void>([](){
        QHash<logview::LogViewLevelEnum, std::shared_ptr<CacheLogFlusher>> flushers;
        logview::readAndCreateLogConfig(flushers);
    }));

    if (result) {
        result->errCode = 0;
        result->errMsg.clear();
    }
}

SIGNAL_UNREG(__LOG_VIEWER__TARGET){
    QString pid = QString::fromLatin1(pluginId);
    QStringList list;
    list.reserve(2);
    list << CREATE_FULL_LEVEL_LOG_INS << OPEN_SINGLE_LOG_VIEWER;
    PluginSignalHub::instance()->unRegistAll(list, [pid, callback](){
        callback(pid.toLatin1());
    });
}

HENGREN_CD_SEM_LOG_VIEWER_SIGNAL_BRIDGE_H
#endif
