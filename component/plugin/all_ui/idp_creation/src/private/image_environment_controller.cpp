#include "image_environment_controller.h"
#include "cde90_message_box.h"
#include "img_env_setting_content.h"
#include "ui_image_environment_controller.h"

ImageEnvironmentController::ImageEnvironmentController(QWidget* parent) : CDE90Window(parent) {
    //如果你没有异步任务需求，请将connect清理掉，保留create和createAction函数
    create();
    createAction();
}

ImageEnvironmentController::~ImageEnvironmentController() noexcept {
    if (ui) {
        delete ui;
    }
}

void ImageEnvironmentController::create() {
    setContent(new QWidget);
    CDE90LayoutHelper helper(getContent());
    helper.registUnknowType("alignContent",
                            [this](QObject* parent,
                                   const QHash<QString, QObject*>&,
                                   QHash<QString, QList<QStringList>>&) -> QObject* {
                                QWidget* widget = dynamic_cast<QWidget*>(parent);
                                if (!widget) {
                                    return nullptr;
                                }
                                alignContent = new ImgEnvSettingContent(widget);
                                alignContent->initParam(&param.alignment);
                                return alignContent;
                            });
    helper.registUnknowType("addressContent",
                            [this](QObject* parent,
                                   const QHash<QString, QObject*>&,
                                   QHash<QString, QList<QStringList>>&) -> QObject* {
                                QWidget* widget = dynamic_cast<QWidget*>(parent);
                                if (!widget) {
                                    return nullptr;
                                }
                                addressContent = new ImgEnvSettingContent(widget);
                                addressContent->initParam(&param.addressing);
                                return addressContent;
                            });
    helper.registUnknowType("measureContent",
                            [this](QObject* parent,
                                   const QHash<QString, QObject*>&,
                                   QHash<QString, QList<QStringList>>&) -> QObject* {
                                QWidget* widget = dynamic_cast<QWidget*>(parent);
                                if (!widget) {
                                    return nullptr;
                                }
                                measureContent = new ImgEnvSettingContent(widget);
                                measureContent->initParam(&param.measurement);
                                return measureContent;
                            });
    ui = new cdsem::UiImageEnvironmentController;
    ui->init(helper);
    
}

void ImageEnvironmentController::createAction() {
    bindCloseBtn(ui->cancelBtn->getQPushButton());
    SEM_OK_ACTION(ui->okBtn->getQPushButton())
}

void ImageEnvironmentController::resetToComponent() {
    ui->useSysParamCBox->setChecked(param.useSysParam);
    alignContent->resetToComponent();
    addressContent->resetToComponent();
    measureContent->resetToComponent();
}
QString ImageEnvironmentController::collectData() {
    param.useSysParam = ui->useSysParamCBox->isChecked();
    QString errMsg = alignContent->collectData();
    if (errMsg.length() > 0) {
        return QString("Alignment: %1").arg(errMsg);
    }
    errMsg = addressContent->collectData();
    if (errMsg.length() > 0) {
        return QString("Addressing: %1").arg(errMsg);
    }
    errMsg = measureContent->collectData();
    if (errMsg.length() > 0) {
        return QString("Measurement: %1").arg(errMsg);
    }
    return "";
}
