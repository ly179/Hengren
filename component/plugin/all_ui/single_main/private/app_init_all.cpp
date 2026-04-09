#include "app_init_all.h"
#include <QWidget>
#include "app_init_loading_helper.h"

#include "app_init_debugs.h"
#include "app_init_hardwares.h"
#include "app_init_scan_params.h"
#include "app_init_single_objects.h"
#include "app_init_plugins.h"

void appendAll(SerialLoadingManager* manager) {
    manager->append(new AppInitPlugins);
    manager->append(new AppInitSingleObjects);
    manager->append(new AppInitScanParams);
    manager->append(new AppInitMemoryCondition);
    manager->append(new AppInitDebugCalibration);
}
