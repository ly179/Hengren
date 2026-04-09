#include "amp_width_roughness_edit_controller.h"
#include <QLayout>
#include "amp_dense_setting_edit_controller.h"
#include "cbox_global_keys.h"
#include "cdsem_number_validator_helper.h"
#include "ui_amp_width_roughness_edit_controller.h"
#include "cdsem_global_data.h"

inline bool checkMeasurePoint(int p) {
    return cdsem::checkLineMeasurePoint(p);
}

AmpWidthRoughnessEditController::AmpWidthRoughnessEditController(QWidget* parent)
    : QWidget(parent) {
    create();
    createAction();
    initRealTimeEdit();
}

AmpWidthRoughnessEditController::~AmpWidthRoughnessEditController() noexcept {
    disconnect(cdsem::CDSEM_GlobalDataHelper::get(), &cdsem::CDSEM_GlobalData::tmpAmpChanged, this, nullptr);
    if (ui) {
        delete ui;
    }
}

void AmpWidthRoughnessEditController::create() {
    this->param = &cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpWidthRoughnessParam();
    ui = new cdsem::UiAmpWidthRoughnessEditController;
    ui->init(this);
    denseSettingController = new AmpDenseSettingEditController(ui->denseObjectSettingContent);
    ui->denseObjectSettingContent->layout()->addWidget(denseSettingController);
    ui->commonSettingController->setFlyerVisible(false);
    ui->commonSettingController->setShapeVisible(false);
}

void AmpWidthRoughnessEditController::createAction() {
    CDSemNumberValidatorHelper::range<1>(1, 200000).put(ui->mDesignValueTextField).autoFixMode();
    CDSemNumberValidatorHelper::range(1, 150).put(ui->mSearchAreaTextField).autoFixMode();
    CDSemNumberValidatorHelper::range(8, 400).put(ui->mMeasurePointTextField).autoFixMode();
    CDSemNumberValidatorHelper::range(1, 400).put(ui->mInspectTextField).autoFixMode();
    CDSemNumberValidatorHelper::range(1, 64).put(ui->mSumLinePointTextField).autoFixMode();
    CDSemNumberValidatorHelper::range(1, 29).put(ui->dmSmoothingTextField).autoFixMode();
    CDSemNumberValidatorHelper::range(3, 19).put(ui->dmDiffTextField).autoFixMode();
    connect(ui->objectCBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int) {
        int val = ui->objectCBox->currentValue();
        bool showDense = cdsem::checkAmpWidthRoughnessObjectIsDense(val);
        denseSettingController->setVisible(showDense);
        refreshAcdType();
        refreshCursorBox();
        refreshMeasurePos();
        refreshData();
        refreshTargetLimitTextField(showDense, ui->acdTargetTextField, ui->acdLimitTextField);
        if (val == 7) {
            denseSettingController->setDenseSettingType(DST_LINE);
        } else if (val == 8) {
            denseSettingController->setDenseSettingType(DST_SPACE);
        }
    });
    connect(ui->acdWayCBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int){
        refreshMeasurePos();
    });
    connect(ui->dataCBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int) {
        int val = ui->dataCBox->currentValue();
        ui->bfCheckBox->setEnabled(val == 3 || val == 4);
    });
    denseSettingController->initParam(&param->denseParam);
    ui->commonSettingController->initParam(&param->commonMethodParam);

    connect(cdsem::CDSEM_GlobalDataHelper::get(), &cdsem::CDSEM_GlobalData::tmpAmpChanged, this, [this](QPointer<QObject> obj, void* pPtr){
        if (obj == this || obj == ui->commonSettingController || obj == denseSettingController) return;
        cdsem::MeasureType mType = cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpMeasureType();
        if (mType == cdsem::MT_WIDTH_ROUGHNESS) {
            if (pPtr == &param->searchArea) {
                ui->mSearchAreaTextField->getQLineEdit()->blockSignals(true);
                ui->mSearchAreaTextField->setText(param->searchArea);
                ui->mSearchAreaTextField->getQLineEdit()->blockSignals(false);
                return;
            }
            if (pPtr == &param->inspectArea) {
                ui->mInspectTextField->getQLineEdit()->blockSignals(true);
                ui->mInspectTextField->setText(param->inspectArea);
                ui->mInspectTextField->getQLineEdit()->blockSignals(false);
                return;
            }
            if (pPtr == &param->designValue) {
                ui->mDesignValueTextField->getQLineEdit()->blockSignals(true);
                ui->mDesignValueTextField->setText(param->designValue.toQStringAndKeepFullDecimal());
                ui->mDesignValueTextField->getQLineEdit()->blockSignals(false);
                return;
            }
        }
    });

    emit ui->objectCBox->getQComboBox()->currentIndexChanged(0);
}

void AmpWidthRoughnessEditController::initRealTimeEdit() {
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
    connect(ui->mDesignValueTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        FloatNumber1 number = fromTextFieldText(ui->mDesignValueTextField,
                                                VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD);
        if (number == nullptr || number == param->designValue) return;
        param->designValue = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->designValue);
    });
    connect(ui->mCursorTypeCBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int){
        if (!param) return;
        int val = ui->mCursorTypeCBox->currentValue();
        if (val < 0 || val == param->cursorType) return;
        param->cursorType = val;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->cursorType);
    });
    connect(ui->mSearchAreaTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->mSearchAreaTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->searchArea) return;
        param->searchArea = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->searchArea);
    });
    connect(ui->mSumLinePointTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->mSumLinePointTextField,
                                                VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->sumLines) return;
        param->sumLines = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->sumLines);
    });
    connect(ui->mMeasurePointTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->mMeasurePointTextField,
                                                VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->measurePoint || !checkMeasurePoint(number)) return;
        param->measurePoint = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->measurePoint);
    });
    connect(ui->mInspectTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->mInspectTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->inspectArea) return;
        param->inspectArea = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->inspectArea);
    });
    connect(ui->measurePositionCBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int){
        if (!param) return;
        int val = ui->measurePositionCBox->currentValue();
        if (val < 0 || val == param->measurePosition) return;
        param->measurePosition = val;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->measurePosition);
    });
    connect(ui->dmMethodCBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int){
        if (!param) return;
        int val = ui->dmMethodCBox->currentValue();
        if (val < 0 || val == param->method) return;
        param->method = val;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->method);
    });
    connect(ui->dmSmoothingTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->dmSmoothingTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->smoothing || (number & 1) == 0) return;
        param->smoothing = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->smoothing);
    });
    connect(ui->dmPreProcessCBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int){
        if (!param) return;
        int val = ui->dmPreProcessCBox->currentValue();
        if (val < 0 || val == param->preProcess) return;
        param->preProcess = val;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->preProcess);
    });
    connect(ui->dmDiffTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->dmDiffTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->diff || (number & 1) == 0) return;
        param->diff = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->diff);
    });
    connect(ui->bfCheckBox, &QCheckBox::toggled, this, [this](bool checked){
        if (!param) return;
        if (param->enableBfFastMode == checked) return;
        param->enableBfFastMode = checked;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->enableBfFastMode);
    });
    connect(ui->acdCBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int){
        if (!param) return;
        int val = ui->acdCBox->currentValue();
        if (val < 0 || val == param->acdParam.acd) return;
        param->acdParam.acd = val;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->acdParam.acd);
    });
    connect(ui->acdWayCBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int){
        if (!param) return;
        int val = ui->acdWayCBox->currentValue();
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

    connect(denseSettingController, &AmpDenseSettingEditController::dataChanged, cdsem::CDSEM_GlobalDataHelper::get(), &cdsem::CDSEM_GlobalData::tmpAmpChanged);
    connect(ui->commonSettingController, &DetectionCommonSettingController::dataChanged, cdsem::CDSEM_GlobalDataHelper::get(), &cdsem::CDSEM_GlobalData::tmpAmpChanged);
}

void AmpWidthRoughnessEditController::refreshAcdType() {
    int old = ui->acdWayCBox->currentValue();
    if (old == -1) {
        old = oldAcdType;
    } else {
        oldAcdType = old;
    }
    ui->acdWayCBox->loadItemFromDatabase(CBOX_AMP_ACD_TYPE, true, [this, old]() {
        int val = ui->objectCBox->currentValue();
        if (cdsem::checkAmpWidthObjectIsDense(val)) {
            ui->acdWayCBox->removeItemByData(0);
        }
        bool dataChanged = false;
        if (old != -1) {
            dataChanged = !ui->acdWayCBox->setCurrentData(old);
        } else if (param->acdParam.acdType != -1) {
            ui->acdWayCBox->setCurrentData(param->acdParam.acdType);
            dataChanged = true;
        }
        if (dataChanged) {
            MainThreadInvoker::getInvoker().runLater([this](){
                if (ui->acdWayCBox->currentIndex() != -1)
                    emit ui->acdWayCBox->getQComboBox()->currentIndexChanged(ui->acdWayCBox->currentIndex());
            }, true);
        }
    });
}

void AmpWidthRoughnessEditController::refreshMeasurePos() {
    int old = ui->measurePositionCBox->currentValue();
    if (old == -1) {
        old = oldMeasurePoint;
    } else {
        oldMeasurePoint = old;
    }
    ui->measurePositionCBox
        ->loadItemFromDatabase(CBOX_AMP_MEASURE_POINT_TYPE, true, [this, old]() {
            int val = ui->objectCBox->currentValue();
            if (cdsem::checkAmpWidthObjectIsDense(val) || ui->acdWayCBox->currentValue() == cdsem::ACDT_MANUAL) {
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

void AmpWidthRoughnessEditController::refreshData() {
    int old = ui->dataCBox->currentValue();
    if (old == -1) {
        old = oldData;
    } else {
        oldData = old;
    }
    ui->dataCBox->loadItemFromDatabase(CBOX_AMP_ROUGHNESS_DATA,
                                       false,
                                       [this, old]() {
                                           int val = ui->objectCBox->currentValue();
                                           if (cdsem::checkAmpWidthRoughnessObjectIsDense(val)) {
                                               QSet<int> invalids;
                                               invalids.insert(3);
                                               invalids.insert(4);
                                               ui->dataCBox->removeItemByData(invalids);
                                           }
                                           if (old != -1) {
                                               ui->dataCBox->setCurrentData(old);
                                           } else if (param->data != -1) {
                                               ui->dataCBox->setCurrentData(param->data);
                                           }
                                       });
}

void AmpWidthRoughnessEditController::refreshCursorBox() {
    if (cdsem::checkAmpWidthRoughnessObjectIsDense(ui->objectCBox->currentValue())) {
        ui->searchAreaLabelContent->hide();
        ui->searchAreaTextContent->hide();
        ui->measurePointLabelContent->hide();
        ui->measurePointTextContent->hide();

        ui->inspectAreaLabelContent->hide();
        ui->inspectAreaTextContent->hide();
        ui->sumLinesLabelContent->hide();
        ui->sumLinesTextContent->hide();
        if (ui->mCursorTypeCBox->getQComboBox()->count() > 1)
            ui->mCursorTypeCBox->removeItemByData(0);
    } else {
        ui->searchAreaLabelContent->show();
        ui->searchAreaTextContent->show();
        ui->measurePointLabelContent->show();
        ui->measurePointTextContent->show();

        ui->inspectAreaLabelContent->show();
        ui->inspectAreaTextContent->show();
        ui->sumLinesLabelContent->show();
        ui->sumLinesTextContent->show();
        if (ui->mCursorTypeCBox->getQComboBox()->count() < 2)
            ui->mCursorTypeCBox->getQComboBox()->insertItem(0, "Two Box", 0);
    }
}

void AmpWidthRoughnessEditController::initAsyncCaller(AsyncCaller* caller) {
    if (this->caller) {
        return;
    }
    this->caller = caller;
    caller->manage(ui->commonSettingController, this);
    caller->manage(ui->measurePositionCBox, this);
    caller->manage(ui->acdWayCBox, this);
    caller->manage(ui->dataCBox, this);
    ui->commonSettingController->initAsyncCaller(caller);
    ui->commonSettingController->setAfterEdgeSearchLoaded([](QSet<int>& invalids) {
        invalids.insert(2);
        invalids.insert(3);
    });
    ui->commonSettingController->refreshEdgeSearchData();
    ui->measurePositionCBox->setAsyncCaller(caller);
    ui->acdWayCBox->setAsyncCaller(caller);
    ui->dataCBox->setAsyncCaller(caller);
    refreshAcdType();
    refreshMeasurePos();
    refreshData();
}

void AmpWidthRoughnessEditController::resetToComponent() {
    if (!param) {
        return;
    }
    ui->objectCBox->setCurrentData(param->object);
    ui->dataCBox->safeSetCurrentData(param->data);
    ui->mDesignValueTextField->setText(param->designValue.toQStringAndKeepFullDecimal());
    ui->mCursorTypeCBox->setCurrentData(param->cursorType);
    ui->mSearchAreaTextField->setText(param->searchArea);
    ui->mSumLinePointTextField->setText(param->sumLines);
    ui->mMeasurePointTextField->setText(param->measurePoint);
    ui->mInspectTextField->setText(param->inspectArea);
    ui->measurePositionCBox->safeSetCurrentData(param->measurePosition);
    ui->dmMethodCBox->setCurrentData(param->method);
    ui->dmSmoothingTextField->setText(param->smoothing);
    ui->dmPreProcessCBox->setCurrentData(param->preProcess);
    ui->dmDiffTextField->setText(param->diff);

    ui->bfCheckBox->setChecked((param->data == 3 || param->data == 4) && param->enableBfFastMode);
    ui->acdCBox->setCurrentData(param->acdParam.acd);
    ui->acdWayCBox->safeSetCurrentData(param->acdParam.acdType);
    ui->acdTargetTextField->setText(param->acdParam.target);
    ui->acdLimitTextField->setText(param->acdParam.limit);

    if (param->data == 1 || param->data == 2) {
        denseSettingController->resetToComponent();
    }
    ui->commonSettingController->resetToComponent();
}

QString AmpWidthRoughnessEditController::collectData(AmpWidthRoughnessParam& param) {
    param.object = ui->objectCBox->currentValue();
    if (param.object < 0) {
        return "Object is not specified.";
    }
    param.data = ui->dataCBox->currentValue();
    if (param.data < 0) {
        return "Data is not specified.";
    }
    param.designValue = fromTextFieldText(ui->mDesignValueTextField,
                                          VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD);
    param.cursorType = ui->mCursorTypeCBox->currentValue();
    if (param.cursorType < 0) {
        return "Cursor is not specified.";
    }
    param.searchArea
        = fromTextFieldText(ui->mSearchAreaTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
    param.sumLines = fromTextFieldText(ui->mSumLinePointTextField,
                                       VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD)
                         .toInt();
    param.measurePoint = fromTextFieldText(ui->mMeasurePointTextField,
                                           VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD)
                             .toInt();
    if (!checkMeasurePoint(param.measurePoint)) {
        return "Measurement point incorrent.";
    }
    param.inspectArea
        = fromTextFieldText(ui->mInspectTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
    param.measurePosition = ui->measurePositionCBox->currentValue();
    if (param.measurePosition < 0) {
        return "Measurement position is not specified.";
    }
    param.method = ui->dmMethodCBox->currentValue();
    if (param.method < 0) {
        return "Detection method is not specified.";
    }
    param.smoothing
        = fromTextFieldText(ui->dmSmoothingTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
    if (!(param.smoothing & 1)) {
        return "Smoothing must be an odd number.";
    }
    param.preProcess = ui->dmPreProcessCBox->currentValue();
    if (param.preProcess < 0) {
        return "Pre process is not specified.";
    }
    param.diff = fromTextFieldText(ui->dmDiffTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
    if (!(param.diff & 1)) {
        return "Differential must be an odd number.";
    }
    param.enableBfFastMode = ui->bfCheckBox->isEnabled() && ui->bfCheckBox->isChecked();

    param.acdParam.acd = ui->acdCBox->currentValue();
    if (param.acdParam.acd < 0) {
        return "ACD is not specified.";
    }
    param.acdParam.acdType = ui->acdWayCBox->currentValue();
    if (param.acdParam.acdType < 0) {
        return "ACD type is not specified.";
    }
    param.acdParam.target
        = fromTextFieldText(ui->acdTargetTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
    param.acdParam.limit
        = fromTextFieldText(ui->acdLimitTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
    if (param.acdParam.limit > param.acdParam.target) {
        return "ACD limit cannot be larger than the target.";
    }

    if (denseSettingController->isVisible()) {
        QString errMsg = denseSettingController->collectData(param.denseParam);
        if (errMsg.size() > 0) {
            return errMsg;
        }
    }
    return ui->commonSettingController->collectData(param.commonMethodParam);
}
