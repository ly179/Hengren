#include "msr_service.h"
#include "msr_dao.h"
#include "dao_manager.h"
#include "msr_data_convert.h"
#include "msr_data_model.h"
#include "exception_code.h"
#include "service_manager.h"
#include "file_seq_service.h"
#include <QMutex>
#include <QMutexLocker>

struct MsrServiceHelper {
    QMutex namelock;
    MsrService* d;
    MsrDao* msrDao;

    int checkNameRepeat(MsrEntity& entity) {
        int errCode;
        quint64 id = d->getIdByName(entity.getClassId(), entity.getMsrName(), &errCode);
        if (errCode) return errCode;
        if (id > 0) {
            if (entity.getId() == 0 || (entity.getId() > 0 && id != entity.getId())) {
                return FILE_NAME_REPEAT_ERROR_CODE;
            }
        }
        return 0;
    }
};

OperationResult& convertFullEntityToModel(OperationResult& result, const MsrEntity& entity, MSR_Chip& data) {
    if (result.errCode) {
        result.errMsg = "Error to load MSR data.";
        return result;
    }
    if (entity.getId() == 0) {
        data = MSR_Chip();
        return result;
    }
    result.errCode = msrModeDataFromEntity(entity.getMsrDetail(), data);
    if (result.errCode) {
        result.errMsg = "Error to parse MSR data.";
        return result;
    }
    data.msrId = entity.getId();
    data.idwId = entity.getIdwId();
    data.idpId = entity.getIdpId();
    data.recipeId = entity.getRecipeId();
    data.classId = entity.getClassId();
    data.fileName = entity.getMsrName();
    return result;
}

MsrService::MsrService() {
    helper = new MsrServiceHelper;
    helper->d = this;
    helper->msrDao = DaoManager::instance()->getFileFactoryDao<MsrDao>();
}

MsrService::~MsrService() noexcept {
    delete helper;
}

int MsrService::save(MsrEntity& entity) {
    QMutexLocker locker(&helper->namelock);

    bool hasTran = helper->msrDao->hasTransaction();
    bool succ = helper->msrDao->beginTransaction(true);
    if (!succ) {
        qCritical() << "Error to open transaction: MsrService::save entity";
        return SQL_OPEN_TRAN_ERROR_CODE;
    }
    int errCode = 0;
    sem::AfterGc gc([this, &errCode, &hasTran](){
        if (!hasTran && errCode && !helper->msrDao->rollback()) {
            qCritical() << "Error to rollback transaction: MsrService::save entity";
        }
    });
    errCode = helper->checkNameRepeat(entity);
    if (errCode) return errCode;
    errCode = helper->msrDao->save(entity);
    if (!errCode && !hasTran && !helper->msrDao->commit()) {
        qCritical() << "Error to commit transaction: MsrService::save entity";
        errCode = SQL_COMMIT_ERROR_CODE;
    }
    return errCode;
}

QList<MsrEntity> MsrService::searchSimple(const MsrSearchDTO& param, int* errCode) {
    return helper->msrDao->searchSimple(param, errCode);
}

MsrEntity MsrService::getById(quint64 id, int* errCode) {
    return helper->msrDao->getById(id, errCode);
}

OperationResult MsrService::save(MSR_Chip& data, bool autoGenName) {
    QMutexLocker locker(&helper->namelock);
    OperationResult result;
    bool hasTran = helper->msrDao->hasTransaction();
    bool succ = helper->msrDao->beginTransaction(true);
    if (!succ) {
        qCritical() << "Error to open transaction: MsrService::save";
        result.errCode = SQL_OPEN_TRAN_ERROR_CODE;
        result.errMsg = "Error to open transaction for MSR saving";
        return result;
    }
    sem::AfterGc gc([this, &result, &hasTran](){
        if (!hasTran && result.errCode && !helper->msrDao->rollback()) {
            qCritical() << "Error to rollback transaction: MsrService::save";
        }
    });

    MsrEntity entity;
    entity.setClassId(data.classId);
    entity.setId(data.msrId);
    if (autoGenName) {
        FileSeqService* seqService = ServiceManager::defaultService<FileSeqService>();
        if (data.classId <= 0) {
            result.errCode = CLASS_NOT_SPECIFIED_ERROR_CODE;
            result.errMsg = "Class is required.";
            return result;
        }
        do {
            entity.setMsrName(seqService->getNextFileKey(cdsem::FT_MSR, &result.errCode));
            if (result.errCode) {
                result.errMsg = "Auto generate name failed.";
                return result;
            }
            result.errCode = helper->checkNameRepeat(entity);
        } while (result.errCode == FILE_NAME_REPEAT_ERROR_CODE);
        result.errCode = helper->checkNameRepeat(entity);
        if (result.errCode) {
            result.errMsg = "Error to check file name.";
            return result;
        }
    } else {
        if (data.fileName.isEmpty()) {
            result.errCode = FILE_NAME_IS_BLANK_ERR_CODE;
            result.errMsg = "File name is not specified.";
            return result;
        }
        entity.setMsrName(data.fileName);
        result.errCode = helper->checkNameRepeat(entity);
        if (result.errCode) {
            if (result.errCode == FILE_NAME_REPEAT_ERROR_CODE) {
                result.errMsg = "File name is already exists.";
            } else {
                result.errMsg = "Error to check file name.";
            }
            return result;
        }
    }

    msrModeDataToEntity(entity.getMsrDetail(), data);
    entity.setIdwId(data.idwId);
    entity.setIdpId(data.idpId);
    entity.setRecipeId(data.recipeId);

    result.errCode = helper->msrDao->saveWithTransaction(entity);
    if (result.errCode) {
        result.errMsg = "Error to save MSR data.";
    } else {
        if (hasTran) {
            data.msrId = entity.getId();
        } else {
            if (helper->msrDao->commit()) {
                data.msrId = entity.getId();
            } else {
                qCritical() << "Error to commit transaction: MsrService::save";
                result.errCode = SQL_COMMIT_ERROR_CODE;
                result.errMsg = "Error to commit transaction for MSR saving.";
            }
        }
    }
    return result;
}

OperationResult MsrService::getById(quint64 id, MSR_Chip& data) {
    OperationResult result;
    MsrEntity entity = helper->msrDao->getById(id, &result.errCode);
    return convertFullEntityToModel(result, entity, data);
}

OperationResult MsrService::getByTopId(quint64 topId, MSR_Chip& chip) {
    OperationResult result;
    MsrEntity entity = helper->msrDao->getByTopId(topId, &result.errCode);
    return convertFullEntityToModel(result, entity, chip);
}

quint64 MsrService::getIdByName(quint64 classId, const QString& name, int* errCode) {
    return helper->msrDao->getIdByName(classId, name, errCode);
}

int MsrService::updateNameOnly(quint64 id, const QString& name) {
    return helper->msrDao->updateNameOnly(id, name);
}

int MsrService::updateNameById(quint64 id, const QString& name) {
    return helper->msrDao->updateNameById(id, name);
}

//逻辑删除
int MsrService::deleteDataById(quint64 id) {
    return helper->msrDao->deleteDataById(id);
}
//物理删除
int MsrService::realDeleteDataById(quint64 id) {
    return helper->msrDao->realDeleteDataById(id);
}
