#include "file_simple_copy_controller.h"
#include "ui_file_simple_copy_controller.h"
#include "cde90_message_box.h"
#include "cde90_process_box.h"
#include "sem_global.h"

FileSimpleCopyController::FileSimpleCopyController(QWidget* parent) : CDE90Window(parent) {
    //如果你没有异步任务需求，请将connect清理掉，保留create和createAction函数
    connect(this, &FileSimpleCopyController::asyncCallerChanged, this, [this](){
        create();
        createAction();
    }, Qt::SingleShotConnection);
}

FileSimpleCopyController::~FileSimpleCopyController() noexcept {
    if (ui) {
        delete ui;
    }
}

void FileSimpleCopyController::create() {
    QWidget* content = new QWidget;
    setContent(content);
    ui = new cdsem::UiFileSimpleCopyController;
    ui->init(content);
}

void FileSimpleCopyController::createAction() {
    bindCloseBtn(ui->cancelBtn->getQPushButton());

    connect(ui->okBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        startCopy();
    });
}

void FileSimpleCopyController::setFromName(const QString& text) {
    ui->fromTextField->setText(text);
}
void FileSimpleCopyController::setToName(const QString& text) {
    if (ui->toTextField->text().isEmpty()) {
        ui->toTextField->setText(text);
    }
}

void FileSimpleCopyController::setCopyParam(const SimpleDataCopyParam& param) {
    this->param = param;
    ui->fromDeviceTextField->setText(param.fromDeviceName);
    ui->toDeviceTextField->setText(param.fromDeviceName);
    ui->toTextField->setText(param.toName);
    bool succ = ACaller->push(this, [this](TSC){
        SimpleDataCopyParam param;
        MainThreadInvoker::getInvoker().blockRun([this, &param](){
            param = this->param;
        });
        QString errMsg = initData(param);
        if (errMsg.length() > 0) {
            simpleMessageBox(this, "Error", errMsg);
        }
    });
    if (succ) {
        loadingBox(this, "Loading", "Please wait");
    }
}

void FileSimpleCopyController::setCopyTitle(const QString& title) {
    ui->titleLabel->setText(title);
    ui->fromLabel->setText(title);
    ui->toLabel->setText(title);
}

void FileSimpleCopyController::startCopy() {
    bool succ = ACaller->push(this, [this](TSC){
        SimpleDataCopyParam param;
        MainThreadInvoker::getInvoker().blockRun([this, &param](){
            param = this->param;
            param.toName = ui->toTextField->text();
        });
        QString errMsg = beginCopy(param);
        if (errMsg.length() > 0) {
            simpleMessageBox(this, "Error", errMsg);
        } else {
            MainThreadInvoker::getInvoker().runLater([this](){
                simpleMessageBox(this, "Information", "Process of Copy Successfully.");
                changeCloseStatusOnce(HW_OK);
                close();
            });
        }
    });
    if (succ) {
        loadingBox(this, "Process", "Working...");
    }
}
