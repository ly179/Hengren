#include "file_idp_copy_controller.h"
#include "ui_file_idp_copy_controller.h"
#include "class_list_selector_controller.h"
#include "cde90_message_box.h"
#include "cde90_process_box.h"
#include "idw_list_selector_controller.h"
#include "sem_global.h"
#include "service_manager.h"
#include "idw_wafer_service.h"
#include "idp_data_service.h"
#include "class_service.h"

FileIdpCopyController::FileIdpCopyController(QWidget* parent) : CDE90Window(parent) {
    //如果你没有异步任务需求，请将connect清理掉，保留create和createAction函数
    connect(this, &FileIdpCopyController::asyncCallerChanged, this, [this](){
        create();
        createAction();
    }, Qt::SingleShotConnection);
}

FileIdpCopyController::~FileIdpCopyController() noexcept {
    if (ui) {
        delete ui;
    }
}

void FileIdpCopyController::create() {
    QWidget* content = new QWidget;
    setContent(content);
    ui = new cdsem::UiFileIdpCopyController;
    ui->init(content);
}

void FileIdpCopyController::createAction() {
    bindCloseBtn(ui->cancelBtn->getQPushButton());
    connect(ui->okBtn->getQPushButton(), &QPushButton::clicked, this, &FileIdpCopyController::beginCopy);
    connect(ui->toClassBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        createClassSelector();
        classSelector->show();
        MainThreadInvoker::getInvoker().moveToScreenCenter(classSelector);
        classSelector->initClassList();
    });
    connect(ui->toIDWBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        createIDWSelector();
        MainThreadInvoker::getInvoker().moveToScreenCenter(idwSelector);
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
                idwSelector->initIdwList(classId);
            });
        });
        if (succ) {
            loadingBox(this, "Loading...", "Loading Class");
        }
    });
}

void FileIdpCopyController::createClassSelector() {
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
void FileIdpCopyController::createIDWSelector() {
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
        ui->toIDWTextField->setText(idwFileName);
        return true;
    });
}

void FileIdpCopyController::setParam(const IDP_CopyDataParam& param) {
    this->param = param;
    ui->fromDeviceTextField->setText(param.fromDeviceName);
    ui->toClassTextField->setText(param.toClassName);
    bool succ = ACaller->push(this, [this](TSC){
        IDP_CopyDataParam param;
        MainThreadInvoker::getInvoker().blockRun([this, &param](){
            param = this->param;
        });
        int errCode;
        IdpDataService* idpService = ServiceManager::defaultService<IdpDataService>();
        IdpDataEntity idpEntity = idpService->getSimpleInfo(param.fromId, &errCode);
        if (errCode) {
            simpleMessageBox(this, "Error", "Error to load IDP detail.");
            return;
        }
        if (idpEntity.getId() == 0) {
            simpleMessageBox(this, "Error", "IDP is not exists.");
            return;
        }
        if (param.toName.isEmpty()) {
            param.toName = QString("Copy of %1").arg(idpEntity.getIdpName());
        }
        if (idpEntity.getIdwId() > 0) {
            IDW_WaferService* idwService = ServiceManager::defaultService<IDW_WaferService>();
            IDW_WaferInfoEntity waferEntity = idwService->getWaferInfoById(idpEntity.getIdwId(), &errCode);
            if (errCode) {
                simpleMessageBox(this, "Error", "Error to load IDW detail.");
                return;
            }
            // if (waferEntity.getId() == 0) {
            //     simpleMessageBox(this, "Error", "IDW is not exists.");
            //     return;
            // }
            if (waferEntity.getId() > 0) {
                if (param.toIdwName.isEmpty()) {
                    param.toIdwName = QString("Copy of %1").arg(waferEntity.getWaferName());
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

        MainThreadInvoker::getInvoker().blockRun([this,&idpEntity, &param](){
            ui->fromIDPTextField->setText(idpEntity.getIdpName());
            ui->toIDWTextField->setText(param.toIdwName);
            ui->toIDPTextField->setText(param.toName);
        });
    });
    if (succ) {
        loadingBox(this, "Working", "Loading...");
    }
}

void FileIdpCopyController::beginCopy() {
    bool succ = ACaller->push(this, [this](TSC){
        IDP_CopyDataParam param;
        MainThreadInvoker::getInvoker().blockRun([this, &param](){
            this->param.toClassName = ui->toClassTextField->text();
            this->param.toIdwName = ui->toIDWTextField->text();
            this->param.toName = ui->toIDPTextField->text();
            param = this->param;
        });
        IdpDataService* service = ServiceManager::defaultService<IdpDataService>();
        IDP_CopyDataResult result = service->copyNew(param);
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
