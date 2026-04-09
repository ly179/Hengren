#include "axis_alignment_wafer_controller.h"
#include <QLayout>
#include "axis_align_detail_content.h"
#include "axis_align_wafer_selector_controller.h"
#include "cde90_message_box.h"
#include "cdsem_number_validator_helper.h"
#include "sem_global.h"
#include "ui_axis_alignment_wafer_controller.h"

AxisAlignmentWaferController::AxisAlignmentWaferController(
    QWidget* parent)
    : CDE90Window(parent) {
    connect(this, &AxisAlignmentWaferController::asyncCallerChanged, this, [this](){
        create();
        createAction();
    }, Qt::SingleShotConnection);
}

AxisAlignmentWaferController::~AxisAlignmentWaferController() noexcept {
    if (ui) {
        delete ui;
    }
}

void AxisAlignmentWaferController::create() {
    ui = new cdsem::UiAxisAlignmentWaferController;
    setContent(new QWidget);
    ui->init(getContent());

    axisAlignmentContent = new AxisAlignDetailContent(ui->detailContent);
    stigmaContent = new AxisAlignDetailContent(1, ui->detailContent);

    axisAlignmentContent->setLineWidth(1);
    axisAlignmentContent->setFrameStyle(QFrame::Box | QFrame::Sunken);
    stigmaContent->setLineWidth(1);
    stigmaContent->setFrameStyle(QFrame::Box | QFrame::Sunken);

    ui->detailContent->layout()->addWidget(axisAlignmentContent);
    ui->detailContent->layout()->addWidget(stigmaContent);
}

void AxisAlignmentWaferController::createAction() {
    CDSemNumberValidatorHelper::intFree()
        .put(ui->chipXTextField)
        .put(ui->chipYTextField)
        .put(ui->chipXDeviationTextField)
        .put(ui->chipYDeviationTextField)
        .autoFixMode();
    CDSemNumberValidatorHelper::min(0).put(ui->memNo1).put(ui->memNo2).put(ui->memNo3).autoFixMode();
    axisAlignmentContent->initParam(&param.alignContentParam);
    stigmaContent->initParam(&param.stigmaContentParam);
    itsChildAndBeManaged(axisAlignmentContent, this);
    itsChildAndBeManaged(stigmaContent, this);
    axisAlignmentContent->initAsyncCaller(ACaller);
    stigmaContent->initAsyncCaller(ACaller);
    bindCloseBtn(ui->cancelBtn->getQPushButton());
    SEM_OK_ACTION(ui->okBtn->getQPushButton())
    connect(ui->chipMoreBtn->getQPushButton(), &QPushButton::clicked, this, [this]() {
        createWaferSelector();
    });
    connect(ui->updateSemConditionCheckBox, &QCheckBox::toggled, this, [this](bool checked) {
        ui->memNo1->setEnabled(checked);
        ui->memNo2->setEnabled(checked);
        ui->memNo3->setEnabled(checked);
    });
    connect(axisAlignmentContent, &AxisAlignDetailContent::shouldHideParent, this, [this](bool v){
        this->setVisible(!v);
    });
    connect(stigmaContent, &AxisAlignDetailContent::shouldHideParent, this, [this](bool v){
        this->setVisible(!v);
    });
}

void AxisAlignmentWaferController::createWaferSelector() {
    waferSelector = new AxisAlignWaferSelectorController(this);
    waferSelector->initAsyncCaller(ACaller);
    waferSelector->setWindowModality(Qt::WindowModal);
    waferSelector->setAttribute(Qt::WA_DeleteOnClose);
    waferSelector->setWindowTitle("Chip Select");
    waferSelector->setResizable(false);
    waferSelector->setOkCallback([this](const AxisAlignSelectedChip& r) {
        ui->chipXTextField->setText(r.x);
        ui->chipYTextField->setText(r.y);
        // param.chipParam.chipNumber = r.number;
        param.chipParam.x = r.x;
        param.chipParam.y = r.y;
    });
    waferSelector->show();
    Integer32 x, y;
    QString str = ui->chipXTextField->text();
    if (str.length() > 0) {
        x = str.toInt();
    }
    str = ui->chipYTextField->text();
    if (str.length() > 0) {
        y = str.toInt();
    }
    waferSelector->initIdwId(67, x, y);
    MainThreadInvoker::getInvoker().moveToParentCenter(this, waferSelector);
}

void AxisAlignmentWaferController::resetToComponent() {
    ui->axisAlignCheckBox->setChecked(param.enableAxis);
    ui->manualAssistCBox->setCurrentData(param.manualAssist == -1 ? 0 : param.manualAssist);
    ui->chipXTextField->setText(param.chipParam.x.toString());
    ui->chipYTextField->setText(param.chipParam.y.toString());
    ui->chipXDeviationTextField->setText(param.chipParam.deviationX.toString());
    ui->chipYDeviationTextField->setText(param.chipParam.deviationY.toString());
    axisAlignmentContent->resetToComponent();
    stigmaContent->resetToComponent();
    ui->confirmCheckBox->setChecked(param.enableConfirm);
    ui->updateSemConditionCheckBox->setChecked(param.enableUpdateSemCondition);
    ui->memNo1->setText(param.memNo1.toString());
    ui->memNo2->setText(param.memNo2.toString());
    ui->memNo3->setText(param.memNo3.toString());
}
QString AxisAlignmentWaferController::collectData() {
    param.enableAxis = ui->axisAlignCheckBox->isChecked();
    param.manualAssist = ui->manualAssistCBox->currentValue();
    if (param.enableAxis && param.manualAssist < 0) {
        return "Manual Assist is not specified.";
    }
    QString str = ui->chipXTextField->text();
    if (str.length() > 0) {
        param.chipParam.x = str.toInt();
    }
    str = ui->chipYTextField->text();
    if (str.length() > 0) {
        param.chipParam.y = str.toInt();
    }
    if ((param.chipParam.x == nullptr) != (param.chipParam.y == nullptr)) {
        return "Chip X and Y must be either both specified or both left unspecified.";
    }
    str = ui->chipXDeviationTextField->text();
    if (str.length() > 0) {
        param.chipParam.deviationX = str.toInt();
    }
    str = ui->chipYDeviationTextField->text();
    if (str.length() > 0) {
        param.chipParam.deviationY = str.toInt();
    }
    QString errMsg = axisAlignmentContent->collectData();
    if (errMsg.length() > 0) {
        return errMsg;
    }
    errMsg = stigmaContent->collectData();
    if (errMsg.length() > 0) {
        return errMsg;
    }
    param.enableConfirm = ui->confirmCheckBox->isChecked();
    param.enableUpdateSemCondition = ui->updateSemConditionCheckBox->isChecked();
    str = ui->memNo1->text();
    if (str.length() > 0) {
        param.memNo1 = str.toInt();
    }
    str = ui->memNo2->text();
    if (str.length() > 0) {
        param.memNo2 = str.toInt();
    }
    str = ui->memNo3->text();
    if (str.length() > 0) {
        param.memNo3 = str.toInt();
    }
    return "";
}

void AxisAlignmentWaferController::setGroup(
    const QString& group) {
    axisAlignmentContent->setGroup(QString("mp_axis_alignment_%1").arg(group));
    stigmaContent->setGroup(QString("mp_axis_stigma_%1").arg(group));
}
