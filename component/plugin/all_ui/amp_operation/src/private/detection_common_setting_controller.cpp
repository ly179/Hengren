#include "detection_common_setting_controller.h"
#include "cbox_global_keys.h"
#include "cdsem_number_validator_helper.h"
#include "ui_detection_common_setting_controller.h"
#include <QSignalBlocker>

bool checkLineStartPoint(int p) {
    p = std::abs(p);
    return p == 8 || p == 4 || p == 2 || p == 1;
}

DetectionCommonSettingController::DetectionCommonSettingController(QWidget* parent)
    : QWidget(parent) {
    create();
    createAction();
    initRealTimeEdit();
}

DetectionCommonSettingController::~DetectionCommonSettingController() noexcept {
    if (ui) {
        delete ui;
    }
}

void DetectionCommonSettingController::create() {
    ui = new cdsem::UiDetectionCommonSettingController;
    ui->init(this);
}

void DetectionCommonSettingController::createAction() {
    CDSemNumberValidatorHelper::range(0, 100)
        .put(ui->threshold1TextField)
        .put(ui->threshold2TextField)
        .autoFixMode();
    CDSemNumberValidatorHelper::range(1, 5)
        .put(ui->edgeNum1TextField)
        .put(ui->edgeNum2TextField)
        .autoFixMode();
    CDSemNumberValidatorHelper::range(-8, 8)
        .put(ui->bLineStartPoint1TextField)
        .put(ui->bLineStartPoint2TextField)
        .autoFixMode();
    CDSemNumberValidatorHelper::range(1, 16)
        .put(ui->bLineArea1TextField)
        .put(ui->bLineArea2TextField)
        .autoFixMode();
    CDSemNumberValidatorHelper::range<1>("0.1", "200000.0")
        .put(ui->flyer1TextField)
        .put(ui->flyer2TextField)
        .autoFixMode();
}

void DetectionCommonSettingController::initRealTimeEdit() {
    connect(ui->flyer1TextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        FloatNumber1 newVal = fromTextFieldText(ui->flyer1TextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD);
        if (newVal == param->flyer1) return;
        param->flyer1 = newVal;
        emit dataChanged(QPointer<QObject>(this), &param->flyer1);
    });
    connect(ui->flyer2TextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        FloatNumber1 newVal = fromTextFieldText(ui->flyer2TextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD);
        if (newVal == param->flyer2) return;
        param->flyer2 = newVal;
        emit dataChanged(QPointer<QObject>(this), &param->flyer2);
    });
    connect(ui->shape1CBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int){
        if (!param) return;
        int val = ui->shape1CBox->currentValue();
        if (val < 0 || val == param->shape1) return;
        param->shape1 = val;
        emit dataChanged(QPointer<QObject>(this), &param->shape1);
    });
    connect(ui->shape2CBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int){
        if (!param) return;
        int val = ui->shape2CBox->currentValue();
        if (val < 0 || val == param->shape2) return;
        param->shape2 = val;
        emit dataChanged(QPointer<QObject>(this), &param->shape2);
    });

    connect(ui->threshold1TextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int val = fromTextFieldText(ui->threshold1TextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (val == param->threshold1) return;
        param->threshold1 = val;
        emit dataChanged(QPointer<QObject>(this), &param->threshold1);
    });
    connect(ui->threshold2TextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int val = fromTextFieldText(ui->threshold2TextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
        if (val == param->threshold2) return;
        param->threshold2 = val;
        emit dataChanged(QPointer<QObject>(this), &param->threshold2);
    });
    connect(ui->edgeNum1TextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int val = fromTextFieldText(ui->edgeNum1TextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD)
                      .toInt();
        if (val == param->edge1) return;
        param->edge1 = val;
        emit dataChanged(QPointer<QObject>(this), &param->edge1);
    });
    connect(ui->edgeNum2TextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int val = fromTextFieldText(ui->edgeNum2TextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD)
                      .toInt();
        if (val == param->edge2) return;
        param->edge2 = val;
        emit dataChanged(QPointer<QObject>(this), &param->edge2);
    });

    connect(ui->bLineStartPoint1TextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int val = fromTextFieldText(ui->bLineStartPoint1TextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD)
                      .toInt();
        if (val == param->baseLineStartPoint1 || !checkLineStartPoint(val)) return;
        param->baseLineStartPoint1 = val;
        emit dataChanged(QPointer<QObject>(this), &param->baseLineStartPoint1);
    });
    connect(ui->bLineStartPoint2TextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int val = fromTextFieldText(ui->bLineStartPoint2TextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD)
                      .toInt();
        if (val == param->baseLineStartPoint2 || !checkLineStartPoint(val)) return;
        param->baseLineStartPoint2 = val;
        emit dataChanged(QPointer<QObject>(this), &param->baseLineStartPoint2);
    });
    connect(ui->bLineArea1TextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int val = fromTextFieldText(ui->bLineArea1TextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD)
                      .toInt();
        if (val == param->baseLineArea1) return;
        param->baseLineArea1 = val;
        emit dataChanged(QPointer<QObject>(this), &param->baseLineArea1);
    });
    connect(ui->bLineArea2TextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        if (!param) return;
        int val = fromTextFieldText(ui->bLineArea2TextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD)
                      .toInt();
        if (val == param->baseLineArea2) return;
        param->baseLineArea2 = val;
        emit dataChanged(QPointer<QObject>(this), &param->baseLineArea2);
    });

    connect(ui->edgeSearchDirect1CBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int){
        if (!param) return;
        int val = ui->edgeSearchDirect1CBox->currentValue();
        if (val < 0 || val == param->edgeSearchDirect1) return;
        param->edgeSearchDirect1 = val;
        emit dataChanged(QPointer<QObject>(this), &param->edgeSearchDirect1);
    });
    connect(ui->edgeSearchDirect2CBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int){
        if (!param) return;
        int val = ui->edgeSearchDirect2CBox->currentValue();
        if (val < 0 || val == param->edgeSearchDirect2) return;
        param->edgeSearchDirect2 = val;
        emit dataChanged(QPointer<QObject>(this), &param->edgeSearchDirect2);
    });
    connect(ui->flyerCheckBox, &QCheckBox::toggled, this, [this](bool checked){
        if (!param) return;
        if (checked == param->enableFlyer) return;
        param->enableFlyer = checked;
        emit dataChanged(QPointer<QObject>(this), &param->enableFlyer);
    });
}

void DetectionCommonSettingController::setTitle(const QString& t1, const QString& t2) {
    ui->title1Label->setText(t1);
    ui->title2Label->setText(t2);
}
void DetectionCommonSettingController::setShapeVisible(bool b) {
    ui->shapeLabel->setVisible(b);
    ui->shape1Content->setVisible(b);
    ui->shape2Content->setVisible(b);
}
void DetectionCommonSettingController::setFlyerVisible(bool b) {
    ui->flyerLabelContent->setVisible(b);
    ui->flyer1InputContent->setVisible(b);
    ui->flyer2InputContent->setVisible(b);
}

void DetectionCommonSettingController::disableFlyer(bool b) {
    ui->flyerCheckBox->setDisabled(b);
    ui->flyer1InputContent->setDisabled(b);
    ui->flyer2InputContent->setDisabled(b);
}

void DetectionCommonSettingController::removeEdgeDirect(QSet<int>& val) {
    ui->edgeSearchDirect1CBox->removeItemByData(val);
    ui->edgeSearchDirect2CBox->removeItemByData(val);
}

void DetectionCommonSettingController::removeShape(int v) {
    ui->shape1CBox->removeItemByData(v);
    ui->shape2CBox->removeItemByData(v);
}

void DetectionCommonSettingController::initAsyncCaller(AsyncCaller* caller) {
    if (this->caller) {
        return;
    }
    this->caller = caller;
    caller->manage(ui->edgeSearchDirect1CBox, this);
    caller->manage(ui->edgeSearchDirect2CBox, this);
    ui->edgeSearchDirect1CBox->setAsyncCaller(caller);
    ui->edgeSearchDirect2CBox->setAsyncCaller(caller);

    caller->manage(ui->shape1CBox, this);
    caller->manage(ui->shape2CBox, this);
    ui->shape1CBox->setAsyncCaller(caller);
    ui->shape2CBox->setAsyncCaller(caller);
}

void DetectionCommonSettingController::refreshEdgeSearchData() {
    int val1 = ui->edgeSearchDirect1CBox->currentValue();
    int val2 = ui->edgeSearchDirect2CBox->currentValue();
    if (val1 != -1) {
        eVal1 = val1;
    } else {
        val1 = eVal1;
    }
    if (val2 != -1) {
        eVal2 = val2;
    } else {
        val2 = eVal2;
    }
    ui->edgeSearchDirect1CBox
        ->loadItemFromDatabase(CBOX_AMP_EDGE_SEARCH, true, [this, val1]() {
            if (param && val1 == -1) {
                ui->edgeSearchDirect1CBox->setCurrentData(param->edgeSearchDirect1);
            }
            if (afterEdgeSearchLoaded) {
                QSet<int> waitRemoves;
                afterEdgeSearchLoaded(waitRemoves);
                if (waitRemoves.size() > 0) {
                    ui->edgeSearchDirect1CBox->removeItemByData(waitRemoves);
                }
                if (val1 != -1) {
                    ui->edgeSearchDirect1CBox->safeSetCurrentData(val1);
                }
            }
        });
    ui->edgeSearchDirect2CBox
        ->loadItemFromDatabase(CBOX_AMP_EDGE_SEARCH, true, [this, val2]() {
            if (param && val2 == -1) {
                ui->edgeSearchDirect2CBox->setCurrentData(param->edgeSearchDirect2);
            }
            if (afterEdgeSearchLoaded) {
                QSet<int> waitRemoves;
                afterEdgeSearchLoaded(waitRemoves);
                if (waitRemoves.size() > 0) {
                    ui->edgeSearchDirect2CBox->removeItemByData(waitRemoves);
                }
                if (val2 != -1) {
                    ui->edgeSearchDirect2CBox->safeSetCurrentData(val2);
                }
            }
        });
}
void DetectionCommonSettingController::refreshShapeData() {
    int val1 = ui->shape1CBox->currentValue();
    int val2 = ui->shape2CBox->currentValue();
    if (val1 != -1) {
        sVal1 = val1;
    } else {
        val1 = sVal1;
    }
    if (val2 != -1) {
        sVal2 = val2;
    } else {
        val2 = sVal2;
    }
    ui->shape1CBox->loadItemFromDatabase(CBOX_AMP_SHAPE, true, [this, val1]() {
        if (param && val1 == -1) {
            ui->shape1CBox->setCurrentData(param->shape1);
        }
        if (afterShapeLoaded) {
            QSet<int> waitRemoves;
            afterShapeLoaded(waitRemoves);
            if (waitRemoves.size() > 0) {
                ui->shape1CBox->removeItemByData(waitRemoves);
            }
            if (val1 != -1)
                ui->shape1CBox->safeSetCurrentData(val1);
        }
    });
    ui->shape2CBox->loadItemFromDatabase(CBOX_AMP_SHAPE, true, [this, val2]() {
        if (param && val2 == -1) {
            ui->shape2CBox->setCurrentData(param->shape2);
        }
        if (afterShapeLoaded) {
            QSet<int> waitRemoves;
            afterShapeLoaded(waitRemoves);
            if (waitRemoves.size() > 0) {
                ui->shape2CBox->removeItemByData(waitRemoves);
            }
            if (val2 != -1)
                ui->shape2CBox->safeSetCurrentData(val2);
        }
    });
}

void DetectionCommonSettingController::resetToComponent() {
    if (!param) {
        return;
    }
    ui->flyer1TextField->setText(param->flyer1.toQStringAndKeepFullDecimal());
    ui->flyer2TextField->setText(param->flyer2.toQStringAndKeepFullDecimal());
    ui->shape1CBox->safeSetCurrentData(param->shape1);
    ui->shape2CBox->safeSetCurrentData(param->shape2);
    ui->threshold1TextField->setText(param->threshold1);
    ui->threshold2TextField->setText(param->threshold2);
    ui->edgeNum1TextField->setText(param->edge1);
    ui->edgeNum2TextField->setText(param->edge2);
    ui->bLineStartPoint1TextField->setText(param->baseLineStartPoint1);
    ui->bLineStartPoint2TextField->setText(param->baseLineStartPoint2);
    ui->bLineArea1TextField->setText(param->baseLineArea1);
    ui->bLineArea2TextField->setText(param->baseLineArea2);
    ui->edgeSearchDirect1CBox->safeSetCurrentData(param->edgeSearchDirect1);
    ui->edgeSearchDirect2CBox->safeSetCurrentData(param->edgeSearchDirect2);
    ui->flyerCheckBox->setChecked(param->enableFlyer);
}
QString DetectionCommonSettingController::collectData(AmpMethodDetectionCommonParam& param) {
    // if (!param) {
    //     return "Detection setting param has not init yet.";
    // }
    if (ui->shape1Content->isVisible()) {
        param.shape1 = ui->shape1CBox->currentValue();
        if (param.shape1 < 0) {
            return QString("Shape %1 is not specified").arg(ui->title1Label->text());
        }
        param.shape2 = ui->shape2CBox->currentValue();
        if (param.shape2 < 0) {
            return QString("Shape %1 is not specified").arg(ui->title2Label->text());
        }
    }

    param.threshold1
        = fromTextFieldText(ui->threshold1TextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
    param.threshold2
        = fromTextFieldText(ui->threshold2TextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
    param.edge1 = fromTextFieldText(ui->edgeNum1TextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD)
                       .toInt();
    param.edge2 = fromTextFieldText(ui->edgeNum2TextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD)
                       .toInt();
    int val = fromTextFieldText(ui->bLineStartPoint1TextField,
                                                   VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD)
                                     .toInt();
    if (!checkLineStartPoint(val)) {
        return QString("Base Line Start Poit(Left) is invalid.");
    }
    param.baseLineStartPoint1 = val;
    val = fromTextFieldText(ui->bLineStartPoint2TextField,
                                                   VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD)
                                     .toInt();
    if (!checkLineStartPoint(val)) {
        return QString("Base Line Start Poit(Right) is invalid.");
    }
    param.baseLineStartPoint2 = val;
    param.baseLineArea1
        = fromTextFieldText(ui->bLineArea1TextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
    param.baseLineArea2
        = fromTextFieldText(ui->bLineArea2TextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD).toInt();
    param.edgeSearchDirect1 = ui->edgeSearchDirect1CBox->currentValue();
    if (param.edgeSearchDirect1 < 0) {
        return QString("Edge search Direction %1 is not specified").arg(ui->title1Label->text());
    }
    param.edgeSearchDirect2 = ui->edgeSearchDirect2CBox->currentValue();
    if (param.edgeSearchDirect2 < 0) {
        return QString("Edge search Direction %1 is not specified").arg(ui->title2Label->text());
    }
    if (ui->flyerCheckBox->isVisible() && ui->flyerCheckBox->isEnabled()) {
        param.enableFlyer = ui->flyerCheckBox->isChecked();
        param.flyer1 = fromTextFieldText(ui->flyer1TextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD);
        param.flyer2 = fromTextFieldText(ui->flyer2TextField, VEVM_MIN_MODE | VEVM_WITH_PLACE_HOLD);
    }
    return "";
}

void DetectionCommonSettingController::disableLeft(bool d) {
    ui->threshold1TextField->setDisabled(d);
    ui->edgeNum1TextField->setDisabled(d);
    ui->bLineStartPoint1TextField->setDisabled(d);
    ui->bLineArea1TextField->setDisabled(d);
    ui->edgeSearchDirect1CBox->setDisabled(d);
    ui->flyer1TextField->setDisabled(d);
}

void DetectionCommonSettingController::disableRight(bool d) {
    ui->threshold2TextField->setDisabled(d);
    ui->edgeNum2TextField->setDisabled(d);
    ui->bLineStartPoint2TextField->setDisabled(d);
    ui->bLineArea2TextField->setDisabled(d);
    ui->edgeSearchDirect2CBox->setDisabled(d);
    ui->flyer2TextField->setDisabled(d);
}
