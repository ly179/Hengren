#ifndef SYSTEM_DEFAULT_CONFIG_SERVICE_H
#define SYSTEM_DEFAULT_CONFIG_SERVICE_H

#include "general_database_service_global.h"
#include "iservice.h"
#include "system_default_config_entity.h"

class SystemDefaultConfigDao;
class CDSEM_GlobalCalibrationPyParamModel;

class SEM_SERVICE_EXPORT SystemDefaultConfigService : public IService {
private:
    SystemDefaultConfigDao *systemDefaultConfigDao;

public:
    SystemDefaultConfigService();
    ~SystemDefaultConfigService() noexcept;

    qint32 saveWithTransaction(const SystemDefaultConfigEntity &);

    SystemDefaultConfigEntity getByKey(const QString &, int *errCode = nullptr);

    int saveGlobalCalibration(const CDSEM_GlobalCalibrationPyParamModel&);
    int getGlobalCalibration(CDSEM_GlobalCalibrationPyParamModel&);
};

#endif // SYSTEM_DEFAULT_CONFIG_SERVICE_H
