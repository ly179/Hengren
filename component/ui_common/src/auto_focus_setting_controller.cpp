#include "auto_focus_setting_controller.h"
#include <QLayout>
#include "cde90_message_box.h"
#include "cdsem_number_validator_helper.h"
#include "focus_auto_content.h"
#include "focus_stigma_content.h"
#include "ui_auto_focus_setting_controller.h"

AutoFocusSettingController::AutoFocusSettingController(bool address, QWidget* parent) : CDE90Window(parent), isAddress(address) {
    connect(this, &AutoFocusSettingController::asyncCallerChanged, this, [this](){
        create();
        createAction();
    }, Qt::SingleShotConnection);
}

AutoFocusSettingController::~AutoFocusSettingController() noexcept {
    if (ui) {
        delete ui;
    }
}

void AutoFocusSettingController::create() {
    ui = new cdsem::UiAutoFocusSettingController;
    setContent(new QWidget);
    ui->init(getContent());
    QWidget* parent;
    if (isAddress) {
        ui->mpFocusContent->hide();
        ui->apFocusContent->show();
        parent = ui->apFocusDetailContent;
    } else {
        ui->apFocusContent->hide();
        ui->mpFocusContent->show();
        parent = ui->mpFocusDetailContent;
    }

    focusAutoContent = new FocusAutoContent(isAddress, parent);
    focusAutoContent->setFrameStyle(QFrame::Sunken | QFrame::Box);
    focusAutoContent->setLineWidth(1);
    focusStigmaContent = new FocusStigmaContent(isAddress, parent);
    focusStigmaContent->setFrameStyle(QFrame::Sunken | QFrame::Box);
    focusStigmaContent->setLineWidth(1);
    parent->layout()->addWidget(focusAutoContent);
    parent->layout()->addWidget(focusStigmaContent);
    
}

void AutoFocusSettingController::createAction() {
    focusAutoContent->initParam(&param.focusParam);
    focusStigmaContent->initParam(&param.stigmaParam);
    //TODO 暂定值
    CDSemNumberValidatorHelper::range<3>(-200, 200).autoFixMode().put(ui->offsetTextField
                                                                          ->getQLineEdit());
    itsChildAndBeManaged(focusAutoContent, this);
    itsChildAndBeManaged(focusStigmaContent, this);
    focusAutoContent->initAsyncCaller(ACaller);
    focusStigmaContent->initAsyncCaller(ACaller);

    bindCloseBtn(ui->cancelBtn->getQPushButton());
    if (isAddress) {
        connect(ui->autoFocusCbox, &QCheckBox::toggled, this, [this](bool checked){
            focusAutoContent->enableCheckParamValid(checked);
            focusStigmaContent->enableCheckParamValid(checked);
        });
    } else {
        connect(ui->focusCbox, &QCheckBox::toggled, this, [this](bool checked){
            focusAutoContent->enableCheckParamValid(checked && ui->stigmaAndFocusRadio->isChecked());
            focusStigmaContent->enableCheckParamValid(checked && ui->stigmaAndFocusRadio->isChecked());
        });
        connect(ui->stigmaAndFocusRadio, &QRadioButton::toggled, this, [this](bool checked){
            focusAutoContent->enableCheckParamValid(checked && ui->focusCbox->isChecked());
            focusStigmaContent->enableCheckParamValid(checked && ui->focusCbox->isChecked());
        });
    }

    SEM_OK_ACTION(ui->okBtn->getQPushButton())
    SEM_STEP_NEXT_ACTION(ui->nextBtn->getQPushButton())
    SEM_STEP_BACK_ACTION(ui->backBtn->getQPushButton())
}

void AutoFocusSettingController::resetToComponent() {
    focusAutoContent->enableCheckParamValid(param.enable);
    focusStigmaContent->enableCheckParamValid(param.enable);
    if (isAddress) {
        ui->autoFocusCbox->setChecked(param.enable);
    } else {
        ui->focusCbox->setChecked(param.enable);
        if (param.focusType == 0) {
            ui->stigmaAndFocusRadio->setChecked(true);
            ui->offsetRadio->setChecked(false);
        } else {
            ui->stigmaAndFocusRadio->setChecked(false);
            ui->offsetRadio->setChecked(true);
        }
        ui->offsetTextField->setText(param.offset.toMinString());
    }
    focusAutoContent->resetToComponent();
    focusStigmaContent->resetToComponent();
}

QString AutoFocusSettingController::collectData() {
    if (isAddress) {
        param.enable = ui->autoFocusCbox->isChecked();
    } else {
        param.enable = ui->focusCbox->isChecked();
        if (ui->stigmaAndFocusRadio->isChecked()) {
            param.focusType = 0;
        } else {
            param.focusType = 1;
        }
        param.offset = ui->offsetTextField->text().toFloat();
    }
    QString errMsg;
    errMsg = focusAutoContent->collectData();
    if (errMsg.length() > 0) {
        return errMsg;
    }
    errMsg = focusStigmaContent->collectData();
    if (errMsg.length() > 0) {
        return errMsg;
    }

    return "";
}

void AutoFocusSettingController::hideNextBtn(
    bool b) {
    ui->nextBtn->setVisible(!b);
}
void AutoFocusSettingController::hideBackBtn(
    bool b) {
    ui->backBtn->setVisible(!b);
}

//禁用一切与机台互动相关的操作
void AutoFocusSettingController::disablePyHandlers(bool b) {
    focusAutoContent->disablePyHandlers(b);
    focusStigmaContent->disablePyHandlers(b);
}

void AutoFocusSettingController::setAPos(const IDW_PointF& aPos) {
    focusAutoContent->setAPos(aPos);
    focusStigmaContent->setAPos(aPos);
}
