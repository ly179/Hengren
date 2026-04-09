#ifndef AMP_DATA_SERVICE_H
#define AMP_DATA_SERVICE_H

#include "amp_data_entity.h"
#include "general_database_service_global.h"
#include "iservice.h"
class AmpDataDao;

class SEM_SERVICE_EXPORT AmpDataService : public IService {
private:
    AmpDataDao *ampDataDao;

public:
    AmpDataService();
    ~AmpDataService() noexcept;

    qint32 saveWithTransaction(AmpDataEntity &);

    AmpDataEntity get(quint64 id, qint32 *errCode = nullptr);

    quint64 checkName(const QString &ampName, qint32 *errCode = nullptr);

    QList<AmpDataEntity> searchList(const AmpDataSearchDTO &param, qint32 *errCode = nullptr);
};

#endif // AMP_DATA_SERVICE_H
