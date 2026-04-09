#include "amp_gap_edit_controller.h"
#include "cbox_global_keys.h"
#include "cdsem_number_validator_helper.h"
#include "ui_amp_gap_edit_controller.h"
#include "cdsem_global_data.h"

inline bool checkMeasurePoint(int p) {
    return cdsem::checkDiameterMeasurePoint(p);
}

AmpGapEditController::AmpGapEditController(QWidget* parent)
    : QWidget(parent) {
    create();
    createAction();
    initRealTimeEdit();
}

AmpGapEditController::~AmpGapEditController() noexcept {
    if (ui) {
        delete ui;
    }
}

void AmpGapEditController::create() {
    param = &cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpGapParam();
    ui = new cdsem::UiAmpGapEditController;
    ui->init(this);
    ui->commonSettingController->setFlyerVisible(false);
    ui->commonSettingController->setTitle("1", "2");
}

void AmpGapEditController::createAction() {
    CDSemNumberValidatorHelper::range(8, 32).put(ui->measurePointTextField).autoFixMode();
    CDSemNumberValidatorHelper::range(1, 64).put(ui->sumLineOrPointTextField).autoFixMode();
    CDSemNumberValidatorHelper::range(1, 64)
        .put(ui->acdTargetTextField)
        .put(ui->acdLimitTextField)
        .autoFixMode();
    CDSemNumberValidatorHelper::range<1>(0, 200000).put(ui->designValueTextField).autoFixMode();
    CDSemNumberValidatorHelper::range<1>(-200000, 200000).put(ui->positionOffsetTextField).autoFixMode();
    CDSemNumberValidatorHelper::range(10, 300).put(ui->searchAreaTextField).autoFixMode();
    CDSemNumberValidatorHelper::range(1, 29).put(ui->smoothintTextField).autoFixMode();
    CDSemNumberValidatorHelper::range(10, 400).put(ui->inspectAreaTextField).autoFixMode();
    CDSemNumberValidatorHelper::range(3, 19).put(ui->diffTextField).autoFixMode();
    connect(ui->kindCBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int) {
        ui->commonSettingController->refreshShapeData();
    });
    ui->commonSettingController->initParam(&param->commonMethodParam);
}

void AmpGapEditController::initRealTimeEdit() {

    connect(ui->kindCBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int){
        if (!param) return;
        int val = ui->kindCBox->currentValue();
        if (val < 0 || val == param->kind) return;
        param->kind = val;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->kind);
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
        int number = fromTextFieldText(ui->measurePointTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->measurePoint || !checkMeasurePoint(number)) return;
        param->measurePoint = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->measurePoint);
    });
    connect(ui->directCBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int){
        if (!param) return;
        int val = ui->directCBox->currentValue();
        if (val < 0 || val == param->direction) return;
        param->direction = val;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->direction);
    });
    connect(ui->sumLineOrPointTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->sumLineOrPointTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->sumLines) return;
        param->sumLines = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->sumLines);
    });
    connect(ui->acdCBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int){
        if (!param) return;
        int val = ui->acdCBox->currentValue();
        if (val < 0 || val == param->acdParam.acdType) return;
        param->acdParam.acdType = val;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->acdParam.acdType);
    });
    connect(ui->acdTargetTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->acdTargetTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->acdParam.target) return;
        param->acdParam.target = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->acdParam.target);
    });
    connect(ui->acdLimitTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->acdLimitTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->acdParam.limit) return;
        param->acdParam.limit = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->acdParam.limit);
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
        FloatNumber1 number = fromTextFieldText(ui->designValueTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD);
        if (number == nullptr || number == param->designValue) return;
        param->designValue = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->designValue);
    });
    connect(ui->positionOffsetTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        FloatNumber1 number = fromTextFieldText(ui->positionOffsetTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD);
        if (number == nullptr || number == param->positionOffset) return;
        param->positionOffset = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->positionOffset);
    });
    connect(ui->searchAreaTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->searchAreaTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->searchArea) return;
        param->searchArea = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->searchArea);
    });
    connect(ui->smoothintTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->smoothintTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->smoothing || (number & 1) == 0) return;
        param->smoothing = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->smoothing);
    });
    connect(ui->inspectAreaTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->inspectAreaTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->inspectArea) return;
        param->inspectArea = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->inspectArea);
    });
    connect(ui->diffTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->diffTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->diff || (number & 1) == 0) return;
        param->diff = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->diff);
    });

    connect(ui->commonSettingController, &DetectionCommonSettingController::dataChanged, cdsem::CDSEM_GlobalDataHelper::get(), &cdsem::CDSEM_GlobalData::tmpAmpChanged);
}

void AmpGapEditController::refreshAcdType() {
    ui->acdCBox->loadItemFromDatabase(CBOX_AMP_ACD_TYPE, true);
}

void AmpGapEditController::initAsyncCaller(AsyncCaller* caller) {
    if (this->caller) {
        return;
    }
    this->caller = caller;
    caller->manage(ui->commonSettingController, this);
    caller->manage(ui->acdCBox, this);
    ui->acdCBox->setAsyncCaller(caller);
    ui->commonSettingController->initAsyncCaller(caller);
    ui->commonSettingController->setAfterEdgeSearchLoaded([](QSet<int>& invalids) {
        invalids.insert(0);
        invalids.insert(1);
    });
    ui->commonSettingController->setAfterShapeLoaded([this](QSet<int>& invalids) {
        if (ui->kindCBox->currentValue() == 1) {
            invalids.insert(0);
        }
    });
    ui->commonSettingController->refreshEdgeSearchData();
    ui->commonSettingController->refreshShapeData();
    refreshAcdType();
}

QString AmpGapEditController::collectData(AmpGapParam& param) {
    param.kind = ui->kindCBox->currentValue();
    if (param.kind < 0) {
        return "Kind is not specified.";
    }
    param.data = ui->dataCBox->currentValue();
    if (param.data < 0) {
        return "Data is not specified.";
    }
    param.measurePoint
        = fromTextFieldText(ui->measurePointTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
    if (!checkMeasurePoint(param.measurePoint)) {
        return "Invalid measurement point. Please ensure the value is either 8, 16, or 32.";
    }
    param.direction = ui->directCBox->currentValue();
    if (param.direction < 0) {
        return "Direction is not specified.";
    }
    param.sumLines = fromTextFieldText(ui->sumLineOrPointTextField,
                                       VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD)
                         .toInt();
    param.acdParam.acdType = ui->acdCBox->currentValue();
    if (param.acdParam.acdType < 0) {
        return "ACD is not specified.";
    }
    param.acdParam.target
        = fromTextFieldText(ui->acdTargetTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
    param.acdParam.limit
        = fromTextFieldText(ui->acdTargetTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
    param.method = ui->methodCBox->currentValue();
    if (param.method < 0) {
        return "Detection method is not specified.";
    }
    param.designValue = fromTextFieldText(ui->acdTargetTextField,
                                          VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD);
    param.positionOffset = fromTextFieldText(ui->positionOffsetTextField,
                                             VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD);
    param.searchArea
        = fromTextFieldText(ui->searchAreaTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
    param.smoothing
        = fromTextFieldText(ui->smoothintTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
    if (!(param.smoothing & 1)) {
        return "Detection smoothing must be an odd number.";
    }
    param.inspectArea
        = fromTextFieldText(ui->inspectAreaTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
    param.diff = fromTextFieldText(ui->diffTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
    if (!(param.diff & 1)) {
        return "Detection differential must be an odd number.";
    }
    return ui->commonSettingController->collectData(param.commonMethodParam);
}
void AmpGapEditController::resetToComponent() {
    if (!param) {
        return;
    }
    ui->kindCBox->setCurrentData(param->kind);
    ui->dataCBox->setCurrentData(param->data);
    ui->measurePointTextField->setText(param->measurePoint);
    ui->directCBox->setCurrentData(param->direction);
    ui->sumLineOrPointTextField->setText(param->sumLines);
    ui->acdCBox->safeSetCurrentData(param->acdParam.acdType);
    ui->acdTargetTextField->setText(param->acdParam.target);
    ui->acdLimitTextField->setText(param->acdParam.limit);
    ui->methodCBox->setCurrentData(param->method);
    ui->designValueTextField->setText(param->designValue.toQStringAndKeepFullDecimal());
    ui->positionOffsetTextField->setText(param->positionOffset.toQStringAndKeepFullDecimal());
    ui->searchAreaTextField->setText(param->searchArea);
    ui->smoothintTextField->setText(param->smoothing);
    ui->inspectAreaTextField->setText(param->inspectArea);
    ui->diffTextField->setText(param->diff);
    ui->commonSettingController->resetToComponent();
}
