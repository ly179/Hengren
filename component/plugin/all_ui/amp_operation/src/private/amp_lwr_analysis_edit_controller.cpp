#include "amp_lwr_analysis_edit_controller.h"
#include "cdsem_number_validator_helper.h"
#include "sem_global.h"
#include "ui_amp_lwr_analysis_edit_controller.h"
#include "cdsem_global_data.h"

AmpLwrAnalysisEditController::AmpLwrAnalysisEditController(QWidget* parent)
    : QWidget(parent) {
    create();
    createAction();
    initRealTimeEdit();
}

AmpLwrAnalysisEditController::~AmpLwrAnalysisEditController() noexcept {
    if (ui) {
        delete ui;
    }
}

void AmpLwrAnalysisEditController::create() {
    param = &cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpLwrAnalysisParam();
    ui = new cdsem::UiAmpLwrAnalysisEditController;
    ui->init(this);
    ui->commonSettingController->setShapeVisible(false);
    ui->commonSettingController->setFlyerVisible(false);
}

void AmpLwrAnalysisEditController::createAction() {
    CDSemNumberValidatorHelper::range(50, 20000).put(ui->transisorTextField).autoFixMode();
    CDSemNumberValidatorHelper::range<1>(0, 20000).put(ui->designValueTextField).autoFixMode();
    CDSemNumberValidatorHelper::range(1, 29).put(ui->smoothintTextField).autoFixMode();
    CDSemNumberValidatorHelper::range(3, 19).put(ui->diffTextField).autoFixMode();
    CDSemNumberValidatorHelper::range(10, 300).put(ui->searchAreaTextField).autoFixMode();
    CDSemNumberValidatorHelper::range(1, 64).put(ui->sumLineOrPointTextField).autoFixMode();
    CDSemNumberValidatorHelper::range(8, 400).put(ui->measurePointTextField).autoFixMode();
    ui->commonSettingController->initParam(&param->commonMethodParam);
}

void AmpLwrAnalysisEditController::initRealTimeEdit() {
    connect(ui->objectCBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int){
        if (!param) return;
        int val = ui->objectCBox->currentValue();
        if (val < 0 || val == param->object) return;
        param->object = val;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->object);
        if (cdsem::CDSEM_GlobalDataHelper::get()->updateTmpAmpMObject()) emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(this, &cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpMeasureObject());
    });
    connect(ui->dataCBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int){
        if (!param) return;
        int val = ui->dataCBox->currentValue();
        if (val < 0 || val == param->data) return;
        param->data = val;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->data);
    });
    connect(ui->measurePointTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->measurePointTextField,
                                                VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->measurePoint) return;
        param->measurePoint = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->measurePoint);
    });
    connect(ui->sumLineOrPointTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->sumLineOrPointTextField,
                                       VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->sumLines) return;
        param->sumLines = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->sumLines);
    });
    connect(ui->transisorTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->transisorTextField,
                                       VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->transistorWidth) return;
        param->transistorWidth = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->transistorWidth);
    });
    connect(ui->methodCBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int){
        if (!param) return;
        int val = ui->methodCBox->currentValue();
        if (val < 0 || val == param->method) return;
        param->method = val;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->method);
    });
    connect(ui->designValueTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        FloatNumber1 number = fromTextFieldText(ui->designValueTextField,
                                       VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD);
        if (number == nullptr || number == param->designValue) return;
        param->designValue = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->designValue);
    });
    connect(ui->searchAreaTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->searchAreaTextField,
                                       VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->searchArea) return;
        param->searchArea = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->searchArea);
    });
    connect(ui->smoothintTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->smoothintTextField,
                                       VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->smoothing || (number & 1) == 0) return;
        param->smoothing = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->smoothing);
    });
    connect(ui->inspectAreaTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->inspectAreaTextField,
                                       VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->inspectArea) return;
        param->inspectArea = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->inspectArea);
    });
    connect(ui->diffTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->diffTextField,
                                       VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->diff || (number & 1) == 0) return;
        param->diff = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->diff);
    });
    connect(ui->commonSettingController, &DetectionCommonSettingController::dataChanged, cdsem::CDSEM_GlobalDataHelper::get(), &cdsem::CDSEM_GlobalData::tmpAmpChanged);
}

void AmpLwrAnalysisEditController::initAsyncCaller(AsyncCaller* caller) {
    if (this->caller) {
        return;
    }
    this->caller = caller;
    caller->manage(ui->commonSettingController, this);
    ui->commonSettingController->initAsyncCaller(caller);
    ui->commonSettingController->setAfterEdgeSearchLoaded([](QSet<int>& invalids) {
        invalids.insert(2);
        invalids.insert(3);
    });
    ui->commonSettingController->refreshEdgeSearchData();
}

void AmpLwrAnalysisEditController::resetToComponent() {
    if (!param) {
        return;
    }
    ui->objectCBox->setCurrentData(param->object);
    ui->dataCBox->setCurrentData(param->data);
    ui->measurePointTextField->setText(param->measurePoint);
    ui->sumLineOrPointTextField->setText(param->sumLines);
    ui->transisorTextField->setText(param->transistorWidth);
    ui->methodCBox->setCurrentData(param->method);
    ui->designValueTextField->setText(param->designValue.toQStringAndKeepFullDecimal());
    ui->searchAreaTextField->setText(param->searchArea);
    ui->smoothintTextField->setText(param->smoothing);
    ui->inspectAreaTextField->setText(param->inspectArea);
    ui->diffTextField->setText(param->diff);
    ui->commonSettingController->resetToComponent();
}
QString AmpLwrAnalysisEditController::collectData(AmpLwrAnalysisParam& param) {
    param.object = ui->objectCBox->currentValue();
    if (param.object < 0) {
        return "Object is not specified.";
    }
    param.data = ui->dataCBox->currentValue();
    if (param.data < 0) {
        return "Data is not specified.";
    }
    param.measurePoint
        = fromTextFieldText(ui->measurePointTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
//    if (param.measurePoint < 32 && param.measurePoint != 8 && param.measurePoint != 16) {
//        return "Measurement point incorrent.";
//    }
    param.sumLines = fromTextFieldText(ui->sumLineOrPointTextField,
                                       VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD)
                         .toInt();
    param.transistorWidth
        = fromTextFieldText(ui->transisorTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
    param.method = ui->methodCBox->currentValue();
    if (param.method < 0) {
        return "Method is not specified.";
    }
    param.designValue = fromTextFieldText(ui->designValueTextField,
                                          VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD);
    param.searchArea
        = fromTextFieldText(ui->searchAreaTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
    param.smoothing
        = fromTextFieldText(ui->smoothintTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
    if (!(param.smoothing & 1)) {
        return "Smoothing must be an odd number.";
    }
    param.inspectArea
        = fromTextFieldText(ui->inspectAreaTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
    param.diff = fromTextFieldText(ui->diffTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
    if (!(param.diff & 1)) {
        return "Differential must be an odd number.";
    }
    return ui->commonSettingController->collectData(param.commonMethodParam);
}
