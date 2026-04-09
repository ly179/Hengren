#include "class_service.h"
#include "class_dao.h"
#include "dao_manager.h"
#include "idp_data_service.h"
#include "idw_wafer_service.h"
#include "recipe_data_service.h"
#include "exception_code.h"
#include "service_manager.h"

ClassService::ClassService() {
    classDao = DaoManager::instance()->getDefaultFactoryDao<ClassDao>();
}
ClassService::~ClassService() noexcept {}

// 返回错误码
qint32 ClassService::create(
    ClassEntity &entity) {
    return classDao->create(entity);
}

// 返回更新了1条还是0条
qint32 ClassService::update(
    const ClassEntity &entity, qint32 *errCode) {
    return classDao->update(entity, errCode);
}

// 返回删除的行数
qint32 ClassService::deleteClassOnly(
    quint64 id, qint32 *errCode) {
    return classDao->deleteClass(id, errCode);
}

ClassEntity ClassService::getById(
    quint64 id, qint32 *errCode) {
    return classDao->getById(id, errCode);
}

QList<ClassEntity> ClassService::searchList(
    const ClassSearchDTO &param, qint32 *errCode) {
    return classDao->searchList(param, errCode);
}

void ClassService::deleteClassAndContent(
    qint64 id, OperationResult *result) {
    if (!classDao->beginTransaction()) {
        if (result) {
            result->errCode = SQL_OPEN_TRAN_ERROR_CODE;
            result->errMsg = "Can't begin this process.";
        }
        return;
    }
    bool succ = false;
    sem::AfterGc gc([this, &succ]() {
        if (succ) {
            classDao->commit();
        } else {
            classDao->rollback();
        }
    });
    int errCode;
    deleteClassOnly(id, &errCode);
    if (errCode) {
        if (result) {
            result->errCode = errCode;
            result->errMsg = "Delete Class Error";
        }
        return;
    }
    IDW_WaferService *waferService = ServiceManager::instance()
                                         ->getDefaultService<IDW_WaferService>();
    errCode = waferService->deleteWaferByClass(id);
    if (errCode) {
        if (result) {
            result->errCode = errCode;
            result->errMsg = "Delete IDW Error";
        }
        return;
    }
    IdpDataService *idpService = ServiceManager::instance()->getDefaultService<IdpDataService>();
    errCode = idpService->deleteIDPByClass(id);
    if (errCode) {
        if (result) {
            result->errCode = errCode;
            result->errMsg = "Delete IDP Error";
        }
        return;
    }
    RecipeDataService *recipeService = ServiceManager::instance()
                                           ->getDefaultService<RecipeDataService>();
    errCode = recipeService->deleteRecipeByClass(id);
    if (errCode) {
        if (result) {
            result->errCode = errCode;
            result->errMsg = "Delete IDP Error";
        }
        return;
    }
    if (result) {
        result->errCode = 0;
        result->errMsg = "";
        succ = true;
    }
}

quint64 ClassService::getByName(const QString& name, int* errCode) {
    return classDao->getByName(name, errCode);
}

OperationResult ClassService::copyNew(quint64 id, const QString& toName) {
    OperationResult result;
    if (toName.isEmpty()) {
        result.errCode = NULL_POINTER_ERR_CODE;
        result.errMsg = "Please specify new class name.";
        return result;
    }
    bool succ = classDao->beginTransaction();
    if (!succ) {
        result.errCode = SQL_OPEN_TRAN_ERROR_CODE;
        result.errMsg = "Open transaction failed.";
        return result;
    }
    {
        succ = false;
        sem::AfterGc gc([&succ, this](){
            if (succ) succ = classDao->commit();
            else classDao->rollback();
        });
        int errCode;
        quint64 existsNameId = getByName(toName, &errCode);
        if (errCode) {
            result.errCode = errCode;
            result.errMsg = "Error to check class name.";
            return result;
        }
        if (existsNameId > 0) {
            result.errCode = FILE_NAME_REPEAT_ERROR_CODE;
            result.errMsg = QString("%1 has been exists.").arg(toName);
            return result;
        }
        ClassEntity classEntity = getById(id, &errCode);
        if (errCode) {
            result.errCode = errCode;
            result.errMsg = "Error to Load Class Info.";
            return result;
        }
        if (classEntity.getId() == 0) {
            result.errCode = DATA_NOT_FOUND_ERROR_CODE;
            result.errMsg = "Class data is not exists.";
            return result;
        }
        classEntity.setId(0);
        classEntity.setClassName(toName);
        classEntity.setCreateTime(QDateTime());
        classEntity.setEditTime(classEntity.getCreateTime());
        errCode = create(classEntity);
        if (errCode) {
            result.errCode = errCode;
            result.errMsg = "Error to create Class.";
            return result;
        }
        QHash<quint64, quint64> oldIdwIdToNewHash;
        QHash<quint64, quint64> oldIdpIdToNewHash;
        {
            IDW_WaferService *waferService = ServiceManager::defaultService<IDW_WaferService>();
            IDW_WaferSearchDTO waferParam;
            waferParam.setClassId(id);
            QList<IDW_WaferInfoEntity> wafers = waferService->searchList(waferParam, &errCode);
            if (errCode) {
                result.errCode = errCode;
                result.errMsg = "Load IDW list error.";
                return result;
            }
            quint64 oldId, topId, newId;
            for (IDW_WaferInfoEntity& entity : wafers) {
                entity = waferService->getWaferInfoById(entity.getId(), &errCode);
                if (errCode) {
                    result.errCode = errCode;
                    result.errMsg = "Error to load IDW detail.";
                    return result;
                }
                oldId = entity.getId();
                topId = entity.getTopId();
                entity.setId(0);
                entity.setCreateTime(QDateTime());
                entity.setEditTime(entity.getCreateTime());
                entity.setClassId(classEntity.getId());
                errCode = waferService->save(entity);
                if (errCode) {
                    result.errCode = errCode;
                    result.errMsg = "Error occern during copy IDW.";
                    return result;
                }
                oldIdwIdToNewHash.insert(oldId, entity.getId());
                entity.setId(oldId);
                entity.setTopId(topId);
            }
            for (IDW_WaferInfoEntity& entity : wafers) {
                if (entity.getTopId() != 0) {
                    topId = oldIdwIdToNewHash.value(entity.getTopId(), 0);
                    if (topId == 0) {
                        result.errCode = UNKNOW_ERR_CODE;
                        result.errMsg = "Reference to IDW old version data error, status: 0";
                        return result;
                    }
                } else {
                    topId = 0;
                }

                newId = oldIdwIdToNewHash.value(entity.getId(), 0);
                if (newId == 0) {
                    continue;
                }
                waferService->updateTopId(newId, topId, &errCode);
                if (errCode) {
                    result.errCode = errCode;
                    result.errMsg = "Reference to IDW old version data error, status: 1";
                    return result;
                }
            }
        }

        {
            IdpDataService* service = ServiceManager::defaultService<IdpDataService>();
            IdpSearchDTO param;
            param.setClassId(id);
            QList<IdpDataEntity> idps = service->searchList(param, &errCode);
            if (errCode) {
                result.errCode = errCode;
                result.errMsg = "Load IDP list error.";
                return result;
            }
            quint64 oldId, topId, newId;
            for (IdpDataEntity& entity : idps) {
                entity = service->get(entity.getId(), &errCode);
                if (errCode) {
                    result.errCode = errCode;
                    result.errMsg = "Error to load IDP detail.";
                    return result;
                }
                oldId = entity.getId();
                topId = entity.getTopId();
                entity.setId(0);
                if (entity.getIdwId() > 0)
                    entity.setIdwId(oldIdwIdToNewHash.value(entity.getIdwId(), 0));
                entity.setCreateTime(QDateTime());
                entity.setEditTime(entity.getCreateTime());
                entity.setClassId(classEntity.getId());
                errCode = service->save(entity);
                if (errCode) {
                    result.errCode = errCode;
                    result.errMsg = "Error occern during copy IDP.";
                    return result;
                }
                oldIdpIdToNewHash.insert(oldId, entity.getId());
                entity.setId(oldId);
                entity.setTopId(topId);
            }

            for (IdpDataEntity& entity : idps) {
                if (entity.getTopId() != 0) {
                    topId = oldIdpIdToNewHash.value(entity.getTopId(), 0);
                    if (topId == 0) {
                        result.errCode = UNKNOW_ERR_CODE;
                        result.errMsg = "Reference to IDP old version data error, status: 0";
                        return result;
                    }
                } else {
                    topId = 0;
                }
                newId = oldIdpIdToNewHash.value(entity.getId(), 0);
                if (newId == 0) {
                    continue;
                }
                service->updateTopId(newId, topId, &errCode);
                if (errCode) {
                    result.errCode = errCode;
                    result.errMsg = "Reference to IDP old version data error, status: 1";
                    return result;
                }
            }
        }

        {
            RecipeDataService* service = ServiceManager::defaultService<RecipeDataService>();
            RecipeSearchDTO param;
            param.setClassId(id);
            QList<RecipeDataEntity> recipes = service->searchList(param, &errCode);
            if (errCode) {
                result.errCode = errCode;
                result.errMsg = "Load Recipe list error.";
                return result;
            }
            QHash<quint64, quint64> oldRecipeIdToNewHash;
            quint64 oldId, topId, newId;
            for (RecipeDataEntity& entity : recipes) {
                entity = service->get(entity.getId(), &errCode);
                if (errCode) {
                    result.errCode = errCode;
                    result.errMsg = "Error to load Recipe detail.";
                    return result;
                }
                oldId = entity.getId();
                topId = entity.getTopId();
                entity.setId(0);
                if (entity.getIdwId() > 0)
                    entity.setIdwId(oldIdwIdToNewHash.value(entity.getIdwId(), 0));
                if (entity.getIdpId() > 0)
                    entity.setIdpId(oldIdpIdToNewHash.value(entity.getIdpId(), 0));
                entity.setCreateTime(QDateTime());
                entity.setEditTime(entity.getCreateTime());
                entity.setClassId(classEntity.getId());
                errCode = service->save(entity);
                if (errCode) {
                    result.errCode = errCode;
                    result.errMsg = "Error occern during copy Recipe.";
                    return result;
                }
                oldRecipeIdToNewHash.insert(oldId, entity.getId());
                entity.setId(oldId);
                entity.setTopId(topId);
            }

            for (RecipeDataEntity& entity : recipes) {
                if (entity.getTopId() != 0) {
                    topId = oldRecipeIdToNewHash.value(entity.getTopId(), 0);
                    if (topId == 0) {
                        result.errCode = UNKNOW_ERR_CODE;
                        result.errMsg = "Reference to IDP old version data error, status: 0";
                        return result;
                    }
                } else {
                    topId = 0;
                }
                newId = oldRecipeIdToNewHash.value(entity.getId(), 0);
                if (newId == 0) {
                    continue;
                }
                service->updateTopId(newId, topId, &errCode);
                if (errCode) {
                    result.errCode = errCode;
                    result.errMsg = "Reference to IDP old version data error, status: 1";
                    return result;
                }
            }
        }
        succ = true;
    }
    if (!succ) {
        result.errCode = result.errCode = SQL_COMMIT_ERROR_CODE;
        result.errMsg = "Error to commit transaction.";
    } else {
        result.errCode = 0;
    }
    return result;
}
