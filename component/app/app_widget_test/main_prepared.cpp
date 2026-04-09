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
#include "app_util.h"
#include "sem_global.h"
#include <QTimer>

#include "cde90_group_box.h"

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

void initPluginIds() {
    QString allPlugins = _SUB_PLUGIN_COMPONENTS_;
    QStringList list = allPlugins.split(" ", Qt::SkipEmptyParts);
    for (QString& pluginId : list) {
        if (pluginId.isEmpty()) continue;
        PluginLibManager::instance()->addUnregistCallback(pluginId, nullptr);
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

        PluginSignalHub::instance()->sendCreateObjMsg<QWidget*>("TEST_IDP_W").result->show();
    }
    return app.exec();
}
