#include "data_manager_signal_bridge.h"
#ifdef __DATA_MANAGER_TARGET
#include "plugin_signal_hub.h"
#include "signal_hub_plugin_ids.h"
#include "signal_hub_single_object_manager.h"
#include "file_manager_controller.h"
#include "template_manager_controller.h"
#include "msr_manager_controller.h"
#include "image_manager_controller.h"
#include "sem_global.h"
#include "relation_image_view_controller.h"
#include "relation_worksheet_controller.h"
#include "cde90_message_box.h"
#include "exception_code.h"
#include "sem_common_data_model.h"

#include "relation_idw_wafer_editor.h"
#include "idw_wafer_relation.h"
#include "relation_idp_data_edit_controller.h"
#include "idp_edit_relation.h"
#include "image_manager_relation.h"
#include "worksheet_relation.h"
#include "template_relation.h"
#include "plugin_version_util.h"

FileManagerController* openSingleFileManager() {
    FileManagerController* fileManagerController = SignalHubSingleObjectManager::instance()->getObject<FileManagerController*>(OPEN_SINGLE_FILE_MANAGER);
    if (fileManagerController) {
        if (!fileManagerController->isActiveWindow()) fileManagerController->activateWindow();
        return fileManagerController;
    }
    fileManagerController = new FileManagerController;
    fileManagerController->setWindowTitle("File Manager(Class, IDW, IDP, Recipe)");
    fileManagerController->setAttribute(Qt::WA_DeleteOnClose);
    fileManagerController->resize(700, 500);
    fileManagerController->setCloseCall([](CDE90WindowCloseStatus){
        SignalHubSingleObjectManager::instance()->removeObject(OPEN_SINGLE_FILE_MANAGER);
        return true;
    });
    if (idw_wafer::singleIDWWindowOpened() || !idw_wafer::idwWaferAvailable()) {
        fileManagerController->enableIDWAction(false);
    }
    if (idp_editor::singleIDPWindowOpened() || !idp_editor::idpEditorAlivable()) {
        fileManagerController->enableIDPAction(false);
    }
    // if (recipeDataEditor || recipeEraserController) fileManagerController->enableRecipeAction(false);

    QObject::connect(fileManagerController,
        &FileManagerController::openIDWEditor,
        fileManagerController,
        [fileManagerController](quint64 classId,
               const QString& className,
               quint64 id,
               const QString& text,
               bool allowCoverSave) {
            MsgReceiveStatus<RelationIDWWaferEditor*> status = idw_wafer::openSingleIdwWaferEditor([classId](quint64 idwId) {
                FileManagerController* fileManagerController = SignalHubSingleObjectManager::instance()->getObject<FileManagerController*>(OPEN_SINGLE_FILE_MANAGER);
                if (!fileManagerController) return;
                fileManagerController->enableIDWAction();
                fileManagerController->refreshSelectedIDWData(classId, idwId);
            });
            fileManagerController->enableIDWAction(!status.ok);
            if (!status.ok) {
                simpleMessageBox(fileManagerController, "Error", "Error to open IDW window");
                return;
            }
            RelationIDWWaferEditor* idwWaferEditor = status.result;
            idwWaferEditor->disableCoverSave(!allowCoverSave);
            idwWaferEditor->setBaseData({text, className, id, classId});
            if (id > 0) {
                idwWaferEditor->loadData(id);
            }
        });

    QObject::connect(fileManagerController,
        &FileManagerController::openIdpEditor,
        fileManagerController,
        [fileManagerController](quint64 classId, quint64 id, const QString& idpName, quint64 idwId, bool allowCoverSave) {
            MsgReceiveStatus<RelationIdpEditController*> status = idp_editor::openSingleIdpEditor([classId](quint64 idwTopId, quint64 idpId){
               FileManagerController* fileManagerController = SignalHubSingleObjectManager::instance()->getObject<FileManagerController*>(OPEN_SINGLE_FILE_MANAGER);
               if (!fileManagerController) return;
               fileManagerController->enableIDPAction();
               fileManagerController->refreshSelectedIDPData(classId, idwTopId, idpId);
            });
            fileManagerController->enableIDPAction(!status.ok);
            if (!status.ok) {
                simpleMessageBox(fileManagerController, "Error", "Error to open IDP window");
                return;
            }
            RelationIdpEditController* idpDataEditor = status.result;
            idpDataEditor->disableCoverSave(!allowCoverSave);
            if (id > 0) {
                IDP_BaseData base;
                base.classId = classId;
                base.idpId = id;
                idpDataEditor->loadExistsData(base);
            } else {
                idpDataEditor->newData(classId, idwId, idpName);
            }
        });
    // QObject::connect(fileManagerController,
    //                  &FileManagerController::openRecipeEditor,
    //                  fileManagerController,
    //                  [this](quint64 classId, quint64 id, const QString& recipeName, bool allowCoverSave) {
    //                      fileManagerController->enableRecipeAction(false);
    //                      recipeDataEditor = new RecipeEditorController;
    //                      recipeDataEditor->setWindowTitle("Recipe Editor");
    //                      recipeDataEditor->setAttribute(Qt::WA_DeleteOnClose);
    //                      recipeDataEditor->setResizable(false);
    //                      recipeDataEditor->initAsyncCaller();
    //                      recipeDataEditor->disableCoverSave(!allowCoverSave);
    //                      QPointer<MainControllerHelper> _this = this;
    //                      recipeDataEditor->setCloseCall(
    //                          [classId, _this](CDE90WindowCloseStatus) -> bool {
    //                              if (!_this) return true;
    //                              if (_this->fileManagerController) {
    //                                  _this->fileManagerController->enableRecipeAction();
    //                                  _this->fileManagerController->refreshSelectedRecipeData(classId, _this->recipeDataEditor->getRecipeId());
    //                              }
    //                              _this->recipeDataEditor = nullptr;
    //                              return true;
    //                          });
    //                      recipeDataEditor->show();
    //                      MainThreadInvoker::getInvoker().moveToScreenTopCenter(recipeDataEditor);
    //                      if (id > 0) {
    //                          Recipe_BaseData base;
    //                          base.classId = classId;
    //                          base.recipeId = id;
    //                          recipeDataEditor->loadExistsData(base);
    //                      } else {
    //                          recipeDataEditor->newData(classId, recipeName);
    //                      }
    //                  });
    // QObject::connect(fileManagerController,
    //                  &FileManagerController::openRecipeEraser,
    //                  fileManagerController,
    //                  [this](quint64 classId) {
    //                      fileManagerController->enableRecipeAction(false);
    //                      recipeEraserController = new RecipeEraserController;
    //                      recipeEraserController->setWindowTitle("Recipe File Eraser");
    //                      recipeEraserController->setAttribute(Qt::WA_DeleteOnClose);
    //                      recipeEraserController->initAsyncCaller();
    //                      QPointer<MainControllerHelper> _this = this;
    //                      recipeEraserController->setCloseCall(
    //                          [classId, _this](CDE90WindowCloseStatus) {
    //                              if (!_this) return true;
    //                              if (_this->fileManagerController) {
    //                                  _this->fileManagerController->enableRecipeAction();
    //                                  _this->fileManagerController->refreshIDWData(classId);
    //                                  _this->fileManagerController->refreshIDPData(classId);
    //                                  _this->fileManagerController->refreshRecipeData(classId);
    //                              }
    //                              _this->recipeEraserController = nullptr;
    //                              return true;
    //                          });
    //                      recipeEraserController->resize(735, 530);
    //                      recipeEraserController->show();
    //                      recipeEraserController->refreshDataList(classId);
    //                  });
    fileManagerController->show();
    MainThreadInvoker::getInvoker().moveToScreenCenter(fileManagerController);
    SignalHubSingleObjectManager::instance()->setObject(OPEN_SINGLE_FILE_MANAGER, fileManagerController);
    return fileManagerController;
}

TemplateManagerController* openSingleTemplateManager() {
    TemplateManagerController* templateManagerController = SignalHubSingleObjectManager::instance()->getObject<TemplateManagerController*>(OPEN_SINGLE_TEMPLATE_MANAGER);
    if (templateManagerController) {
        if (!templateManagerController->isActiveWindow()) templateManagerController->activateWindow();
        return templateManagerController;
    }
    templateManagerController = new TemplateManagerController;
    templateManagerController->setWindowTitle("Template Manager");
    templateManagerController->setAttribute(Qt::WA_DeleteOnClose);
    templateManagerController->resize(700, 500);
    templateManagerController->setCloseCall([](CDE90WindowCloseStatus){
        SignalHubSingleObjectManager::instance()->removeObject(OPEN_SINGLE_TEMPLATE_MANAGER);
        return true;
    });
    if (idp_template::singleApWindowOpened() || !idp_template::apTemplateWindowAvailable()) {
        templateManagerController->enableApAction(false);
    }
    if (idp_template::singleMultiMsWindowOpened() || !idp_template::multiMsTemplateWindowAvailable()) {
        templateManagerController->enableMsAction(false);
    }
    if (idp_template::singleMpWindowOpened() || !idp_template::mpTemplateWindowAvailable()) {
        templateManagerController->enableMpAction(false);
    }
    if (idp_template::singleMpIpWindowOpened() || !idp_template::mpipTemplateWindowAvailable()) {
        templateManagerController->enableMpIpAction(false);
    }
    QObject::connect(templateManagerController,
        &TemplateManagerController::openAlignmentEditor,
        templateManagerController,
        [templateManagerController](quint64 id, const QString& title) {
            MsgReceiveStatus<RelationAPTemplateEditController*> status = idp_template::openSingleApTemplateWindow([](quint64 id){
                TemplateManagerController* templateManagerController = SignalHubSingleObjectManager::instance()->getObject<TemplateManagerController*>(OPEN_SINGLE_TEMPLATE_MANAGER);
                if (!templateManagerController) return;
                templateManagerController->refreshAlignmentList(id);
                templateManagerController->enableApAction(true);
            },
            id, title);
            templateManagerController->enableApAction(!status.ok);
            if (!status.ok) {
                simpleMessageBox(templateManagerController, "Error", "Error to open AP Template window");
            }
        });
    QObject::connect(templateManagerController,
        &TemplateManagerController::openAddressingEditor,
        templateManagerController,
        [templateManagerController](quint64 id, const QString& title) {
            MsgReceiveStatus<RelationMSMultiTemplateEditController*> status = idp_template::openSingleMultiMsTemplateWindow([](quint64 id){
                TemplateManagerController* templateManagerController = SignalHubSingleObjectManager::instance()->getObject<TemplateManagerController*>(OPEN_SINGLE_TEMPLATE_MANAGER);
                if (!templateManagerController) return;
                templateManagerController->refreshAddressingList(id);
                templateManagerController->enableMsAction(true);
            }, id, title);
            templateManagerController->enableMsAction(!status.ok);
            if (!status.ok) {
                simpleMessageBox(templateManagerController, "Error", "Error to open MS Template window");
            }
        });
    QObject::connect(templateManagerController,
        &TemplateManagerController::openMpEditor,
        templateManagerController,
        [templateManagerController](quint64 id, const QString& title) {
            MsgReceiveStatus<RelationMPTemplateEditController*> status = idp_template::openSingleMpTemplateWindow([](quint64 id){
                TemplateManagerController* templateManagerController = SignalHubSingleObjectManager::instance()->getObject<TemplateManagerController*>(OPEN_SINGLE_TEMPLATE_MANAGER);
                if (!templateManagerController) return;
                templateManagerController->refreshMPList(id);
                templateManagerController->enableMpAction(true);
            }, id, title);
            templateManagerController->enableMpAction(!status.ok);
            if (!status.ok) {
                simpleMessageBox(templateManagerController, "Error", "Error to open MP Template window");
            }
        });
    QObject::connect(templateManagerController,
        &TemplateManagerController::openMpIpEditor,
        templateManagerController,
        [templateManagerController](quint64 id, const QString& title) {
            MsgReceiveStatus<RelationMpIpTemplateEditController*> status = idp_template::openSingleMpIpTemplateWindow([](quint64 id){
                TemplateManagerController* templateManagerController = SignalHubSingleObjectManager::instance()->getObject<TemplateManagerController*>(OPEN_SINGLE_TEMPLATE_MANAGER);
                if (!templateManagerController) return;
                templateManagerController->refreshMPIPList(id);
                templateManagerController->enableMpIpAction(true);
            }, id, title);
            templateManagerController->enableMpIpAction(!status.ok);
            if (!status.ok) {
                simpleMessageBox(templateManagerController, "Error", "Error to open MP/IP Template window");
            }
        });
    templateManagerController->show();
    MainThreadInvoker::getInvoker().moveToScreenCenter(templateManagerController);
    SignalHubSingleObjectManager::instance()->setObject(OPEN_SINGLE_TEMPLATE_MANAGER, templateManagerController);
    return templateManagerController;
}

ImageManagerController* openSingleImageManager() {
    ImageManagerController* imageController = SignalHubSingleObjectManager::instance()->getObject<ImageManagerController*>(OPEN_SINGLE_IMAGE_MANAGER);
    if (imageController) {
        if (!imageController->isActiveWindow()) imageController->activateWindow();
        return imageController;
    }
    imageController = new ImageManagerController;
    imageController->setAttribute(Qt::WA_DeleteOnClose);
    imageController->setWindowTitle("Image Manager");
    imageController->setWindowModality(Qt::NonModal);
    imageController->initAsyncCaller();
    imageController->setCloseCall([](CDE90WindowCloseStatus){
        SignalHubSingleObjectManager::instance()->removeObject(OPEN_SINGLE_IMAGE_MANAGER);
        return true;
    });

    QObject::connect(imageController,
        &ImageManagerController::openImageView,
        imageController,
        [imageController](const QList<cdsem::ImageViewListData>& imageList, int index) {
            MsgReceiveStatus<RelationImageViewController*> w = image_manager::openImageViewController(imageList, index, 0, nullptr);
            if (!w.ok) {
                simpleMessageBox(imageController, "Error", "Error to open Image Display window");
            }
        });

    imageController->show();
    MainThreadInvoker::getInvoker().moveToScreenCenter(imageController);
    SignalHubSingleObjectManager::instance()->setObject(OPEN_SINGLE_IMAGE_MANAGER, imageController);
    return imageController;
}

MsrManagerController* openSingleMSRManager() {
    MsrManagerController* msrController = SignalHubSingleObjectManager::instance()->getObject<MsrManagerController*>(OPEN_SINGLE_MSR_MANAGER);
    if (msrController) {
        if (!msrController->isActiveWindow()) msrController->activateWindow();
        return msrController;
    }
    msrController = new MsrManagerController;
    msrController->setAttribute(Qt::WA_DeleteOnClose);
    msrController->setWindowTitle("MSR Manager");
    msrController->setWindowModality(Qt::NonModal);
    msrController->initAsyncCaller();
    msrController->setCloseCall([](CDE90WindowCloseStatus){
        SignalHubSingleObjectManager::instance()->removeObject(OPEN_SINGLE_MSR_MANAGER);
        return true;
    });

    QObject::connect(msrController,
                     &MsrManagerController::openWorkSheet,
                     msrController,
                     [msrController](quint64 msrTopId) {
                         MsgReceiveStatus<RelationWorksheetController*> w = worksheet::openSingleWorksheetController(msrTopId);
                         if (!w.ok) {
                             simpleMessageBox(msrController, "Error", "Error to open WorkSheet Window.");
                             return;
                         }
                     });

    msrController->show();
    MainThreadInvoker::getInvoker().moveToScreenCenter(msrController);
    SignalHubSingleObjectManager::instance()->setObject(OPEN_SINGLE_MSR_MANAGER, msrController);
    return msrController;
}

SIGNAL_CHECK(__DATA_MANAGER_TARGET) {
    Q_UNUSED(sysVersion)
#ifdef _DATA_MANAGER_VERSION_
    bool succ = readVersion(_DATA_MANAGER_VERSION_, *libVersion);
    if (succ) {
        result->errCode = 0;
        result->errMsg.clear();
        //TODO 未来可添加版本号处理机制
    } else {
        result->errCode = LIBRARY_VERSION_READ_ERROR_CODE;
        result->errMsg = "Data Manager version is not defined.";
    }
#else
#warning "Data Manager component version is not defined."
    result->errCode = 0;
    result->errMsg.clear();
#endif
}

SIGNAL_REG(__DATA_MANAGER_TARGET) {
    Q_UNUSED(sysVersion)
    PluginSignalHub::instance()->registSlot(OPEN_SINGLE_FILE_MANAGER, PluginHubAnyFunc<RelationFileManager*>([](){
        return openSingleFileManager();
    }));
    PluginSignalHub::instance()->registSlot(OPEN_SINGLE_TEMPLATE_MANAGER, PluginHubAnyFunc<RelationTemplateManager*>([](){
        return openSingleTemplateManager();
    }));
    PluginSignalHub::instance()->registSlot(OPEN_SINGLE_IMAGE_MANAGER, PluginHubAnyFunc<RelationImageManager*>([](){
        return openSingleImageManager();
    }));
    PluginSignalHub::instance()->registSlot(OPEN_SINGLE_MSR_MANAGER, PluginHubAnyFunc<RelationMSRManager*>([](){
        return openSingleMSRManager();
    }));
    result->errCode = 0;
    result->errMsg.clear();
}

SIGNAL_UNREG(__DATA_MANAGER_TARGET) {
    QList<QString> list;
    list.reserve(4);
    list << OPEN_SINGLE_FILE_MANAGER << OPEN_SINGLE_TEMPLATE_MANAGER << OPEN_SINGLE_IMAGE_MANAGER << OPEN_SINGLE_MSR_MANAGER;
    QString pid = QString::fromLatin1(pluginId);
    PluginSignalHub::instance()->unRegistAll(list, [pid, callback](){
        callback(pid.toLatin1());
    });
}

DATA_MANAGER_SIGNAL_BRIDGE_H
#endif //
