#ifndef MEMORY_CONDITION_DAO_H
#define MEMORY_CONDITION_DAO_H

#include "abstract_dao.h"
#include "memory_condition_entity.h"
#include "general_database_dao_global.h"

class SEM_DAO_EXPORT MemoryConditionDao : public AbstractDao{
public:
    int save(const MemoryConditionEntity&);

    MemoryConditionEntity getById(quint64, int * errCode = nullptr);

    QList<MemoryConditionEntity> list(int * errCode = nullptr);

    int deleteById(quint64);
};

#endif // MEMORY_CONDITION_DAO_H
