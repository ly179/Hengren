#ifndef MSR_DAO_H
#define MSR_DAO_H

#include "abstract_dao.h"
#include "general_database_dao_global.h"
#include "msr_entity.h"

class SqlConnectionFactory;

class SEM_DAO_EXPORT MsrDao : public AbstractDao{
private:
    QSharedPointer<SqlConnectionFactory> getConnectionFactory() override;

    qint32 checkAndSave(MsrEntity& entity);
public:

    int save(MsrEntity&);
    qint32 saveWithTransaction(MsrEntity &);

    qint32 updateTopId(quint64 id, quint64 topId, int *errCode);

    QList<MsrEntity> searchSimple(const MsrSearchDTO&, int* errCode = nullptr);
    MsrEntity getById(quint64 id, int* errCode = nullptr);
    MsrEntity getByTopId(quint64 topId, int* errCode = nullptr);

    quint64 getIdByName(quint64 classId, const QString& name, int* errCode = nullptr);

    int updateNameOnly(quint64 id, const QString& name);

    int updateNameById(quint64, const QString& name);

    //逻辑删除
    int deleteDataById(quint64);
    //物理删除
    int realDeleteDataById(quint64);

};

#endif // MSR_DAO_H
