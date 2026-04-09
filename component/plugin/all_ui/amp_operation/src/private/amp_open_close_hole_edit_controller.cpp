#include "amp_open_close_hole_edit_controller.h"
#include "cdsem_number_validator_helper.h"
#include "ui_amp_open_close_hole_edit_controller.h"
#include "cdsem_global_data.h"
#include <QPointer>

AmpOpenCloseHoleEditController::AmpOpenCloseHoleEditController(QWidget* parent)
    : QWidget(parent) {
    create();
    createAction();
    initRealTimeEdit();
}

AmpOpenCloseHoleEditController::~AmpOpenCloseHoleEditController() noexcept {
    if (ui) {
        delete ui;
    }
}

void AmpOpenCloseHoleEditController::create() {
    param = &cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpOpenCloseParam();
    ui = new cdsem::UiAmpOpenCloseHoleEditController;
    ui->init(this);
}

void AmpOpenCloseHoleEditController::createAction() {
    CDSemNumberValidatorHelper::range<2>(0, 1)
        .put(ui->upperTextField)
        .put(ui->lowerTextField)
        .autoFixMode();
    CDSemNumberValidatorHelper::range(20, 80).put(ui->cutLevelTextField).autoFixMode();
}

void AmpOpenCloseHoleEditController::initRealTimeEdit() {
    connect(ui->dataCBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int){
        if (!param) return;
        int val = ui->dataCBox->currentValue();
        if (val < 0 || val == param->data) return;
        param->data = val;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->data);
    });
    connect(ui->lowerTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        FloatNumber1 number = fromTextFieldText(ui->lowerTextField,
                                       VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD);
        if (number == nullptr || number == param->openLowThreshold) return;
        param->openLowThreshold = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->openLowThreshold);
    });
    connect(ui->upperTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        FloatNumber1 number = fromTextFieldText(ui->upperTextField,
                                                VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD);
        if (number == nullptr || number == param->openUpThreshold) return;
        param->openUpThreshold = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->openUpThreshold);
    });
    connect(ui->cutLevelTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->cutLevelTextField,
                                                VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->cutLevel) return;
        param->cutLevel = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->cutLevel);
    });
    connect(ui->magTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->magTextField,
                                       VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->mag) return;
        param->mag = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->mag);
    });
}

void AmpOpenCloseHoleEditController::resetToComponent() {
    if (!param) {
        return;
    }
    ui->dataCBox->setCurrentData(param->data);
    ui->lowerTextField->setText(param->openLowThreshold.toQStringAndKeepFullDecimal());
    ui->upperTextField->setText(param->openUpThreshold.toQStringAndKeepFullDecimal());
    ui->cutLevelTextField->setText(param->cutLevel);
    //TODO 待图像注册时初始化
}
QString AmpOpenCloseHoleEditController::collectData(AmpOpenCloseHoleParam& param) {
    param.data = ui->dataCBox->currentValue();
    if (param.data < 0) {
        return "Data is not specified.";
    }
    param.openLowThreshold = fromTextFieldText(ui->lowerTextField,
                                               VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD);
    param.openUpThreshold = fromTextFieldText(ui->upperTextField,
                                              VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD);
    param.cutLevel = fromTextFieldText(ui->cutLevelTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD)
                         .toInt();
    //TODO 待图像注册时初始化
    param.mag = fromTextFieldText(ui->magTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
    return "";
}
