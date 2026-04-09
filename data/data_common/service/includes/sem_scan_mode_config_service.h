#ifndef SEM_SCAN_MODE_CONFIG_SERVICE_H
#define SEM_SCAN_MODE_CONFIG_SERVICE_H

#include "general_database_service_global.h"
#include "iservice.h"
#include "scan_mode_config_entity.h"

class SemScanModeConfigDao;

class SEM_SERVICE_EXPORT SemScanModeConfigService : public IService {
private:
    SemScanModeConfigDao* semScanModeConfigDao;

public:
    SemScanModeConfigService();
    ~SemScanModeConfigService() noexcept;

    qint32 saveWithTransaction(QList<ScanModeEntity>& list);

    QList<ScanModeEntity> get(qint32* errCode = nullptr);

    //初始化全局的扫描参数
    //如果初始化过了，则不会再次初始化
    //如果将reload设置为true，则会再次初始化
    int initGlobal(bool reload = false);
};

#endif // SEM_SCAN_MODE_CONFIG_SERVICE_H
