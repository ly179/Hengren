#include "amp_ellipse_edit_controller.h"
#include "cbox_global_keys.h"
#include "cdsem_number_validator_helper.h"
#include "ui_amp_ellipse_edit_controller.h"
#include "cdsem_global_data.h"

bool checkBaseStartPoint(int val) {
    return val == 1 || val == 2 || val == 4 || val == 8;
}

AmpEllipseEditController::AmpEllipseEditController(QWidget* parent)
    : QWidget(parent) {
    create();
    createAction();
    initRealTimeEdit();
}

AmpEllipseEditController::~AmpEllipseEditController() noexcept {
    disconnect(cdsem::CDSEM_GlobalDataHelper::get(), &cdsem::CDSEM_GlobalData::tmpAmpChanged, this, nullptr);
    if (ui) {
        delete ui;
    }
}

void AmpEllipseEditController::create() {
    param = &cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpEllipseParam();
    ui = new cdsem::UiAmpEllipseEditController;
    ui->init(this);
}

void AmpEllipseEditController::createAction() {
    CDSemNumberValidatorHelper::range(1, 64)
        .put(ui->acdTargetTextField)
        .put(ui->acdLimitTextField)
        .autoFixMode();
    CDSemNumberValidatorHelper::range(1, 200000).put(ui->designValueTextField).autoFixMode();
    CDSemNumberValidatorHelper::range(25, 300)
        .put(ui->cursorSizeXTextField)
        .put(ui->cursorSizeYTextField)
        .autoFixMode();
    CDSemNumberValidatorHelper::range(1, 29).put(ui->smoothingTextField).autoFixMode();
    CDSemNumberValidatorHelper::range(3, 19).put(ui->diffTextField).autoFixMode();
    CDSemNumberValidatorHelper::range(0, 100).put(ui->thresholdTextField).autoFixMode();
    CDSemNumberValidatorHelper::range(1, 32).put(ui->sumLineOrPointTextField).autoFixMode();
    CDSemNumberValidatorHelper::range(1, 3)
        .put(ui->edgeNumberTextField)
        .put(ui->edgeTotalTextField)
        .autoFixMode();
    CDSemNumberValidatorHelper::range(1, 8).put(ui->baseLineStartPointTextField).autoFixMode();
    CDSemNumberValidatorHelper::range(1, 16).put(ui->baseLineAreaTextField).autoFixMode();
    CDSemNumberValidatorHelper::range(0, 15).put(ui->imgSmoothTextField).autoFixMode();

    connect(ui->acdWayCBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int) {
        int val = ui->acdWayCBox->currentValue();
        if (val == 1) {
            ui->patternSearchContent->show();
            ui->cursorPosContent->hide();
        } else {
            ui->patternSearchContent->hide();
            ui->cursorPosContent->show();
        }
    });

    connect(cdsem::CDSEM_GlobalDataHelper::get(), &cdsem::CDSEM_GlobalData::tmpAmpChanged, this, [this](QPointer<QObject> obj, void* pPtr){
        if (obj == this) return;
        cdsem::MeasureType mType = cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpMeasureType();
        if (mType == cdsem::MT_ELLIPSE) {
            if (pPtr == &param->cursorSizeX) {
                ui->cursorSizeXTextField->getQLineEdit()->blockSignals(true);
                ui->cursorSizeXTextField->setText(param->cursorSizeX);
                ui->cursorSizeXTextField->getQLineEdit()->blockSignals(false);
                return;
            }
            if (pPtr == &param->cursorSizeY) {
                ui->cursorSizeYTextField->getQLineEdit()->blockSignals(true);
                ui->cursorSizeYTextField->setText(param->cursorSizeY);
                ui->cursorSizeYTextField->getQLineEdit()->blockSignals(false);
                return;
            }
        }
    });

    ui->patternSearchContent->hide();
    ui->cursorPosContent->show();
}

void AmpEllipseEditController::refreshAcdType() {
    int old = ui->acdWayCBox->currentValue();
    if (old == -1) {
        old = oldAcdType;
    } else {
        oldAcdType = old;
    }
    ui->acdWayCBox->loadItemFromDatabase(CBOX_AMP_ACD_TYPE, true, [this, old]() {
        int val = ui->objectCBox->currentValue();
        if (val == 1 || val == 2) {
            ui->acdWayCBox->removeItemByData(0);
        }
        if (old != -1) {
            ui->acdWayCBox->setCurrentData(old);
        } else if (param->acdParam.acdType != -1) {
            ui->acdWayCBox->setCurrentData(param->acdParam.acdType);
        }
    });
}

void AmpEllipseEditController::initRealTimeEdit() {
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
    connect(ui->preProcessCBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int){
        if (!param) return;
        int val = ui->preProcessCBox->currentValue();
        if (val < 0 || val == param->preProcess) return;
        param->preProcess = val;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->preProcess);
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
    connect(ui->patternSearchCBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int){
        if (!param) return;
        int val = ui->patternSearchCBox->currentValue();
        if (val < 0 || val == param->acdPatternSearch) return;
        param->acdPatternSearch = val;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->acdPatternSearch);
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
        if (number == param->designValue) return;
        param->designValue = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->designValue);
    });
    connect(ui->cursorSizeXTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->cursorSizeXTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->cursorSizeX) return;
        param->cursorSizeX = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->cursorSizeX);
    });
    connect(ui->cursorSizeYTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->cursorSizeYTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->cursorSizeY) return;
        param->cursorSizeY = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->cursorSizeY);
    });
    connect(ui->smoothingTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->smoothingTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->smoothing || (number & 1) == 0) return;
        param->smoothing = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->smoothing);
    });
    connect(ui->diffTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->smoothingTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->diff || (number & 1) == 0) return;
        param->diff = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->diff);
    });
    connect(ui->thresholdTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->thresholdTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->threshold) return;
        param->threshold = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->threshold);
    });
    connect(ui->sumLineOrPointTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->sumLineOrPointTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->sumLines) return;
        param->sumLines = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->sumLines);
    });
    connect(ui->convenProfileCheckBox, &QCheckBox::toggled, this, [this](bool checked){
        if (!param) return;
        if (param->enableConventProfile == checked) return;
        param->enableConventProfile = checked;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->enableConventProfile);
    });
    connect(ui->highSenCheckBox, &QCheckBox::toggled, this, [this](bool checked){
        if (!param) return;
        if (param->enableHighSenDetect == checked) return;
        param->enableHighSenDetect = checked;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->enableHighSenDetect);
    });
    connect(ui->acceptPriCheckBox, &QCheckBox::toggled, this, [this](bool checked){
        if (!param) return;
        if (param->enableAcceptPriEdgeSignal == checked) return;
        param->enableAcceptPriEdgeSignal = checked;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->enableAcceptPriEdgeSignal);
    });
    connect(ui->bottomSufCheckBox, &QCheckBox::toggled, this, [this](bool checked){
        if (!param) return;
        if (param->enableBottomSufMask == checked) return;
        param->enableBottomSufMask = checked;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->enableBottomSufMask);
    });
    connect(ui->waveMatchCheckBox, &QCheckBox::toggled, this, [this](bool checked){
        if (!param) return;
        if (param->enableWaveMatch == checked) return;
        param->enableWaveMatch = checked;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->enableWaveMatch);
    });
    connect(ui->edgeDetectByPRCheckBox, &QCheckBox::toggled, this, [this](bool checked){
        if (!param) return;
        if (param->enableEdgeDetectByPR == checked) return;
        param->enableEdgeDetectByPR = checked;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->enableEdgeDetectByPR);
    });
    connect(ui->imgSmoothCheckBox, &QCheckBox::toggled, this, [this](bool checked){
        if (!param) return;
        if (param->enableImgSmooth == checked) return;
        param->enableImgSmooth = checked;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->enableImgSmooth);
    });

    connect(ui->edgeNumberTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->edgeNumberTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->edgeNum) return;
        param->edgeNum = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->edgeNum);
    });
    connect(ui->edgeTotalTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->edgeTotalTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->edgeTotal) return;
        param->edgeTotal = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->edgeTotal);
    });
    connect(ui->baseLineStartPointTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->baseLineStartPointTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->baseLineStartPoint || !checkBaseStartPoint(number)) return;
        param->baseLineStartPoint = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->baseLineStartPoint);
    });
    connect(ui->baseLineAreaTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->baseLineAreaTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->baseLineArea) return;
        param->baseLineArea = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->baseLineArea);
    });
    connect(ui->imgSmoothTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int number = fromTextFieldText(ui->imgSmoothTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (number == param->imgSmoothing) return;
        param->imgSmoothing = number;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->imgSmoothing);
    });
    connect(ui->edgeSearchDirectCBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int){
        if (!param) return;
        int val = ui->edgeSearchDirectCBox->currentValue();
        if (val < 0 || val == param->edgeSearchDirect) return;
        param->edgeSearchDirect = val;
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), &param->edgeSearchDirect);
    });
}

void AmpEllipseEditController::initAsyncCaller(AsyncCaller* caller) {
    if (this->caller) {
        return;
    }
    this->caller = caller;
    caller->manage(ui->acdWayCBox, this);
    ui->acdWayCBox->setAsyncCaller(caller);
    refreshAcdType();
}

QString AmpEllipseEditController::collectData(AmpEllipseParam& param) {
    param.object = ui->objectCBox->currentValue();
    if (param.object < 0) {
        return "Object is not specified.";
    }
    param.data = ui->dataCBox->currentValue();
    if (param.data < 0) {
        return "Data is not specified.";
    }
    param.preProcess = ui->preProcessCBox->currentValue();
    if (param.preProcess < 0) {
        return "Pre process is not specified.";
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
    param.acdPatternSearch = ui->patternSearchCBox->currentValue();
    if (param.acdPatternSearch < 0) {
        return "ACD pattern search is not specified.";
    }
    param.method = ui->methodCBox->currentValue();
    if (param.method < 0) {
        return "Detection method is not specified.";
    }
    param.designValue = fromTextFieldText(ui->designValueTextField,
                                          VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD);
    param.cursorSizeX
        = fromTextFieldText(ui->cursorSizeXTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
    param.cursorSizeY
        = fromTextFieldText(ui->cursorSizeYTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
    param.smoothing
        = fromTextFieldText(ui->smoothingTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
    if (!(param.smoothing & 1)) {
        return "Smoothing must be an odd number.";
    }
    param.diff = fromTextFieldText(ui->diffTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
    if (!(param.diff & 1)) {
        return "Differential must be an odd number.";
    }
    param.threshold
        = fromTextFieldText(ui->thresholdTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
    param.sumLines = fromTextFieldText(ui->sumLineOrPointTextField,
                                       VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD)
                         .toInt();
    param.enableConventProfile = ui->convenProfileCheckBox->isChecked();
    param.enableHighSenDetect = ui->highSenCheckBox->isChecked();
    param.enableAcceptPriEdgeSignal = ui->acceptPriCheckBox->isChecked();
    param.enableBottomSufMask = ui->bottomSufCheckBox->isChecked();
    param.enableWaveMatch = ui->waveMatchCheckBox->isChecked();
    param.enableEdgeDetectByPR = ui->edgeDetectByPRCheckBox->isChecked();
    param.enableImgSmooth = ui->imgSmoothCheckBox->isChecked();

    //TODO 如果以下参数与与它关联的checkbox状态相关联，加上if即可，目前尚不确定。
    param.edgeNum = fromTextFieldText(ui->edgeNumberTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD)
                        .toInt();
    param.edgeTotal
        = fromTextFieldText(ui->edgeTotalTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
    param.baseLineStartPoint = fromTextFieldText(ui->baseLineStartPointTextField,
                                                 VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD)
                                   .toInt();
    if (!checkBaseStartPoint(param.baseLineStartPoint)) {
        return "Base Line Start Point is invalid.";
    }
    param.baseLineArea
        = fromTextFieldText(ui->baseLineAreaTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
    param.edgeSearchDirect = ui->edgeSearchDirectCBox->currentValue();
    if (param.edgeSearchDirect < 0) {
        return "Edge Dearch Direct. is not specified.";
    }
    param.imgSmoothing
        = fromTextFieldText(ui->imgSmoothTextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
    return "";
}
void AmpEllipseEditController::resetToComponent() {
    if (!param) {
        return;
    }
    ui->objectCBox->setCurrentData(param->object);
    ui->dataCBox->setCurrentData(param->data);
    ui->preProcessCBox->setCurrentData(param->preProcess);
    ui->acdCBox->setCurrentData(param->acdParam.acd);
    ui->acdWayCBox->safeSetCurrentData(param->acdParam.acdType);
    ui->acdTargetTextField->setText(param->acdParam.target);
    ui->acdLimitTextField->setText(param->acdParam.limit);
    ui->patternSearchCBox->setCurrentData(param->acdPatternSearch);
    ui->methodCBox->setCurrentData(param->method);
    ui->designValueTextField->setText(param->designValue.toQStringAndKeepFullDecimal());
    ui->cursorSizeXTextField->setText(param->cursorSizeX);
    ui->cursorSizeYTextField->setText(param->cursorSizeY);
    ui->smoothingTextField->setText(param->smoothing);
    ui->diffTextField->setText(param->diff);
    ui->thresholdTextField->setText(param->threshold);
    ui->sumLineOrPointTextField->setText(param->sumLines);
    ui->convenProfileCheckBox->setChecked(param->enableConventProfile);
    ui->highSenCheckBox->setChecked(param->enableHighSenDetect);
    ui->acceptPriCheckBox->setChecked(param->enableAcceptPriEdgeSignal);
    ui->bottomSufCheckBox->setChecked(param->enableBottomSufMask);
    ui->waveMatchCheckBox->setChecked(param->enableWaveMatch);
    ui->edgeDetectByPRCheckBox->setChecked(param->enableEdgeDetectByPR);
    ui->imgSmoothCheckBox->setChecked(param->enableImgSmooth);

    //TODO 如果以下参数与与它关联的checkbox状态相关联，加上if即可，目前尚不确定。
    ui->edgeNumberTextField->setText(param->edgeNum);
    ui->edgeTotalTextField->setText(param->edgeTotal);
    ui->baseLineStartPointTextField->setText(param->baseLineStartPoint);
    ui->baseLineAreaTextField->setText(param->baseLineArea);
    ui->edgeSearchDirectCBox->setCurrentData(param->edgeSearchDirect);
    ui->imgSmoothTextField->setText(param->imgSmoothing);
}
