#ifndef CLASS_SERVICE_H
#define CLASS_SERVICE_H

#include "class_entity.h"
#include "general_database_service_global.h"
#include "iservice.h"
#include "sem_common_data_model.h"
class ClassDao;
struct OperationResult;

class SEM_SERVICE_EXPORT ClassService : public IService {
private:
    ClassDao *classDao;

public:
    ClassService();
    ~ClassService() noexcept;

    // 返回错误码
    qint32 create(ClassEntity &);

    // 返回更新了1条还是0条
    qint32 update(const ClassEntity &, qint32 *errCode = nullptr);

    // 返回删除的行数
    qint32 deleteClassOnly(quint64, qint32 *errCode = nullptr);

    ClassEntity getById(quint64, qint32 *errCode = nullptr);

    QList<ClassEntity> searchList(const ClassSearchDTO &param, qint32 *errCode = nullptr);

    void deleteClassAndContent(qint64 id, OperationResult *result = nullptr);

    quint64 getByName(const QString&, int* errCode = nullptr);

    //class拷贝，拷贝会保留IDW、IDP和Recipe的一切历史版本
    OperationResult copyNew(quint64 id, const QString& toName);
};

#endif // CLASS_SERVICE_H
