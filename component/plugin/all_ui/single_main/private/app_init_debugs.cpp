#include "app_init_debugs.h"
#include "cdsem_global_data.h"
#include "service_manager.h"
#include "system_default_config_service.h"
#include "system_default_config_global_keys.h"
#include "om_camera_manager.h"
#include "sem_exception.h"

void AppInitDebugOm::load() {
    SystemDefaultConfigService* service = ServiceManager::defaultService<SystemDefaultConfigService>();
    int errCode;
    SystemDefaultConfigEntity entity = service->getByKey(sysConf::OM_LIGHT_PORT, &errCode);
    if (errCode) {
        emit overLoad(OperationResult {QString("Error to load OM Light port"), errCode});
        return;
    }
    if (entity.getId() == 0) {
        emit overLoad(OperationResult {QString("OM Debug Light port is not exists"), DATA_NOT_FOUND_ERROR_CODE}, true);
        return;
    }
    emit overLoad(OperationResult {});
}

AppInitDebugCalibration::AppInitDebugCalibration(QObject* parent) : AppInitLoadingData(parent) {
    setLoadOnTaskThread(true);
    setTitle("Init Debug Calibration");
}

void AppInitDebugCalibration::load() {
    SystemDefaultConfigService* service = ServiceManager::defaultService<SystemDefaultConfigService>();
    int errCode = service->getGlobalCalibration(CALIBRATION_ENV);
    if (errCode) {
        emit overLoad(OperationResult {"Error to load Calibration data.", errCode});
        return;
    }
    emit overLoad(OperationResult {});
}
