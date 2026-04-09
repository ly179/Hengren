#include "idp_data_service.h"
#include "dao_manager.h"
#include "idp_data_dao.h"
#include "idw_wafer_service.h"
#include "class_service.h"
#include "service_manager.h"
#include "exception_code.h"
#include <QMutex>
#include <QMutexLocker>

struct IdpDataServiceHelper {
    QMutex nameLock;
    IdpDataService* d;
    IdpDataDao *idpDataDao;

    int checkNameRepeat(IdpDataEntity& entity) {
        int errCode;
        quint64 id = d->getIdpIdByName(entity.getClassId(), entity.getIdpName(), &errCode);
        if (errCode) return errCode;
        if (id > 0) {
            if (entity.getId() == 0 || (entity.getId() > 0 && id != entity.getId())) {
                return FILE_NAME_REPEAT_ERROR_CODE;
            }
        }
        return 0;
    }
};

int checkAndSetIdwTopId(IdpDataEntity & entity) {
    if (entity.getIdwId() > 0 && entity.getIdwTopId() <= 0) {
        int errCode;
        IDW_WaferInfoEntity wEntity = ServiceManager::defaultService<IDW_WaferService>()->getSimpleInfoById(entity.getIdwId(), &errCode);
        if (errCode) return errCode;
        if (wEntity.getId() == 0) {
            qCritical() << "IDW not found while saving IDP, idw_id: " << entity.getIdwId();
            return DATA_NOT_FOUND_ERROR_CODE;
        }
        entity.setIdwTopId(wEntity.getTopId());
    }
    return 0;
}

IdpDataService::IdpDataService() {
    helper = new IdpDataServiceHelper;
    helper->d = this;
    helper->idpDataDao = DaoManager::instance()->getDefaultFactoryDao<IdpDataDao>();
}

IdpDataService::~IdpDataService() noexcept {
    delete helper;
}

qint32 IdpDataService::save(IdpDataEntity & entity) {
    QMutexLocker locker(&helper->nameLock);
    bool hasTran = helper->idpDataDao->hasTransaction();
    bool succ = helper->idpDataDao->beginTransaction(true);
    if (!succ) {
        qCritical() << "Error to open transaction: IdpDataService::save";
        return SQL_OPEN_TRAN_ERROR_CODE;
    }
    int errCode = 0;
    sem::AfterGc gc([this, &errCode, &hasTran](){
        if (!hasTran && errCode && !helper->idpDataDao->rollback()) {
            qCritical() << "Error to rollback transaction: IdpDataService::save";
        }
    });
    errCode = checkAndSetIdwTopId(entity);
    if (errCode) return errCode;
    errCode = helper->checkNameRepeat(entity);
    if (errCode) return errCode;
    errCode = helper->idpDataDao->save(entity);
    if (!errCode && !hasTran && !helper->idpDataDao->commit()) {
        qCritical() << "Error to commit transaction: IdpDataService::save";
        errCode = SQL_COMMIT_ERROR_CODE;
    }
    return errCode;
}

qint32 IdpDataService::updateTopId(quint64 id, quint64 topId, int *errCode) {
    return helper->idpDataDao->updateTopId(id, topId, errCode);
}

qint32 IdpDataService::saveWithTransaction(IdpDataEntity &entity) {
    QMutexLocker locker(&helper->nameLock);
    bool hasTran = helper->idpDataDao->hasTransaction();
    bool succ = helper->idpDataDao->beginTransaction(true);
    if (!succ) {
        qCritical() << "Error to open transaction: IdpDataService::save";
        return SQL_OPEN_TRAN_ERROR_CODE;
    }
    int errCode = 0;
    sem::AfterGc gc([this, &errCode, &hasTran](){
        if (!hasTran && errCode && !helper->idpDataDao->rollback()) {
            qCritical() << "Error to rollback transaction: IdpDataService::saveWithTransaction";
        }
    });
    errCode = helper->checkNameRepeat(entity);
    if (errCode) return errCode;
    if (entity.getIdwId() > 0 && entity.getIdwTopId() <= 0) {
        errCode = checkAndSetIdwTopId(entity);
        if (errCode) return errCode;
        errCode = helper->idpDataDao->saveWithTransaction(entity);
    } else {
        errCode = helper->idpDataDao->saveWithTransaction(entity);
    }
    if (!errCode && !hasTran && !helper->idpDataDao->commit()) {
        qCritical() << "IDP save data commit transaction error.";
        return SQL_COMMIT_ERROR_CODE;
    }
    return errCode;
}

IdpDataEntity IdpDataService::get(quint64 id, qint32 *errCode) {
    return helper->idpDataDao->get(id, errCode);
}

IdpDataEntity IdpDataService::getByTopId(quint64 topId, qint32 *errCode) {
    return helper->idpDataDao->getByTopId(topId, errCode);
}

IdpDataEntity IdpDataService::getSimpleInfo(quint64 id, qint32 *errCode) {
    return helper->idpDataDao->getSimpleInfo(id, errCode);
}

QString IdpDataService::getIdpName(qint32 id, qint32 *errCode) {
    return helper->idpDataDao->getIdpName(id, errCode);
}

quint64 IdpDataService::getIdpIdByName(quint64 classId, const QString& name, qint32* errCode) {
    return helper->idpDataDao->getSimpleIdpByName(classId, name, errCode).getId();
}

IdpDataEntity IdpDataService::getSimpleIdpByName(quint64 classId, const QString& name, qint32* errCode) {
    return helper->idpDataDao->getSimpleIdpByName(classId, name, errCode);
}

QList<IdpDataEntity> IdpDataService::searchList(const IdpSearchDTO &param, qint32 *errCode) {
    return helper->idpDataDao->searchList(param, errCode);
}

//检查某个IDW是否关联了IDP，如果需要忽略某个IDP的检查传第二个参数
quint64 IdpDataService::checkIdwHasIdp(quint64 idwId, quint64 skipIdpId, int *errCode) {
    return helper->idpDataDao->checkIdwHasIdp(idwId, skipIdpId, errCode);
}

qint32 IdpDataService::updateIdpNameOnly(quint64 id, const QString &title) {
    return helper->idpDataDao->updateIdpNameOnly(id, title);
}

// 如果当前版本是有效版本，则会同时删除历史版本？
qint32 IdpDataService::deleteIDPById(quint64 id) {
    return helper->idpDataDao->deleteIDPById(id);
}

qint32 IdpDataService::deleteIDPByClass(quint64 classId) {
    return helper->idpDataDao->deleteIDPByClass(classId);
}

IDP_CopyDataResult IdpDataService::copyNew(const IDP_CopyDataParam& param) {
    IDP_CopyDataResult result;
    if (param.toClassName.isEmpty()) {
        result.errCode = NULL_POINTER_ERR_CODE;
        result.errMsg = "Please specify class name.";
        return result;
    }
    if (param.toName.isEmpty()) {
        result.errCode = NULL_POINTER_ERR_CODE;
        result.errMsg = "Please specify IDP name.";
        return result;
    }
    bool succ = helper->idpDataDao->beginTransaction(false);
    if (!succ) {
        result.errCode = SQL_OPEN_TRAN_ERROR_CODE;
        result.errMsg = "Error to open transaction.";
        return result;
    }
    {
        succ = false;
        sem::AfterGc gc([this, &succ](){
            if (succ) succ = helper->idpDataDao->commit();
            else helper->idpDataDao->rollback();
        });
        int errCode;
        ClassService* classService = ServiceManager::defaultService<ClassService>();
        quint64 classId = classService->getByName(param.toClassName, &errCode);
        if (errCode) {
            result.errCode = errCode;
            result.errMsg = "Error to check class name.";
            return result;
        }
        if (classId == 0) {
            ClassEntity classEntity;
            classEntity.setClassName(param.toClassName);
            errCode = classService->create(classEntity);
            if (errCode) {
                result.errCode = errCode;
                result.errMsg = "Error to create class.";
                return result;
            }
            classId = classEntity.getId();
        }

        IdpDataEntity idpEntity = get(param.fromId, &errCode);
        if (errCode) {
            result.errCode = errCode;
            result.errMsg = "Error to load IDP detail.";
            return result;
        }
        if (idpEntity.getId() == 0) {
            result.errCode = DATA_NOT_FOUND_ERROR_CODE;
            result.errMsg = "IDP data is not exists.";
            return result;
        }

        QString repeatWarnMsg = "";

        quint64 existsIdpId = getIdpIdByName(classId, param.toName, &errCode);
        if (errCode) {
            result.errCode = errCode;
            result.errMsg = "Error to check IDP name.";
            return result;
        }
        if (existsIdpId == param.fromId) {
            result.errCode = FILE_NAME_OVERWRITE_SELF_ERROR_CODE;
            result.errMsg = "Cannot overwrite IDP file self.";
            return result;
        }
        if (existsIdpId > 0 && !param.overwrite) {
            repeatWarnMsg = "IDP file name already exists.";
        }

        quint64 newIdwId = 0;
        if (idpEntity.getIdwId() > 0) {
            newIdwId = idpEntity.getIdwId();
            IDW_WaferService* idwService = ServiceManager::defaultService<IDW_WaferService>();
            IDW_WaferInfoEntity idwEntity = idwService->getWaferInfoById(idpEntity.getIdwId(), &errCode);
            if (errCode) {
                result.errCode = errCode;
                result.errMsg = "Error to load IDW detail.";
                return result;
            }
            if (idwEntity.getId() > 0) {
                if (param.toIdwName.isEmpty()) {
                    result.errCode = NULL_POINTER_ERR_CODE;
                    result.errMsg = "Please specify IDW name.";
                    return result;
                }

                quint64 existsIdwId = idwService->getWaferIdByName(param.toIdwName, classId, &errCode);
                if (errCode) {
                    result.errCode = errCode;
                    result.errMsg = "Error to check IDW name.";
                    return result;
                }
                if (existsIdwId > 0 && existsIdwId != idpEntity.getIdwId()) {
                    if (!param.overwrite) {
                        if (repeatWarnMsg.length() > 0) {
                            repeatWarnMsg.append("\nIDW file name already exists.");
                        } else {
                            repeatWarnMsg = "IDW file name already exists.";
                        }
                        result.errCode = FILE_NAME_REPEAT_ERROR_CODE;
                        result.errMsg = repeatWarnMsg;
                        return result;
                    }
                    errCode = idwService->deleteWaferById(existsIdwId);
                    if (errCode) {
                        result.errCode = errCode;
                        result.errMsg = "Error to overwrite IDW.";
                        return result;
                    }
                }
                if (existsIdwId == 0 || existsIdwId != idpEntity.getIdwId()) {
                    idwEntity.setId(0);
                    idwEntity.setCreateTime(QDateTime());
                    idwEntity.setEditTime(idwEntity.getCreateTime());
                    idwEntity.setClassId(classId);
                    idwEntity.setWaferName(param.toIdwName);
                    //如果IDW更新，IDP关联的IDW从Hitachi这边来看是不会联动更新的，使用的依旧是旧的IDW
                    //但是，拷贝后，这里将IDW拷贝为一个全新版本。
                    idwEntity.setInvalid(0);
                    errCode = idwService->saveWithTransaction(idwEntity);
                    if (errCode) {
                        result.errCode = errCode;
                        result.errMsg = "Error to copy IDW.";
                        return result;
                    }
                    newIdwId = idwEntity.getId();
                }
            }
        }
        if (repeatWarnMsg.length() > 0) {
            result.errCode = FILE_NAME_REPEAT_ERROR_CODE;
            result.errMsg = repeatWarnMsg;
            return result;
        }

        if (existsIdpId > 0) {
            errCode = deleteIDPById(existsIdpId);
            if (errCode) {
                result.errCode = errCode;
                result.errMsg = "Error to overwrite IDP.";
                return result;
            }
        }


        idpEntity.setId(0);
        idpEntity.setCreateTime(QDateTime());
        idpEntity.setEditTime(idpEntity.getCreateTime());
        idpEntity.setClassId(classId);
        idpEntity.setIdwId(newIdwId);
        idpEntity.setIdpName(param.toName);
        errCode = saveWithTransaction(idpEntity);
        if (errCode) {
            result.errCode = errCode;
            result.errMsg = "Error to copy IDP.";
            return result;
        }


        result.newClassId = idpEntity.getClassId();
        result.newId = idpEntity.getId();
        succ = true;
    }
    if (!succ) {
        result.errCode = SQL_COMMIT_ERROR_CODE;
        result.errMsg = "Error to commit transaction.";
    }
    return result;
}
