#ifndef TEMPLATE_INFO_SERVICE_H
#define TEMPLATE_INFO_SERVICE_H

#include "general_database_service_global.h"
#include "iservice.h"
#include "template_info_entity.h"

class TemplateInfoDao;

class SEM_SERVICE_EXPORT TemplateInfoService : public IService {
private:
    TemplateInfoDao *templateInfoDao;

public:
    TemplateInfoService();
    ~TemplateInfoService() noexcept;

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

    quint64 checkName(const QString &name, const Integer32 &type, qint32 *errCode = nullptr);
    TemplateInfoEntity getSimpleInfoByName(const QString &name, const Integer32 &type, qint32 *errCode = nullptr);

    qint32 overrideAndSave(TemplateInfoEntity &, quint64 id = 0);
};

#endif // TEMPLATE_INFO_SERVICE_H
