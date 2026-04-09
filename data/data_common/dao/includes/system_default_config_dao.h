#ifndef SYSTEM_DEFAULT_CONFIG_DAO_H
#define SYSTEM_DEFAULT_CONFIG_DAO_H
#include <QString>
#include "abstract_dao.h"
#include "general_database_dao_global.h"
#include "system_default_config_entity.h"

class SEM_DAO_EXPORT SystemDefaultConfigDao : public AbstractDao {
private:
    qint32 save(const SystemDefaultConfigEntity &);

public:
    qint32 saveWithTransaction(const SystemDefaultConfigEntity &);

    SystemDefaultConfigEntity getByKey(const QString&,
                                       int *errCode);
};

#endif // SYSTEM_DEFAULT_CONFIG_DAO_H
