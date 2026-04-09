#include "amp_diameter_hole_edit_controller.h"
#include "cbox_global_keys.h"
#include "cdsem_number_validator_helper.h"
#include "ui_amp_diameter_hole_edit_controller.h"
#include "cdsem_global_data.h"

inline bool checkMeasurePoint(int p) {
    return cdsem::checkDiameterMeasurePoint(p);
}


AmpDiameterHoleEditController::AmpDiameterHoleEditController(QWidget* parent)
    : QWidget(parent) {
    create();
    createAction();
    initRealTimeEdit();
}

AmpDiameterHoleEditController::~AmpDiameterHoleEditController() noexcept {
    disconnect(cdsem::CDSEM_GlobalDataHelper::get(), &cdsem::CDSEM_GlobalData::tmpAmpChanged, this, nullptr);
    if (ui) {
        delete ui;
    }
}

void AmpDiameterHoleEditController::create() {
    param = &cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpDiameterHoleParam();
    ui = new cdsem::UiAmpDiameterHoleEditController;
    ui->init(this);
    ui->commonSettingController->setShapeVisible(false);
    ui->commonSettingController->setFlyerVisible(false);
}

void AmpDiameterHoleEditController::createAction() {
    CDSemNumberValidatorHelper::range(1, 64)
        .put(ui->acdLimitTextField)
        .put(ui->acdTargetTextField)
        .autoFixMode();
    CDSemNumberValidatorHelper::range(8, 32).put(ui->measurePointTextField).autoFixMode();
    CDSemNumberValidatorHelper::range(1, 64).put(ui->sumLineOrPointTextField).autoFixMode();
    CDSemNumberValidatorHelper::range(4, 24).put(ui->diametersTextField).autoFixMode();
    CDSemNumberValidatorHelper::range<1>(1, 200000).put(ui->designValueTextFiled).autoFixMode();
    CDSemNumberValidatorHelper::range(10, 150).put(ui->searchAreaTextField).autoFixMode();
    CDSemNumberValidatorHelper::range(1, 29).put(ui->smoothingTextField).autoFixMode();
    CDSemNumberValidatorHelper::range(1, 400).put(ui->lineInspectAreaTextField).autoFixMode();
    CDSemNumberValidatorHelper::range(2, 32).put(ui->radialInspectAreaTextField).autoFixMode();
    CDSemNumberValidatorHelper::range(3, 19).put(ui->diffTextField).autoFixMode();
    connect(ui->kindCBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int) {
        int val = ui->kindCBox->currentValue();
        if (val == 0 || val == 2) {
            ui->measurePointContent->hide();
            ui->measurePointTextField->hide();
            ui->sumLineOrPointContent->hide();
            ui->sumLineOrPointTextField->hide();
        }
        if (val == 0 || val == 1) {
            ui->lineInspectAreaTextField->show();
            ui->radialInspectAreaTextField->hide();
        } else {
            ui->lineInspectAreaTextField->hide();
            ui->radialInspectAreaTextField->show();
        }
        if (val == 0) {
            ui->dataLabelContent->hide();
            ui->inspectAreaLabel->hide();
            ui->dataCBox->hide();
            ui->diametersLabelContent->hide();
            ui->diametersTextField->hide();
            ui->radialInspectAreaLabel->hide();
            ui->preProcessContent->show();
            ui->directionLabel->show();
            ui->directionCBox->show();
            ui->sumLineLabel->show();
            ui->lineInspectAreaLabel->show();
        } else if (val == 1) {
            ui->dataCBox->hide();
            ui->sumLineLabel->hide();
            ui->dataLabelContent->hide();
            ui->diametersLabelContent->hide();
            ui->diametersTextField->hide();
            ui->radialInspectAreaLabel->hide();
            ui->inspectAreaLabel->show();
            ui->lineInspectAreaLabel->show();
            ui->measurePointContent->show();
            ui->measurePointTextField->show();
            ui->sumLineOrPointContent->show();
            ui->sumLineOrPointTextField->show();
            ui->directionLabel->show();
            ui->directionCBox->show();
        } else if (val == 2) {
            ui->lineInspectAreaLabel->hide();
            ui->directionLabel->hide();
            ui->directionCBox->hide();
            ui->sumLineLabel->hide();
            ui->radialInspectAreaLabel->show();
            ui->dataCBox->show();
            ui->dataLabelContent->show();
            ui->preProcessContent->show();
            ui->diametersLabelContent->show();
            ui->diametersTextField->show();
            ui->inspectAreaLabel->show();
        }
    });
    connect(ui->acdTypeCBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int){
        refreshMeasurePos();
    });
    ui->commonSettingController->initParam(&param->commonMethodParam);

    connect(cdsem::CDSEM_GlobalDataHelper::get(), &cdsem::CDSEM_GlobalData::tmpAmpChanged, this, [this](QPointer<QObject> obj, void* pPtr){
        if (obj == this || obj == ui->commonSettingController) return;
        cdsem::MeasureType mType = cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpMeasureType();
        if (mType == cdsem::MT_DIAMETER_HOLE) {
            if (pPtr == &param->searchArea) {
                ui->searchAreaTextField->getQLineEdit()->blockSignals(true);
                ui->searchAreaTextField->setText(param->searchArea);
                ui->searchAreaTextField->getQLineEdit()->blockSignals(false);
                return;
            }
            if (pPtr == &param->sumLineOrInspectArea && (param->kind == 0 || param->kind == 1)) {
                ui->lineInspectAreaTextField->getQLineEdit()->blockSignals(true);
                ui->lineInspectAreaTextField->setText(param->sumLineOrInspectArea);
                ui->lineInspectAreaTextField->getQLineEdit()->blockSignals(false);
                return;
            }
            if (pPtr == &param->designValue) {
                ui->designValueTextFiled->getQLineEdit()->blockSignals(true);
                ui->designValueTextFiled->setText(param->designValue.toQStringAndKeepFullDecimal());
                ui->designValueTextFiled->getQLineEdit()->blockSignals(false);
                return;
            }
        }
    });

    emit ui->kindCBox->getQComboBox()->currentIndexChanged(0);
}

void AmpDiameterHoleEditController::initRealTimeEdit() {
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
    connect(ui->kindCBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int){
        if (!param) return;
        int val = ui->kindCBox->currentValue();
        if (val < 0 || val == param->kind) return;
        param->kind = val;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->kind);
        if (val == 0 || val == 1) {
            val = fromTextFieldText(ui->lineInspectAreaTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
            if (param->sumLineOrInspectArea != val) {
                param->sumLineOrInspectArea = val;
                emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->sumLineOrInspectArea);
            }
        } else if (val == 2) {
            val = fromTextFieldText(ui->radialInspectAreaTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
            if (param->radiusInspectArea != val) {
                param->radiusInspectArea = val;
                emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->radiusInspectArea);
            }
        }
    });
    connect(ui->directionCBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int){
        if (!param) return;
        int val = ui->directionCBox->currentValue();
        if (val < 0 || val == param->direction) return;
        param->direction = val;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->direction);
    });
    connect(ui->centerCBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int){
        if (!param) return;
        int val = ui->centerCBox->currentValue();
        if (val < 0 || val == param->center) return;
        param->center = val;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->center);
    });
    connect(ui->preProcessCBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int){
        if (!param) return;
        int val = ui->preProcessCBox->currentValue();
        if (val < 0 || val == param->preProcess) return;
        param->preProcess = val;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->preProcess);
    });
    connect(ui->designValueTextFiled->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        FloatNumber1 number = fromTextFieldText(ui->designValueTextFiled,
                                                VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD);
        if (number == nullptr || number == param->designValue) return;
        param->designValue = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->designValue);
    });
    connect(ui->methodCBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int){
        if (!param) return;
        int val = ui->methodCBox->currentValue();
        if (val < 0 || val == param->method) return;
        param->method = val;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->method);
    });
    connect(ui->searchAreaTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->searchAreaTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->searchArea) return;
        param->searchArea = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->searchArea);
    });
    connect(ui->smoothingTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->smoothingTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->smoothing || (number & 1) == 0) return;
        param->smoothing = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->smoothing);
    });
    connect(ui->lineInspectAreaTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->lineInspectAreaTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->sumLineOrInspectArea) return;
        param->sumLineOrInspectArea = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->sumLineOrInspectArea);
    });
    connect(ui->radialInspectAreaTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->radialInspectAreaTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->radiusInspectArea) return;
        param->radiusInspectArea = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->radiusInspectArea);
    });
    connect(ui->diffTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->diffTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->diff || (number & 1) == 0) return;
        param->diff = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->diff);
    });
    connect(ui->measurePositionCBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int){
        if (!param) return;
        int val = ui->measurePositionCBox->currentValue();
        if (val < 0 || val == param->measurePosition) return;
        param->measurePosition = val;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->measurePosition);
    });
    connect(ui->measurePointTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->measurePointTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->measurePoint || !checkMeasurePoint(number)) return;
        param->measurePoint = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->measurePoint);
    });
    connect(ui->sumLineOrPointTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->sumLineOrPointTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->sumLinesPerPoint) return;
        param->sumLinesPerPoint = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->sumLinesPerPoint);
    });
    connect(ui->diametersTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->diametersTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->diameters) return;
        param->diameters = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->diameters);
    });
    connect(ui->acdCBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int){
        if (!param) return;
        int val = ui->acdCBox->currentValue();
        if (val < 0 || val == param->acdParam.acd) return;
        param->acdParam.acd = val;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->acdParam.acd);
    });
    connect(ui->acdTypeCBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int){
        if (!param) return;
        int val = ui->acdTypeCBox->currentValue();
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
    connect(ui->commonSettingController, &DetectionCommonSettingController::dataChanged, cdsem::CDSEM_GlobalDataHelper::get(), &cdsem::CDSEM_GlobalData::tmpAmpChanged);
}

void AmpDiameterHoleEditController::refreshMeasurePos() {
    int old = ui->measurePositionCBox->currentValue();
    if (old == -1) {
        old = oldMeasurePoint;
    } else {
        oldMeasurePoint = old;
    }
    ui->measurePositionCBox
        ->loadItemFromDatabase(CBOX_AMP_MEASURE_POINT_TYPE, true, [this, old]() {
            int val = ui->objectCBox->currentValue();
            if (cdsem::checkAmpWidthObjectIsDense(val) || ui->acdTypeCBox->currentValue() == cdsem::ACDT_MANUAL) {
                ui->measurePositionCBox->removeItemByData(1);
            }
            bool dataChanged = false;
            if (old != -1) {
                dataChanged = !ui->measurePositionCBox->setCurrentData(old);
            } else if (param->measurePosition != -1) {
                ui->measurePositionCBox->setCurrentData(param->measurePosition);
                dataChanged = true;
            }
            if (dataChanged) {
                MainThreadInvoker::getInvoker().runLater([this](){
                    if (ui->measurePositionCBox->currentIndex() != -1)
                        emit ui->measurePositionCBox->getQComboBox()->currentIndexChanged(ui->measurePositionCBox->currentIndex());
                }, true);
            }
        });
}

void AmpDiameterHoleEditController::refreshAcdType() {
    int old = ui->acdTypeCBox->currentValue();
    if (old == -1) {
        old = oldAcdType;
    } else {
        oldAcdType = old;
    }
    ui->acdTypeCBox->loadItemFromDatabase(CBOX_AMP_ACD_TYPE, true, [this, old]() {
        int val = ui->objectCBox->currentValue();
        if (cdsem::checkAmpWidthObjectIsDense(val)) {
            ui->acdTypeCBox->removeItemByData(0);
        }
        bool dataChanged = false;
        if (old != -1) {
            dataChanged = !ui->acdTypeCBox->setCurrentData(old);
        } else if (param->acdParam.acdType != -1) {
            ui->acdTypeCBox->setCurrentData(param->acdParam.acdType);
            dataChanged = true;
        }
        if (dataChanged) {
            MainThreadInvoker::getInvoker().runLater([this](){
                if (ui->acdTypeCBox->currentIndex() != -1)
                    emit ui->acdTypeCBox->getQComboBox()->currentIndexChanged(ui->acdTypeCBox->currentIndex());
            }, true);
        }
    });
}

void AmpDiameterHoleEditController::initAsyncCaller(AsyncCaller* caller) {
    if (this->caller) {
        return;
    }
    this->caller = caller;
    caller->manage(ui->commonSettingController, this);
    caller->manage(ui->measurePositionCBox, this);
    caller->manage(ui->acdTypeCBox, this);
    ui->measurePositionCBox->setAsyncCaller(caller);
    ui->acdTypeCBox->setAsyncCaller(caller);
    ui->commonSettingController->initAsyncCaller(caller);
    ui->commonSettingController->setAfterEdgeSearchLoaded([](QSet<int>& invalids) {
        invalids.insert(2);
        invalids.insert(3);
    });
    ui->commonSettingController->refreshEdgeSearchData();
    refreshAcdType();
    refreshMeasurePos();
}

QString AmpDiameterHoleEditController::collectData(AmpDiameterHoleParam& param) {
    param.object = ui->objectCBox->currentValue();
    if (param.object < 0) {
        return "Object is not specified.";
    }
    param.kind = ui->kindCBox->currentValue();
    if (param.kind < 0) {
        return "Kind is not specified.";
    }
    if (param.kind == 0 || param.kind == 1) {
        param.direction = ui->directionCBox->currentValue();
        if (param.direction < 0) {
            return "Direction is not specified.";
        }
        if (param.kind == 1) {
            param.measurePoint = fromTextFieldText(ui->measurePointTextField,
                                                   VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD)
                                     .toInt();
            if (!checkMeasurePoint(param.measurePoint)) {
                return "Invalid measurement point. Please ensure the value is either 8, 16, or 32.";
            }
            param.sumLinesPerPoint = fromTextFieldText(ui->sumLineOrPointTextField,
                                               VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD)
                                 .toInt();
        }
        param.sumLineOrInspectArea
            = fromTextFieldText(ui->lineInspectAreaTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
    } else if (param.kind == 2) {
        param.data = ui->dataCBox->currentValue();
        if (param.data < 0) {
            return "Data is not specified.";
        }
        param.diameters = fromTextFieldText(ui->diametersTextField,
                                            VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD)
                              .toInt();
        param.radiusInspectArea
            = fromTextFieldText(ui->radialInspectAreaTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
    }
    param.center = ui->centerCBox->currentValue();
    if (param.center < 0) {
        return "Centering is not specified.";
    }
    param.preProcess = ui->preProcessCBox->currentValue();
    if (param.preProcess < 0) {
        return "Pre process is not specified.";
    }
    param.designValue = fromTextFieldText(ui->designValueTextFiled,
                                          VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD);
    param.method = ui->methodCBox->currentValue();
    if (param.method < 0) {
        return "Detection method is not specified.";
    }
    param.smoothing
        = fromTextFieldText(ui->smoothingTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
    if (!(param.smoothing & 1)) {
        return "Smoothing must be an odd number.";
    }
    param.searchArea
        = fromTextFieldText(ui->searchAreaTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();

    param.diff = fromTextFieldText(ui->diffTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
    if (!(param.diff & 1)) {
        return "Differential must be an odd number.";
    }
    param.measurePosition = ui->measurePositionCBox->currentValue();
    if (param.measurePosition < 0) {
        return "Measurement position is not specified.";
    }
    param.acdParam.acd = ui->acdCBox->currentValue();
    if (param.acdParam.acd < 0) {
        return "ACD is not specified.";
    }
    param.acdParam.acdType = ui->acdTypeCBox->currentValue();
    if (param.acdParam.acd < 0) {
        return "ACD Type is not specified.";
    }
    param.acdParam.target = fromTextFieldText(ui->acdTargetTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
    param.acdParam.limit = fromTextFieldText(ui->acdLimitTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
    return ui->commonSettingController->collectData(param.commonMethodParam);
}
void AmpDiameterHoleEditController::resetToComponent() {
    if (!param) {
        return;
    }
    ui->dataCBox->setCurrentData(param->object);
    ui->kindCBox->setCurrentData(param->kind);
    if (param->kind == 0 || param->kind == 1) {
        ui->directionCBox->setCurrentData(param->direction);
        if (param->kind == 1) {
            ui->measurePointTextField->setText(param->measurePoint);
            ui->sumLineOrPointTextField->setText(param->sumLinesPerPoint);
        }
    } else if (param->kind == 2) {
        ui->dataCBox->setCurrentData(param->data);
        ui->diametersTextField->setText(param->diameters);
    }
    ui->centerCBox->setCurrentData(param->center);
    ui->preProcessCBox->setCurrentData(param->preProcess);
    ui->designValueTextFiled->setText(param->designValue.toQStringAndKeepFullDecimal());
    ui->methodCBox->setCurrentData(param->method);
    ui->searchAreaTextField->setText(param->searchArea);
    ui->smoothingTextField->setText(param->smoothing);
    if (param->kind == 2) {
        ui->radialInspectAreaTextField->setText(param->radiusInspectArea);
    } else {
        ui->lineInspectAreaTextField->setText(param->sumLineOrInspectArea);
    }
    ui->diffTextField->setText(param->diff);
    ui->measurePositionCBox->safeSetCurrentData(param->measurePosition);
    ui->acdTypeCBox->setCurrentData(param->acdParam.acdType);
    ui->acdCBox->setCurrentData(param->acdParam.acd);
    ui->acdLimitTextField->setText(param->acdParam.limit);
    ui->acdTargetTextField->setText(param->acdParam.target);
    ui->commonSettingController->resetToComponent();
}
