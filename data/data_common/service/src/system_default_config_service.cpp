#include "system_default_config_service.h"
#include "dao_manager.h"
#include "system_default_config_dao.h"
#include "cdsem_global_env_data_model_convert.h"
#include "system_default_config_keys.h"

SystemDefaultConfigService::SystemDefaultConfigService() {
    systemDefaultConfigDao = DaoManager::instance()->getDefaultFactoryDao<SystemDefaultConfigDao>();
}

SystemDefaultConfigService::~SystemDefaultConfigService() noexcept {}

qint32 SystemDefaultConfigService::saveWithTransaction(
    const SystemDefaultConfigEntity &entity) {
    return systemDefaultConfigDao->saveWithTransaction(entity);
}

SystemDefaultConfigEntity SystemDefaultConfigService::getByKey(
    const QString &key, int *errCode) {
    return systemDefaultConfigDao->getByKey(key, errCode);
}

int SystemDefaultConfigService::saveGlobalCalibration(const CDSEM_GlobalCalibrationPyParamModel& param) {
    QString json;
    calibrationPyParamToJson(json, param);
    SystemDefaultConfigEntity entity;
    entity.setSKey(SDC_GLOBAL_CALIBRATION);
    entity.setSValue(json);
    return saveWithTransaction(entity);
}

int SystemDefaultConfigService::getGlobalCalibration(CDSEM_GlobalCalibrationPyParamModel& param) {
    int errCode;
    SystemDefaultConfigEntity entity = getByKey(SDC_GLOBAL_CALIBRATION, &errCode);
    if (errCode) return errCode;
    if (entity.getId() == 0) return 0;
    return calibrationPyParamFromJson(entity.getSValue(), param);
}
