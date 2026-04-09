#include "memory_condition_service.h"
#include "dao_manager.h"
#include "memory_condition_dao.h"
#include "memory_condition_model.h"
#include "memory_condition_model_convert.h"
#include "exception_code.h"

MemoryConditionService::MemoryConditionService() {
    conditionDao = DaoManager::instance()->getDefaultFactoryDao<MemoryConditionDao>();
}
MemoryConditionService::~MemoryConditionService() noexcept {}

int MemoryConditionService::save(const MemoryConditionEntity& entity) {
    return conditionDao->save(entity);
}

MemoryConditionEntity MemoryConditionService::getById(quint64 id, int * errCode) {
    return conditionDao->getById(id, errCode);
}

QList<MemoryConditionEntity> MemoryConditionService::list(int * errCode) {
    return conditionDao->list(errCode);
}

int MemoryConditionService::deleteById(quint64 id) {
    return conditionDao->deleteById(id);
}

int MemoryConditionService::save(const MemoryModelDataPack& pack) {
    if (pack.memNo < 1 || pack.memNo > MAX_MEM_NO) {
        return DATA_INDEX_OUT_RANGE_ERROR_CODE;
    }
    MemoryConditionEntity entity;
    entity.setId(pack.memNo);
    toMemoryConditionDataModelJson(entity.getConditionDetail(), pack.data);
    return save(entity);
}

QList<MemoryModelDataPack> MemoryConditionService::modelList(int * errCode) {
    int eCode;
    QList<MemoryModelDataPack> modelList;
    QList<MemoryConditionEntity> list = this->list(&eCode);
    if (eCode) {
        if (errCode) *errCode = eCode;
        return modelList;
    }
    if (list.isEmpty()) {
        if (errCode) *errCode = 0;
        return modelList;
    }
    modelList.reserve(list.size());
    MemoryModelDataPack pack;
    for (MemoryConditionEntity& entity : list) {
        pack.memNo = entity.getId();
        eCode = fromMemoryConditionDataModelJson(entity.getConditionDetail(), pack.data);
        if (eCode) {
            if (errCode) *errCode = eCode;
            modelList.clear();
            return modelList;
        }
        modelList.append(pack);
    }
    if (errCode) *errCode = 0;
    return modelList;
}
