#include "focus_auto_content.h"
#include "auto_focus_offset_setting_controller.h"
#include "auto_focus_threshold_setting_controller.h"
#include "cbox_global_keys.h"
#include "cdsem_number_validator_helper.h"
#include "template_data_model.h"
#include "ui_focus_auto_content.h"
#include "cdsem_global_data.h"

struct FocusAutoContentDataHelper {
    //IDP中的MP的dx、dy与原点
    //用于reg按钮运算相对坐标使用，如果aPos为空，则reg按钮不可用
    IDW_PointF aPos;
    AutoFocusSettingParam* param = nullptr;
    AsyncCaller* caller = nullptr;
    AutoFocusOffsetSettingController* offsetSettingController = nullptr;
    AutoFocusThresholdSettingController* thresholdSettingController = nullptr;
    bool asAddress = false;
    bool needCheckParamValid = true;
};

FocusAutoContent::FocusAutoContent(
    QWidget* parent)
    : CDE90Frame(parent) {
    //如果你没有异步任务需求，请将connect清理掉，保留create和createAction函数
    create();
    createAction();
}

FocusAutoContent::FocusAutoContent(
    bool asAddress, QWidget* parent)
    : CDE90Frame(parent) {
    //如果你没有异步任务需求，请将connect清理掉，保留create和createAction函数
    dataHelper = new FocusAutoContentDataHelper;
    dataHelper->asAddress = asAddress;
    create();
    createAction();
}

FocusAutoContent::~FocusAutoContent() noexcept {
    if (dataHelper) delete dataHelper;
    if (ui) {
        delete ui;
    }
}

void FocusAutoContent::create() {
    ui = new cdsem::UiFocusAutoContent;
    ui->init(this);
    ui->chargingVoltageLabel->setVisible(!dataHelper->asAddress);
    ui->chargingVoltageContent->setVisible(!dataHelper->asAddress);
}

void FocusAutoContent::createAction() {
    CDSemNumberValidatorHelper::range<3>(-200, 200)
        .put(ui->offsetTextField->getQLineEdit())
        .autoFixMode();

    CDSemNumberValidatorHelper::range(0, 999).autoFixMode().put(
        ui->thresholdTextField->getQLineEdit());
    CDSemNumberValidatorHelper::intFree().autoFixMode().put(ui->magTextField->getQLineEdit());

    CDSemNumberValidatorHelper::doubleFree<IDW_ACCURACY>()
        .autoFixMode()
        .put(ui->dxTextField->getQLineEdit())
        .put(ui->dyTextField->getQLineEdit());
    CDSemNumberValidatorHelper::range<1>("0", "10").autoFixMode().put(
        ui->waitTextField->getQLineEdit());

    connect(ui->offsetBtn->getQPushButton(), &CDE90Button::clicked, this, [this]() {
        createOffsetSettingController(ui->methodComboBox->currentData().toInt());
        MainThreadInvoker::getInvoker().moveToParentCenter(this, dataHelper->offsetSettingController);
        dataHelper->offsetSettingController->setInitParam(ui->offsetTextField->text().toInt());
        dataHelper->offsetSettingController->show();
    });

    connect(ui->thresholdBtn->getQPushButton(), &CDE90Button::clicked, this, [this]() {
        createThresholdSettingController();
        MainThreadInvoker::getInvoker().moveToParentCenter(this, dataHelper->thresholdSettingController);
        dataHelper->thresholdSettingController->setInitParam(ui->thresholdTextField->text().toInt());
        dataHelper->thresholdSettingController->show();
    });

    connect(ui->readBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        ui->magTextField->setText(SEM_PY_ENV.getMag());
        ui->frameItemBox->safeSetCurrentData(SEM_PY_ENV.getFrame());
        ui->scanRateItemBox->safeSetCurrentData(SEM_PY_ENV.getScanRate());
    });

    connect(ui->regBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        if (dataHelper->aPos.x == nullptr || dataHelper->aPos.y == nullptr) return;
        IDW_PointF pos = cdsem::WaferStageCoordinateConvert::instance()->waferPosUm();
        QPointF beamPos = SEM_PY_ENV.getBeamPos();
        pos = cdsem::WaferStageCoordinateConvert::instance()->fieldPos(pos, IDW_PointF{beamPos.x() * 1000'000, beamPos.y() * 1000'000});
        ui->dxTextField->setText((pos.x - dataHelper->aPos.x).toMinString());
        ui->dyTextField->setText((pos.y - dataHelper->aPos.y).toMinString());
    });
    ui->regBtn->setDisabled(true);
}

void FocusAutoContent::createOffsetSettingController(
    int mode) {
    if (!dataHelper->offsetSettingController) {
        dataHelper->offsetSettingController = new AutoFocusOffsetSettingController(dataHelper->asAddress, mode, this);
        dataHelper->offsetSettingController->setWindowModality(Qt::WindowModal);
        dataHelper->offsetSettingController->setWindowTitle("Auto Focus Offset");
        dataHelper->offsetSettingController->setResizable(false);
        dataHelper->offsetSettingController->initAsyncCaller(dataHelper->caller);
        dataHelper->offsetSettingController->setOkCallback([this](int offset) {
            ui->offsetTextField->setText(offset);
            return true;
        });
    } else {
        dataHelper->offsetSettingController->changeMode(mode);
    }
}

void FocusAutoContent::createThresholdSettingController() {
    if (!dataHelper->thresholdSettingController) {
        dataHelper->thresholdSettingController = new AutoFocusThresholdSettingController(this);
        dataHelper->thresholdSettingController->setWindowModality(Qt::WindowModal);
        dataHelper->thresholdSettingController->setWindowTitle("Auto Focus Threshold");
        dataHelper->thresholdSettingController->setResizable(false);
        dataHelper->thresholdSettingController->initAsyncCaller(dataHelper->caller);
        dataHelper->thresholdSettingController->setOkCallback([this](int threshold) {
            ui->thresholdTextField->setText(threshold);
            return true;
        });
    }
}

void FocusAutoContent::reloadItemBox() {
    if (!dataHelper->caller) {
        return;
    }
    if (dataHelper->asAddress) {
        ui->methodComboBox->loadItemFromDatabase(CBOX_AP_AUTO_FOCUS_METHOD);
    } else {
        ui->methodComboBox->loadItemFromDatabase(CBOX_MP_AUTO_FOCUS_METHOD);
    }
    ui->frameItemBox->loadItemFromDatabase(CBOX_TYPE_SEM_IMG_FRAME);
    ui->scanRateItemBox->loadItemFromDatabase(CBOX_TYPE_SEM_IMG_SCAN_RATE);
    ui->pixelItemBox->loadItemFromDatabase(CBOX_TYPE_SEM_IMG_X_PIX_COUNT);
}

void FocusAutoContent::initParam(
    AutoFocusSettingParam* param) {
    dataHelper->param = param;
}

void FocusAutoContent::initAsyncCaller(
    AsyncCaller* caller) {
    if (dataHelper->caller) {
        return;
    }
    dataHelper->caller = caller;
    caller->manage(ui->methodComboBox, this);
    ui->methodComboBox->setAsyncCaller(caller);
    caller->manage(ui->frameItemBox, this);
    ui->frameItemBox->setAsyncCaller(caller);
    caller->manage(ui->scanRateItemBox, this);
    ui->scanRateItemBox->setAsyncCaller(caller);
    caller->manage(ui->pixelItemBox, this);
    ui->pixelItemBox->setAsyncCaller(caller);
    reloadItemBox();
}

void FocusAutoContent::resetToComponent() {
    ui->autoFocusCbox->setChecked(dataHelper->param->enable);
    ui->dxTextField->setText(dataHelper->param->relativePos.x.toMinString());
    ui->dyTextField->setText(dataHelper->param->relativePos.y.toMinString());
    ui->waitTextField->setText(dataHelper->param->waitSeconds.toMinString());
    ui->methodComboBox->safeSetCurrentData(dataHelper->param->method);
    ui->frameItemBox->safeSetCurrentData(dataHelper->param->frame);
    ui->scanRateItemBox->safeSetCurrentData(dataHelper->param->scanRate);
    ui->pixelItemBox->safeSetCurrentData(dataHelper->param->pixelXY);
    ui->offsetTextField->setText(dataHelper->param->offset.toMinString());
    ui->thresholdTextField->setText(dataHelper->param->threshold.toString());
    ui->lowChargeVoltTextField->setText(dataHelper->param->lowChargVolt.toString());
    ui->highChargeVoltTextField->setText(dataHelper->param->highChargVolt.toString());
    if (dataHelper->param->mag == nullptr) {
        ui->magTextField->setText(SEM_PY_ENV.getMag());
    } else {
        ui->magTextField->setText(dataHelper->param->mag.toString());
    }
}

QString FocusAutoContent::collectData() {
    dataHelper->param->enable = ui->autoFocusCbox->isChecked();
    QString val;
    val = ui->dxTextField->text();
    if (val.isEmpty()) {
        dataHelper->param->relativePos.x.clear();
    } else {
        dataHelper->param->relativePos.x = val;
    }
    val = ui->dyTextField->text();
    if (val.isEmpty()) {
        dataHelper->param->relativePos.y.clear();
    } else {
        dataHelper->param->relativePos.y = val;
    }
    val = ui->magTextField->text();
    if (val.isEmpty()) {
        dataHelper->param->mag.setNull();
    } else {
        dataHelper->param->mag = val.toInt();
    }
    val = ui->waitTextField->text();
    if (val.isEmpty()) {
        dataHelper->param->waitSeconds.clear();
    } else {
        dataHelper->param->waitSeconds = val;
    }
    val = ui->methodComboBox->currentData();
    if (dataHelper->needCheckParamValid && dataHelper->param->enable && val.isEmpty()) {
        return "Auto focus method is not specified.";
    }
    dataHelper->param->method = val.isEmpty() ? -1 : val.toInt();

    val = ui->pixelItemBox->currentData();
    if (dataHelper->needCheckParamValid && dataHelper->param->enable && val.isEmpty()) {
        return "Auto focus Pixel XY is not specified.";
    }
    dataHelper->param->pixelXY = val.isEmpty() ? -1 : val.toInt();


    val = ui->frameItemBox->currentData();
    if (dataHelper->needCheckParamValid && dataHelper->param->enable && val.isEmpty()) {
        return "Auto focus frame is not specified.";
    }
    dataHelper->param->frame = val.isEmpty() ? -1 : val.toInt();
    val = ui->scanRateItemBox->currentData();
    if (dataHelper->needCheckParamValid && dataHelper->param->enable && val.isEmpty()) {
        return "Auto focus scan rate is not specified.";
    }
    dataHelper->param->scanRate = val.isEmpty() ? -1 : val.toInt();


    val = ui->offsetTextField->text();
    if (val.isEmpty()) {
        dataHelper->param->offset.clear();
    } else {
        dataHelper->param->offset = val.toFloat();
    }
    val = ui->thresholdTextField->text();
    if (val.isEmpty()) {
        dataHelper->param->threshold.setNull();
    } else {
        dataHelper->param->threshold = val.toInt();
    }
    return "";
}

//禁用一切与机台互动相关的操作
void FocusAutoContent::disablePyHandlers(bool b) {
    ui->regBtn->getQPushButton()->setDisabled(b);
}

void FocusAutoContent::setAPos(const IDW_PointF& aPos) {
    dataHelper->aPos = aPos;
    ui->regBtn->setDisabled(aPos.x == nullptr || aPos.y == nullptr);
}

void FocusAutoContent::enableCheckParamValid(bool b) {
    dataHelper->needCheckParamValid = b;
}
