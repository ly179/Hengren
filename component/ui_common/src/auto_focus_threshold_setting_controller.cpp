#include "auto_focus_threshold_setting_controller.h"
#include "ui_auto_focus_threshold_setting_controller.h"

AutoFocusThresholdSettingController::AutoFocusThresholdSettingController(
    QWidget* parent)
    : CDE90Window(parent) {
    connect(
        this,
        &AutoFocusThresholdSettingController::asyncCallerChanged,
        this,
        [this]() {
            create();
            createAction();
        },
        Qt::SingleShotConnection);
}

AutoFocusThresholdSettingController::~AutoFocusThresholdSettingController() noexcept {
    if (ui) {
        delete ui;
    }
}

void AutoFocusThresholdSettingController::create() {
    ui = new cdsem::UiAutoFocusThresholdSettingController;
    setContent(new QWidget);
    ui->init(getContent());
}

void AutoFocusThresholdSettingController::createAction() {
    bindCloseBtn(ui->cancelBtn->getQPushButton());
    connect(ui->okBtn->getQPushButton(), &CDE90Button::clicked, this, [this]() {
        if (okCallback && !okCallback(ui->thresholdTextField->text().toInt())) {
            return;
        }
        close();
    });
}

void AutoFocusThresholdSettingController::setInitParam(
    int threshold) {
    ui->thresholdTextField->setText(threshold);
}
