#include "amp_width_edit_controller.h"
#include <QLayout>
#include "amp_dense_setting_edit_controller.h"
#include "cbox_global_keys.h"
#include "cdsem_number_validator_helper.h"
#include "ui_amp_width_edit_controller.h"
#include "cdsem_global_data.h"
#include <QPointer>

inline bool checkMeasurePoint(int p) {
    return cdsem::checkLineMeasurePoint(p);
}

AmpWidthEditController::AmpWidthEditController(QWidget* parent)
    : QWidget(parent) {
    //如果你没有异步任务需求，请将connect清理掉，保留create和createAction函数
    create();
    createAction();
    initRealTimeEdit();
}

AmpWidthEditController::~AmpWidthEditController() noexcept {
    disconnect(cdsem::CDSEM_GlobalDataHelper::get(), &cdsem::CDSEM_GlobalData::tmpAmpChanged, this, nullptr);
    if (ui) {
        delete ui;
    }
}

void AmpWidthEditController::create() {
    this->param = &cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpWidthParam();
    ui = new cdsem::UiAmpWidthEditController;
    ui->init(this);
    denseSettingController = new AmpDenseSettingEditController(ui->denseObjectSettingContent);
    denseSettingController->initParam(&param->denseParam);
    ui->denseObjectSettingContent->layout()->addWidget(denseSettingController);
    ui->commonSettingController->disableFlyer(true);
    ui->commonSettingController->setShapeVisible(false);
    ui->commonSettingController->initParam(&param->commonMethodParam);
    QSizePolicy p = ui->dataItemBox->getQComboBox()->sizePolicy();
    p.setRetainSizeWhenHidden(true);
    ui->dataItemBox->getQComboBox()->setSizePolicy(p);
}

void AmpWidthEditController::createAction() {
    CDSemNumberValidatorHelper::range<1>("1.0", "200000.0").put(ui->mDesignValueTextField).autoFixMode();
    CDSemNumberValidatorHelper::range(10, 150).put(ui->mSearchAreaTextField).autoFixMode();
    CDSemNumberValidatorHelper::range(8, 400).put(ui->mMeasurePointTextField).autoFixMode();
    CDSemNumberValidatorHelper::range(1, 400).put(ui->mSumLineOrInspectTextField).autoFixMode();
    CDSemNumberValidatorHelper::range(1, 64).put(ui->mSumLinePointTextField).autoFixMode();
    CDSemNumberValidatorHelper::range(1, 29).put(ui->dmSmoothingTextField).autoFixMode();
    CDSemNumberValidatorHelper::range(3, 19).put(ui->dmDiffTextField).autoFixMode();
    CDSemNumberValidatorHelper::range<1>("50.0", "100.0").put(ui->dmSuccRateTextField).autoFixMode();
    CDSemNumberValidatorHelper::range(20, 90)
        .put(ui->wOuterThresholdTextField)
        .put(ui->wInnerThresholdTextField)
        .autoFixMode();

    connect(ui->objectCBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int) {
        int val = ui->objectCBox->currentValue();
        bool showDense = cdsem::checkAmpWidthObjectIsDense(val);
        denseSettingController->setVisible(showDense);
        refreshAcdType();
        refreshMeasureDirection();
        refreshKind();
        refreshMeasurePos();
        ui->waveMatchContent->setVisible(!showDense);
        refreshTargetLimitTextField(showDense, ui->acdTargetTextField, ui->acdLimitTextField);
        if (val == 7) {
            denseSettingController->setDenseSettingType(DST_LINE);
        } else if (val == 8) {
            denseSettingController->setDenseSettingType(DST_SPACE);
        } else if (val == 9) {
            denseSettingController->setDenseSettingType(DST_PITCH_L);
        } else if (val == 10) {
            denseSettingController->setDenseSettingType(DST_PITCH_R);
        }
    });

    connect(ui->kindCBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int) {
        bool status = ui->kindCBox->currentValue() == 1;
        ui->mMeasurePointLabelContent->setVisible(status);
        ui->mMeasurePointInputContent->setVisible(status);
        ui->mSumLinePointLabelContent->setVisible(status);
        ui->mSumLinePointInputContent->setVisible(status);
        ui->dataItemBox->setVisible(status);
        ui->dataLabel->setVisible(status);
        ui->dmRotCorrCBox->setVisible(status);
        ui->rotCorrLabel->setVisible(status);
        ui->mSumLinesLabel->setVisible(!status);
        ui->mInspectAreaLabel->setVisible(status);
        ui->dmSuccRateTextField->setDisabled(!status);
        ui->commonSettingController->disableFlyer(!status);
    });

    connect(ui->acdWayCBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int){
        refreshMeasurePos();
    });

    connect(cdsem::CDSEM_GlobalDataHelper::get(), &cdsem::CDSEM_GlobalData::tmpAmpChanged, this, [this](QPointer<QObject> obj, void* pPtr){
        if (obj == this || obj == ui->commonSettingController || obj == denseSettingController) return;
        cdsem::MeasureType mType = cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpMeasureType();
        if (mType == cdsem::MT_WIDTH) {
            if (pPtr == &param->searchArea) {
                ui->mSearchAreaTextField->getQLineEdit()->blockSignals(true);
                ui->mSearchAreaTextField->setText(param->searchArea);
                ui->mSearchAreaTextField->getQLineEdit()->blockSignals(false);
                return;
            }
            if (pPtr == &param->sumLineOrInspectArea) {
                ui->mSearchAreaTextField->getQLineEdit()->blockSignals(true);
                ui->mSumLineOrInspectTextField->setText(param->sumLineOrInspectArea);
                ui->mSearchAreaTextField->getQLineEdit()->blockSignals(false);
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

void AmpWidthEditController::initRealTimeEdit() {
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
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this) ,&param->kind);
    });
    connect(ui->dataItemBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int){
        if (!param) return;
        int val = ui->dataItemBox->currentValue();
        if (val < 0 || val == param->data) return;
        param->data = val;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this) ,&param->data);
    });
    connect(ui->mDesignValueTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        FloatNumber1 val = fromTextFieldText(ui->mDesignValueTextField,
                                               VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD);
        if (val == nullptr || val == param->designValue) return;
        param->designValue = val;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->designValue);
    });
    connect(ui->dmSuccRateTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        FloatNumber1 val = fromTextFieldText(ui->dmSuccRateTextField,
                                             VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD);
        if (val == nullptr || val == param->succRate) return;
        param->succRate = val;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->succRate);
    });
    connect(ui->mCursorTypeCBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int){
        if (!param) return;
        int val = ui->mCursorTypeCBox->currentValue();
        if (val < 0 || val == param->cursorType) return;
        param->cursorType = val;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->cursorType);
    });
    connect(ui->mDirectionCBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int){
        if (!param) return;
        int val = ui->mDirectionCBox->currentValue();
        if (val < 0 || val == param->direction) return;
        param->direction = val;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->direction);
    });
    connect(ui->mSearchAreaTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int val = fromTextFieldText(ui->mSearchAreaTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (val == param->searchArea) return;
        param->searchArea = val;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->searchArea);
    });
    connect(ui->mSumLineOrInspectTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int val = fromTextFieldText(ui->mSumLineOrInspectTextField,
                                    VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (val == param->sumLineOrInspectArea) return;
        param->sumLineOrInspectArea = val;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->sumLineOrInspectArea);
    });
    connect(ui->mSumLinePointTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        if (param->kind != 1) {
            return;
        }
        int val = fromTextFieldText(ui->mSumLinePointTextField,
                                    VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (val == param->sumLinesPerPoint) return;
        param->sumLinesPerPoint = val;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->sumLinesPerPoint);
    });
    connect(ui->mMeasurePointTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        if (param->kind != 1) {
            return;
        }
        int val = fromTextFieldText(ui->mMeasurePointTextField,
                                    VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (val == param->measurePoint || !checkMeasurePoint(val)) return;
        param->measurePoint = val;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->measurePoint);
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
        int val = fromTextFieldText(ui->dmSmoothingTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (val == param->smoothing || (val & 1) == 0) return;
        param->smoothing = val;
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
        int val = fromTextFieldText(ui->dmDiffTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (val == param->diff || (val & 1) == 0) return;
        param->diff = val;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->diff);
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
        int val = fromTextFieldText(ui->acdTargetTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (val == param->acdParam.target) return;
        param->acdParam.target = val;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->acdParam.target);
    });
    connect(ui->acdLimitTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int val = fromTextFieldText(ui->acdLimitTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (val == param->acdParam.limit) return;
        param->acdParam.limit = val;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->acdParam.limit);
    });
    connect(ui->waveMatchCBox, &QCheckBox::toggled, this, [this](bool checked){
        if (!param) return;
        if (cdsem::checkAmpWidthObjectIsDense(param->object)) return;
        if (param->waveParam.enable == checked) return;
        param->waveParam.enable = checked;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->waveParam.enable);
    });
    connect(ui->wInnerThresholdTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int val = fromTextFieldText(ui->wInnerThresholdTextField,
                                    VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (val == param->waveParam.innerThreshold) return;
        param->waveParam.innerThreshold = val;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->waveParam.innerThreshold);
    });
    connect(ui->wOuterThresholdTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int val = fromTextFieldText(ui->wOuterThresholdTextField,
                                    VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (val == param->waveParam.outerThreshold) return;
        param->waveParam.outerThreshold = val;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->waveParam.outerThreshold);
    });
    connect(ui->dmRotCorrCBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int){
        if (!param) return;
        int val = ui->dmRotCorrCBox->currentValue();
        if (val < 0 || val == param->rotCorr) return;
        param->rotCorr = val;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->rotCorr);
    });
    connect(denseSettingController, &AmpDenseSettingEditController::dataChanged, cdsem::CDSEM_GlobalDataHelper::get(), &cdsem::CDSEM_GlobalData::tmpAmpChanged);
    connect(ui->commonSettingController, &DetectionCommonSettingController::dataChanged, cdsem::CDSEM_GlobalDataHelper::get(), &cdsem::CDSEM_GlobalData::tmpAmpChanged);
}

void AmpWidthEditController::resetToComponent() {
    if (!param) {
        return;
    }
    ui->objectCBox->setCurrentData(param->object);
    ui->kindCBox->safeSetCurrentData(param->kind);
    ui->dataItemBox->setCurrentData(param->data);
    ui->mDesignValueTextField->setText(param->designValue.toQStringAndKeepFullDecimal());
    ui->dmSuccRateTextField->setText(param->succRate.toQStringAndKeepFullDecimal());
    ui->mCursorTypeCBox->setCurrentData(param->cursorType);
    ui->mDirectionCBox->safeSetCurrentData(param->direction);
    ui->mSearchAreaTextField->setText(param->searchArea);
    ui->mSumLineOrInspectTextField->setText(param->sumLineOrInspectArea);
    if (param->kind == 1) {
        ui->mSumLinePointTextField->setText(param->sumLinesPerPoint);
        ui->mMeasurePointTextField->setText(param->measurePoint);
        ui->dmRotCorrCBox->setCurrentData(param->rotCorr);
    }
    ui->measurePositionCBox->safeSetCurrentData(param->measurePosition);
    ui->dmMethodCBox->setCurrentData(param->method);
    ui->dmSmoothingTextField->setText(param->smoothing);
    ui->dmPreProcessCBox->setCurrentData(param->preProcess);
    ui->dmDiffTextField->setText(param->diff);
    ui->acdCBox->setCurrentData(param->acdParam.acd);
    ui->acdWayCBox->safeSetCurrentData(param->acdParam.acdType);
    ui->acdTargetTextField->setText(param->acdParam.target);
    ui->acdLimitTextField->setText(param->acdParam.limit);

    if (!cdsem::checkAmpWidthObjectIsDense(param->object)) {
        //TODO Wave match内容放到一起，如果Ellipse的Wave Match与这里的相同，则将这块代码提出去。
        ui->waveMatchCBox->setChecked(param->waveParam.enable);
        if (param->waveParam.enable) {
            ui->wInnerThresholdTextField->setText(param->waveParam.innerThreshold);
            ui->wOuterThresholdTextField->setText(param->waveParam.outerThreshold);
            // param->waveParam.mag = ui->wMagTextField->text().toInt();
            // param->waveParam.magY = ui->wMagYTextField->text().toInt();
            // param->waveParam.vac = ui->wVacTextField->text().toInt();
            // param->waveParam.frame = ui->wFrameTextField->text().toInt();
            // param->waveParam.scan = ui->wScanTextField->text().toInt();
            // param->waveParam.pixel
        }
    } else {
        denseSettingController->resetToComponent();
    }
    ui->commonSettingController->resetToComponent();
}

QString AmpWidthEditController::collectData(AmpWidthParam& param) {
    param.object = ui->objectCBox->currentValue();
    if (param.object < 0) {
        return "Object is not specified.";
    }
    param.kind = ui->kindCBox->currentValue();
    if (param.kind < 0) {
        return "Kind is not specified.";
    }
    param.data = ui->dataItemBox->currentValue();
    if (param.data < 0) {
        return "Data is not specified.";
    }

    param.designValue = fromTextFieldText(ui->mDesignValueTextField,
                                          VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD);
    param.succRate = fromTextFieldText(ui->dmSuccRateTextField,
                                       VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD);
    param.cursorType = ui->mCursorTypeCBox->currentValue();
    if (param.cursorType < 0) {
        return "Cursor type is not specified.";
    }
    param.direction = ui->mDirectionCBox->currentValue();
    if (param.direction < 0) {
        return "Direction is not specified.";
    }
    param.searchArea
        = fromTextFieldText(ui->mSearchAreaTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
    param.sumLineOrInspectArea = fromTextFieldText(ui->mSumLineOrInspectTextField,
                                                   VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD)
                                     .toInt();
    if (param.kind == 1) {
        param.sumLinesPerPoint = fromTextFieldText(ui->mSumLinePointTextField,
                                           VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD)
                             .toInt();
        param.measurePoint = fromTextFieldText(ui->mMeasurePointTextField,
                                               VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD)
                                 .toInt();
        param.rotCorr = ui->dmRotCorrCBox->currentValue();
        if (param.rotCorr < 0) {
            return "Rotation Correct is not specified.";
        }
        if (!checkMeasurePoint(param.measurePoint)) {
            return "Measurement point incorrent.";
        }
    }
    param.measurePosition = ui->measurePositionCBox->currentValue();
    if (param.measurePosition < 0) {
        return "Measurement point is not specified.";
    }
    param.method = ui->dmMethodCBox->currentValue();
    if (param.method < 0) {
        return "Detection Method is not specified.";
    }
    param.smoothing
        = fromTextFieldText(ui->dmSmoothingTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
    if (!(param.smoothing & 1)) {
        return "Detection smoothing must be an odd number.";
    }
    param.preProcess = ui->dmPreProcessCBox->currentValue();
    if (param.preProcess < 0) {
        return "Pre process is not specified.";
    }
    param.diff = fromTextFieldText(ui->dmDiffTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
    if (!(param.diff & 1)) {
        return "Detection differential must be an odd number.";
    }

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
    param.acdParam.centers = cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpWidthParam().acdParam.centers;
    if (param.acdParam.limit > param.acdParam.target) {
        return "ACD limit cannot be larger than the target.";
    }

    if (!cdsem::checkAmpWidthObjectIsDense(param.object)) {
        //TODO Wave match内容放到一起，如果Ellipse的Wave Match与这里的相同，则将这块代码提出去。
        param.waveParam.enable = ui->waveMatchCBox->isChecked();
        if (param.waveParam.enable) {
            param.waveParam.innerThreshold = fromTextFieldText(ui->wInnerThresholdTextField,
                                                               VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD)
                                                 .toInt();
            param.waveParam.outerThreshold = fromTextFieldText(ui->wOuterThresholdTextField,
                                                               VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD)
                                                 .toInt();
            // param.waveParam.mag = ui->wMagTextField->text().toInt();
            // param.waveParam.magY = ui->wMagYTextField->text().toInt();
            // param.waveParam.vac = ui->wVacTextField->text().toInt();
            // param.waveParam.frame = ui->wFrameTextField->text().toInt();
            // param.waveParam.scan = ui->wScanTextField->text().toInt();
            // param.waveParam.pixel
        }
    } else {
        QString errMsg = denseSettingController->collectData(param.denseParam);
        if (errMsg.size() > 0) {
            return errMsg;
        }
    }
    return ui->commonSettingController->collectData(param.commonMethodParam);
}

void AmpWidthEditController::refreshAcdType() {
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

void AmpWidthEditController::refreshMeasureDirection() {
    if (cdsem::checkAmpWidthObjectIsDense(ui->objectCBox->currentValue())) {
        ui->sumLineOrInspectAreaContent->hide();
        ui->sumLineOrInspectAreaLabelContent->hide();
        ui->searchAreaLabelContent->hide();
        ui->searchAreaContent->hide();
        if (ui->mCursorTypeCBox->getQComboBox()->count() > 1)
            ui->mCursorTypeCBox->removeItemByData(0);
    } else {
        ui->sumLineOrInspectAreaContent->show();
        ui->sumLineOrInspectAreaLabelContent->show();
        ui->searchAreaLabelContent->show();
        ui->searchAreaContent->show();
        if (ui->mCursorTypeCBox->getQComboBox()->count() < 2)
            ui->mCursorTypeCBox->getQComboBox()->insertItem(0, "Two Box", 0);
    }
    int old = ui->mDirectionCBox->currentValue();
    if (old == -1) {
        old = oldMeasureDirection;
    } else {
        oldMeasureDirection = old;
    }
    ui->mDirectionCBox->loadItemFromDatabase(CBOX_AMP_MEASURE_AREA_DIRECTION,
                                             true,
                                             [this, old]() {
                                                 int val = ui->objectCBox->currentValue();
                                                 if (cdsem::checkAmpWidthObjectIsDense(val)) {
                                                     ui->mDirectionCBox->removeItemByData(1);
                                                 }
                                                 bool dataChanged = false;
                                                 if (old != -1) {
                                                     dataChanged = !ui->mDirectionCBox->setCurrentData(old);
                                                 } else if (param->direction != -1) {
                                                     ui->mDirectionCBox->setCurrentData(
                                                         param->direction);
                                                     dataChanged = true;
                                                 }
                                                 if (dataChanged) {
                                                     MainThreadInvoker::getInvoker().runLater([this](){
                                                         if (ui->mDirectionCBox->currentIndex() != -1)
                                                             emit ui->mDirectionCBox->getQComboBox()->currentIndexChanged(ui->mDirectionCBox->currentIndex());
                                                     }, true);
                                                 }
                                             });
}

void AmpWidthEditController::refreshKind() {
    int old = ui->kindCBox->currentValue();
    if (old == -1) {
        old = oldKind;
    } else {
        oldKind = old;
    }
    ui->kindCBox->loadItemFromDatabase(CBOX_AMP_WIDTH_KIND, true, [this, old]() {
        int val = ui->objectCBox->currentValue();
        if (cdsem::checkAmpWidthObjectIsDense(val)) {
            ui->kindCBox->removeItemByData(0);
        }
        bool dataChanged = false;
        if (old != -1) {
            dataChanged = !ui->kindCBox->setCurrentData(old);
        } else if (param->kind != -1) {
            ui->kindCBox->setCurrentData(param->kind);
            dataChanged = true;
        }
        if (dataChanged) {
            MainThreadInvoker::getInvoker().runLater([this](){
                if (ui->kindCBox->currentIndex() != -1)
                    emit ui->kindCBox->getQComboBox()->currentIndexChanged(ui->kindCBox->currentIndex());
            }, true);
        }
    });
}

void AmpWidthEditController::refreshMeasurePos() {
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

void AmpWidthEditController::refreshPreProcess() {
    int old = ui->dmPreProcessCBox->currentValue();
    if (old == -1) {
        old = oldPreProcess;
    } else {
        oldPreProcess = old;
    }
    ui->dmPreProcessCBox->loadItemFromDatabase(CBOX_AMP_PRE_PROCESS,
                                               true,
                                               [this, old]() {
                                                   bool dataChanged = false;
                                                   if (old != -1) {
                                                       dataChanged = !ui->dmPreProcessCBox->setCurrentData(old);
                                                   } else if (param->preProcess != -1) {
                                                       ui->dmPreProcessCBox->setCurrentData(
                                                           param->preProcess);
                                                       dataChanged = true;
                                                   }
                                                   if (dataChanged) {
                                                       MainThreadInvoker::getInvoker().runLater([this](){
                                                           if (ui->dmPreProcessCBox->currentIndex() != -1)
                                                               emit ui->dmPreProcessCBox->getQComboBox()->currentIndexChanged(ui->dmPreProcessCBox->currentIndex());
                                                       }, true);
                                                   }
                                               });
}

void AmpWidthEditController::initAsyncCaller(AsyncCaller* caller) {
    if (this->caller) {
        return;
    }
    this->caller = caller;
    caller->manage(ui->commonSettingController, this);
    caller->manage(ui->acdWayCBox, this);
    caller->manage(ui->mDirectionCBox, this);
    caller->manage(ui->kindCBox, this);
    caller->manage(ui->measurePositionCBox, this);
    caller->manage(ui->dmPreProcessCBox, this);
    ui->commonSettingController->initAsyncCaller(caller);
    ui->commonSettingController->setAfterEdgeSearchLoaded([](QSet<int>& invalids) {
        invalids.insert(2);
        invalids.insert(3);
    });
    ui->commonSettingController->refreshEdgeSearchData();
    ui->acdWayCBox->setAsyncCaller(caller);
    ui->mDirectionCBox->setAsyncCaller(caller);
    ui->kindCBox->setAsyncCaller(caller);
    ui->measurePositionCBox->setAsyncCaller(caller);
    ui->dmPreProcessCBox->setAsyncCaller(caller);
    refreshAcdType();
    refreshMeasureDirection();
    refreshKind();
    refreshMeasurePos();
    refreshPreProcess();
}
