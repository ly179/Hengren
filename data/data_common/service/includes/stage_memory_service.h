//
// Created by Shisan on 2026/1/14.
//
#ifndef HENGREN_CD_SEM_STAGE_MEMORY_SERVICE_H
#define HENGREN_CD_SEM_STAGE_MEMORY_SERVICE_H
#include  "iservice.h"
#include "general_database_service_global.h"

class StageMemoryEntity;
class StageMemoryDao;
class SEM_SERVICE_EXPORT StageMemoryService : public IService{
private:
    StageMemoryDao* memDao;
public:
    StageMemoryService();
    ~StageMemoryService() noexcept override;

    int save(StageMemoryEntity&);
    QList<StageMemoryEntity> allAvailableMems(int* errCode = nullptr);
    int updateToDelete(quint64);
};


#endif //HENGREN_CD_SEM_STAGE_MEMORY_SERVICE_H