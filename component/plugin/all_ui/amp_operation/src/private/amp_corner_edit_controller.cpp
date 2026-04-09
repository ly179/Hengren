#include "amp_corner_edit_controller.h"
#include "cdsem_global_resource.h"
#include "cdsem_number_validator_helper.h"
#include "ui_amp_corner_edit_controller.h"
#include "cdsem_global_data.h"
#include <QPointer>

AmpCornerEditController::AmpCornerEditController(QWidget* parent)
    : QWidget(parent) {
    create();
    createAction();
    initRealTimeEdit();
}

AmpCornerEditController::~AmpCornerEditController() noexcept {
    if (ui) {
        delete ui;
    }
}

void AmpCornerEditController::create() {
    param = &cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpCornerParam();
    ui = new cdsem::UiAmpCornerEditController;
    ui->init(this);
    ui->objectCBox->getQComboBox()->addItem(ResourceManager::getResourceManager().getCornerLeftTop(),
                                            "",
                                            0);
    ui->objectCBox->getQComboBox()
        ->addItem(ResourceManager::getResourceManager().getCornerLeftBottom(), "", 1);
    ui->objectCBox->getQComboBox()
        ->addItem(ResourceManager::getResourceManager().getCornerRightTop(), "", 2);
    ui->objectCBox->getQComboBox()
        ->addItem(ResourceManager::getResourceManager().getCornerRightBottom(), "", 3);
}

void AmpCornerEditController::createAction() {
    CDSemNumberValidatorHelper::range(1, 64).put(ui->measurePointTextField).autoFixMode();
    CDSemNumberValidatorHelper::range<1>(1, 200000).put(ui->designValueTextField).autoFixMode();
    CDSemNumberValidatorHelper::range(1, 29).put(ui->smoothingTextField).autoFixMode();
    CDSemNumberValidatorHelper::range(3, 19).put(ui->diffTextField).autoFixMode();
    CDSemNumberValidatorHelper::range(0, 100).put(ui->thresholdTextField).autoFixMode();
}

void AmpCornerEditController::initRealTimeEdit() {
    connect(ui->objectCBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int){
        if (!param) return;
        int val = ui->objectCBox->currentValue();
        if (val < 0 || val == param->object) return;
        param->object = val;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->object);
        if (cdsem::CDSEM_GlobalDataHelper::get()->updateTmpAmpMObject()) emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(this, &cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpMeasureObject());
    });
    connect(ui->edgeCBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int){
        if (!param) return;
        int val = ui->edgeCBox->currentValue();
        if (val < 0 || val == param->edge) return;
        param->edge = val;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->edge);
    });
    connect(ui->dataCBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int){
        if (!param) return;
        int val = ui->dataCBox->currentValue();
        if (val < 0 || val == param->data) return;
        param->data = val;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->data);
    });
    connect(ui->designValueTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        FloatNumber1 number = fromTextFieldText(ui->designValueTextField,
                                                VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD);
        if (number == nullptr || number == param->designValue) return;
        param->designValue = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->designValue);
    });
    connect(ui->measurePointTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->measurePointTextField,
                                                VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->measurePoint) return;
        param->measurePoint = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->measurePoint);
    });
    connect(ui->methodCBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int){
        if (!param) return;
        int val = ui->methodCBox->currentValue();
        if (val < 0 || val == param->method) return;
        param->method = val;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->method);
    });
    connect(ui->smoothingTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->smoothingTextField,
                                       VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->smoothing || (number & 1) == 0) return;
        param->smoothing = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->smoothing);
    });
    connect(ui->diffTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->diffTextField,
                                       VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->diff || (number & 1) == 0) return;
        param->diff = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->diff);
    });
    connect(ui->thresholdTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->thresholdTextField,
                                       VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->threshold) return;
        param->threshold = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->threshold);
    });
}

QString AmpCornerEditController::collectData(AmpCornerParam& param) {
    param.object = ui->objectCBox->currentValue();
    if (param.object < 0) {
        return "Object is not specified.";
    }
    param.edge = ui->edgeCBox->currentValue();
    if (param.edge < 0) {
        return "Edge is not specified.";
    }
    param.data = ui->dataCBox->currentValue();
    if (param.data < 0) {
        return "Data is not specified.";
    }
    param.measurePoint
        = fromTextFieldText(ui->measurePointTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
    param.method = ui->methodCBox->currentValue();
    if (param.method < 0) {
        return "Detection method is not specified.";
    }
    param.designValue = fromTextFieldText(ui->designValueTextField,
                                          VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD);
    param.smoothing
        = fromTextFieldText(ui->smoothingTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
    if (!(param.smoothing & 1)) {
        return "Detection smoothing must be an odd number.";
    }

    param.diff = fromTextFieldText(ui->diffTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
    if (!(param.diff & 1)) {
        return "Detection differential must be an odd number.";
    }
    param.threshold
        = fromTextFieldText(ui->thresholdTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
    return "";
}
void AmpCornerEditController::resetToComponent() {
    if (!param) {
        return;
    }
    ui->objectCBox->setCurrentData(param->object);
    ui->edgeCBox->setCurrentData(param->edge);
    ui->dataCBox->setCurrentData(param->data);
    ui->measurePointTextField->setText(param->measurePoint);
    ui->methodCBox->setCurrentData(param->method);
    ui->designValueTextField->setText(param->designValue.toQStringAndKeepFullDecimal());
    ui->smoothingTextField->setText(param->smoothing);
    ui->diffTextField->setText(param->diff);
    ui->thresholdTextField->setText(param->threshold);
}
