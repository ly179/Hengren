//
// Created by Shisan on 2026/1/14.
//

#ifndef HENGREN_CD_SEM_STAGE_MEMORY_DAO_H
#define HENGREN_CD_SEM_STAGE_MEMORY_DAO_H
#include "abstract_dao.h"
#include "general_database_dao_global.h"

class StageMemoryEntity;
class SEM_DAO_EXPORT StageMemoryDao : public AbstractDao{

public:
    int save(StageMemoryEntity&);
    QList<StageMemoryEntity> allAvailableMems(int* errCode = nullptr);
    int updateToDelete(quint64);
};


#endif //HENGREN_CD_SEM_STAGE_MEMORY_DAO_H