#ifndef MSR_SERVICE_H
#define MSR_SERVICE_H

#include "general_database_service_global.h"
#include "iservice.h"
#include "msr_entity.h"
#include "sem_common_data_model.h"
struct OperationResult;
struct MSR_Chip;

struct MsrServiceHelper;

class SEM_SERVICE_EXPORT MsrService : public IService{
    friend struct MsrServiceHelper;
private:
    MsrServiceHelper* helper;
public:
    MsrService();
    ~MsrService() noexcept;

    int save(MsrEntity&);
    QList<MsrEntity> searchSimple(const MsrSearchDTO&, int* errCode = nullptr);
    MsrEntity getById(quint64 id, int* errCode = nullptr);

    OperationResult save(MSR_Chip&, bool autoGenName = false);
    OperationResult getById(quint64 id, MSR_Chip&);
    OperationResult getByTopId(quint64 id, MSR_Chip&);

    quint64 getIdByName(quint64 classId, const QString& name, int* errCode = nullptr);

    int updateNameOnly(quint64 id, const QString& name);

    int updateNameById(quint64 id, const QString& name);

    //逻辑删除
    int deleteDataById(quint64);
    //物理删除
    int realDeleteDataById(quint64);
};

#endif // MSR_SERVICE_H
