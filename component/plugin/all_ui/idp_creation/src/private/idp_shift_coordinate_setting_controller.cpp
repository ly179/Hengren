#include "idp_shift_coordinate_setting_controller.h"
#include "cde90_message_box.h"
#include "cdsem_number_validator_helper.h"
#include "ui_idp_shift_coordinate_setting_controller.h"

IdpShiftCoordinateSettingController::IdpShiftCoordinateSettingController(QWidget* parent) : CDE90Dialog(parent) {
    //如果你没有异步任务需求，请将connect清理掉，保留create和createAction函数
    connect(this, &IdpShiftCoordinateSettingController::asyncCallerChanged, this, [this](){
        create();
        createAction();
    }, Qt::SingleShotConnection);
}

IdpShiftCoordinateSettingController::~IdpShiftCoordinateSettingController() noexcept {
    if (ui) {
        delete ui;
    }
}

void IdpShiftCoordinateSettingController::create() {
    ui = new cdsem::UiIdpShiftCoordinateSettingController;
    setContent(new QWidget);
    ui->init(getContent());
}

void IdpShiftCoordinateSettingController::createAction() {
    CDSemNumberValidatorHelper::doubleFree<2>()
        .put(ui->xTextField->getQLineEdit())
        .put(ui->yTextField->getQLineEdit())
        .autoFixMode();
    bindCloseBtn(ui->cancelBtn->getQPushButton());
    SEM_OK_ACTION(ui->okBtn->getQPushButton())
}

QString IdpShiftCoordinateSettingController::collectData() {
    QString val = ui->xTextField->text();
    if (val.isEmpty()) {
        val = "0";
    }
    param.x = val;
    val = ui->yTextField->text();
    if (val.isEmpty()) {
        val = "0";
    }
    param.y = val;
    return "";
}

void IdpShiftCoordinateSettingController::resetToComponent() {
    ui->xTextField->setText(param.x.toQStringAndKeepFullDecimal());
    ui->yTextField->setText(param.y.toQStringAndKeepFullDecimal());
}
