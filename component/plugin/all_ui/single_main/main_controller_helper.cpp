#include "main_controller_helper.h"
#include "ap_template_edit_controller.h"
#include "file_manager_controller.h"
#include "idp_data_editor_controller.h"
#include "idw_wafer_editor.h"
#include "image_operation_window_helper.h"
#include "job_controller.h"
#include "lock_loader_controller.h"
#include "mp_template_edit_controller.h"
#include "ms_template_edit_controller.h"
#include "recipe_editor_controller.h"
#include "recipe_eraser_controller.h"
#include "role_list_controller.h"
#include "sem_global.h"
#include "stage_controller.h"
#include "template_manager_controller.h"
#include "template_mpip_edit_controller.h"
#include "user_list_controller.h"
#include "manual_control_controller.h"
#include "worksheet_controller.h"
#include "signal_hub.h"
#include "signal_hub_ids.h"
#include "main_controller.h"
#include "msr_manager_controller.h"
#include "image_manager_controller.h"
#include "msr_data_model.h"
#include <QPointer>

class TmpQObject : public QObject {
    AsyncCaller* caller;
public:
    TmpQObject(AsyncCaller* caller) : caller(caller) {}
    ~TmpQObject() noexcept {delete caller;}
};

struct MSR_Chip;

MainControllerHelper::MainControllerHelper(QObject* parent) : QObject(parent) {
    SignalHub::instance()->registSlot<const MSR_Chip&>(SIGNAL_HUB_OPEN_WORK_SHEET, HubAnyFunc<const MSR_Chip&>([this](const MSR_Chip& c){
        if (MainThreadInvoker::getInvoker().isUiThread()) {
            openWorkSheetController();
            workSheetController->resetData(c);
            return;
        }
        MainThreadInvoker::getInvoker().runLater([this, c](){
            openWorkSheetController();
            workSheetController->resetData(c);
        });
    }));
    SignalHub::instance()->registSlot<const MSR_MP_Chip_MetaData&, const MSR_MP_Data&>(SIGNAL_HUB_APPEND_DATA_TO_WORK_SHEET, HubAnyFunc<const MSR_MP_Chip_MetaData&, const MSR_MP_Data&>([this](const MSR_MP_Chip_MetaData& metaData, const MSR_MP_Data& d){
        if (MainThreadInvoker::getInvoker().isUiThread()) {
            if (workSheetController) {
                workSheetController->appendData(metaData, d);
            }
            return;
        }
        MainThreadInvoker::getInvoker().runLater([this, metaData, d](){
            if (workSheetController) {
                workSheetController->appendData(metaData, d);
            }
        });

    }));
}

MainControllerHelper::~MainControllerHelper() noexcept {
    SignalHub::instance()->unRegistAll(SIGNAL_HUB_OPEN_WORK_SHEET);
    SignalHub::instance()->unRegistAll(SIGNAL_HUB_APPEND_DATA_TO_WORK_SHEET);
    if (manualController) manualController->release();
    if (fileManagerController) fileManagerController->release();
    if (templateManagerController) templateManagerController->release();
    if (loaderController) loaderController->release();
    if (stageController) stageController->release();
    if (workSheetController) workSheetController->release();
    if (jobController) jobController->release();
    if (userController) userController->release();
    if (roleController) roleController->release();
    if (msrController) msrController->release();
}

void MainControllerHelper::setMainWidget(MainController* parent) {
    this->parent = parent;
}

void MainControllerHelper::openManualController() {
    if (manualController) {
        if (!manualController->isActiveWindow()) manualController->activateWindow();
        return;
    }
    manualController = new  ManualControlController;
    manualController->setAttribute(Qt::WA_DeleteOnClose);
    manualController->setWindowTitle("Manual Controller");
    manualController->initAsyncCaller();
    manualController->setResizable(false);
    manualController->hideFullSizeBtn();
    manualController->setCloseCall([this](CDE90WindowCloseStatus){
        emit manualControllerVisible(false);
        manualController = nullptr;
        return true;
    });
    manualController->show();
    MainThreadInvoker::getInvoker().moveToScreenTopCenter(manualController);
    emit manualControllerVisible(true);
}

void MainControllerHelper::openFileController() {
    if (fileManagerController) {
        if (!fileManagerController->isActiveWindow()) fileManagerController->activateWindow();
        return;
    }
    fileManagerController = new FileManagerController;
    fileManagerController->setWindowTitle("File Manager(Class, IDW, IDP, Recipe)");
    fileManagerController->setAttribute(Qt::WA_DeleteOnClose);
    fileManagerController->resize(700, 500);
    fileManagerController->setCloseCall([this](CDE90WindowCloseStatus){
        emit fileManagerControllerVisible(false);
        fileManagerController = nullptr;
        return true;
    });
    if (idwWaferEditor) fileManagerController->enableIDWAction(false);
    if (idpDataEditor) fileManagerController->enableIDPAction(false);
    if (recipeDataEditor || recipeEraserController) fileManagerController->enableRecipeAction(false);

    QObject::connect(fileManagerController,
                     &FileManagerController::openIDWEditor,
                     fileManagerController,
                     [this](quint64 classId,
                            const QString& className,
                            quint64 id,
                            const QString& text,
                            bool allowCoverSave) {
                         fileManagerController->enableIDWAction(false);
                         idwWaferEditor = new IdwWaferEditor;
                         idwWaferEditor->disableCoverSave(!allowCoverSave);
                         idwWaferEditor->setBaseData({text, className, id, classId});
                         idwWaferEditor->setWindowTitle("IDW Editor");
                         idwWaferEditor->setAttribute(Qt::WA_DeleteOnClose);
                         QPointer<MainControllerHelper> _this = this;
                         idwWaferEditor->setExitCallback(
                             [classId, _this, this]() {
                                 if (!_this) return;
                                 if (_this->fileManagerController) {
                                     _this->fileManagerController->enableIDWAction();
                                     _this->fileManagerController->refreshSelectedIDWData(classId, _this->idwWaferEditor->getIdwId());
                                 }
                                 idwWaferEditor = nullptr;
                             });
                         // idwWaferEditor->setResizable(false);
                         idwWaferEditor->show();
                         idwWaferEditor->resize(QSize(1115, idwWaferEditor->height()));
                         MainThreadInvoker::getInvoker().moveToScreenTopCenter(idwWaferEditor);
                         if (id > 0) {
                             idwWaferEditor->loadData(id);
                         }
                     });

    QObject::connect(fileManagerController,
                     &FileManagerController::openIdpEditor,
                     fileManagerController,
                     [this](quint64 classId, quint64 id, const QString& idpName, quint64 idwId, bool allowCoverSave) {
                         fileManagerController->enableIDPAction(false);
                         idpDataEditor = new IdpDataEditorController;
                         idpDataEditor->setWindowTitle("IDP Editor");
                         idpDataEditor->setAttribute(Qt::WA_DeleteOnClose);
                         idpDataEditor->initAsyncCaller();
                         idpDataEditor->disableCoverSave(!allowCoverSave);
                         QPointer<MainControllerHelper> _this = this;
                         idpDataEditor->setCloseCall(
                             [classId, _this](CDE90WindowCloseStatus) -> bool {
                                 if (!_this) return true;
                                 if (_this->fileManagerController) {
                                     _this->fileManagerController->enableIDPAction();
                                     _this->fileManagerController->refreshSelectedIDPData(classId, _this->idpDataEditor->getIdwTopId(), _this->idpDataEditor->getIdpId());
                                 }
                                 _this->idpDataEditor = nullptr;
                                 return true;
                             });
                         idpDataEditor->show();
                         MainThreadInvoker::getInvoker().moveToScreenTopCenter(idpDataEditor);
                         if (id > 0) {
                             IDP_BaseData base;
                             base.classId = classId;
                             base.idpId = id;
                             idpDataEditor->loadExistsData(base);
                         } else {
                             idpDataEditor->newData(classId, idwId, idpName);
                         }
                     });
    QObject::connect(fileManagerController,
                     &FileManagerController::openRecipeEditor,
                     fileManagerController,
                     [this](quint64 classId, quint64 id, const QString& recipeName, bool allowCoverSave) {
                         fileManagerController->enableRecipeAction(false);
                         recipeDataEditor = new RecipeEditorController;
                         recipeDataEditor->setWindowTitle("Recipe Editor");
                         recipeDataEditor->setAttribute(Qt::WA_DeleteOnClose);
                         recipeDataEditor->setResizable(false);
                         recipeDataEditor->initAsyncCaller();
                         recipeDataEditor->disableCoverSave(!allowCoverSave);
                         QPointer<MainControllerHelper> _this = this;
                         recipeDataEditor->setCloseCall(
                             [classId, _this](CDE90WindowCloseStatus) -> bool {
                                 if (!_this) return true;
                                 if (_this->fileManagerController) {
                                     _this->fileManagerController->enableRecipeAction();
                                     _this->fileManagerController->refreshSelectedRecipeData(classId, _this->recipeDataEditor->getRecipeId());
                                 }
                                 _this->recipeDataEditor = nullptr;
                                 return true;
                             });
                         recipeDataEditor->show();
                         MainThreadInvoker::getInvoker().moveToScreenTopCenter(recipeDataEditor);
                         if (id > 0) {
                             Recipe_BaseData base;
                             base.classId = classId;
                             base.recipeId = id;
                             recipeDataEditor->loadExistsData(base);
                         } else {
                             recipeDataEditor->newData(classId, recipeName);
                         }
                     });
    QObject::connect(fileManagerController,
                     &FileManagerController::openRecipeEraser,
                     fileManagerController,
                     [this](quint64 classId) {
                         fileManagerController->enableRecipeAction(false);
                         recipeEraserController = new RecipeEraserController;
                         recipeEraserController->setWindowTitle("Recipe File Eraser");
                         recipeEraserController->setAttribute(Qt::WA_DeleteOnClose);
                         recipeEraserController->initAsyncCaller();
                         QPointer<MainControllerHelper> _this = this;
                         recipeEraserController->setCloseCall(
                             [classId, _this](CDE90WindowCloseStatus) {
                                 if (!_this) return true;
                                 if (_this->fileManagerController) {
                                     _this->fileManagerController->enableRecipeAction();
                                     _this->fileManagerController->refreshIDWData(classId);
                                     _this->fileManagerController->refreshIDPData(classId);
                                     _this->fileManagerController->refreshRecipeData(classId);
                                 }
                                 _this->recipeEraserController = nullptr;
                                 return true;
                             });
                         recipeEraserController->resize(735, 530);
                         recipeEraserController->show();
                         recipeEraserController->refreshDataList(classId);
                     });
    fileManagerController->show();
    MainThreadInvoker::getInvoker().moveToScreenCenter(fileManagerController);
    emit fileManagerControllerVisible(true);
}

void MainControllerHelper::openTemplateController() {
    if (templateManagerController) {
        if (!templateManagerController->isActiveWindow()) templateManagerController->activateWindow();
        return;
    }
    templateManagerController = new TemplateManagerController;
    templateManagerController->setWindowTitle("Template Manager");
    templateManagerController->setAttribute(Qt::WA_DeleteOnClose);
    templateManagerController->resize(700, 500);
    templateManagerController->setCloseCall([this](CDE90WindowCloseStatus){
        emit templateManagerControllerVisible(false);
        templateManagerController = nullptr;
        return true;
    });
    QObject::connect(templateManagerController,
                     &TemplateManagerController::openAlignmentEditor,
                     templateManagerController,
                     [this](quint64 id, const QString& title) {
                         apTemplateEditController = new APTemplateEditController(
                             templateManagerController);
                         apTemplateEditController->setAttribute(Qt::WA_DeleteOnClose);
                         apTemplateEditController->setWindowTitle("Template Editor (Alignment)");
                         apTemplateEditController->setWindowModality(Qt::WindowModal);
                         apTemplateEditController->initAsyncCaller();
                         apTemplateEditController->setCloseCall([this](CDE90WindowCloseStatus) {
                             templateManagerController->refreshAlignmentList(apTemplateEditController->getId());
                             return true;
                         });
                         apTemplateEditController->show();
                         MainThreadInvoker::getInvoker().moveToScreenCenter(apTemplateEditController);
                         if (id > 0) {
                             apTemplateEditController->loadData(id);
                         } else {
                             apTemplateEditController->initData(title);
                         }
                     });
    QObject::connect(templateManagerController,
                     &TemplateManagerController::openAddressingEditor,
                     templateManagerController,
                     [this](quint64 id, const QString& title) {
                         msTemplateEditController = new MSTemplateEditController(
                             templateManagerController);
                         msTemplateEditController->setAttribute(Qt::WA_DeleteOnClose);
                         msTemplateEditController->setWindowTitle("Template Editor (Addressing)");
                         msTemplateEditController->setWindowModality(Qt::WindowModal);
                         msTemplateEditController->initAsyncCaller();
                         msTemplateEditController->setCloseCall([this](CDE90WindowCloseStatus) {
                             templateManagerController->refreshAddressingList(msTemplateEditController->getId());
                             return true;
                         });
                         msTemplateEditController->show();
                         MainThreadInvoker::getInvoker().moveToScreenCenter(msTemplateEditController);
                         if (id > 0) {
                             msTemplateEditController->loadData(id);
                         } else {
                             msTemplateEditController->initData(title);
                         }
                     });
    QObject::connect(templateManagerController,
                     &TemplateManagerController::openMpEditor,
                     templateManagerController,
                     [this](quint64 id, const QString& title) {
                         mpTemplateEditController = new MPTemplateEditController(
                             templateManagerController);
                         mpTemplateEditController->setAttribute(Qt::WA_DeleteOnClose);
                         mpTemplateEditController->setWindowTitle("Template Editor (Measurement)");
                         mpTemplateEditController->setWindowModality(Qt::WindowModal);
                         mpTemplateEditController->initAsyncCaller();
                         mpTemplateEditController->setCloseCall([this](CDE90WindowCloseStatus) {
                             templateManagerController->refreshMPList(mpTemplateEditController->getId());
                             return true;
                         });
                         mpTemplateEditController->show();
                         MainThreadInvoker::getInvoker().moveToScreenCenter(mpTemplateEditController);
                         if (id > 0) {
                             mpTemplateEditController->loadData(id);
                         } else {
                             mpTemplateEditController->initData(title);
                         }
                     });
    QObject::connect(templateManagerController,
                     &TemplateManagerController::openMpIpEditor,
                     templateManagerController,
                     [this](quint64 id, const QString& title) {
                         mpipEditController = new TemplateMpipEditController(
                             templateManagerController);
                         mpipEditController->setAttribute(Qt::WA_DeleteOnClose);
                         mpipEditController->setWindowTitle("MP/IP Information Editor");
                         mpipEditController->setWindowModality(Qt::WindowModal);
                         mpipEditController->initAsyncCaller();
                         mpipEditController->setCloseCall([this](CDE90WindowCloseStatus) {
                             templateManagerController->refreshMPIPList(mpipEditController->getId());
                             return true;
                         });
                         mpipEditController->show();
                         MainThreadInvoker::getInvoker().moveToScreenCenter(mpipEditController);
                         if (id > 0) {
                             mpipEditController->loadData(id);
                         } else {
                             mpipEditController->initData(title);
                         }
                     });
    templateManagerController->show();
    MainThreadInvoker::getInvoker().moveToScreenCenter(templateManagerController);
    emit templateManagerControllerVisible(true);
}

void MainControllerHelper::openImageOperationController() {
    ImageOperationWindowHelper::instance()->show();
    // QRect rect = parent->geometry();
    // QScreen* screen = QApplication::primaryScreen();
    // QRect sRect = screen->availableGeometry();
    // ImageOperationWindowHelper::get()->move(sRect.width() - ImageOperationWindowHelper::get()->width(),
    //                                         sRect.bottom() - rect.height() - ImageOperationWindowHelper::get()->height());
}

void MainControllerHelper::openRecipeController() {
    if (jobController) {
        if (!jobController->isActiveWindow()) jobController->activateWindow();
        return;
    }
    jobController = new JobController;
    jobController->setAttribute(Qt::WA_DeleteOnClose);
    jobController->setResizable(false);
    jobController->setWindowTitle("Recipe Controller");
    jobController->setCloseCall([this](CDE90WindowCloseStatus){
        emit recipeControllerVisible(false);
        jobController = nullptr;
        return true;
    });
    jobController->initAsyncCaller();
    jobController->show();
    MainThreadInvoker::getInvoker().moveToScreenCenter(jobController);
    emit recipeControllerVisible(true);
}

void MainControllerHelper::openWorkSheetController() {
    if (workSheetController) {
        if (!workSheetController->isActiveWindow()) workSheetController->activateWindow();
        return;
    }
    workSheetController = new WorksheetController;
    workSheetController->setAttribute(Qt::WA_DeleteOnClose);
    // workSheetController->setResizable(false);
    workSheetController->setWindowTitle("Work Sheet");
    workSheetController->setCloseCall([this](CDE90WindowCloseStatus){
        emit worksheetControllerVisible(false);
        workSheetController = nullptr;
        return true;
    });
    workSheetController->initAsyncCaller();
    workSheetController->show();
    MainThreadInvoker::getInvoker().moveToScreenCenter(workSheetController);
    emit worksheetControllerVisible(true);
}

void MainControllerHelper::openStageController() {
    if (stageController) {
        if (!stageController->isActiveWindow()) stageController->activateWindow();
        return;
    }
    stageController = new StageController;
    stageController->setAttribute(Qt::WA_DeleteOnClose);
    stageController->setWindowTitle("Stage Controller");
    stageController->setResizable(false);
    stageController->hideFullSizeBtn();
    stageController->setCloseCall([this](CDE90WindowCloseStatus){
        emit stageControllerVisible(false);
        stageController = nullptr;
        return true;
    });
    stageController->initAsyncCaller();
    stageController->show();
    MainThreadInvoker::getInvoker().moveToScreenCenter(stageController);
    emit stageControllerVisible(true);
}

void MainControllerHelper::openLoaderController() {
    if (loaderController) {
        if (!loaderController->isActiveWindow()) loaderController->activateWindow();
        return;
    }
    loaderController = new LockLoaderController;
    loaderController->setAttribute(Qt::WA_DeleteOnClose);
    loaderController->setWindowTitle("Loader Controller");
    loaderController->setCloseCall([this](CDE90WindowCloseStatus){
        emit loaderControllerVisible(false);
        loaderController = nullptr;
        return true;
    });
    loaderController->initAsyncCaller();
    loaderController->show();
    MainThreadInvoker::getInvoker().moveToScreenCenter(loaderController);
    emit loaderControllerVisible(true);
}

void MainControllerHelper::openAmpController() {
    SignalHub::instance()->sendMsg<int, const QString&>(SIGNAL_HUB_OPEN_AMP, -1, "");
}
void MainControllerHelper::openMSRController() {
    if (msrController) {
        if (!msrController->isActiveWindow()) msrController->activateWindow();
        return;
    }
    msrController = new MsrManagerController;
    msrController->setAttribute(Qt::WA_DeleteOnClose);
    msrController->setWindowTitle("MSR Manager");
    msrController->setWindowModality(Qt::NonModal);
    msrController->initAsyncCaller();
    msrController->setCloseCall([this](CDE90WindowCloseStatus){
        emit msrControllerVisible(false);
        msrController = nullptr;
        return true;
    });

    QObject::connect(msrController,
                     &MsrManagerController::openWorkSheet,
                     msrController,
                     [this](quint64 msrTopId) {
                         openWorkSheetController();
                         workSheetController->resetData(msrTopId);
                     });

    msrController->show();
    MainThreadInvoker::getInvoker().moveToScreenCenter(msrController);
    emit msrControllerVisible(true);
}

void MainControllerHelper::openImageController() {
    if (imageController) {
        if (!imageController->isActiveWindow()) imageController->activateWindow();
        return;
    }
    imageController = new ImageManagerController;
    imageController->setAttribute(Qt::WA_DeleteOnClose);
    imageController->setWindowTitle("Image Manager");
    imageController->setWindowModality(Qt::NonModal);
    imageController->initAsyncCaller();
    imageController->setCloseCall([this](CDE90WindowCloseStatus){
        emit imageControllerVisible(false);
        imageController = nullptr;
        return true;
    });

    QObject::connect(imageController,
                     &ImageManagerController::openImageView,
                     imageController,
                     [](const QList<cdsem::ImageViewListData>& imageList, int index) {
                         SignalHub::instance()->sendMsg<const QList<cdsem::ImageViewListData>&, int, int, quint64*>(SIGNAL_HUB_SEND_IMAGE_LIST_TO_VIEWER, imageList, index, 0, nullptr);
                     });

    imageController->show();
    MainThreadInvoker::getInvoker().moveToScreenCenter(imageController);
    emit imageControllerVisible(true);
}
