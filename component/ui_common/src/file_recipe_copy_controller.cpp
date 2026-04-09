#include "file_recipe_copy_controller.h"
#include "ui_file_recipe_copy_controller.h"
#include "class_list_selector_controller.h"
#include "idw_list_selector_controller.h"
#include "idp_list_selector_controller.h"
#include "cde90_message_box.h"
#include "cde90_process_box.h"
#include "sem_global.h"
#include "service_manager.h"
#include "idw_wafer_service.h"
#include "idp_data_service.h"
#include "recipe_data_service.h"
#include "class_service.h"

FileRecipeCopyController::FileRecipeCopyController(QWidget* parent) : CDE90Window(parent) {
    //如果你没有异步任务需求，请将connect清理掉，保留create和createAction函数
    connect(this, &FileRecipeCopyController::asyncCallerChanged, this, [this](){
        create();
        createAction();
    }, Qt::SingleShotConnection);
}

FileRecipeCopyController::~FileRecipeCopyController() noexcept {
    if (ui) {
        delete ui;
    }
}

void FileRecipeCopyController::create() {
    QWidget* content = new QWidget;
    setContent(content);
    ui = new cdsem::UiFileRecipeCopyController;
    ui->init(content);
}

void FileRecipeCopyController::createAction() {
    bindCloseBtn(ui->cancelBtn->getQPushButton());
    connect(ui->okBtn->getQPushButton(), &QPushButton::clicked, this, &FileRecipeCopyController::beginCopy);
    connect(ui->toClassBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        createClassSelector();
        classSelector->show();
        MainThreadInvoker::getInvoker().moveToScreenCenter(classSelector);
        classSelector->initClassList();
    });
    connect(ui->toIDPBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        createIDPSelector();
        QString className = ui->toClassTextField->text();
        bool succ = ACaller->push(this, [this, className](TSC){
            ClassService* service = ServiceManager::defaultService<ClassService>();
            int errCode;
            quint64 classId = service->getByName(className, &errCode);
            if (errCode) {
                simpleMessageBox(this, "Error", "Error to load class info.");
                return;
            }
            MainThreadInvoker::getInvoker().runLater([this, classId](){
                idpSelector->show();
                idpSelector->initIdpList(classId);
                MainThreadInvoker::getInvoker().moveToScreenCenter(idpSelector);
            });
        });
        if (succ) {
            loadingBox(this, "Loading...", "Loading Class");
        }
    });
    connect(ui->toIDWBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        createIDWSelector();
        QString className = ui->toClassTextField->text();
        bool succ = ACaller->push(this, [this, className](TSC){
            ClassService* service = ServiceManager::defaultService<ClassService>();
            int errCode;
            quint64 classId = service->getByName(className, &errCode);
            if (errCode) {
                simpleMessageBox(this, "Error", "Error to load class info.");
                return;
            }
            MainThreadInvoker::getInvoker().runLater([this, classId](){
                idwSelector->show();
                openedRecipeSelfIdw = true;
                idwSelector->initIdwList(classId);
                MainThreadInvoker::getInvoker().moveToScreenCenter(idwSelector);
            });
        });
        if (succ) {
            loadingBox(this, "Loading...", "Loading Class");
        }
    });
    connect(ui->toIDWofIDPBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        createIDWSelector();
        QString className = ui->toClassTextField->text();
        bool succ = ACaller->push(this, [this, className](TSC){
            ClassService* service = ServiceManager::defaultService<ClassService>();
            int errCode;
            quint64 classId = service->getByName(className, &errCode);
            if (errCode) {
                simpleMessageBox(this, "Error", "Error to load class info.");
                return;
            }
            MainThreadInvoker::getInvoker().runLater([this, classId](){
                idwSelector->show();
                openedRecipeSelfIdw = false;
                idwSelector->initIdwList(classId);
                MainThreadInvoker::getInvoker().moveToScreenCenter(idwSelector);
            });
        });
        if (succ) {
            loadingBox(this, "Loading...", "Loading Class");
        }
    });
}

void FileRecipeCopyController::createClassSelector() {
    if (classSelector) {
        return;
    }
    classSelector = new ClassListSelectorController(this);
    classSelector->setWindowTitle("File Selector: Class");
    classSelector->setWindowModality(Qt::WindowModal);
    classSelector->initAsyncCaller(ACaller);
    classSelector->setCloseCall([this](CDE90WindowCloseStatus status){
        if (status != HW_OK) {
            return true;
        }
        QString classFileName = classSelector->getInputText();
        if (classFileName.isEmpty()) {
            simpleMessageBox(classSelector, "Error", "Please specify a class.");
            return false;
        }
        ui->toClassTextField->setText(classFileName);
        return true;
    });
}
void FileRecipeCopyController::createIDWSelector() {
    if (idwSelector) {
        return;
    }
    idwSelector = new IdwListSelectorController(this);
    idwSelector->setWindowTitle("File Selector: IDW");
    idwSelector->setWindowModality(Qt::WindowModal);
    idwSelector->initAsyncCaller(ACaller);
    idwSelector->setCloseCall([this](CDE90WindowCloseStatus status){
        if (status != HW_OK) {
            return true;
        }
        QString idwFileName = idwSelector->getInputText();
        if (idwFileName.isEmpty()) {
            simpleMessageBox(idwSelector, "Error", "Please specify a IDW.");
            return false;
        }
        if (openedRecipeSelfIdw)
            ui->toIDWTextField->setText(idwFileName);
        else
            ui->toIDWofIDPTextField->setText(idwFileName);
        return true;
    });
}

void FileRecipeCopyController::createIDPSelector() {
    if (idpSelector) {
        return;
    }
    idpSelector = new IdpListSelectorController(this);
    idpSelector->setWindowTitle("File Selector: IDP");
    idpSelector->setWindowModality(Qt::WindowModal);
    idpSelector->initAsyncCaller(ACaller);
    idpSelector->setCloseCall([this](CDE90WindowCloseStatus status){
        if (status != HW_OK) {
            return true;
        }
        QString idpFileName = idpSelector->getInputText();
        if (idpFileName.isEmpty()) {
            simpleMessageBox(idpSelector, "Error", "Please specify a IDP.");
            return false;
        }
        ui->toIDPTextField->setText(idpFileName);
        return true;
    });
}

void FileRecipeCopyController::setParam(const Recipe_CopyDataParam& param) {
    this->param = param;
    ui->fromDeviceTextField->setText(param.fromDeviceName);
    ui->toClassTextField->setText(param.toClassName);
    bool succ = ACaller->push(this, [this](TSC){
        Recipe_CopyDataParam param;
        MainThreadInvoker::getInvoker().blockRun([this, &param](){
            param = this->param;
        });
        int errCode;
        RecipeDataService* recipeService = ServiceManager::defaultService<RecipeDataService>();
        RecipeDataEntity recipeEntity = recipeService->getSimpleInfo(param.fromId, &errCode);
        if (errCode) {
            simpleMessageBox(this, "Error", "Error to load Recipe detail.");
            return;
        }
        if (recipeEntity.getId() == 0) {
            simpleMessageBox(this, "Error", "Recipe is not exists.");
            return;
        }
        if (param.toName.isEmpty()) {
            param.toName = QString("Copy of %1").arg(recipeEntity.getRecipeName());
        }
        if (recipeEntity.getIdwId() > 0) {
            IDW_WaferService* idwService = ServiceManager::defaultService<IDW_WaferService>();
            IDW_WaferInfoEntity idwEntity = idwService->getWaferInfoById(recipeEntity.getIdwId(), &errCode);
            if (errCode) {
                simpleMessageBox(this, "Error", "Error to load IDW detail.");
                return;
            }
            if (idwEntity.getId() > 0) {
                if (param.toIdwName.isEmpty()) {
                    param.toIdwName = QString("Copy of %1").arg(idwEntity.getWaferName());
                }
                MainThreadInvoker::getInvoker().runLater([this](){
                    ui->toIDWTextField->setEnabled(true);
                    ui->toIDWBtn->setEnabled(true);
                });
            } else {
                MainThreadInvoker::getInvoker().runLater([this](){
                    ui->toIDWTextField->setDisabled(true);
                    ui->toIDWBtn->setDisabled(true);
                });
            }
        } else {
            MainThreadInvoker::getInvoker().runLater([this](){
                ui->toIDWTextField->setDisabled(true);
                ui->toIDWBtn->setDisabled(true);
            });
        }

        if (recipeEntity.getIdpId() > 0) {
            IdpDataService* idpService = ServiceManager::defaultService<IdpDataService>();
            IdpDataEntity idpEntity = idpService->getSimpleInfo(recipeEntity.getIdpId(), &errCode);
            if (errCode) {
                simpleMessageBox(this, "Error", "Error to load IDP detail.");
                return;
            }
            if (idpEntity.getId() > 0) {
                if (param.toIdpName.isEmpty()) {
                    param.toIdpName = QString("Copy of %1").arg(idpEntity.getIdpName());
                }
                MainThreadInvoker::getInvoker().runLater([this](){
                    ui->toIDPTextField->setEnabled(true);
                    ui->toIDPBtn->setEnabled(true);
                });

                if (idpEntity.getIdwId() > 0) {
                    IDW_WaferService* idwService = ServiceManager::defaultService<IDW_WaferService>();
                    IDW_WaferInfoEntity idwEntity = idwService->getWaferInfoById(idpEntity.getIdwId(), &errCode);
                    if (errCode) {
                        simpleMessageBox(this, "Error", "Error to load IDW of IDP detail.");
                        return;
                    }
                    if (idwEntity.getId() > 0) {
                        if (param.toIdwOfIdpName.isEmpty()) {
                            param.toIdwOfIdpName = QString("Copy of %1").arg(idwEntity.getWaferName());
                        }
                        MainThreadInvoker::getInvoker().runLater([this](){
                            ui->toIDWofIDPTextField->setEnabled(true);
                            ui->toIDWofIDPBtn->setEnabled(true);
                        });
                    } else {
                        MainThreadInvoker::getInvoker().runLater([this](){
                            ui->toIDWofIDPTextField->setDisabled(true);
                            ui->toIDWofIDPBtn->setDisabled(true);
                        });
                    }
                } else {
                    MainThreadInvoker::getInvoker().runLater([this](){
                        ui->toIDWofIDPTextField->setDisabled(true);
                        ui->toIDWofIDPBtn->setDisabled(true);
                    });
                }


            } else {
                MainThreadInvoker::getInvoker().runLater([this](){
                    ui->toIDPTextField->setDisabled(true);
                    ui->toIDPBtn->setDisabled(true);
                    ui->toIDWofIDPTextField->setDisabled(true);
                    ui->toIDWofIDPBtn->setDisabled(true);
                });
            }
        } else {
            MainThreadInvoker::getInvoker().runLater([this](){
                ui->toIDPTextField->setDisabled(true);
                ui->toIDPBtn->setDisabled(true);
                ui->toIDWofIDPTextField->setDisabled(true);
                ui->toIDWofIDPBtn->setDisabled(true);
            });
        }


        MainThreadInvoker::getInvoker().blockRun([this,&recipeEntity, &param](){
            ui->fromRecipeTextField->setText(recipeEntity.getRecipeName());
            ui->toIDWTextField->setText(param.toIdwName);
            ui->toIDPTextField->setText(param.toIdpName);
            ui->toRecipeTextField->setText(param.toName);
            ui->toIDWofIDPTextField->setText(param.toIdwOfIdpName);
        });
    });
    if (succ) {
        loadingBox(this, "Working", "Loading...");
    }
}

void FileRecipeCopyController::beginCopy() {
    bool succ = ACaller->push(this, [this](TSC){
        Recipe_CopyDataParam param;
        MainThreadInvoker::getInvoker().blockRun([this, &param](){
            this->param.toClassName = ui->toClassTextField->text();
            this->param.toIdwName = ui->toIDWTextField->text();
            this->param.toIdpName = ui->toIDPTextField->text();
            this->param.toIdwOfIdpName = ui->toIDWofIDPTextField->text();
            this->param.toName = ui->toRecipeTextField->text();
            param = this->param;
        });
        RecipeDataService* service = ServiceManager::defaultService<RecipeDataService>();
        Recipe_CopyDataResult result = service->copyNew(param);
        if (result.errCode) {
            if (result.errCode == FILE_NAME_REPEAT_ERROR_CODE) {
                int r = showYesNoBox(this, "Information", QString("%1\nDo you want to replace it ?").arg(result.errMsg));
                if (r) {
                    return;
                }
                param.overwrite = true;
                result = service->copyNew(param);
            }
            if (result.errCode) {
                simpleMessageBox(this, "Error", result.errMsg);
                return;
            }
        }
        MainThreadInvoker::getInvoker().runLater([this, result](){
            this->afterCopyClassId = result.newClassId;
            this->afterCopyId = result.newId;
            simpleMessageBox(this, "Information", "Process of Copy Successfully.");
            changeCloseStatusOnce(HW_OK);
            close();
        });
    });
    if (succ) {
        loadingBox(this, "Working", "Loading...");
    }
}
