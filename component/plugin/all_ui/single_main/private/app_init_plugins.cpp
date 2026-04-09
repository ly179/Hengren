#include "app_init_plugins.h"
#include "plugin_lib_manager.h"

AppInitPlugins::AppInitPlugins(QObject* parent) : AppInitLoadingData(parent) {
    setLoadOnTaskThread(true);
    setTitle("Loading Plugin");
}

void AppInitPlugins::load() {
    PluginLibManager::instance()->loadPlugins();
    emit overLoad(OperationResult {}, true);
}