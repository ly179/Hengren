#ifndef IDP_DATA_SERVICE_H
#define IDP_DATA_SERVICE_H

#include "general_database_service_global.h"
#include "idp_data_entity.h"
#include "iservice.h"
#include "idp_data_copy_module.h"

struct IDP_CopyDataParam;
struct IdpDataServiceHelper;

class SEM_SERVICE_EXPORT IdpDataService : public IService {
    friend struct IdpDataServiceHelper;
private:
    IdpDataServiceHelper* helper;

public:
    IdpDataService();
    ~IdpDataService() noexcept;

    qint32 save(IdpDataEntity &);
    qint32 updateTopId(quint64 id, quint64 topId, int *errCode = nullptr);
    qint32 saveWithTransaction(IdpDataEntity &);

    IdpDataEntity get(quint64 id, qint32 *errCode = nullptr);
    IdpDataEntity getByTopId(quint64 topId, qint32 *errCode = nullptr);
    IdpDataEntity getSimpleInfo(quint64 id, qint32 *errCode = nullptr);

    QString getIdpName(qint32 id, qint32 *errCode = nullptr);

    quint64 getIdpIdByName(quint64 classId, const QString& name, qint32* errCode = nullptr);
    IdpDataEntity getSimpleIdpByName(quint64 classId, const QString& name, qint32* errCode = nullptr);

    QList<IdpDataEntity> searchList(const IdpSearchDTO &param, qint32 *errCode = nullptr);

    //检查某个IDW是否关联了IDP，如果需要忽略某个IDP的检查传第二个参数
    quint64 checkIdwHasIdp(quint64 idwId, quint64 skipIdpId = 0, int *errCode = nullptr);

    qint32 updateIdpNameOnly(quint64 id, const QString &title);

    // 如果当前版本是有效版本，则会同时删除历史版本？
    qint32 deleteIDPById(quint64 id);

    qint32 deleteIDPByClass(quint64 classId);

    IDP_CopyDataResult copyNew(const IDP_CopyDataParam& param);
};

#endif // IDP_DATA_SERVICE_H
