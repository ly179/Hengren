#ifndef CLASS_DAO_H
#define CLASS_DAO_H
#include <QList>
#include "abstract_dao.h"
#include "class_entity.h"
#include "general_database_dao_global.h"

class SEM_DAO_EXPORT ClassDao : public AbstractDao {
public:

    // 返回错误码
    qint32 create(ClassEntity&);

    // 返回更新了1条还是0条
    qint32 update(const ClassEntity&,
                  qint32 *errCode = nullptr);

    // 返回删除的行数
    qint32            deleteClass(quint64,
                                  qint32 *errCode = nullptr);

    ClassEntity       getById(quint64,
                              qint32 *errCode = nullptr);

    QList<ClassEntity>searchList(const ClassSearchDTO& param,
                                 qint32               *errCode = nullptr);

    quint64 getByName(const QString&, int* errCode);
};

#endif // CLASS_DAO_H
