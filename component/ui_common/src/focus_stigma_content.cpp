#include "focus_stigma_content.h"
#include "auto_focus_offset_setting_controller.h"
#include "cbox_global_keys.h"
#include "cdsem_number_validator_helper.h"
#include "template_data_model.h"
#include "ui_focus_stigma_content.h"
#include "cdsem_global_data.h"

struct FocusStigmaContentHelper {
    // IDP的原点 + dx/dy的坐标
    //用于reg按钮运算相对坐标使用，如果dPos为空，则reg按钮不可用
    IDW_PointF aPos;
    AutoStigmaSettingParam* param = nullptr;
    AsyncCaller* caller = nullptr;
    AutoFocusOffsetSettingController* offsetSettingController = nullptr;
    bool asAddress = false;
    bool needCheckParamValid = true;
};

FocusStigmaContent::FocusStigmaContent(
    QWidget* parent)
    : CDE90Frame(parent) {
    create();
    createAction();
}
FocusStigmaContent::FocusStigmaContent(
    bool asAddress, QWidget* parent)
    : CDE90Frame(parent) {
    dataHelper = new FocusStigmaContentHelper;
    dataHelper->asAddress = asAddress;
    create();
    createAction();
}

FocusStigmaContent::~FocusStigmaContent() noexcept {
    if (dataHelper) delete dataHelper;
    if (ui) {
        delete ui;
    }
}

void FocusStigmaContent::create() {
    ui = new cdsem::UiFocusStigmaContent;
    ui->init(this);
}

void FocusStigmaContent::createAction() {
    CDSemNumberValidatorHelper::range<3>(-200, 200).autoFixMode().put(ui->offsetTextField
                                                                          ->getQLineEdit());

    CDSemNumberValidatorHelper::intFree().autoFixMode().put(ui->magTextField->getQLineEdit());

    CDSemNumberValidatorHelper::doubleFree<IDW_ACCURACY>()
        .autoFixMode()
        .put(ui->dxTextField->getQLineEdit())
        .put(ui->dyTextField->getQLineEdit());
    CDSemNumberValidatorHelper::range<1>("0", "10").autoFixMode().put(
        ui->waitTextField->getQLineEdit());

    connect(ui->offsetBtn->getQPushButton(), &CDE90Button::clicked, this, [this]() {
        createOffsetSettingController(ui->modeComboBox->currentData().toInt());
        MainThreadInvoker::getInvoker().moveToParentCenter(this, dataHelper->offsetSettingController);
        dataHelper->offsetSettingController->setInitParam(ui->offsetTextField->text().toInt());
        dataHelper->offsetSettingController->show();
    });

    connect(ui->readBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        ui->magTextField->setText(SEM_PY_ENV.getMag());
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

void FocusStigmaContent::createOffsetSettingController(
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

void FocusStigmaContent::reloadItemBox() {
    if (!dataHelper->caller) {
        return;
    }
    ui->modeComboBox->loadItemFromDatabase(CBOX_AUTO_STIGMA_MODE);
}

void FocusStigmaContent::initParam(
    AutoStigmaSettingParam* param) {
    dataHelper->param = param;
}

void FocusStigmaContent::initAsyncCaller(
    AsyncCaller* caller) {
    if (dataHelper->caller) {
        return;
    }
    dataHelper->caller = caller;
    caller->manage(ui->modeComboBox, this);
    ui->modeComboBox->setAsyncCaller(caller);
    reloadItemBox();
}

void FocusStigmaContent::resetToComponent() {
    ui->autoStigmaCbox->setChecked(dataHelper->param->enable);
    ui->dxTextField->setText(dataHelper->param->relativePos.x.toMinString());
    ui->dyTextField->setText(dataHelper->param->relativePos.y.toMinString());
    ui->waitTextField->setText(dataHelper->param->waitSeconds.toMinString());
    ui->modeComboBox->setCurrentData(dataHelper->param->mode);
    ui->offsetTextField->setText(dataHelper->param->offset.toMinString());
    if (dataHelper->param->mag == nullptr) {
        ui->magTextField->setText(SEM_PY_ENV.getMag());
    } else {
        ui->magTextField->setText(dataHelper->param->mag.toString());
    }
}

QString FocusStigmaContent::collectData() {
    dataHelper->param->enable = ui->autoStigmaCbox->isChecked();
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
    val = ui->waitTextField->text();
    if (val.isEmpty()) {
        dataHelper->param->waitSeconds.clear();
    } else {
        dataHelper->param->waitSeconds = val;
    }
    val = ui->magTextField->text();
    if (val.isEmpty()) {
        dataHelper->param->mag.setNull();
    } else {
        dataHelper->param->mag = val.toInt();
    }
    val = ui->modeComboBox->currentData();
    if (dataHelper->needCheckParamValid && dataHelper->param->enable && val.isEmpty()) {
        return "Auto Stigma mode is not specified.";
    }
    dataHelper->param->mode = val.isEmpty() ? -1 : val.toInt();
    val = ui->offsetTextField->text();
    if (val.isEmpty()) {
        dataHelper->param->offset.clear();
    } else {
        dataHelper->param->offset = val.toFloat();
    }
    return "";
}

//禁用一切与机台互动相关的操作
void FocusStigmaContent::disablePyHandlers(bool b) {
    ui->regBtn->getQPushButton()->setDisabled(b);
}

void FocusStigmaContent::setAPos(const IDW_PointF& aPos) {
    dataHelper->aPos = aPos;
    ui->regBtn->setDisabled(aPos.x == nullptr || aPos.y == nullptr);
}

void FocusStigmaContent::enableCheckParamValid(bool b) {
    dataHelper->needCheckParamValid = b;
}

