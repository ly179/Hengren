#include "ifem_setting_controller.h"
#include "cde90_message_box.h"
#include "cdsem_number_validator_helper.h"
#include "ui_ifem_setting_controller.h"

IfemSettingController::IfemSettingController(QWidget* parent) : CDE90Window(parent) {
    create();
    createAction();
}

IfemSettingController::~IfemSettingController() noexcept {
    if (ui) {
        delete ui;
    }
}

void IfemSettingController::create() {
    ui = new cdsem::UiIfemSettingController;
    setContent(new QWidget);
    ui->init(getContent());
}

void IfemSettingController::createAction() {
    CDSemNumberValidatorHelper::range<1>(50, 2000)
        .put(ui->diameterTextField)
        .put(ui->widthTextField)
        .put(ui->pitchXTextField)
        .put(ui->pitchYTextField)
        .autoFixMode();
    CDSemNumberValidatorHelper::range<1>(100, 10000).put(ui->lengthYTextField).autoFixMode();
    CDSemNumberValidatorHelper::range(1, 9)
        .put(ui->numberXTextField)
        .put(ui->numberYTextField)
        .put(ui->targetXTextField)
        .put(ui->targetYTextField)
        .autoFixMode();
    bindCloseBtn(ui->cancelBtn->getQPushButton());
    SEM_OK_ACTION(ui->okBtn->getQPushButton())
    connect(ui->kindCBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this]() {
        exchangeKind(ui->kindCBox->currentValue());
    });
    connect(ui->selectMeasurePosRadio, &QRadioButton::toggled, this, [this](bool c) {
        ui->targetXTextField->setDisabled(!c);
        ui->targetYTextField->setDisabled(!c);
    });
    emit ui->kindCBox->getQComboBox()->currentIndexChanged(0);
}

void IfemSettingController::exchangeKind(
    int val) {
    if (val == 0) {
        ui->exampleCanvas->setImage(QImage(":/icon/ifem_example/n_bar.png", "png"));
        setDiameterVisible(false);
        setPitchYVisible(false);
        setNumberYVisible(false);
        setTargetYVisible(false);
        setLenYVisible(true);
        setWidth1Visible(true);
    } else if (val == 1) {
        ui->exampleCanvas->setImage(QImage(":/icon/ifem_example/pierce.png", "png"));
        setDiameterVisible(false);
        setPitchYVisible(false);
        setLenYVisible(false);
        setNumberYVisible(false);
        setTargetYVisible(false);
        setWidth1Visible(true);
    } else if (val == 2) {
        ui->exampleCanvas->setImage(QImage(":/icon/ifem_example/hole.png", "png"));
        setLenYVisible(false);
        setWidth1Visible(false);
        setDiameterVisible(true);
        setPitchYVisible(true);
        setNumberYVisible(true);
        setTargetYVisible(true);
    }
}

void IfemSettingController::setDiameterVisible(
    bool v) {
    ui->diameterLabel->setVisible(v);
    ui->diameterRangeLabel->setVisible(v);
    ui->diameterUnitLabel->setVisible(v);
    ui->diameterTextField->setVisible(v);
}

void IfemSettingController::setPitchYVisible(
    bool v) {
    ui->pitchYLabel->setVisible(v);
    ui->pitchYRangeLabel->setVisible(v);
    ui->pitchYUnitLabel->setVisible(v);
    ui->pitchYTextField->setVisible(v);
}

void IfemSettingController::setLenYVisible(
    bool v) {
    ui->lenYLabel->setVisible(v);
    ui->lenYRangeLabel->setVisible(v);
    ui->lenYUnitLabel->setVisible(v);
    ui->lengthYTextField->setVisible(v);
}

void IfemSettingController::setNumberYVisible(
    bool v) {
    ui->numberYLabel->setVisible(v);
    ui->numberYRangeLabel->setVisible(v);
    ui->numberYTextField->setVisible(v);
}
void IfemSettingController::setTargetYVisible(
    bool v) {
    ui->targetYLabelContent->setVisible(v);
    ui->targetYRangeLabel->setVisible(v);
    ui->targetYTextField->setVisible(v);
}

void IfemSettingController::setWidth1Visible(
    bool v) {
    ui->widthLabel->setVisible(v);
    ui->widthRangeLabel->setVisible(v);
    ui->widthUnitLabel->setVisible(v);
    ui->widthTextField->setVisible(v);
}

void IfemSettingController::resetToComponent() {
    ui->kindCBox->setCurrentData(param.kind);
    ui->widthTextField->setText(param.width1.toQStringAndKeepFullDecimal());
    ui->pitchXTextField->setText(param.pitchX.toQStringAndKeepFullDecimal());
    ui->pitchYTextField->setText(param.pitchY.toQStringAndKeepFullDecimal());
    ui->lengthYTextField->setText(param.lengthY.toQStringAndKeepFullDecimal());
    ui->diameterTextField->setText(param.diameter.toQStringAndKeepFullDecimal());
    ui->numberXTextField->setText(param.numberX.toString());
    ui->numberYTextField->setText(param.numberY.toString());
    ui->targetXTextField->setText(param.targetX.toString());
    ui->targetYTextField->setText(param.targetY.toString());
    if (param.targetType == 0) {
        ui->measureCursorPosRadio->setChecked(true);
    } else if (param.targetType == 1) {
        ui->selectMeasurePosRadio->setChecked(true);
    }
}
QString IfemSettingController::collectData() {
    param.kind = ui->kindCBox->currentValue();
    if (param.kind < 0) {
        return "Kind is not specified.";
    }
    QString val;
    param.width1 = ui->widthTextField->text();
    param.pitchX = ui->pitchXTextField->text();
    val = ui->targetXTextField->text();
    if (val.isEmpty()) {
        param.targetX = nullptr;
    } else {
        param.targetX = val.toInt();
    }
    val = ui->numberXTextField->text();
    if (val.isEmpty()) {
        param.numberX = nullptr;
    } else {
        param.numberX = val.toInt();
    }
    if (ui->measureCursorPosRadio->isChecked()) {
        param.targetType = 0;
    } else {
        param.targetType = 1;
    }
    if (param.kind == 0) {
        param.lengthY = ui->lengthYTextField->text();
        param.diameter.clear();
        param.pitchY.clear();
        param.numberY = nullptr;
        param.targetY = nullptr;
    } else if (param.kind == 1) {
        param.diameter.clear();
        param.pitchY.clear();
        param.lengthY.clear();
        param.numberY = nullptr;
        param.targetY = nullptr;
    } else if (param.kind == 2) {
        param.lengthY.clear();
        param.diameter = ui->diameterTextField->text();
        param.pitchY = ui->pitchYTextField->text();
        val = ui->numberYTextField->text();
        if (val.isEmpty()) {
            param.numberY = nullptr;
        } else {
            param.numberY = val.toInt();
        }
        val = ui->targetYTextField->text();
        if (val.isEmpty()) {
            param.targetY = nullptr;
        } else {
            param.targetY = val.toInt();
        }
    }
    return "";
}
