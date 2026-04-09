#ifndef SEM_SCAN_MODE_CONFIG_DAO_H
#define SEM_SCAN_MODE_CONFIG_DAO_H
#include "general_database_dao_global.h"
#include <QList>

#include "abstract_dao.h"
#include "scan_mode_config_entity.h"

class SEM_DAO_EXPORT SemScanModeConfigDao : public AbstractDao {
private:
    qint32 save(QList<ScanModeEntity>& list);

public:
    qint32 saveWithTransaction(QList<ScanModeEntity>& list);

    QList<ScanModeEntity> get(qint32* errCode = nullptr);
};

#endif // SEM_SCAN_MODE_CONFIG_DAO_H
