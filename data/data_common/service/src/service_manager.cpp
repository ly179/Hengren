#include "service_manager.h"
#include "amp_data_service.h"
#include "class_service.h"
#include "idp_data_service.h"
#include "idw_wafer_service.h"
#include "recipe_data_service.h"
#include "sem_scan_mode_config_service.h"
#include "system_default_config_service.h"
#include "template_info_service.h"
#include "log_record_service.h"
#include "image_relation_record_service.h"
#include "msr_service.h"
#include "memory_condition_service.h"
#include  "stage_memory_service.h"

#include "image_info_service.h"
#include "file_seq_service.h"
#include "cbox_config_service.h"

#include "base_service_manager.h"
#include "core_service_manager.h"

QHash<std::type_index, IService*> unclearableHash;

ServiceManager::ServiceManager() {
    allServices.insert(std::type_index(typeid(IDW_WaferService)), new IDW_WaferService);
    allServices.insert(std::type_index(typeid(SemScanModeConfigService)), new SemScanModeConfigService);
    allServices.insert(std::type_index(typeid(ClassService)), new ClassService);
    allServices.insert(std::type_index(typeid(SystemDefaultConfigService)), new SystemDefaultConfigService);
    allServices.insert(std::type_index(typeid(IdpDataService)), new IdpDataService);
    allServices.insert(std::type_index(typeid(AmpDataService)), new AmpDataService);
    allServices.insert(std::type_index(typeid(TemplateInfoService)), new TemplateInfoService);
    allServices.insert(std::type_index(typeid(RecipeDataService)), new RecipeDataService);
    allServices.insert(std::type_index(typeid(MemoryConditionService)), new MemoryConditionService);
    allServices.insert(std::type_index(typeid(StageMemoryService)), new StageMemoryService);

    allServices.insert(std::type_index(typeid(ImageRelationRecordService)), new ImageRelationRecordService);
    allServices.insert(std::type_index(typeid(MsrService)), new MsrService);

    allServices.insert(std::type_index(typeid(LogRecordService)), new LogRecordService);

    auto it = allServices.insert(std::type_index(typeid(CboxConfigService)), BaseServiceManager::defaultService<CboxConfigService>());
    unclearableHash.insert(it.key(), it.value());
    it = allServices.insert(std::type_index(typeid(ImageInfoService)), CoreServiceManager::defaultService<ImageInfoService>());
    unclearableHash.insert(it.key(), it.value());
    it = allServices.insert(std::type_index(typeid(FileSeqService)), CoreServiceManager::defaultService<FileSeqService>());
    unclearableHash.insert(it.key(), it.value());
}

ServiceManager::~ServiceManager() noexcept {
    for (auto it = allServices.begin(); it != allServices.end(); ++ it) {
        if (unclearableHash.contains(it.key())) continue;
        delete *it;
    }
    allServices.clear();
    unclearableHash.clear();
}
