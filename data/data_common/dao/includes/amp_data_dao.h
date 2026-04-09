#ifndef AMP_DATA_DAO_H
#define AMP_DATA_DAO_H
#include <QList>
#include "abstract_dao.h"
#include "amp_data_entity.h"
#include "general_database_dao_global.h"

class SEM_DAO_EXPORT AmpDataDao : public AbstractDao {
private:
    qint32 save(AmpDataEntity &);

public:
    qint32 saveWithTransaction(AmpDataEntity &);

    AmpDataEntity get(quint64 id, qint32 *errCode = nullptr);

    quint64 checkName(const QString &ampName, qint32 *errCode = nullptr);

    QList<AmpDataEntity> searchList(const AmpDataSearchDTO &param, qint32 *errCode = nullptr);
};

#endif // AMP_DATA_DAO_H
