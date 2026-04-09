#include "template_save_box_controller.h"
#include "cdsem_textfield_string_validator.h"
#include "ui_template_save_box_controller.h"
#include "sem_global.h"

TemplateSaveBoxController::TemplateSaveBoxController(QWidget* parent) : CDE90Window(parent) {
    //如果你没有异步任务需求，请将connect清理掉，保留create和createAction函数
    create();
    createAction();
}

TemplateSaveBoxController::~TemplateSaveBoxController() noexcept {
    if (ui) {
        delete ui;
    }
}

void TemplateSaveBoxController::create() {
    ui = new cdsem::UiTemplateSaveBoxController;
    setContent(new QWidget);
    ui->init(getContent());
}

void TemplateSaveBoxController::createAction() {
    bindCloseBtn(ui->cancel->getQPushButton());
    CDSemTextFieldStringValidator* validator = new CDSemTextFieldStringValidator(
        ui->nameTextField->getQLineEdit());
    validator->setRange(0, FILE_NAME_LENGTH);
    ui->nameTextField->getQLineEdit()->setValidator(validator);
    connect(ui->overloadCheckBox,
            &QCheckBox::toggled,
            ui->nameTextField->getQLineEdit(),
            &QLineEdit::setDisabled);

    connect(ui->okBtn->getQPushButton(), &QPushButton::clicked, this, [this]() {
        if (!okCallback) {
            return;
        }
        TemplateSaveBoxData param;
        param.checked = ui->overloadCheckBox->isChecked();
        param.text = ui->nameTextField->text();
        if (okCallback(param)) {
            changeCloseStatusOnce(HW_OK);
            close();
        }
    });
}

void TemplateSaveBoxController::resetComponent(
    const TemplateSaveBoxData& param) {
    ui->nameTextField->setText(param.text);
    ui->overloadCheckBox->setChecked(param.checked);
    ui->nameTextField->getQLineEdit()->setDisabled(ui->overloadCheckBox->isEnabled()
                                                   && ui->overloadCheckBox->isChecked());
}

void TemplateSaveBoxController::disableCheckBox(
    bool b) {
    ui->overloadCheckBox->setDisabled(b);
    ui->nameTextField->getQLineEdit()->setDisabled(ui->overloadCheckBox->isEnabled()
                                                   && ui->overloadCheckBox->isChecked());
}

void TemplateSaveBoxController::setCheckBoxText(
    const QString& text) {
    ui->overloadCheckBox->setText(text);
}
