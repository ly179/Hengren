#ifndef IDW_WAFER_SERVICE_H
#define IDW_WAFER_SERVICE_H
#include "general_database_service_global.h"
#include "idw_wafer_entity.h"
#include "iservice.h"
#include "idw_wafer_copy_model.h"

struct IDW_WaferServiceHelper;
class SEM_SERVICE_EXPORT IDW_WaferService : public IService {
    friend struct IDW_WaferServiceHelper;
private:
    IDW_WaferServiceHelper* helper;

public:
    IDW_WaferService();
    ~IDW_WaferService() noexcept;

    qint32 save(IDW_WaferInfoEntity &);
    qint32 updateTopId(quint64 id, quint64 topId, int *errCode = nullptr);
    qint32 saveWithTransaction(IDW_WaferInfoEntity &);

    IDW_WaferInfoEntity getWaferInfoById(quint64 id, qint32 *errCode = nullptr);
    IDW_WaferInfoEntity getWaferInfoByTopId(quint64 topId, qint32 *errCode = nullptr);
    IDW_WaferInfoEntity getSimpleInfoById(quint64 id, qint32 *errCode = nullptr);
    IDW_WaferInfoEntity getSimpleInfoByTopId(quint64 id, qint32 *errCode = nullptr);

    QString getWaferName(quint64 id, qint32 *errCode = nullptr);

    quint64 getWaferIdByName(const QString &name, quint64 classId, int *errCode = nullptr);
    IDW_WaferInfoEntity getSimpleWaferByName(const QString &name, quint64 classId, int *errCode = nullptr);

    quint64 getClassIdByWaferId(quint64, qint32* errCode = nullptr);

    QList<IDW_WaferInfoEntity> searchList(const IDW_WaferSearchDTO &param,
                                          qint32 *errCode = nullptr);

    qint32 updateWaferNameOnly(quint64 id, const QString &title);

    // 如果当前版本是有效版本，则会同时删除历史版本？
    qint32 deleteWaferById(quint64 id);

    qint32 deleteWaferByClass(quint64 classId);

    //IDW拷贝
    //拷贝会丢弃一切历史版本。
    IDW_CopyDataResult copyNew(const IDW_CopyDataParam& param);
};

#endif // IDW_WAFER_SERVICE_H
