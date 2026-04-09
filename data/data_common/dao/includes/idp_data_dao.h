#ifndef IDP_DATA_DAO_H
#define IDP_DATA_DAO_H
#include <QList>
#include "abstract_dao.h"
#include "general_database_dao_global.h"
#include "idp_data_entity.h"
class SEM_DAO_EXPORT IdpDataDao : public AbstractDao {
private:
    qint32 checkAndSave(IdpDataEntity &);

public:

    qint32 save(IdpDataEntity &);

    qint32 updateTopId(quint64 id, quint64 topId, int *errCode = nullptr);

    qint32 saveWithTransaction(IdpDataEntity &);

    IdpDataEntity get(quint64 id, qint32 *errCode = nullptr);
    IdpDataEntity getByTopId(quint64 topId, qint32 *errCode = nullptr);
    IdpDataEntity getSimpleInfo(quint64 id, qint32 *errCode = nullptr);

    QString getIdpName(qint32 id, qint32 *errCode = nullptr);

    IdpDataEntity getSimpleIdpByName(quint64 classId, const QString& name, qint32* errCode = nullptr);

    QList<IdpDataEntity> searchList(const IdpSearchDTO &param, qint32 *errCode = nullptr);

    //检查某个IDW是否关联了IDP，如果需要忽略某个IDP的检查传第二个参数
    quint64 checkIdwHasIdp(quint64 idwId, quint64 skipIdpId = 0, int *errCode = nullptr);

    qint32 updateIdpNameOnly(quint64 id, const QString &title);

    // 如果当前版本是有效版本，则会同时删除历史版本？
    qint32 deleteIDPById(quint64 id);

    qint32 deleteIDPByClass(quint64 classId);
};

#endif // IDP_DATA_DAO_H
