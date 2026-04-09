#include "amp_mppc_edit_controller.h"
#include "cbox_global_keys.h"
#include "cdsem_number_validator_helper.h"
#include "ui_amp_mppc_edit_controller.h"
#include "cdsem_global_data.h"

inline bool checkMeasurePoint(int p) {
    return cdsem::checkDiameterMeasurePoint(p);
}

AmpMppcEditController::AmpMppcEditController(QWidget* parent)
    : QWidget(parent) {
    create();
    createAction();
    initRealTimeEdit();
}

AmpMppcEditController::~AmpMppcEditController() noexcept {
    if (ui) {
        delete ui;
    }
}

void AmpMppcEditController::create() {
    param = &cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpMppcParam();
    ui = new cdsem::UiAmpMppcEditController;
    ui->init(this);
}

void AmpMppcEditController::createAction() {
    CDSemNumberValidatorHelper::range(1, 200).put(ui->sumLineTextField).autoFixMode();
    CDSemNumberValidatorHelper::range<1>("1", "200000").put(ui->designValueTextField).autoFixMode();
    CDSemNumberValidatorHelper::range(10, 150).put(ui->searchAreaTextField).autoFixMode();
    CDSemNumberValidatorHelper::range(1, 400).put(ui->inspectAreaTextField).autoFixMode();
    CDSemNumberValidatorHelper::range(1, 29).put(ui->smoothingTextField).autoFixMode();
    CDSemNumberValidatorHelper::range(8, 32).put(ui->measurePointTextField).autoFixMode();
    CDSemNumberValidatorHelper::range(0, 100)
        .put(ui->upperInnerTextField)
        .put(ui->upperOuterTextField)
        .put(ui->lowerInnerTextField)
        .put(ui->lowerOuterTextField)
        .put(ui->basePointTextField)
        .autoFixMode();
    CDSemNumberValidatorHelper::range(40, 90).put(ui->bbcTextField).autoFixMode();
    connect(ui->kindCBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int) {
        refreshData();
    });
}
void AmpMppcEditController::initRealTimeEdit() {
    connect(ui->objectCBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int){
        if (!param) return;
        int val = ui->objectCBox->currentValue();
        if (val < 0 || val == param->object) return;
        param->object = val;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->object);
        if (cdsem::CDSEM_GlobalDataHelper::get()->updateTmpAmpMObject()) emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(this, &cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpMeasureObject());
    });
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
        int number = fromTextFieldText(ui->measurePointTextField,
                                       VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->measurePoint || !checkMeasurePoint(number)) return;
        param->measurePoint = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->measurePoint);
    });
    connect(ui->sumLineTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->sumLineTextField,
                                       VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->sumLines) return;
        param->sumLines = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->sumLines);
    });
    connect(ui->cursorCBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int){
        if (!param) return;
        int val = ui->cursorCBox->currentValue();
        if (val < 0 || val == param->cursorType) return;
        param->cursorType = val;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->cursorType);
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
    connect(ui->smoothingTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->smoothingTextField,
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
    connect(ui->lowerInnerTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->lowerInnerTextField,
                                       VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->tInLow) return;
        param->tInLow = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->tInLow);
    });
    connect(ui->upperInnerTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->upperInnerTextField,
                                       VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->tInUp) return;
        param->tInUp = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->tInUp);
    });
    connect(ui->lowerOuterTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->lowerOuterTextField,
                                       VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->tOutLow) return;
        param->tOutLow = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->tOutLow);
    });
    connect(ui->upperOuterTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->upperOuterTextField,
                                       VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->tOutUp) return;
        param->tOutUp = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->tOutUp);
    });
    connect(ui->basePointTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->basePointTextField,
                                       VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->basePoint) return;
        param->basePoint = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->basePoint);
    });
    connect(ui->bbcTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->bbcTextField,
                                       VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->brightBlanceCheck) return;
        param->brightBlanceCheck = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->brightBlanceCheck);
    });
    connect(ui->measurePositionCBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int){
        if (!param) return;
        int val = ui->measurePositionCBox->currentValue();
        if (val < 0 || val == param->measurePosition) return;
        param->measurePosition = val;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->measurePosition);
    });
}

void AmpMppcEditController::refreshData() {
    int val = ui->kindCBox->currentValue();
    if (val < 0) {
        return;
    }
    int curr = ui->dataCBox->currentValue();
    if (curr < 0) {
        curr = oldData;
    } else {
        oldData = curr;
    }
    ui->dataCBox->loadItemFromDatabase(CBOX_AMP_MPPC_DATA,
                                       true,
                                       [this, val, curr]() {
                                           QSet<int> vals;
                                           if (val == 0) {
                                               vals.insert(9);
                                               vals.insert(10);
                                           } else if (val == 1) {
                                               vals.insert(6);
                                               vals.insert(7);
                                               vals.insert(8);
                                           }
                                           ui->dataCBox->removeItemByData(vals);
                                           if (curr >= 0) {
                                               ui->dataCBox->setCurrentData(curr);
                                           }
                                       });
}

void AmpMppcEditController::initAsyncCaller(AsyncCaller* caller) {
    if (this->caller) {
        return;
    }
    this->caller = caller;
    caller->manage(ui->kindCBox, this);
    caller->manage(ui->dataCBox, this);
    caller->manage(ui->measurePositionCBox, this);
    ui->kindCBox->setAsyncCaller(caller);
    ui->dataCBox->setAsyncCaller(caller);
    ui->measurePositionCBox->setAsyncCaller(caller);
    ui->kindCBox->loadItemFromDatabase(CBOX_AMP_MPPC_KIND, true, [this]() {
        int val = ui->kindCBox->currentValue();
        if (val < 0) {
            return;
        }
        refreshData();
    });
    refreshMeasurePos();
}

void AmpMppcEditController::refreshMeasurePos() {
    ui->measurePositionCBox->loadItemFromDatabase(CBOX_AMP_MEASURE_POINT_TYPE, true);
}

void AmpMppcEditController::resetToComponent() {
    if (!param) {
        return;
    }
    ui->objectCBox->setCurrentData(param->object);
    ui->kindCBox->safeSetCurrentData(param->kind);
    ui->dataCBox->safeSetCurrentData(param->data);
    ui->measurePointTextField->setText(param->measurePoint);
    ui->sumLineTextField->setText(param->sumLines);
    ui->cursorCBox->setCurrentData(param->cursorType);
    ui->designValueTextField->setText(param->designValue.toQStringAndKeepFullDecimal());
    ui->searchAreaTextField->setText(param->searchArea);
    ui->smoothingTextField->setText(param->smoothing);
    ui->inspectAreaTextField->setText(param->inspectArea);
    ui->lowerInnerTextField->setText(param->tInLow);
    ui->upperInnerTextField->setText(param->tInUp);
    ui->lowerOuterTextField->setText(param->tOutLow);
    ui->upperOuterTextField->setText(param->tOutUp);
    ui->basePointTextField->setText(param->basePoint);
    ui->bbcTextField->setText(param->brightBlanceCheck);
    ui->measurePositionCBox->safeSetCurrentData(param->measurePosition);
}
QString AmpMppcEditController::collectData(AmpMppcParam& param) {
    param.object = ui->objectCBox->currentValue();
    if (param.object < 0) {
        return "Object is not specified.";
    }
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
    param.sumLines = fromTextFieldText(ui->sumLineTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD)
                         .toInt();
    param.cursorType = ui->cursorCBox->currentValue();
    if (param.cursorType < 0) {
        return "Cursor type is not specified.";
    }
    param.designValue = fromTextFieldText(ui->designValueTextField,
                                          VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD);
    param.searchArea
        = fromTextFieldText(ui->searchAreaTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
    param.smoothing
        = fromTextFieldText(ui->smoothingTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
    if (!(param.smoothing & 1)) {
        return "Detection smoothing must be an odd number.";
    }
    param.inspectArea
        = fromTextFieldText(ui->inspectAreaTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
    param.tInLow = fromTextFieldText(ui->lowerInnerTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD)
                       .toInt();
    param.tInUp = fromTextFieldText(ui->upperInnerTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD)
                      .toInt();
    param.tOutLow = fromTextFieldText(ui->lowerOuterTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD)
                        .toInt();
    param.tOutUp = fromTextFieldText(ui->upperOuterTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD)
                       .toInt();
    param.basePoint
        = fromTextFieldText(ui->basePointTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
    param.brightBlanceCheck
        = fromTextFieldText(ui->bbcTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();

    param.measurePosition = ui->measurePositionCBox->currentValue();
    if (param.measurePosition < 0) {
        return "Measurement position is not specified.";
    }
    return "";
}
