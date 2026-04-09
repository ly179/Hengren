#include "recipe_data_service.h"
#include "dao_manager.h"
#include "idp_data_service.h"
#include "idw_wafer_service.h"
#include "recipe_data_dao.h"
#include "exception_code.h"
#include "service_manager.h"
#include "sem_common_data_model.h"
#include "class_service.h"
#include <QMutex>
#include <QMutexLocker>

struct RecipeDataServiceHelper {
    QMutex nameLock;
    RecipeDataDao *recipeDao;
    RecipeDataService* d;

    int checkNameRepeat(RecipeDataEntity& entity) {
        int errCode;
        quint64 id = d->getRecipeIdByName(entity.getClassId(), entity.getRecipeName(), &errCode);
        if (errCode) return errCode;
        if (id > 0) {
            if (entity.getId() == 0 || (entity.getId() > 0 && id != entity.getId())) {
                return FILE_NAME_REPEAT_ERROR_CODE;
            }
        }
        return 0;
    }
};

int checkAndSetIdwTopId(RecipeDataEntity & entity) {
    if (entity.getIdwId() > 0 && entity.getIdwTopId() <= 0) {
        int errCode;
        IDW_WaferInfoEntity wEntity = ServiceManager::defaultService<IDW_WaferService>()->getSimpleInfoById(entity.getIdwId(), &errCode);
        if (errCode) return errCode;
        if (wEntity.getId() == 0) {
            qCritical() << "IDW not found while saving Recipe, idw_id: " << entity.getIdwId();
            return DATA_NOT_FOUND_ERROR_CODE;
        }
        entity.setIdwTopId(wEntity.getTopId());
    }
    return 0;
}

int checkAndSetIdpTopId(RecipeDataEntity & entity) {
    if (entity.getIdpId() > 0 && entity.getIdpTopId() <= 0) {
        int errCode;
        IdpDataEntity pEntity = ServiceManager::defaultService<IdpDataService>()->getSimpleInfo(entity.getIdpId(), &errCode);
        if (errCode) return errCode;
        if (pEntity.getId() == 0) {
            qCritical() << "IDP not found while saving Recipe, idp_id: " << entity.getIdpId();
            return DATA_NOT_FOUND_ERROR_CODE;
        }
        entity.setIdpTopId(pEntity.getTopId());
    }
    return 0;
}

RecipeDataService::RecipeDataService() {
    helper = new RecipeDataServiceHelper;
    helper->d = this;
    helper->recipeDao = DaoManager::instance()->getDefaultFactoryDao<RecipeDataDao>();
}

RecipeDataService::~RecipeDataService() noexcept {
    delete helper;
}

qint32 RecipeDataService::save(RecipeDataEntity & entity) {
    QMutexLocker locker(&helper->nameLock);
    bool hasTran = helper->recipeDao->hasTransaction();
    bool succ = helper->recipeDao->beginTransaction(true);
    if (!succ) {
        qCritical() << "Error to open transaction: RecipeDataService::save";
        return SQL_OPEN_TRAN_ERROR_CODE;
    }
    int errCode = 0;
    sem::AfterGc gc([this, &errCode, &hasTran](){
        if (!hasTran && errCode && !helper->recipeDao->rollback()) {
            qCritical() << "Error to rollback transaction: RecipeDataService::save";
        }
    });
    errCode = checkAndSetIdwTopId(entity);
    if (errCode) return errCode;
    errCode = checkAndSetIdpTopId(entity);
    if (errCode) return errCode;
    errCode = helper->checkNameRepeat(entity);
    if (errCode) return errCode;
    errCode = helper->recipeDao->save(entity);
    if (!errCode && !hasTran && !helper->recipeDao->commit()) {
        qCritical() << "Error to commit transaction: RecipeDataService::save";
        errCode = SQL_COMMIT_ERROR_CODE;
    }
    return errCode;
}

qint32 RecipeDataService::updateTopId(quint64 id, quint64 topId, int* errCode) {
    return helper->recipeDao->updateTopId(id, topId, errCode);
}

qint32 RecipeDataService::saveWithTransaction(
    RecipeDataEntity &entity) {
    QMutexLocker locker(&helper->nameLock);

    bool hasTran = helper->recipeDao->hasTransaction();
    bool succ = helper->recipeDao->beginTransaction(true);
    if (!succ) {
        qCritical() << "Error to open transaction: RecipeDataService::saveWithTransaction";
        return SQL_OPEN_TRAN_ERROR_CODE;
    }
    int errCode = 0;
    sem::AfterGc gc([this, &errCode, &hasTran](){
        if (!hasTran && errCode && !helper->recipeDao->rollback()) {
            qCritical() << "Error to rollback transaction: RecipeDataService::saveWithTransaction";
        }
    });

    errCode = helper->checkNameRepeat(entity);
    if (errCode) return errCode;
    if ((entity.getIdwId() > 0 && entity.getIdwTopId() <= 0) || (entity.getIdpId() > 0 && entity.getIdpTopId() <= 0)) {
        errCode = checkAndSetIdwTopId(entity);
        if (errCode) return errCode;
        errCode = checkAndSetIdpTopId(entity);
        if (errCode) return errCode;
        errCode = helper->recipeDao->saveWithTransaction(entity);
    } else {
        helper->recipeDao->saveWithTransaction(entity);
    }
    if (!errCode && !hasTran && !helper->recipeDao->commit()) {
        qCritical() << "Error to commit transaction: RecipeDataService::saveWithTransaction";
        errCode = SQL_COMMIT_ERROR_CODE;
    }
    return errCode;
}

RecipeDataEntity RecipeDataService::get(
    quint64 id, qint32 *errCode) {
    return helper->recipeDao->get(id, errCode);
}
RecipeDataEntity RecipeDataService::getSimpleInfo(
    quint64 id, qint32 *errCode) {
    return helper->recipeDao->getSimpleInfo(id, errCode);
}

quint64 RecipeDataService::getRecipeIdByName(quint64 classId, const QString& name, qint32* errCode) {
    return helper->recipeDao->getRecipeIdByName(classId, name, errCode);
}

RecipeDataEntity RecipeDataService::getSimpleRecipeByName(quint64 classId, const QString& name, qint32* errCode) {
    return helper->recipeDao->getSimpleRecipeByName(classId, name, errCode);
}

QList<RecipeDataEntity> RecipeDataService::searchList(
    const RecipeSearchDTO &param, qint32 *errCode) {
    return helper->recipeDao->searchList(param, errCode);
}

qint32 RecipeDataService::updateRecipeNameOnly(
    quint64 id, const QString &title) {
    return helper->recipeDao->updateRecipeNameOnly(id, title);
}

// 如果当前版本是有效版本，则会同时删除历史版本？
qint32 RecipeDataService::deleteRecipeById(
    quint64 id) {
    return helper->recipeDao->deleteRecipeById(id);
}

qint32 RecipeDataService::deleteRecipeByClass(
    quint64 classId) {
    return helper->recipeDao->deleteRecipeByClass(classId);
}

void RecipeDataService::eraseRecipe(
    qint64 id, bool removeIdw, bool removeIdp, OperationResult *result) {
    if (!helper->recipeDao->beginTransaction()) {
        if (result) {
            result->errCode = SQL_OPEN_TRAN_ERROR_CODE;
            result->errMsg = "Can't process this data.";
        }
        return;
    }
    bool succ = false;
    sem::AfterGc gc([&succ, this]() {
        if (succ) {
            helper->recipeDao->commit();
        } else {
            helper->recipeDao->rollback();
        }
    });
    int errCode;
    RecipeDataEntity entity = getSimpleInfo(id, &errCode);
    if (errCode) {
        succ = false;
        if (result) {
            result->errCode = SQL_QUERY_ERR_CODE;
            result->errMsg = "Failed to check Recipe file";
        }
        return;
    }
    IdpDataService *idpService = ServiceManager::instance()->getDefaultService<IdpDataService>();
    if (entity.getIdpId() > 0 && removeIdp) {
        errCode = idpService->deleteIDPById(entity.getIdpId());
        if (errCode) {
            succ = false;
            if (result) {
                result->errCode = SQL_UPDATE_ERR_CODE;
                result->errMsg = "Failed to delete IDP file.";
            }
            return;
        }
    }
    if (entity.getIdwId() > 0 && removeIdw) {
        quint64 ref = idpService->checkIdwHasIdp(entity.getIdwId(),
                                                 removeIdp ? entity.getIdpId() : 0,
                                                 &errCode);
        if (errCode) {
            succ = false;
            if (result) {
                result->errCode = SQL_QUERY_ERR_CODE;
                result->errMsg = "Failed to check reference IDW.";
            }
            return;
        }
        if (ref == 0) {
            IDW_WaferService *idwService = ServiceManager::instance()
                                               ->getDefaultService<IDW_WaferService>();
            errCode = idwService->deleteWaferById(entity.getIdwId());
            if (errCode) {
                succ = false;
                if (result) {
                    result->errCode = SQL_UPDATE_ERR_CODE;
                    result->errMsg = "Failed to delete IDW.";
                }
                return;
            }
        }
    }
    errCode = deleteRecipeById(id);
    if (errCode) {
        succ = false;
        if (result) {
            result->errCode = SQL_UPDATE_ERR_CODE;
            result->errMsg = "Failed to delete Recipe.";
        }
        return;
    }
    if (result) {
        result->errCode = 0;
    }
    succ = true;
}

Recipe_CopyDataResult RecipeDataService::copyNew(const Recipe_CopyDataParam& param) {
    Recipe_CopyDataResult result;
    if (param.toClassName.isEmpty()) {
        result.errCode = NULL_POINTER_ERR_CODE;
        result.errMsg = "Please specify class name.";
        return result;
    }
    if (param.toName.isEmpty()) {
        result.errCode = NULL_POINTER_ERR_CODE;
        result.errMsg = "Please specify Recipe name.";
        return result;
    }
    bool succ = helper->recipeDao->beginTransaction(false);
    if (!succ) {
        result.errCode = SQL_OPEN_TRAN_ERROR_CODE;
        result.errMsg = "Error to open transaction.";
        return result;
    }
    {
        succ = false;
        sem::AfterGc gc([this, &succ](){
            if (succ) succ = helper->recipeDao->commit();
            else helper->recipeDao->rollback();
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

        IdpDataService* idpService = ServiceManager::defaultService<IdpDataService>();
        IDW_WaferService* idwService = ServiceManager::defaultService<IDW_WaferService>();

        RecipeDataEntity recipeEntity = get(param.fromId, &errCode);
        if (errCode) {
            result.errCode = errCode;
            result.errMsg = "Error to load Recipe detail.";
            return result;
        }
        if (recipeEntity.getId() == 0) {
            result.errCode = DATA_NOT_FOUND_ERROR_CODE;
            result.errMsg = "Recipe data is not exists.";
            return result;
        }
        quint64 existsRecipeId = getRecipeIdByName(classId, param.toName, &errCode);
        if (errCode) {
            result.errCode = errCode;
            result.errMsg = "Error to check Recipe name.";
            return result;
        }
        if (existsRecipeId == param.fromId) {
            result.errCode = FILE_NAME_OVERWRITE_SELF_ERROR_CODE;
            result.errMsg = "Cannot overwrite Recipe file self.";
            return result;
        }
        QString repeatWarnMsg = "";
        if (existsRecipeId > 0) {
            if (!param.overwrite) {
                repeatWarnMsg = "Recipe file name already exists.";
            } else {
                errCode = deleteRecipeById(existsRecipeId);
                if (errCode) {
                    result.errCode = errCode;
                    result.errMsg = "Error to overwrite Recipe.";
                    return result;
                }
            }
        }
        IdpDataEntity idpEntity;
        IDW_WaferInfoEntity idwEntity;
        IDW_WaferInfoEntity idwOfIdpEntity;
        quint64 existsIdpId = 0;
        quint64 existsIdwId = 0;
        quint64 existsIdwOfIdpId = 0;
        quint64 newIdwId = 0, newIdpId = 0, newIdwOfIdpId = 0;
        if (recipeEntity.getIdpId() > 0) {
            idpEntity = idpService->get(recipeEntity.getIdpId(), &errCode);
            if (errCode) {
                result.errCode = errCode;
                result.errMsg = "Error to load IDP detail.";
                return result;
            }
        }
        if (idpEntity.getId() > 0) {
            newIdpId = idpEntity.getId();
            if (param.toIdpName.isEmpty()) {
                result.errCode = NULL_POINTER_ERR_CODE;
                result.errMsg = "Please specify IDP name.";
                return result;
            }
            existsIdpId = idpService->getIdpIdByName(classId, param.toIdpName, &errCode);
            if (errCode) {
                result.errCode = errCode;
                result.errMsg = "Error to check IDP name.";
                return result;
            }
            if (existsIdpId > 0 && existsIdpId != recipeEntity.getIdpId()) {
                if (!param.overwrite) {
                    if (repeatWarnMsg.length() > 0) {
                        repeatWarnMsg.append("\nIDP file name already exists.");
                    } else {
                        repeatWarnMsg = "IDP file name already exists.";
                    }
                } else {
                    errCode = idpService->deleteIDPById(existsIdpId);
                    if (errCode) {
                        result.errCode = errCode;
                        result.errMsg = "Error to overwrite IDP.";
                        return result;
                    }
                }
            }

            if (idpEntity.getIdwId() > 0) {
                idwOfIdpEntity = idwService->getWaferInfoById(idpEntity.getIdwId(), &errCode);
                if (errCode) {
                    result.errCode = errCode;
                    result.errMsg = "Error to load IDW of IDP.";
                    return result;
                }
            }
        }

        if (recipeEntity.getIdwId() > 0) {
            if (param.toIdwName.isEmpty()) {
                result.errCode = NULL_POINTER_ERR_CODE;
                result.errMsg = "Please specify IDW name.";
                return result;
            }
            idwEntity = idwService->getWaferInfoById(recipeEntity.getIdwId(), &errCode);
            if (errCode) {
                result.errCode = errCode;
                result.errMsg = "Error to load IDW.";
                return result;
            }
        }

        if (idwEntity.getId() > 0) {
            newIdwId = idwEntity.getId();
            if (param.toIdwName.isEmpty()) {
                result.errCode = NULL_POINTER_ERR_CODE;
                result.errMsg = "Please specify IDW name.";
                return result;
            }
            existsIdwId = idwService->getWaferIdByName(param.toIdwName, classId, &errCode);
            if (errCode) {
                result.errCode = errCode;
                result.errMsg = "Error to check IDW name.";
                return result;
            }
            if (existsIdwId > 0 && existsIdwId != recipeEntity.getIdwId()) {
                if (!param.overwrite) {
                    if (repeatWarnMsg.length() > 0) {
                        repeatWarnMsg.append("\nIDW file name already exists.");
                    } else {
                        repeatWarnMsg = "IDW file name already exists.";
                    }
                } else {
                    errCode = idwService->deleteWaferById(existsIdwId);
                    if (errCode) {
                        result.errCode = errCode;
                        result.errMsg = "Error to overwrite IDW.";
                        return result;
                    }
                }
            }
        }

        if (idwOfIdpEntity.getId() > 0) {
            newIdwOfIdpId = idwOfIdpEntity.getId();
            if (param.toIdwOfIdpName.isEmpty()) {
                result.errCode = NULL_POINTER_ERR_CODE;
                result.errMsg = "Please specify IDW of IDP name.";
                return result;
            }
            existsIdwOfIdpId = idwService->getWaferIdByName(param.toIdwOfIdpName, classId, &errCode);
            if (errCode) {
                result.errCode = errCode;
                result.errMsg = "Error to check IDW of IDP name.";
                return result;
            }
            if (existsIdwOfIdpId > 0 && existsIdwOfIdpId != idpEntity.getIdwId()) {
                if (!param.overwrite) {
                    if (repeatWarnMsg.length() > 0) {
                        repeatWarnMsg.append("\nIDW of IDP file name already exists.");
                    } else {
                        repeatWarnMsg = "IDW of IDP file name already exists.";
                    }
                } else {
                    errCode = idwService->deleteWaferById(existsIdwOfIdpId);
                    if (errCode) {
                        result.errCode = errCode;
                        result.errMsg = "Error to overwrite IDW of IDP.";
                        return result;
                    }
                }
            }
        }

        if (repeatWarnMsg.length() > 0) {
            result.errCode = FILE_NAME_REPEAT_ERROR_CODE;
            result.errMsg = repeatWarnMsg;
            return result;
        }

        if (existsIdpId > 0) {
            errCode = deleteRecipeById(existsIdpId);
            if (errCode) {
                result.errCode = errCode;
                result.errMsg = "Error to overwrite Recipe.";
                return result;
            }
        }

        if (idwEntity.getId() > 0 && (existsIdwId == 0 || existsIdwId != recipeEntity.getIdwId())) {
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
        if (idwOfIdpEntity.getId() > 0 && (existsIdwOfIdpId == 0 || existsIdwOfIdpId != idpEntity.getIdwId())) {
            idwOfIdpEntity.setId(0);
            idwOfIdpEntity.setCreateTime(QDateTime());
            idwOfIdpEntity.setEditTime(idwOfIdpEntity.getCreateTime());
            idwOfIdpEntity.setClassId(classId);
            idwOfIdpEntity.setWaferName(param.toIdwOfIdpName);
            //如果IDW更新，IDP关联的IDW从Hitachi这边来看是不会联动更新的，使用的依旧是旧的IDW
            //但是，拷贝后，这里将IDW拷贝为一个全新版本。
            idwOfIdpEntity.setInvalid(0);
            errCode = idwService->saveWithTransaction(idwOfIdpEntity);
            if (errCode) {
                result.errCode = errCode;
                result.errMsg = "Error to copy IDW of IDP.";
                return result;
            }
            newIdwOfIdpId = idwOfIdpEntity.getId();
        }
        if (idpEntity.getId() > 0 && (existsIdpId == 0 || existsIdpId != recipeEntity.getIdpId())) {
            idpEntity.setId(0);
            idpEntity.setCreateTime(QDateTime());
            idpEntity.setEditTime(idpEntity.getCreateTime());
            idpEntity.setClassId(classId);
            idpEntity.setIdwId(newIdwOfIdpId);
            idpEntity.setIdpName(param.toIdpName);
            idpEntity.setInvalid(0);
            errCode = idpService->saveWithTransaction(idpEntity);
            if (errCode) {
                result.errCode = errCode;
                result.errMsg = "Error to copy IDP.";
                return result;
            }
            newIdpId = idpEntity.getId();
        }

        recipeEntity.setId(0);
        recipeEntity.setCreateTime(QDateTime());
        recipeEntity.setEditTime(idpEntity.getCreateTime());
        recipeEntity.setClassId(classId);
        recipeEntity.setIdwId(newIdwId);
        recipeEntity.setIdpId(newIdpId);
        recipeEntity.setRecipeName(param.toName);
        errCode = saveWithTransaction(recipeEntity);
        if (errCode) {
            result.errCode = errCode;
            result.errMsg = "Error to copy Recipe.";
            return result;
        }

        result.newClassId = recipeEntity.getClassId();
        result.newId = recipeEntity.getId();
        succ = true;
    }
    if (!succ) {
        result.errCode = SQL_COMMIT_ERROR_CODE;
        result.errMsg = "Error to commit transaction.";
    }
    return result;
}
