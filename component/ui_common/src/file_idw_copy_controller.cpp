#include "file_idw_copy_controller.h"
#include "ui_file_idw_copy_controller.h"
#include "service_manager.h"
#include "idw_wafer_service.h"
#include "idw_wafer_data_model.h"
#include "sem_global.h"
#include "cde90_message_box.h"
#include "cde90_process_box.h"
#include "class_list_selector_controller.h"

FileIdwCopyController::FileIdwCopyController(QWidget* parent) : CDE90Window(parent) {
    //如果你没有异步任务需求，请将connect清理掉，保留create和createAction函数
    connect(this, &FileIdwCopyController::asyncCallerChanged, this, [this](){
        create();
        createAction();
    }, Qt::SingleShotConnection);
}

FileIdwCopyController::~FileIdwCopyController() noexcept {
    if (ui) {
        delete ui;
    }
}

void FileIdwCopyController::create() {
    QWidget* content = new QWidget;
    setContent(content);
    ui = new cdsem::UiFileIdwCopyController;
    ui->init(content);
}

void FileIdwCopyController::createAction() {
    bindCloseBtn(ui->cancelBtn->getQPushButton());
    connect(ui->okBtn->getQPushButton(), &QPushButton::clicked, this, &FileIdwCopyController::beginCopy);
    connect(ui->toClassBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        createClassSelector();
        classSelector->show();
        MainThreadInvoker::getInvoker().moveToScreenCenter(classSelector);
        classSelector->initClassList();
    });
}

void FileIdwCopyController::createClassSelector() {
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

void FileIdwCopyController::setParam(const IDW_CopyDataParam& param) {
    this->param = param;
    ui->fromDeviceTextField->setText(param.fromDeviceName);
    ui->toClassTextField->setText(param.className);
    bool succ = ACaller->push(this, [this](TSC){
        IDW_CopyDataParam param;
        MainThreadInvoker::getInvoker().blockRun([this, &param](){
            param = this->param;
        });
        int errCode;
        IDW_WaferService* waferService = ServiceManager::defaultService<IDW_WaferService>();
        IDW_WaferInfoEntity waferEntity = waferService->getWaferInfoById(param.fromId, &errCode);
        if (errCode) {
            simpleMessageBox(this, "Error", "Error to load IDW detail.");
            return;
        }
        if (waferEntity.getId() == 0) {
            simpleMessageBox(this, "Error", "IDW is not exists.");
            return;
        }
        if (param.toName.isEmpty()) {
            param.toName = QString("Copy of %1").arg(waferEntity.getWaferName());
        }
        MainThreadInvoker::getInvoker().blockRun([this, &waferEntity, &param](){
            ui->fromIDWTextField->setText(waferEntity.getWaferName());
            ui->toIDWTextField->setText(param.toName);
        });
    });
    if (succ) {
        loadingBox(this, "Working", "Loading...");
    }
}

void FileIdwCopyController::beginCopy() {
    bool succ = ACaller->push(this, [this](TSC){
        IDW_CopyDataParam param;
        MainThreadInvoker::getInvoker().blockRun([this, &param](){
            this->param.className = ui->toClassTextField->text();
            this->param.toName = ui->toIDWTextField->text();
            param = this->param;
        });
        IDW_WaferService* waferService = ServiceManager::defaultService<IDW_WaferService>();
        IDW_CopyDataResult result = waferService->copyNew(param);
        if (result.errCode) {
            if (result.errCode == FILE_NAME_REPEAT_ERROR_CODE) {
                int r = showYesNoBox(this, "Information", QString("File \"%1\" already exists, do you want to replace it ?").arg(param.toName));
                if (r) {
                    return;
                }
                param.overwrite = true;
                result = waferService->copyNew(param);
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
