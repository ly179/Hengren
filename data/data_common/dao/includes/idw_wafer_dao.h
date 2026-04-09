#ifndef IDW_WAFER_DAO_H
#define IDW_WAFER_DAO_H
#include "abstract_dao.h"
#include "general_database_dao_global.h"
#include "idw_wafer_entity.h"
class SEM_DAO_EXPORT IDW_WaferDao : public AbstractDao {
private:
    qint32 checkAndSave(IDW_WaferInfoEntity &);

public:

    qint32 save(IDW_WaferInfoEntity &);

    qint32 updateTopId(quint64 id, quint64 topId, int *errCode = nullptr);

    qint32 saveWithTransaction(IDW_WaferInfoEntity &);

    IDW_WaferInfoEntity getWaferInfoById(quint64 id, qint32 *errCode = nullptr);
    IDW_WaferInfoEntity getWaferInfoByTopId(quint64 topId, qint32 *errCode = nullptr);
    IDW_WaferInfoEntity getSimpleInfoById(quint64 id, qint32 *errCode = nullptr);
    IDW_WaferInfoEntity getSimpleInfoByTopId(quint64 topId, qint32 *errCode = nullptr);

    QString getWaferName(quint64 id, qint32 *errCode = nullptr);

    IDW_WaferInfoEntity getSimpleWaferIdByName(const QString &name, quint64 classId, int *errCode = nullptr);

    quint64 getClassIdByWaferId(quint64, qint32* errCode = nullptr);

    QList<IDW_WaferInfoEntity>searchList(const IDW_WaferSearchDTO& param,
                                         qint32                   *errCode = nullptr);

    qint32                    updateWaferNameOnly(quint64        id,
                                                  const QString& title);

    // 如果当前版本是有效版本，则会同时删除历史版本？
    qint32 deleteWaferById(quint64 id);

    qint32 deleteWaferByClass(quint64 classId);
};

#endif // IDW_WAFER_DAO_H
