#include "auto_focus_offset_setting_controller.h"
#include "cdsem_data_common.h"
#include "cdsem_number_validator_helper.h"
#include "ui_auto_focus_offset_setting_controller.h"

AutoFocusOffsetSettingController::AutoFocusOffsetSettingController(
    bool address, int mode, QWidget* parent)
    : CDE90Window(parent)
    , mode(mode)
    , isAddress(address) {
    connect(
        this,
        &AutoFocusOffsetSettingController::asyncCallerChanged,
        this,
        [this]() {
            create();
            createAction();
            changeMode(this->mode);
            setInitParam(this->offset);
        },
        Qt::SingleShotConnection);
}

AutoFocusOffsetSettingController::~AutoFocusOffsetSettingController() noexcept {
    if (ui) {
        delete ui;
    }
}

void AutoFocusOffsetSettingController::create() {
    setContent(new QWidget);
    ui = new cdsem::UiAutoFocusOffsetSettingController;
    ui->init(getContent());
}

void AutoFocusOffsetSettingController::createAction() {
    //TODO 暂定值
    CDSemNumberValidatorHelper::range(-200, 200).autoFixMode().put(
        ui->offsetTextField->getQLineEdit());
    bindCloseBtn(ui->cancelBtn->getQPushButton());

    connect(ui->setBtn->getQPushButton(), &CDE90Button::clicked, this, [this]() {
        if (okCallback && !okCallback(ui->offsetTextField->text().toInt())) {
            return;
        }
        close();
    });
}

void AutoFocusOffsetSettingController::changeMode(
    int mode) {
    this->mode = mode;
    if (!isAddress
        && (this->mode == cdsem::MP_AUTO_FOCUS_METHOD_FAST2
            || mode == cdsem::MP_AUTO_FOCUS_METHOD_DENSE)) {
        ui->diff2Label->setText(
            "2.Please focus manually or run Focusing Tool\nin Image Operation.");
    } else {
        ui->diff2Label->setText("2.Please focus manually.");
    }
}

void AutoFocusOffsetSettingController::setInitParam(
    int offset) {
    this->offset = offset;
    ui->offsetTextField->setText(offset);
}
