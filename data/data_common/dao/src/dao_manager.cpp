#include "dao_manager.h"
#include "amp_data_dao.h"
#include "class_dao.h"
#include "idp_data_dao.h"
#include "idw_wafer_dao.h"
#include "recipe_data_dao.h"
#include "sem_scan_mode_config_dao.h"
#include "system_default_config_dao.h"
#include "template_info_dao.h"
#include "log_record_dao.h"
#include "image_relation_record_dao.h"
#include "msr_dao.h"
#include "memory_condition_dao.h"
#include "stage_memory_dao.h"

#include "dao_manager.h"
#include "cbox_config_dao.h"
#include "image_info_dao.h"
#include "file_seq_dao.h"
#include "core_dao_manager.h"
#include "base_dao_manager.h"

QHash<std::type_index, IDao*> unClearableHash;

DaoManager::~DaoManager() noexcept {
    for (auto it = allDaos.begin(); it != allDaos.end(); ++ it) {
        QHash<std::type_index, IDao*> h = *it;
        for (auto it2 = h.begin(); it2 != h.end(); ++ it2) {
            if (unClearableHash.contains(it2.key())) continue;
            delete *it2;
        }
    }
    allDaos.clear();
    unClearableHash.clear();
}

DaoManager::DaoManager() {
    {
        QHash<std::type_index, IDao*> hash;
        hash.insert(std::type_index(typeid(IDW_WaferDao)), new IDW_WaferDao);
        hash.insert(std::type_index(typeid(SemScanModeConfigDao)), new SemScanModeConfigDao);
        hash.insert(std::type_index(typeid(ClassDao)), new ClassDao);
        hash.insert(std::type_index(typeid(SystemDefaultConfigDao)), new SystemDefaultConfigDao);
        hash.insert(std::type_index(typeid(IdpDataDao)), new IdpDataDao);
        hash.insert(std::type_index(typeid(AmpDataDao)), new AmpDataDao);
        hash.insert(std::type_index(typeid(TemplateInfoDao)), new TemplateInfoDao);
        hash.insert(std::type_index(typeid(RecipeDataDao)), new RecipeDataDao);
        hash.insert(std::type_index(typeid(MemoryConditionDao)), new MemoryConditionDao);
        hash.insert(std::type_index(typeid(StageMemoryDao)), new StageMemoryDao);
        auto it  = hash.insert(std::type_index(typeid(CboxConfigDao)), BaseDaoManager::instance()->getDefaultFactoryDao<CboxConfigDao>());
        unClearableHash.insert(it.key(), it.value());
        allDaos.insert(SQL_DEFAULT_FACOTRY, hash);
    }
    {
        QHash<std::type_index, IDao*> hash;
        hash.insert(std::type_index(typeid(ImageRelationRecordDao)), new ImageRelationRecordDao);
        hash.insert(std::type_index(typeid(MsrDao)), new MsrDao);
        auto it = hash.insert(std::type_index(typeid(ImageInfoDao)), CoreDaoManager::instance()->getFileFactoryDao<ImageInfoDao>());
        unClearableHash.insert(it.key(), it.value());
        it = hash.insert(std::type_index(typeid(FileSeqDao)), CoreDaoManager::instance()->getFileFactoryDao<FileSeqDao>());
        unClearableHash.insert(it.key(), it.value());
        allDaos.insert(SQL_FILE_FACTORY, hash);
    }
    {
        QHash<std::type_index, IDao*> hash;
        allDaos.insert(SQL_AUTH_FACTORY, hash);
    }
    {
        QHash<std::type_index, IDao*> hash;
        hash.insert(std::type_index(typeid(LogRecordDao)), new LogRecordDao);
        allDaos.insert(SQL_LOG_FACTORY, hash);
    }
}
