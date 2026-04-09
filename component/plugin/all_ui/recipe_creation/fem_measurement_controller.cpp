#include "fem_measurement_controller.h"
#include "cde90_message_box.h"
#include "cdsem_number_validator_helper.h"
#include "fem_measurement_canvas.h"
#include "recipe_data_model.h"
#include "ui_fem_measurement_controller.h"

FemMeasurementController::FemMeasurementController(QWidget* parent) : CDE90Window(parent) {
    //如果你没有异步任务需求，请将connect清理掉，保留create和createAction函数
    create();
    createAction();
}

FemMeasurementController::~FemMeasurementController() noexcept {
    if (ui) {
        delete ui;
    }
}

void FemMeasurementController::create() {
    QWidget* content = new QWidget;
    setContent(content);
    ui = new cdsem::UiFemMeasurementController;
    CDE90LayoutHelper helper(content);
    helper.registUnknowType("femCanvas",
                            [this](QObject* parent,
                                   const QHash<QString, QObject*>&,
                                   QHash<QString, QList<QStringList>>&) -> QObject* {
                                QWidget* widget = dynamic_cast<QWidget*>(parent);
                                if (!widget) {
                                    return nullptr;
                                }
                                canvas = new FemMeasurementCanvas(widget);
                                return canvas;
                            });
    ui->init(helper);
    
}

void FemMeasurementController::createAction() {
    CDSemNumberValidatorHelper::range<3>(-100, 100)
        .put(ui->focusStartTextField)
        .put(ui->focusStepTextField)
        .put(ui->exposureStepTextField)
        .autoFixMode();
    CDSemNumberValidatorHelper::range<3>(0, 2000).put(ui->exposureStartTextField).autoFixMode();
    connect(ui->femTypeItemBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this]() {
        canvas->setType(ui->femTypeItemBox->currentValue());
    });
    connect(ui->lowLeftChipRadio, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) {
            canvas->toFromCenter(false);
        }
    });
    connect(ui->centerChipRadio, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked) {
            canvas->toFromCenter(true);
        }
    });
    bindCloseBtn(ui->cancelBtn->getQPushButton());
    CDSEM_OK_ACTION2(ui->okBtn->getQPushButton(), FemMeasureModel)
}

QString FemMeasurementController::collectData(
    FemMeasureModel& param) {
    param.femType = ui->femTypeItemBox->currentValue();
    if (param.femType < 0) {
        return "FEM direction is not specified.";
    }
    param.foucsStart = ui->focusStartTextField->text();
    param.foucsStep = ui->focusStepTextField->text();
    param.exposureStart = ui->exposureStartTextField->text();
    param.exposureStep = ui->exposureStepTextField->text();
    if (ui->lowLeftChipRadio->isChecked()) {
        param.startChip = 0;
    } else if (ui->centerChipRadio->isChecked()) {
        param.startChip = 1;
    } else {
        return "Start chip is not specified.";
    }
    return "";
}

void FemMeasurementController::resetToComponent(
    const FemMeasureModel& param) {
    ui->femTypeItemBox->setCurrentData(param.femType);
    ui->focusStartTextField->setText(param.foucsStart.toQStringAndKeepFullDecimal());
    ui->focusStepTextField->setText(param.foucsStep.toQStringAndKeepFullDecimal());
    ui->exposureStartTextField->setText(param.exposureStart.toQStringAndKeepFullDecimal());
    ui->exposureStepTextField->setText(param.exposureStep.toQStringAndKeepFullDecimal());
    if (param.startChip == 1) {
        ui->centerChipRadio->setChecked(true);
    } else {
        ui->lowLeftChipRadio->setChecked(true);
    }
}
