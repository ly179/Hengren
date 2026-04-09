#include "amp_dense_setting_edit_controller.h"
#include "cdsem_number_validator_helper.h"
#include "ui_amp_dense_setting_edit_controller.h"
#include "cdsem_global_data.h"
#include <QPointer>

AmpDenseSettingEditController::AmpDenseSettingEditController(QWidget* parent)
    : QWidget(parent) {
    create();
    createAction();
    initRealTimeEdit();
}

AmpDenseSettingEditController::~AmpDenseSettingEditController() noexcept {
    if (ui) {
        delete ui;
    }
}

void AmpDenseSettingEditController::create() {
    ui = new cdsem::UiAmpDenseSettingEditController;
    ui->init(this);
}

void AmpDenseSettingEditController::createAction() {
    CDSemNumberValidatorHelper::range(20, 4016).put(ui->dCursorSizeXTextField).autoFixMode();
    CDSemNumberValidatorHelper::range(1, 3522).put(ui->dCursorSizeYTextField).autoFixMode();
    CDSemNumberValidatorHelper::intFree().put(ui->dStartLineTextField).autoFixMode();
    CDSemNumberValidatorHelper::min(0).put(ui->dMeasNumTextField).autoFixMode();

    connect(cdsem::CDSEM_GlobalDataHelper::get(), &cdsem::CDSEM_GlobalData::tmpAmpChanged, this, [this](QPointer<QObject> obj, void* pPtr){
        if (obj == this) return;
        cdsem::MeasureType mType = cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpMeasureType();
        if (mType == cdsem::MT_WIDTH) {
            if (pPtr == &param->cursorSizeX) {
                ui->dCursorSizeXTextField->getQLineEdit()->blockSignals(true);
                ui->dCursorSizeXTextField->setText(param->cursorSizeX);
                ui->dCursorSizeXTextField->getQLineEdit()->blockSignals(false);
            }
            if (pPtr == &param->cursorSizeY) {
                ui->dCursorSizeYTextField->getQLineEdit()->blockSignals(true);
                ui->dCursorSizeYTextField->setText(param->cursorSizeY);
                ui->dCursorSizeYTextField->getQLineEdit()->blockSignals(false);
            }
        }
    });
}

void AmpDenseSettingEditController::initRealTimeEdit() {
    connect(ui->dMeasLineCBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int){
        if (!param) return;
        int val = ui->dMeasLineCBox->currentValue();
        if (val < 0 || val == param->measLine) return;
        param->measLine = val;
        emit dataChanged(QPointer<QObject>(this), &param->measLine);
    });
    connect(ui->dStartLineTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        QString val = ui->dStartLineTextField->text();
        if (val.isEmpty()) return;
        int n = val.toInt();
        if (param->startLine == n) return;
        param->startLine = n;
        emit dataChanged(QPointer<QObject>(this), &param->startLine);
    });
    connect(ui->dMeasDirectionCBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int){
        if (!param) return;
        int val = ui->dMeasDirectionCBox->currentValue();
        if (val < 0 || val == param->measDirection) return;
        param->measDirection = val;
        emit dataChanged(QPointer<QObject>(this), &param->measDirection);
    });

    connect(ui->dMeasNumTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int val = fromTextFieldText(ui->dMeasNumTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD)
                      .toInt();
        if (param->measNum == val) return;
        param->measNum = val;
        emit dataChanged(QPointer<QObject>(this), &param->measNum);
    });
    connect(ui->dCursorSizeXTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int val = fromTextFieldText(ui->dCursorSizeXTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (param->cursorSizeX == val) return;
        param->cursorSizeX = val;
        emit dataChanged(QPointer<QObject>(this), &param->cursorSizeX);
    });
    connect(ui->dCursorSizeYTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int val = fromTextFieldText(ui->dCursorSizeYTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (param->cursorSizeY == val) return;
        param->cursorSizeY = val;
        emit dataChanged(QPointer<QObject>(this), &param->cursorSizeY);
    });
    connect(ui->dEdgeDetectCheckBox, &QCheckBox::toggled, this, [this](bool checked){
        if (!param) return;
        if (checked == param->enableEdgeDetection) return;
        param->enableEdgeDetection = checked;
        emit dataChanged(QPointer<QObject>(this), &param->enableEdgeDetection);
    });
}

void AmpDenseSettingEditController::resetToComponent() {
    if (!param) {
        return;
    }
    ui->dMeasLineCBox->setCurrentData(param->measLine);
    ui->dStartLineTextField->setText(param->startLine);
    ui->dMeasDirectionCBox->setCurrentData(param->measDirection);
    ui->dMeasNumTextField->setText(param->measNum);
    ui->dCursorSizeXTextField->setText(param->cursorSizeX);
    ui->dCursorSizeYTextField->setText(param->cursorSizeY);
    ui->dEdgeDetectCheckBox->setChecked(param->enableEdgeDetection);
}

QString AmpDenseSettingEditController::collectData(AmpDenseObjectParam& param) {
    param.measLine = ui->dMeasLineCBox->currentValue();
    if (param.measLine < 0) {
        return "Dense object Meas. line is not specified.";
    }
    QString tVal = ui->dStartLineTextField->text();
    if (tVal.isEmpty()) {
        return "Dense object start line is required.";
    }
    param.startLine = tVal.toInt();
    param.measDirection = ui->dMeasDirectionCBox->currentValue();
    if (param.measDirection < 0) {
        return "Dense object Meas. Direction is not specified.";
    }
    param.measNum = fromTextFieldText(ui->dMeasNumTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD)
                         .toInt();
    param.cursorSizeX
        = fromTextFieldText(ui->dCursorSizeXTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
    param.cursorSizeY
        = fromTextFieldText(ui->dCursorSizeYTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
    param.enableEdgeDetection = ui->dEdgeDetectCheckBox->isChecked();
    return "";
}

void AmpDenseSettingEditController::setDenseSettingType(DenseSettingType t) {
    switch (t) {
    case DST_LINE:
        ui->measTypeLabel->setText("Meas. Line");
        ui->startNumLabel->setText("Start Line");
        break;
    case DST_SPACE:
        ui->measTypeLabel->setText("Meas. Space");
        ui->startNumLabel->setText("Start Space");
        break;
    case DST_PITCH_L:
        ui->measTypeLabel->setText("Meas. Pitch(L)");
        ui->startNumLabel->setText("Start Pitch(L)");
        break;
    case DST_PITCH_R:
        ui->measTypeLabel->setText("Meas. Pitch(R)");
        ui->startNumLabel->setText("Start Pitch(R)");
        break;
    default:
        break;
    }
}

void refreshTargetLimitTextField(bool isDense, CDE90TextFieldBox* target, CDE90TextFieldBox* limit) {
    const QValidator* qv = target->getQLineEdit()->validator();
    if (isDense) {
        if (qv) {
            const CDSemNumberValidator* v = dynamic_cast<const CDSemNumberValidator*>(qv);
            if (v && v->max() == "200") {
                return;
            }
            CDSemNumberValidatorHelper::range(1, 200).put(target).put(limit).autoFixMode();
            if (qv) {
                delete qv;
            }
        }
    } else {
        const CDSemNumberValidator* v = dynamic_cast<const CDSemNumberValidator*>(qv);
        if (!v || v->max() != "64") {
            CDSemNumberValidatorHelper::range(1, 64).put(target).put(limit).autoFixMode();
            if (qv) {
                delete qv;
            }
        }
        if (fromTextFieldText(target).toInt() > 64) {
            target->setText(64);
        }
        if (fromTextFieldText(limit).toInt() > 64) {
            limit->setText(64);
        }
    }
}
