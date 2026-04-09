#include "pre_dose_setting_controller.h"
#include "cde90_message_box.h"
#include "cdsem_number_validator_helper.h"
#include "ui_pre_dose_setting_controller.h"
#include "cdsem_global_data.h"

PreDoseSettingController::PreDoseSettingController(QWidget* parent) : CDE90Dialog(parent) {
    connect(this, &PreDoseSettingController::asyncCallerChanged, this, [this](){
        create();
        createAction();
    }, Qt::SingleShotConnection);
}

PreDoseSettingController::~PreDoseSettingController() noexcept {
    if (ui) {
        delete ui;
    }
}

void PreDoseSettingController::create() {
    ui = new cdsem::UiPreDoseSettingController;
    setContent(new QWidget);
    ui->init(getContent());
}

void PreDoseSettingController::createAction() {
    {
        CDSemNumberValidatorHelper helper = CDSemNumberValidatorHelper::range(10, 100000);
        helper.put(ui->magTextField->getQLineEdit()).autoFixMode();
    }
    {
        CDSemNumberValidatorHelper helper = CDSemNumberValidatorHelper::range(-800, 800);
        helper.put(ui->vaccTextField->getQLineEdit()).autoFixMode();
    }
    {
        CDSemNumberValidatorHelper helper = CDSemNumberValidatorHelper::range<1>("1", "50");
        helper.put(ui->waitSecondsTextField->getQLineEdit()).autoFixMode();
    }
    bindCloseBtn(ui->cancelBtn->getQPushButton());
    ui->vaccModeBtn->setAsyncCaller(getAsyncCaller());
    itsChildAndBeManaged(ui->vaccModeBtn, this);
    SEM_OK_ACTION(ui->okBtn->getQPushButton())
    SEM_STEP_NEXT_ACTION(ui->nextBtn->getQPushButton())
    SEM_STEP_BACK_ACTION(ui->backBtn->getQPushButton())

    connect(ui->preCbox, &QCheckBox::toggled, this, [this](bool checked) {
        ui->paramContent->setEnabled(checked);
    });

    connect(ui->customVaccCBox, &QCheckBox::toggled, this, [this](bool checked) {
        ui->vaccTextField->setEnabled(checked);
        ui->vaccModeBtn->setEnabled(checked);
    });

    connect(ui->readBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        ui->magTextField->setText(SEM_PY_ENV.getMag());
    });
}

void PreDoseSettingController::resetToComponent() {
    ui->preCbox->setChecked(param.enable);
    ui->magTextField->setText(QString::number(param.mag));
    ui->waitSecondsTextField->setText(param.waitSeconds.toQStringAndKeepFullDecimal());
    ui->customVaccCBox->setChecked(param.customVacc);
    ui->vaccModeBtn->safeSetCurrentData(QString::number(param.vaccMode));
    ui->unChangeModeCBox->setChecked(param.unChangeMode);
    ui->highModeCBox->setChecked(param.highMode);
    ui->memTextField->setText(QString::number(param.memoryNumber));
    ui->vaccTextField->setText(QString::number(param.vacc));
}

QString PreDoseSettingController::collectData() {
    param.enable = ui->preCbox->isChecked();
    param.mag = ui->magTextField->text().toInt();
    QString waitSeconds = ui->waitSecondsTextField->text();
    if (waitSeconds.isEmpty()) {
        waitSeconds = "0";
    }
    param.waitSeconds = waitSeconds;
    param.customVacc = ui->customVaccCBox->isChecked();
    if (ui->preCbox->isChecked() && ui->customVaccCBox->isChecked() && ui->vaccModeBtn->currentIndex() < 0) {
        return "Vacc mode not specified.";
    }
    param.vaccMode = ui->vaccModeBtn->currentData().toInt();
    param.unChangeMode = ui->unChangeModeCBox->isChecked();
    param.highMode = ui->highModeCBox->isChecked();
    param.memoryNumber = ui->memTextField->text().toInt();
    param.vacc = ui->vaccTextField->text().toInt();
    return "";
}

void PreDoseSettingController::hideNextBtn(
    bool b) {
    ui->nextBtn->setVisible(!b);
}
void PreDoseSettingController::hideBackBtn(
    bool b) {
    ui->backBtn->setVisible(!b);
}
