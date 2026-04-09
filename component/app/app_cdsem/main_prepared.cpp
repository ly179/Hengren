#include "main_prepared.h"
#include <QApplication>
#include "appliation_luncher_prepare.h"
#include "app_process_lock.h"
#include "cde90_message_box.h"
#include "cdsem_app_node_register.h"
#include "log_qt_bind.h"
#include "plugin_lib_manager.h"
#include "main_control_relation.h"
#include "relation_main_controller.h"
#include "base_init_handler.h"
#include "task_init_handler.h"
#include "app_util.h"
#include "sem_global.h"
#include "modules/config_module_task.h"
#include "modules/hardware_module_task.h"
#include <QTimer>
#include "log_viewer_relation.h"

void preparedEnv(QApplication& app) {
    bindQtLog(&app);
    cde90::registCDSEMComponent();
}

void initPluginPath() {
#ifdef __PLUGIN_DIR
    PluginLibManager::instance()->addPluginPath(__PLUGIN_DIR);
#endif
#ifdef __PLUGIN_LIB_DIR
    PluginLibManager::instance()->addPluginLibPath(__PLUGIN_LIB_DIR);
#endif
}

void unregistCallback(const char* c) {
    QString pluginId = QString::fromLatin1(c);
    qDebug() << "plugin: " << pluginId << " unregisted.";
}

void initPluginIds() {
    QString allPlugins = _SUB_PLUGIN_COMPONENTS_;
    QStringList list = allPlugins.split(" ", Qt::SkipEmptyParts);
    for (QString& pluginId : list) {
        if (pluginId.isEmpty()) continue;
#ifdef QT_DEBUG
        PluginLibManager::instance()->addUnregistCallback(pluginId + "d", nullptr);
#else
        PluginLibManager::instance()->addUnregistCallback(pluginId, nullptr);
#endif
    }
#ifdef _LOCKED_PLUGIN_VERSION_
    QString allLockedPlugins = "";
    list = allPlugins.split(" ", Qt::SkipEmptyParts);
    QString pluginId, version;
    for (int i = 1; i < list.size(); i += 2) {
        pluginId = list[i - 1];
        version = list[i];
        if (!PluginLibManager::instance()->addLockedVersion(pluginId, version)) {
            qCritical() << "Error to append locekd plugin version: " << pluginId << ", " << version;
        }
    }
#endif
}

RelationMainController* showMainControl() {
    MsgReceiveStatus<RelationMainController*> mainControlPane = main_control::createMainControl();
    if (!mainControlPane.ok) {
        simpleMessageBox(nullptr, "Error", "Main Control Pane is not exists.");
        return nullptr;
    }
    mainControlPane.result->setAttribute(Qt::WA_DeleteOnClose);
    mainControlPane.result->initAsyncCaller();
    mainControlPane.result->show();
    return mainControlPane.result;
}

int run(QApplication& app) {
    setCoreApp(&app);
    beforeLunchSetting(&app,
#ifdef __SOURCE_CODE_DIR
                       __SOURCE_CODE_DIR
#else
                       ""
#endif
                       );
    if (!uniqueProcess(&app, 100000)) {
        simpleMessageBox(nullptr, "Error", "The application process has been started up.");
        return 0;
    } else {
        preparedEnv(app);
        initPluginPath();
        initPluginIds();
        PluginLibManager::instance()->loadPlugins();
        log_viewer::prepareLogViewInstance();
        RelationMainController* mainController = showMainControl();
        if (!mainController) {
            QTimer::singleShot(1000, [](){QApplication::exit(1);});
        } else {
            initBaseServer();
            TaskInitWorker* worker = new TaskInitWorker(mainController);
            mainController->setCloseCall([worker](CDE90WindowCloseStatus){
                worker->notifyToStop();
                return true;
            });
            mainController->getAsyncCaller()->manage(worker, mainController);
            worker->initAsyncCaller(mainController->getAsyncCaller());
            TaskModule* module = new TaskInitConfigModule(worker);
            worker->appendTaskPackage(module);

            module = new HardwareModuleTask(worker);
            worker->appendTaskPackage(module);
            worker->startTask();
        }
    }
    return app.exec();
}
