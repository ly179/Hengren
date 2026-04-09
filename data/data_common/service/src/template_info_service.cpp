#include "template_info_service.h"
#include "dao_manager.h"
#include "template_info_dao.h"
#include "sem_common.h"

TemplateInfoService::TemplateInfoService() {
    templateInfoDao = DaoManager::instance()->getDefaultFactoryDao<TemplateInfoDao>();
}

TemplateInfoService::~TemplateInfoService() noexcept {}

qint32 TemplateInfoService::saveWithTransaction(
    TemplateInfoEntity &entity) {
    return templateInfoDao->saveWithTransaction(entity);
}

TemplateInfoEntity TemplateInfoService::getById(
    quint64 id, int *errCode) {
    return templateInfoDao->getById(id, errCode);
}
QList<TemplateInfoEntity> TemplateInfoService::getByIds(
    const QList<quint64> &ids, int *errCode) {
    return templateInfoDao->getByIds(ids, errCode);
}
TemplateInfoEntity TemplateInfoService::getDataByIdForNewVersion(
    quint64 id, int *errCode) {
    return templateInfoDao->getDataByIdForNewVersion(id, errCode);
}
QList<TemplateInfoEntity> TemplateInfoService::getDatasByIdsForNewVersion(
    const QList<quint64> &ids, int *errCode) {
    return templateInfoDao->getDatasByIdsForNewVersion(ids, errCode);
}
TemplateInfoEntity TemplateInfoService::getSimpleInfoById(
    quint64 id, int *errCode) {
    return templateInfoDao->getSimpleInfoById(id, errCode);
}
QList<TemplateInfoEntity> TemplateInfoService::getSimpleInfoByIds(
    const QList<quint64> &ids, int *errCode) {
    return templateInfoDao->getSimpleInfoByIds(ids, errCode);
}

TemplateInfoEntity TemplateInfoService::getSimpleInfoByIdForNewVersion(
    quint64 id, int *errCode) {
    return templateInfoDao->getSimpleInfoByIdForNewVersion(id, errCode);
}

QList<TemplateInfoEntity> TemplateInfoService::searchList(
    const TemplateInfoSearchDTO &param, qint32 *errCode) {
    return templateInfoDao->searchList(param, errCode);
}

qint32 TemplateInfoService::deleteTemplate(
    quint64 id, qint32 *errCode) {
    return templateInfoDao->deleteTemplate(id, errCode);
}

int TemplateInfoService::updateNameOnly(
    quint64 id, const QString &text) {
    return templateInfoDao->updateNameOnly(id, text);
}

quint64 TemplateInfoService::checkName(
    const QString &name, const Integer32 &type, qint32 *errCode) {
    return templateInfoDao->getIdByName(name, type, errCode);
}

TemplateInfoEntity TemplateInfoService::getSimpleInfoByName(const QString &name, const Integer32 &type, qint32 *errCode) {
    return templateInfoDao->getSimpleInfoByName(name, type, errCode);
}

qint32 TemplateInfoService::overrideAndSave(
    TemplateInfoEntity &entity, quint64 id) {
    bool hasTran = templateInfoDao->hasTransaction();
    bool succ = templateInfoDao->beginTransaction(true);
    if (!succ) {
        qCritical() << "Open transaction fail when TemplateInfoDao::save";
        return SQL_CONNECTION_OPEN_ERR_CODE;
    }
    int errCode;
    if (id > 0) {
        templateInfoDao->deleteTemplate(id, &errCode);
        if (errCode) {
            succ = false;
            return SQL_UPDATE_ERR_CODE;
        }
    }
    sem::AfterGc gc([this, &succ, hasTran]() {
        if (!hasTran) {
            if (succ) {
                templateInfoDao->commit();
            } else {
                templateInfoDao->rollback();
            }
        }
    });
    errCode = templateInfoDao->saveWithTransaction(entity);
    if (errCode) {
        succ = false;
    }
    return errCode;
}
