#include "z_sensor_setting_controller.h"
#include "cde90_message_box.h"
#include "cdsem_number_validator_helper.h"
#include "ui_z_sensor_setting_controller.h"

ZSensorSettingController::ZSensorSettingController(QWidget* parent) : CDE90Dialog(parent) {
    create();
    createAction();
}

ZSensorSettingController::~ZSensorSettingController() noexcept {
    if (ui) {
        delete ui;
    }
}

void ZSensorSettingController::create() {
    ui = new cdsem::UiZSensorSettingController;
    setContent(new QWidget);
    ui->init(getContent());
}

void ZSensorSettingController::createAction() {
    {
        CDSemNumberValidatorHelper helper = CDSemNumberValidatorHelper::range(0, 10);
        helper.put(ui->offsetTextField->getQLineEdit()).autoFixMode();
    }

    SEM_OK_ACTION(ui->okBtn->getQPushButton())
    bindCloseBtn(ui->cancelBtn->getQPushButton());
    SEM_STEP_NEXT_ACTION(ui->nextBtn->getQPushButton())
    SEM_STEP_BACK_ACTION(ui->backBtn->getQPushButton())

    connect(ui->enableCbox, &QCheckBox::toggled, this, [this](bool checked) {
        ui->paramContent->setEnabled(checked);
    });
}

void ZSensorSettingController::resetToComponent() {
    ui->offsetTextField->setText(QString::number(param.offset));
    ui->enableCbox->setChecked(param.enable);
}

QString ZSensorSettingController::collectData() {
    param.enable = ui->enableCbox->isChecked();
    param.offset = ui->offsetTextField->text().toInt();
    return "";
}

void ZSensorSettingController::hideNextBtn(
    bool b) {
    ui->nextBtn->setVisible(!b);
}
void ZSensorSettingController::hideBackBtn(
    bool b) {
    ui->backBtn->setVisible(!b);
}
