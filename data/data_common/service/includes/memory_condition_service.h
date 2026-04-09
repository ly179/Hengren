#ifndef MEMORY_CONDITION_SERVICE_H
#define MEMORY_CONDITION_SERVICE_H

#include "memory_condition_entity.h"
#include "iservice.h"
#include "general_database_service_global.h"

class MemoryConditionDao;
struct MemoryModelData;
struct MemoryModelDataPack;
class SEM_SERVICE_EXPORT MemoryConditionService : public IService {
private:
    MemoryConditionDao* conditionDao;
public:
    MemoryConditionService();
    ~MemoryConditionService() noexcept;

    int save(const MemoryConditionEntity&);

    MemoryConditionEntity getById(quint64, int * errCode = nullptr);

    QList<MemoryConditionEntity> list(int * errCode = nullptr);

    int deleteById(quint64);

    int save(const MemoryModelDataPack&);
    QList<MemoryModelDataPack> modelList(int * errCode = nullptr);
};

#endif // MEMORY_CONDITION_SERVICE_H
