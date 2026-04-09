#ifndef TEMPLATE_INFO_DAO_H
#define TEMPLATE_INFO_DAO_H

#include "abstract_dao.h"
#include "general_database_dao_global.h"
#include "template_info_entity.h"

class SEM_DAO_EXPORT TemplateInfoDao : public AbstractDao {
private:
    int save(TemplateInfoEntity &entity);

public:
    qint32 saveWithTransaction(TemplateInfoEntity &);

    TemplateInfoEntity getById(quint64, int *errCode = nullptr);
    QList<TemplateInfoEntity> getByIds(const QList<quint64> &ids, int *errCode = nullptr);
    TemplateInfoEntity getDataByIdForNewVersion(quint64 id, int *errCode = nullptr);
    QList<TemplateInfoEntity> getDatasByIdsForNewVersion(const QList<quint64> &ids,
                                                         int *errCode = nullptr);
    TemplateInfoEntity getSimpleInfoById(quint64, int *errCode = nullptr);
    QList<TemplateInfoEntity> getSimpleInfoByIds(const QList<quint64> &ids, int *errCode = nullptr);
    TemplateInfoEntity getSimpleInfoByIdForNewVersion(quint64, int *errCode = nullptr);

    QList<TemplateInfoEntity> searchList(const TemplateInfoSearchDTO &param,
                                         qint32 *errCode = nullptr);

    qint32 deleteTemplate(quint64, qint32 *errCode = nullptr);

    int updateNameOnly(quint64 id, const QString &text);

    quint64 getIdByName(const QString &name, const Integer32 &type, qint32 *errCode = nullptr);
    TemplateInfoEntity getSimpleInfoByName(const QString &name, const Integer32 &type, qint32 *errCode = nullptr);
};

#endif // TEMPLATE_INFO_DAO_H
