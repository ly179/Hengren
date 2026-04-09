#include "image_info_controller.h"
#include "ui_image_info_controller.h"
#include "key_value_view_helper.h"
#include "image_save_model.h"
#include "cbox_global_keys.h"
#include "service_manager.h"
#include "cbox_config_service.h"
#include "sem_global.h"

ImageInfoController::ImageInfoController(QWidget* parent) : CDE90Window(parent) {
    connect(this, &ImageInfoController::asyncCallerChanged, this, [this](){
        create();
        createAction();
    }, Qt::SingleShotConnection);
}

ImageInfoController::~ImageInfoController() noexcept {
    if (ui) {
        delete ui;
    }
}

void ImageInfoController::create() {
    QWidget* content = new QWidget;
    setContent(content);
    ui = new cdsem::UiImageInfoController;
    ui->init(content);
}

void ImageInfoController::createAction() {
    bindCloseBtn(ui->closeBtn->getQPushButton());
}

void ImageInfoController::setImageInfo(const ImageAddtionDataModel& addition) {
    if (addition.photoImagetype == cdsem::PIT_OM) {
        KeyValueViewHelper helper;
        helper.append("Rotation", addition.getImageRot().toMinString());
        helper.append("Magnification", QString("x%1").arg(addition.mag));
        helper.append("Brightness", addition.getBrightness().toMinString());
        if (addition.waferPos.x != nullptr && addition.waferPos.y != nullptr) {
            FloatNumber9 x = addition.waferPos.x * 1000;
            FloatNumber9 y = addition.waferPos.y * 1000;
            helper.append("Stage Pos", QString("%1 x %2 mm").arg(x.toQString(4), y.toQString(4)));
        }
        helper.toHtml(ui->imageInfoTextArea);
    } else if (addition.photoImagetype == cdsem::PIT_SEM) {
        int scanRate = addition.scanRate;
        ui->imageInfoTextArea->setText("Loading...");
        ACaller->push(this, [this, scanRate, addition](TSC){
            CboxConfigService* service = ServiceManager::defaultService<CboxConfigService>();
            CBoxMixKey key = CBoxMixKey(CBOX_TYPE_SEM_IMG_SCAN_RATE, scanRate);
            QString scanRateText;
            if (service->cacheText(key)) {
                scanRateText = "Error";
            } else {
                scanRateText = service->getText(key);
            }
            MainThreadInvoker::getInvoker().runLater([this, scanRateText, addition](){
                KeyValueViewHelper helper;
                if (addition.realSize.isValid()) {
                    if (addition.originSize.isValid() && addition.realSize != addition.originSize) {
                        helper.append("Real Size", QString("%1x%2").arg(addition.realSize.width()).arg(addition.realSize.height()));
                        helper.append("Original Size", QString("%1x%2").arg(addition.originSize.width()).arg(addition.originSize.height()));
                    } else {
                        helper.append("Pixel Size", QString("%1x%2").arg(addition.realSize.width()).arg(addition.realSize.height()));
                    }
                } else if (addition.originSize.isValid()) {
                    if (addition.realSize.isValid() && addition.realSize != addition.originSize) {
                        helper.append("Real Size", QString("%1x%2").arg(addition.realSize.width()).arg(addition.realSize.height()));
                        helper.append("Original Size", QString("%1x%2").arg(addition.originSize.width()).arg(addition.originSize.height()));
                    } else {
                        helper.append("Pixel Size", QString("%1x%2").arg(addition.originSize.width()).arg(addition.originSize.height()));
                    }
                } else {
                    helper.append("Pixel Size", "-");
                }
                if (addition.waferPos.x != nullptr && addition.waferPos.y != nullptr)
                    helper.append("Wafer Position", QString("(%1,%2)µm").arg((addition.waferPos.x * 1000'000).toMinString(3), (addition.waferPos.y * 1000'000).toMinString(3)));
                else
                    helper.append("Wafer Position", "-");
                helper.append("EHT", QString("%1 V").arg(toMinQString(addition.eht, 3)));
                helper.append("Beam Offset", QString("(%1,%2)µm").arg(FloatNumber3(addition.beamPos.x() * 1000'000).toMinString(), FloatNumber3(addition.beamPos.y() * 1000'000).toMinString()));
                helper.append("Stage Offset", QString("(%1,%2)µm").arg(FloatNumber3(addition.stagePos.x() * 1000'000).toMinString(), FloatNumber3(addition.stagePos.y() * 1000'000).toMinString()));
                helper.append("Magnification", QString("x%1").arg(addition.mag));
                helper.append("Aperture", QString("%1%,%2%").arg(addition.aperture.x()).arg(addition.aperture.y()));
                helper.append("Stigma", QString("%1%,%2%").arg(addition.stigma.x()).arg(addition.stigma.y()));
                helper.append("Pixel Ratio", QString("%1").arg(FloatNumber3(addition.pixelSize).toMinString()));
                helper.append("SignalA", QString("%1").arg(addition.signalA.signalName));
                helper.append("SignalA Brightness", QString("%1%").arg(FloatNumber3(addition.signalA.brightness).toMinString()));
                helper.append("SignalA Contrast", QString("%1%").arg(FloatNumber3(addition.signalA.contrast).toMinString()));
                if (addition.mixSignal) {
                    helper.append("SignalB", QString("%1%").arg(addition.signalB.signalName));
                    helper.append("SignalB Brightness", QString("%1%").arg(FloatNumber3(addition.signalB.brightness).toMinString()));
                    helper.append("SignalB Contrast", QString("%1%").arg(FloatNumber3(addition.signalB.contrast).toMinString()));
                }
                helper.append("Rotation", QString("%1°").arg(FloatNumber3(addition.imageRot).toMinString()));
                helper.append("Frame", QString("%1").arg(FloatNumber3(addition.frame).toMinString()));
                helper.append("Enable IProbe", QString("%1").arg(addition.optiProbe ? "Yes" : "No"));
                helper.append("Aperture No.", QString("%1").arg(addition.aptNum < 0 ? "-" : QString::number(addition.aptNum + 1)));
                helper.append("IProbe", QString("%1pA").arg(FloatNumber3(addition.iProbe).toMinString()));
                helper.append("Work Distance", QString("%1mm").arg(FloatNumber4(addition.workDistance).toMinString()));
                helper.append("Scan Rate", QString("%1").arg(scanRateText));
                if (addition.imageType == cdsem::SIT_PHOTO) {
                    helper.append("Scan", "Integ");
                } else if (addition.imageType == cdsem::SIT_LIVE) {
                    helper.append("Scan", "View");
                }
                helper.toHtml(ui->imageInfoTextArea);
            });
        });
    } else {
        ui->imageInfoTextArea->clear();
    }
}
void ImageInfoController::setImageInfo(const QString& html) {
    ui->imageInfoTextArea->setHtml(html);
}

void ImageInfoController::setName(const QString& text) {
    ui->imageNameTextField->setText(text);
}
