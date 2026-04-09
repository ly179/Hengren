#include "app_init_scan_params.h"
#include "sem_scan_mode_config_service.h"
#include "service_manager.h"
#include "memory_condition_service.h"
#include "service_manager.h"
#include "cdsem_global_data.h"
#include "memory_condition_model.h"

AppInitScanParams::AppInitScanParams(QObject* parent) : AppInitLoadingData(parent) {
    setLoadOnTaskThread(true);
    setTitle("Load Scan Config");
}

void AppInitScanParams::load() {
    SemScanModeConfigService* service = ServiceManager::defaultService<SemScanModeConfigService>();
    int errCode = service->initGlobal();
    emit overLoad(OperationResult{errCode == 0 ? "" : "Error to load Scan Config Parameters.", errCode});
}

AppInitMemoryCondition::AppInitMemoryCondition(QObject* parent) : AppInitLoadingData(parent) {
    setLoadOnTaskThread(true);
    setTitle("Load Memory Condition");
}

void AppInitMemoryCondition::load() {
    MemoryConditionService* service = ServiceManager::defaultService<MemoryConditionService>();
    OperationResult result;
    QList<MemoryModelDataPack> list = service->modelList(&result.errCode);
    if (result.errCode) {
        result.errMsg = "Error to load memory condition.";
    }
    if (list.size() > 0) {
        for (MemoryModelDataPack& p : list) {
            cdsem::CDSEM_GlobalDataHelper::get()->setMemoryCondition(p.memNo, p.data);
        }
    }
    emit overLoad(result);
}
