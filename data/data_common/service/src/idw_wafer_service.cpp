#include "idw_wafer_service.h"
#include "dao_manager.h"
#include "idw_wafer_dao.h"
#include "class_service.h"
#include "service_manager.h"
#include <QMutex>
#include <QMutexLocker>
#include "exception_code.h"

struct IDW_WaferServiceHelper {
    QMutex nameLock;
    IDW_WaferService* d;
    IDW_WaferDao *idwDao;

    int checkNameRepeat(IDW_WaferInfoEntity& entity) {
        int errCode;
        quint64 id = d->getWaferIdByName(entity.getWaferName(), entity.getClassId(), &errCode);
        if (errCode) return errCode;
        if (id > 0) {
            if (entity.getId() == 0 || (entity.getId() > 0 && id != entity.getId())) {
                return FILE_NAME_REPEAT_ERROR_CODE;
            }
        }
        return 0;
    }
};

IDW_WaferService::IDW_WaferService() {
    helper = new IDW_WaferServiceHelper;
    helper->d = this;
    helper->idwDao = DaoManager::instance()->getDefaultFactoryDao<IDW_WaferDao>();
}

IDW_WaferService::~IDW_WaferService() noexcept {
    delete helper;
}

qint32 IDW_WaferService::save(IDW_WaferInfoEntity & entity) {
    QMutexLocker locker(&helper->nameLock);
    bool hasTran = helper->idwDao->hasTransaction();
    bool succ = helper->idwDao->beginTransaction(true);
    if (!succ) {
        qCritical() << "Error to open transaction: IDW_WaferService::save";
        return SQL_OPEN_TRAN_ERROR_CODE;
    }
    int errCode = 0;
    sem::AfterGc gc([this, &errCode, &hasTran](){
        if (!hasTran && errCode && !helper->idwDao->rollback()) {
            qCritical() << "Error to rollback transaction: IDW_WaferService::save";
        }
    });
    errCode = helper->checkNameRepeat(entity);
    if (errCode) return errCode;
    errCode = helper->idwDao->save(entity);
    if (!errCode && !hasTran && !helper->idwDao->commit()) {
        qCritical() << "Error to commit transaction: IDW_WaferService::save";
        errCode = SQL_COMMIT_ERROR_CODE;
    }
    return errCode;
}

qint32 IDW_WaferService::updateTopId(quint64 id, quint64 topId, int *errCode) {
    return helper->idwDao->updateTopId(id, topId, errCode);
}

qint32 IDW_WaferService::saveWithTransaction(
    IDW_WaferInfoEntity &entity) {
    QMutexLocker locker(&helper->nameLock);
    bool hasTran = helper->idwDao->hasTransaction();
    bool succ = helper->idwDao->beginTransaction(true);
    if (!succ) {
        qCritical() << "Error to open transaction: IDW_WaferService::saveWithTransaction";
        return SQL_OPEN_TRAN_ERROR_CODE;
    }
    int errCode = 0;
    sem::AfterGc gc([this, &errCode, &hasTran](){
        if (!hasTran && errCode && !helper->idwDao->rollback()) {
            qCritical() << "Error to rollback transaction: IDW_WaferService::saveWithTransaction";
        }
    });
    errCode = helper->checkNameRepeat(entity);
    if (errCode) return errCode;
    errCode = helper->idwDao->saveWithTransaction(entity);
    if (!errCode && !hasTran && !helper->idwDao->commit()) {
        qCritical() << "Error to commit transaction: IDW_WaferService::saveWithTransaction";
        errCode = SQL_COMMIT_ERROR_CODE;
    }
    return errCode;
}

IDW_WaferInfoEntity IDW_WaferService::getWaferInfoById(
    quint64 id, qint32 *errCode) {
    return helper->idwDao->getWaferInfoById(id, errCode);
}

IDW_WaferInfoEntity IDW_WaferService::getWaferInfoByTopId(quint64 topId, qint32 *errCode) {
    return helper->idwDao->getWaferInfoByTopId(topId, errCode);
}

IDW_WaferInfoEntity IDW_WaferService::getSimpleInfoById(quint64 id, qint32 *errCode) {
    return helper->idwDao->getSimpleInfoById(id, errCode);
}

IDW_WaferInfoEntity IDW_WaferService::getSimpleInfoByTopId(quint64 id, qint32 *errCode) {
    return helper->idwDao->getSimpleInfoByTopId(id, errCode);
}

QString IDW_WaferService::getWaferName(
    quint64 id, qint32 *errCode) {
    return helper->idwDao->getWaferName(id, errCode);
}

quint64 IDW_WaferService::getWaferIdByName(
    const QString &name, quint64 classId, int *errCode) {
    return helper->idwDao->getSimpleWaferIdByName(name, classId, errCode).getId();
}

IDW_WaferInfoEntity IDW_WaferService::getSimpleWaferByName(const QString &name, quint64 classId, int *errCode) {
    return helper->idwDao->getSimpleWaferIdByName(name, classId, errCode);
}

quint64 IDW_WaferService::getClassIdByWaferId(quint64 id, qint32* errCode) {
    return helper->idwDao->getClassIdByWaferId(id, errCode);
}

QList<IDW_WaferInfoEntity> IDW_WaferService::searchList(
    const IDW_WaferSearchDTO &param, qint32 *errCode) {
    return helper->idwDao->searchList(param, errCode);
}

qint32 IDW_WaferService::updateWaferNameOnly(
    quint64 id, const QString &title) {
    return helper->idwDao->updateWaferNameOnly(id, title);
}

// 如果当前版本是有效版本，则会同时删除历史版本？
qint32 IDW_WaferService::deleteWaferById(
    quint64 id) {
    return helper->idwDao->deleteWaferById(id);
}

qint32 IDW_WaferService::deleteWaferByClass(
    quint64 classId) {
    return helper->idwDao->deleteWaferByClass(classId);
}

IDW_CopyDataResult IDW_WaferService::copyNew(const IDW_CopyDataParam& param) {
    IDW_CopyDataResult result;
    if (param.className.isEmpty()) {
        result.errCode = NULL_POINTER_ERR_CODE;
        result.errMsg = "Please specify class name.";
        return result;
    }
    if (param.toName.isEmpty()) {
        result.errCode = NULL_POINTER_ERR_CODE;
        result.errMsg = "Please specify IDW name.";
        return result;
    }
    bool succ = helper->idwDao->beginTransaction(false);
    if (!succ) {
        result.errCode = SQL_OPEN_TRAN_ERROR_CODE;
        result.errMsg = "Error to open transaction.";
        return result;
    }
    {
        succ = false;
        sem::AfterGc gc([this, &succ](){
            if (succ) succ = helper->idwDao->commit();
            else helper->idwDao->rollback();
        });
        int errCode;
        ClassService* classService = ServiceManager::defaultService<ClassService>();
        quint64 classId = classService->getByName(param.className, &errCode);
        if (errCode) {
            result.errCode = errCode;
            result.errMsg = "Error to check class name.";
            return result;
        }
        if (classId == 0) {
            ClassEntity classEntity;
            classEntity.setClassName(param.className);
            errCode = classService->create(classEntity);
            if (errCode) {
                result.errCode = errCode;
                result.errMsg = "Error to create class.";
                return result;
            }
            classId = classEntity.getId();
        }
        quint64 existsIdwId = getWaferIdByName(param.toName, classId, &errCode);
        if (errCode) {
            result.errCode = errCode;
            result.errMsg = "Error to check IDW name.";
            return result;
        }
        if (existsIdwId == param.fromId) {
            result.errCode = FILE_NAME_OVERWRITE_SELF_ERROR_CODE;
            result.errMsg = "Cannot overwrite IDW file self.";
            return result;
        }
        if (existsIdwId > 0) {
            if (!param.overwrite) {
                result.errCode = FILE_NAME_REPEAT_ERROR_CODE;
                result.errMsg = "IDW file name already exists.";
                return result;
            }
            errCode = deleteWaferById(existsIdwId);
            if (errCode) {
                result.errCode = errCode;
                result.errMsg = "Error to overwrite IDW.";
                return result;
            }
        }
        IDW_WaferInfoEntity idwEntity = getWaferInfoById(param.fromId, &errCode);
        if (errCode) {
            result.errCode = errCode;
            result.errMsg = "Error to load IDW detail.";
            return result;
        }
        if (idwEntity.getId() == 0) {
            result.errCode = NULL_POINTER_ERR_CODE;
            result.errMsg = "IDW data is not exists.";
            return result;
        }
        idwEntity.setId(0);
        idwEntity.setCreateTime(QDateTime());
        idwEntity.setEditTime(idwEntity.getCreateTime());
        idwEntity.setClassId(classId);
        idwEntity.setWaferName(param.toName);
        errCode = saveWithTransaction(idwEntity);
        if (errCode) {
            result.errCode = errCode;
            result.errMsg = "Error to copy IDW.";
            return result;
        }
        result.newClassId = idwEntity.getClassId();
        result.newId = idwEntity.getId();
        succ = true;
    }
    if (!succ) {
        result.errCode = SQL_COMMIT_ERROR_CODE;
        result.errMsg = "Error to commit transaction.";
    }
    return result;
}
