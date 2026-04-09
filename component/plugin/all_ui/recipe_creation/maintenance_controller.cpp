#include "maintenance_controller.h"
#include "cde90_message_box.h"
#include "cdsem_number_validator_helper.h"
#include "recipe_data_model.h"
#include "ui_maintenance_controller.h"

MaintenanceController::MaintenanceController(QWidget* parent) : CDE90Window(parent) {
    create();
    createAction();
}

MaintenanceController::~MaintenanceController() noexcept {
    if (ui) {
        delete ui;
    }
}

void MaintenanceController::create() {
    ui = new cdsem::UiMaintenanceController;
    setContent(new QWidget);
    ui->init(getContent());
}

void MaintenanceController::createAction() {
    CDSemNumberValidatorHelper::doubleFree<1>()
        .put(ui->scopeXTextField)
        .put(ui->scopeYTextField)
        .put(ui->scopeXDiffTextField)
        .put(ui->scopeYDiffTextField)
        .autoFixMode();
    CDSemNumberValidatorHelper::doubleFree<3>().put(ui->stageMonitorTextField).autoFixMode();
    CDSemNumberValidatorHelper::intFree()
        .put(ui->zSensorTextField)
        .put(ui->zSensorDiffTextField)
        .put(ui->cdCheckNextMsrNoTextField)
        .autoFixMode();
    connect(ui->cdCheckCBox, &QCheckBox::toggled, this, [this](bool checked) {
        ui->cdCheckRefLabel->setEnabled(checked);
        ui->cdCheckRefMoreBtn->setEnabled(checked);
        ui->cdCheckMeasureLabel->setEnabled(checked);
        ui->cdCheckMeasureTextField->setEnabled(checked);
        ui->cdCheckMeasureExeLabel->setEnabled(checked);
        ui->cdCheckNextMsrNoLabel->setEnabled(checked);
        ui->cdCheckNextMsrNoTextField->setEnabled(checked);
    });
    connect(ui->stageMonitorCBox, &QCheckBox::toggled, this, [this](bool checked) {
        ui->stageMonitorRangeLabel->setEnabled(checked);
        ui->stageMonitorRangeDiffLabel->setEnabled(checked);
        ui->stageMonitorTextField->setEnabled(checked);
        ui->stageMonitorUnitLabel->setEnabled(checked);
    });
    connect(ui->pgMonitorCBox, &QCheckBox::toggled, this, [this](bool checked) {
        ui->pgMonitorSetupLabel->setEnabled(checked);
        ui->pgMonitorSteupMorBtn->setEnabled(checked);
    });
    bindCloseBtn(ui->cancalBtn->getQPushButton());
    CDSEM_OK_ACTION2(ui->okBtn->getQPushButton(), MaintenanceDataModel)
}
QString MaintenanceController::collectData(
    MaintenanceDataModel& param) {
    OmSemScopeCorrectionModel& scopeParam = param.omSemScopeCorrection;
    scopeParam.enableOmSemScopeCorrection = ui->scopeCorrectionCBox->isChecked();
    scopeParam.targetRangeX = ui->scopeXTextField->text();
    scopeParam.targetRangeY = ui->scopeYTextField->text();
    scopeParam.targetRangeDiffX = ui->scopeXDiffTextField->text();
    scopeParam.targetRangeDiffY = ui->scopeYDiffTextField->text();
    scopeParam.allowAdjustment = ui->scopeAllowAdjustmentCBox->isChecked();

    ZSensorOffsetModel& zParam = param.zSensorOffset;
    zParam.enableZSensorOffset = ui->zSensorCBox->isChecked();
    QString text = ui->zSensorTextField->text();
    if (text.length() > 0) {
        zParam.offset = text.toInt();
    } else {
        zParam.offset = nullptr;
    }
    text = ui->zSensorDiffTextField->text();
    if (text.length() > 0) {
        zParam.lsb = text.toInt();
    } else {
        zParam.lsb = nullptr;
    }
    zParam.allowAdjustment = ui->zSensorAllowAdjustmentCBox->isChecked();

    CDCheckModel& cdCheckParam = param.cdCheck;
    cdCheckParam.enableCDCheck = ui->cdCheckCBox->isChecked();
    if (cdCheckParam.enableCDCheck) {
        text = ui->cdCheckNextMsrNoTextField->text();
        if (text.length() > 0) {
            cdCheckParam.nextMsrNo = text.toInt();
        } else {
            cdCheckParam.nextMsrNo = nullptr;
        }
        //TODO CDCheck其他参数未知
    }

    StageMonitorModel& stageMonitorParam = param.stageMonitor;
    stageMonitorParam.enableStageMonitor = ui->stageMonitorCBox->isChecked();
    if (stageMonitorParam.enableStageMonitor) {
        stageMonitorParam.range = ui->stageMonitorTextField->text();
    }

    PgMonitorModel& pgMonitorParam = param.pgMonitor;
    pgMonitorParam.enablePgMonitor = ui->pgMonitorCBox->isChecked();
    //TODO PG其他参数未知
    return "";
}

void MaintenanceController::resetToComponent(
    const MaintenanceDataModel& param) {
    ui->scopeCorrectionCBox->setChecked(param.omSemScopeCorrection.enableOmSemScopeCorrection);
    ui->scopeXTextField->setText(
        param.omSemScopeCorrection.targetRangeX.toQStringAndKeepFullDecimal());
    ui->scopeYTextField->setText(
        param.omSemScopeCorrection.targetRangeY.toQStringAndKeepFullDecimal());
    ui->scopeXDiffTextField->setText(
        param.omSemScopeCorrection.targetRangeDiffX.toQStringAndKeepFullDecimal());
    ui->scopeYDiffTextField->setText(
        param.omSemScopeCorrection.targetRangeDiffY.toQStringAndKeepFullDecimal());
    ui->scopeAllowAdjustmentCBox->setChecked(param.omSemScopeCorrection.allowAdjustment);

    ui->zSensorCBox->setChecked(param.zSensorOffset.enableZSensorOffset);
    ui->zSensorTextField->setText(param.zSensorOffset.offset.toString());
    ui->zSensorDiffTextField->setText(param.zSensorOffset.lsb.toString());
    ui->zSensorAllowAdjustmentCBox->setChecked(param.zSensorOffset.allowAdjustment);

    ui->cdCheckCBox->setChecked(param.cdCheck.enableCDCheck);
    ui->cdCheckNextMsrNoTextField->setText(param.cdCheck.nextMsrNo.toString());

    ui->stageMonitorCBox->setChecked(param.stageMonitor.enableStageMonitor);
    ui->stageMonitorTextField->setText(param.stageMonitor.range.toQStringAndKeepFullDecimal());

    ui->pgMonitorCBox->setChecked(param.pgMonitor.enablePgMonitor);
}
