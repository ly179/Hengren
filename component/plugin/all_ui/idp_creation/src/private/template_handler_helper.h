#ifndef TEMPLATE_HANDLER_HELPER_H
#define TEMPLATE_HANDLER_HELPER_H

#include <QByteArray>
#include "cde90_message_box.h"
#include "cde90_process_box.h"
#include "idp_data_model.h"
#include "image_info_service.h"
#include "sem_global.h"
#include "service_manager.h"
#include "template_info_model.h"
#include "template_info_model_convert.h"
#include "template_info_service.h"
#include "template_save_box_controller.h"
#include "transaction_manager_service.h"
#include "image_relation_record_entity.h"
#include "image_relation_record_service.h"
#include "image_info_service.h"

//除MPIP以外的(AP、MS、MP)IDP AP Editor/Sequence Editor页面的Template下拉菜单的Save按钮动作的辅助函数
//特点：动态模板链接save时，不会触发回调。静态链接、未链接模板时会触发回调
//目前的逻辑是Import、Static Load和未关联的模板在Save后都会强制转为Static Load。
template<class T>
class IdpSaveBoxSaveCaller {
private:
    //在UI线程上回调
    std::function<void(quint64, const TemplateInfoModel<T>&, const QList<quint64>&)> succCallback;
    ImageRelationDataType relationType;
    TemplateSaveBoxController* savingBox;
    //在任务异步线程上回调
    std::function<bool(TemplateInfoModel<T>&, QList<quint64>&)> saveImageCallback;

public:
    inline void setSuccCallback(
        const std::function<void(quint64, const TemplateInfoModel<T>&, const QList<quint64>&)>& succCallback) {
        this->succCallback = succCallback;
    }
    inline void setSaveImageCallback(
        const std::function<bool(TemplateInfoModel<T>&, QList<quint64>&)> saveImageCallback) {
        this->saveImageCallback = saveImageCallback;
    }
    inline void setSaveBox(
        TemplateSaveBoxController* savingBox) {
        this->savingBox = savingBox;
    }

    inline void setRelationType(ImageRelationDataType type) {
        this->relationType = type;
    }

    //保存模板，override = true时，refType = 1才生效。即无视输入名称，直接覆盖param.id的数据（生成新版本，作废旧版本）
    void idpSaveBoxCall(
        TemplateInfoModel<T>& param, int refType, bool overwrite = false) {
        QString typeName;
        if (param.type == TEMP_ALIGN) {
            typeName = "AP";
        } else if (param.type == TEMP_MS) {
            typeName = "MS";
        } else if (param.type == TEMP_MP) {
            typeName = "MP";
        } else {
            return;
        }
        TransactionObject dObj = TransactionManagerService::beginDefaultTransaction();
        TransactionObject fObj = TransactionManagerService::beginFileTransaction();
        bool succ = false;
        sem::AfterGc gc([&succ, &dObj, &fObj](){
            if (succ) return;
            dObj.rollback();
            fObj.rollback();
        });
        QList<quint64> imageIds;
        imageIds.reserve(3);
        if (saveImageCallback) {
            if (!saveImageCallback(param, imageIds)) {
                simpleMessageBox(savingBox,
                                 "Error",
                                 QString("Error to save %1 template image data.").arg(typeName));
                return;
            }
        }
        TemplateInfoService* service = ServiceManager::instance()
                                           ->getDefaultService<TemplateInfoService>();
        TemplateInfoEntity entity;
        int errCode;

        //如果不是动态的模板，无论对应的数据是否存在，都允许保存，因为保存后理论上应该是新的数据线，与旧数据没有直接关系了。
        //统一改为：如果是overwrite，则覆盖原文件（底层还是增量记录），否则为saveAs
        if (overwrite) {
            if (param.id == 0) {
                simpleMessageBox(savingBox,
                                     "Error",
                                     QString("%1 template is not exists.").arg(typeName));
                return;
            }
            TemplateInfoEntity tmpEntity = refType == 0
                                               ? service->getSimpleInfoByIdForNewVersion(param.id,
                                                                                         &errCode)
                                               : service->getSimpleInfoById(param.id, &errCode);
            if (errCode) {
                simpleMessageBox(savingBox,
                                 "Error",
                                 QString("Error to save %1 template.").arg(typeName));
                return;
            }
            if (tmpEntity.getId() == 0) {
                simpleMessageBox(savingBox,
                                     "Error",
                                     QString("%1 template is not exists.").arg(typeName));
            }
            param.comment = tmpEntity.getTemplateComment();
            param.name = tmpEntity.getTemplateName();
        } else {
            if (param.name.isEmpty()) {
                simpleMessageBox(savingBox, "Info", "Please input name.");
                return;
            }
            quint64 cid = service->checkName(param.name, param.type, &errCode);
            if (cid > 0 && (param.id == 0 || param.id != cid)) {
                simpleMessageBox(savingBox,
                                 "Error",
                                 QString("File \"%1\" is already exists.").arg(param.name));
                return;
            }
            if (cid == 0 && param.id > 0)
                param.id = 0;
        }
        templateInfoModelToEnity(entity, param);
        errCode = service->saveWithTransaction(entity);
        if (errCode) {
            simpleMessageBox(savingBox,
                             "Error",
                             QString("Error to save %1 template.").arg(typeName));
            return;
        }
        ImageRelationRecordService* refService = ServiceManager::defaultService<ImageRelationRecordService>();
        errCode = refService->updateRelation(entity.getId(), relationType, imageIds);
        if (errCode) {
            simpleMessageBox(savingBox,
                             "Error",
                             QString("Error to save %1 template when saving image relation.").arg(typeName));
            return;
        }
        succ = fObj.commit();
        if (!succ) {
            simpleMessageBox(savingBox,
                             "Error",
                             QString("Error to save %1 template when commiting transaction.").arg(typeName));
            return;
        }
        succ = dObj.commit();
        if (!succ) {
            ImageInfoService* imageService = ServiceManager::defaultService<ImageInfoService>();
            errCode = imageService->realDeleteByIds(imageIds);
            if (errCode) {
                qCritical() << QString("Error to rollback image when fail to save template %1.").arg(typeName);
            }
            errCode = refService->realDeleteImageRelation(entity.getTemplateType(), relationType, imageIds);
            if (errCode) {
                qCritical() << QString("Error to rollback image relation when fail to save template %1.").arg(typeName);
            }
            return;
        }
        succ = true;
        param.id = entity.getId();
        MainThreadInvoker::getInvoker().runLater([this, param, imageIds, typeName]() {
            if (succCallback) {
                succCallback(param.id, param, imageIds);
            }
            savingBox->close();
        });
    }
};

template<class T>
void idpPrepareSaveBoxCall(
    TemplateInfoModel<T>& preparedParam,
    const T& templateParam,
    quint64 templateId,
    const QString& name,
    char templateType) {
    preparedParam.data = templateParam;
    preparedParam.type = templateType;
    preparedParam.name = name;
    preparedParam.id = templateId;
}

template<class T>
void idpOpenSaveBox(
    QWidget* _this,
    AsyncCaller* caller,
    const TemplateLinkedModel<T>& param,
    TemplateSaveBoxController* saveBoxController) {
    if (param.templateId == 0) {
        saveBoxController->disableCheckBox(true);
        saveBoxController->resetComponent({});
        saveBoxController->show();
        return;
    }
    quint64 templateId = param.templateId;
    char refType = param.templateRefType;
    saveBoxController->disableCheckBox(param.templateId == 0);
    bool succ = caller->push(_this, [_this, templateId, refType, saveBoxController](TSC) mutable {
        TemplateInfoService* service = ServiceManager::instance()
                                           ->getDefaultService<TemplateInfoService>();

        QString name;
        if (templateId > 0) {
            int errCode;
            TemplateInfoEntity entity = refType == 0
                                            ? service->getSimpleInfoByIdForNewVersion(templateId,
                                                                                      &errCode)
                                            : service->getSimpleInfoById(templateId, &errCode);
            if (errCode) {
                simpleMessageBox(_this, "Error", "Error to load template data.");
                return;
            }
            if (entity.getId() > 0) {
                name = entity.getTemplateName();
            }
        }
        MainThreadInvoker::getInvoker().blockRun([&name, saveBoxController]() {
            saveBoxController->resetComponent({name, false});
            saveBoxController->show();
        });
    });
    if (succ) {
        CDE90ProcessBox* box = loadingMessageBox(_this, "Working", "Loading", true);
        if (!caller->pushFocus(_this, [box](TSC) { releaseLoadingBoxLater(box); })) {
            releaseLoadingBoxLater(box);
        }
    }
}

#endif // TEMPLATE_HANDLER_HELPER_H
