#include "config_module_task.h"
#include "app_task_worker.h"
#include "cdsem_global_data.h"
#include "service_manager.h"
#include "system_default_config_service.h"
#include "sem_scan_mode_config_service.h"
#include "image_scan_live_mode_helper.h"
#include "memory_condition_service.h"
#include "memory_condition_model.h"

TaskInitConfigModule::TaskInitConfigModule(QObject* parent) : TaskModule(parent) {}

TaskInitConfigModule::~TaskInitConfigModule() noexcept {}

OperationResult TaskInitConfigModule::doLoad(TSC cmd) {
    OperationResult result;
    if (step == 0){
        if (cmd.isStop()) return result;
        emit statusInfoChanged("Loading Calibration...");
        SystemDefaultConfigService* service = ServiceManager::defaultService<SystemDefaultConfigService>();
        int errCode = service->getGlobalCalibration(CALIBRATION_ENV);
        if (errCode) {
            result.errCode = errCode;
            result.errMsg = "Failed to load Calibration.";
            return result;
        }
        ++ step;
    }
    if (step == 1) {
        if (cmd.isStop()) return result;
        emit statusInfoChanged("Loading Scan Config...");
        if (!ImageScanLiveModeHelper::get()->isReady()) {
            SemScanModeConfigService* service = ServiceManager::defaultService<SemScanModeConfigService>();
            int errCode = service->initGlobal();
            if (errCode) {
                result.errCode = errCode;
                result.errMsg = "Failed to load Scan Config.";
                return result;
            }
        }
        ++ step;
    }
    if (step == 2) {
        if (cmd.isStop()) return result;
        emit statusInfoChanged("Loading Memory Condition...");
        MemoryConditionService* service = ServiceManager::defaultService<MemoryConditionService>();
        OperationResult result;
        QList<MemoryModelDataPack> list = service->modelList(&result.errCode);
        if (result.errCode) {
            result.errMsg = "Error to load memory condition.";
            return result;
        }
        if (list.size() > 0) {
            for (MemoryModelDataPack& p : list) {
                cdsem::CDSEM_GlobalDataHelper::get()->setMemoryCondition(p.memNo, p.data);
            }
        }
        ++ step;
    }
    return result;
}
