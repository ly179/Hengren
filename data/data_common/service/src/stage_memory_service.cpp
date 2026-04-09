//
// Created by Shisan on 2026/1/14.
//

#include "stage_memory_service.h"
#include "stage_memory_entity.h"
#include "stage_memory_dao.h"
#include "dao_manager.h"

StageMemoryService::StageMemoryService() {
    memDao = DaoManager::instance()->getDefaultFactoryDao<StageMemoryDao>();
}

StageMemoryService::~StageMemoryService() noexcept {}

int StageMemoryService::save(StageMemoryEntity& entity) {
    return memDao->save(entity);
}

QList<StageMemoryEntity> StageMemoryService::allAvailableMems(int* errCode) {
    return memDao->allAvailableMems(errCode);
}

int StageMemoryService::updateToDelete(quint64 id) {
    return memDao->updateToDelete(id);
}