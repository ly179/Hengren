#include "pattern_reg_info_controller.h"
#include <QGridLayout>
#include "cde90_label.h"
#include "cbox_global_keys.h"
#include "cde90_widget.h"
#include "sem_global.h"
#include "ui_pattern_reg_info_controller.h"
#include "service_manager.h"
#include "cbox_config_service.h"
#include "cde90_message_box.h"

struct PatternRegInfoControllerDataHelper {
    PatternRegInfoParam param;
    IDW_PointF relativePos;
    PatternRegInfoController* d;

    QWidget* infoLabelWidget;
    QGridLayout* infoLabelLayout;

    int labelCounter = 0;

    CDE90Label* addLabelInfo(const QString& name, const QString& value) {
        CDE90Label* keyLabel = new CDE90Label(name, infoLabelWidget);
        CDE90Label* valueLabel = new CDE90Label(value, infoLabelWidget);
        infoLabelLayout->addWidget(keyLabel, labelCounter, 0, Qt::AlignRight);
        infoLabelLayout->addWidget(valueLabel, labelCounter, 1, Qt::AlignLeft);
        ++ labelCounter;
        return valueLabel;
    }

    void showAllLabels() {
        const QObjectList& children = infoLabelWidget->children();
        QWidget* widget;
        for (int i = 0; i < children.size(); ++ i) {
            widget = qobject_cast<QWidget*>(children[i]);
            if (!widget) continue;
            infoLabelLayout->removeWidget(widget);
            widget->deleteLater();
        }
        labelCounter = 0;
        const ImageAddtionDataModel& envInfo = param.imageAdditionData;
        if (envInfo.photoImagetype == cdsem::PIT_OM) {
            addLabelInfo("Rotation:", envInfo.getImageRot().toMinString());
            addLabelInfo("Magnification:", envInfo.mag <= 0 ? "" : QString("x%1").arg(envInfo.mag));
            addLabelInfo("Brightness:", envInfo.getBrightness().toMinString());
            if (param.imageAdditionData.waferPos.x != nullptr && param.imageAdditionData.waferPos.y != nullptr) {
                FloatNumber9 x = param.imageAdditionData.waferPos.x * 1000;
                FloatNumber9 y = param.imageAdditionData.waferPos.y * 1000;
                addLabelInfo("Stage Pos:", QString("%1 x %2 mm").arg(x.toQString(4), y.toQString(4)));
            }
        } else if (envInfo.photoImagetype == cdsem::PIT_SEM) {
            CboxConfigService* service = ServiceManager::defaultService<CboxConfigService>();
            CBoxMixKey key = CBoxMixKey(CBOX_TYPE_SEM_IMG_SCAN_RATE, envInfo.scanRate);
            bool scanRateTextCached;
            addLabelInfo("Size:", envInfo.originSize.width() <= 0 || envInfo.originSize.height() <= 0 ? "" : QString("%1x%2 Pixel").arg(envInfo.originSize.width()).arg(envInfo.originSize.height()));
            addLabelInfo("Rotation:", envInfo.getImageRot().toMinString());
            addLabelInfo("Magnification:", envInfo.mag <= 0 ? "" : QString("x%1").arg(envInfo.mag));
            addLabelInfo("Frame:", envInfo.frame <= 0 ? "" : QString::number(envInfo.frame));
            QPointer<CDE90Label> rateLabel = addLabelInfo("Scan Rate:", service->getText(key, &scanRateTextCached));
            addLabelInfo("Mix Signal:", envInfo.mixSignal ? "Yes" : "No");
            addLabelInfo("SignalA:", envInfo.signalA.signalName);
            addLabelInfo("SignalA Brightness:", envInfo.signalA.getBrightness().toMinString());
            addLabelInfo("SignalA Contrast:", envInfo.signalA.getContrast().toMinString());
            if (envInfo.mixSignal) {
                addLabelInfo("SignalB:", envInfo.signalB.signalName);
                addLabelInfo("SignalB Brightness:", envInfo.signalB.getBrightness().toMinString());
                addLabelInfo("SignalB Contrast:", envInfo.signalB.getContrast().toMinString());
            }

            if (param.imageAdditionData.waferPos.x != nullptr && param.imageAdditionData.waferPos.y != nullptr) {
                FloatNumber9 x = param.imageAdditionData.waferPos.x;
                FloatNumber9 y = param.imageAdditionData.waferPos.y;
                FloatNumberPointF<9> beamPos = param.imageAdditionData.getBeamPos();
                x += beamPos.x;
                y += beamPos.y;
                x *= 1000;
                y *= 1000;
                addLabelInfo("Stage Pos:", QString("%1 x %2 mm").arg(x.toQString(4), y.toQString(4)));
            }
            if (relativePos.x != nullptr && relativePos.y != nullptr) {
                addLabelInfo("Relative Pos: ", QString("%1 x %2 µm").arg(relativePos.x.toMinString(), relativePos.y.toMinString()));
            }

            if (!scanRateTextCached) {
                rateLabel->setText("Loading...");
                d->getAsyncCaller()->push(d, [key, rateLabel, service](TSC) {
                    QString text;
                    if (service->cacheText(key)) {
                        text = "Error";
                    } else {
                        text = service->getText(key);
                    }
                    MainThreadInvoker::getInvoker().runLater([rateLabel, text](){
                        if (rateLabel)
                            rateLabel->setText(text);
                    });
                });
            }
        }
    }
};

PatternRegInfoController::PatternRegInfoController(QWidget* parent) : CDE90Window(parent) {
    //如果你没有异步任务需求，请将connect清理掉，保留create和createAction函数
    connect(
        this,
        &PatternRegInfoController::asyncCallerChanged,
        this,
        [this]() {
            create();
            createAction();
        },
        Qt::SingleShotConnection);
}

PatternRegInfoController::~PatternRegInfoController() noexcept {
    if (dataHelper) delete dataHelper;
    if (ui) {
        delete ui;
    }
}

void PatternRegInfoController::create() {
    dataHelper = new PatternRegInfoControllerDataHelper;
    dataHelper->d = this;
    ui = new cdsem::UiPatternRegInfoController;
    setContent(new QWidget);
    ui->init(getContent());
}

void PatternRegInfoController::createAction() {
    bindCloseBtn(ui->cancelBtn->getQPushButton());
}

void PatternRegInfoController::setParam(
    const PatternRegInfoParam& param, const IDW_PointF& relativePos) {
    dataHelper->relativePos = relativePos;
    dataHelper->param = param;
    ui->nameTextField->setText(param.name);
    if (param.modelArea.isValid()) {
        ui->modeSizeXTextField->setText(param.modelArea.width());
        ui->modeSizeYTextField->setText(param.modelArea.height());
    } else {
        const QSize& originSize = param.imageAdditionData.originSize;
        ui->modeSizeXTextField->setText(originSize.width());
        ui->modeSizeYTextField->setText(originSize.height());
    }
    ui->scoreTextField->setText(param.score);
    ui->qualityTextField->setText(param.quality);
    if (param.kind == 1) {
        ui->detectPatternTextField->setText(param.detectedPatterns);
    } else {
        ui->detectPatternTextField->getQLineEdit()->clear();
    }
    cdsem::PatternRegParamOpenMode m = cdsem::PatternRegParamOpenMode(param.model);
    ui->lineSpaceContent->setVisible(m == cdsem::PRPOM_MP_ADD);
    QString kindKey;
    if (m == cdsem::PRPOM_AP_OM || m == cdsem::PRPOM_AP_SEM) {
        kindKey = CBOX_TYPE_AP_MODEL_KIND;
    } else if (m == cdsem::PRPOM_MP_ADD) {
        kindKey = CBOX_TYPE_ADDRESS_MODEL_KIND;
    } else if (m == cdsem::PRPOM_MP_M) {
        kindKey = CBOX_TYPE_MP_MODEL_KIND;
    } else {
        return;
    }
    CboxConfigService* service = ServiceManager::defaultService<CboxConfigService>();
    bool cached[4];
    ui->algorithmTextField->setText(service->getText(CBoxMixKey(CBOX_TYPE_AP_MODEL_ALG, param.alg), &cached[0]));
    ui->kindTextField->setText(service->getText(CBoxMixKey(kindKey, param.kind), &cached[1]));
    ui->lineSpaceDetectTextField->setText(service->getText(CBoxMixKey(CBOX_LINE_SPACE_DETECTION, param.lineSpaceDetection), &cached[2]));
    ui->processTextField->setText(service->getText(CBoxMixKey(CBOX_PR_ALG_PROCESSOR, param.processor), &cached[3]));
    if (!cached[0] || !cached[1] || !cached[2] || !cached[3]) {
        if (!cached[0]) {
            ui->algorithmTextField->setText("Loading...");
        }
        if (!cached[1]) {
            ui->kindTextField->setText("Loading...");
        }
        if (!cached[2]) {
            ui->lineSpaceDetectTextField->setText("Loading...");
        }
        if (!cached[3]) {
            ui->processTextField->setText("Loading...");
        }
        int kind = param.kind;
        int process = param.processor;
        int lineSpaceDet = param.lineSpaceDetection;
        int alg = param.alg;
        bool succ = ACaller->push(this, [this, service, kindKey, kind, process, alg, lineSpaceDet, cached](TSC) {
            CBoxMixKey key;
            int errCode;
            if (!cached[0]) {
                key = CBoxMixKey(CBOX_TYPE_AP_MODEL_ALG, alg);
                errCode = service->cacheText(key);
                MainThreadInvoker::getInvoker().runLater([errCode, this, service, key](){
                    ui->algorithmTextField->setText(errCode == 0 ? service->getText(key) : "Error");
                });
            }
            if (!cached[1]) {
                key = CBoxMixKey(kindKey, kind);
                errCode = service->cacheText(key);
                MainThreadInvoker::getInvoker().runLater([errCode, this, service, key](){
                    ui->kindTextField->setText(errCode == 0 ? service->getText(key) : "Error");
                });
            }
            if (!cached[2]) {
                key = CBoxMixKey(CBOX_LINE_SPACE_DETECTION, lineSpaceDet);
                errCode = service->cacheText(key);
                MainThreadInvoker::getInvoker().runLater([errCode, this, service, key](){
                    ui->lineSpaceDetectTextField->setText(errCode == 0 ? service->getText(key) : "Error");
                });
            }
            if (!cached[3]) {
                key = CBoxMixKey(CBOX_PR_ALG_PROCESSOR, process);
                errCode = service->cacheText(key);
                MainThreadInvoker::getInvoker().runLater([errCode, this, service, key](){
                    ui->processTextField->setText(errCode == 0 ? service->getText(key) : "Error");
                });
            }
        });
        if (!succ) {
            ui->kindTextField->getQLineEdit()->clear();
            ui->lineSpaceDetectTextField->getQLineEdit()->clear();
            ui->processTextField->getQLineEdit()->clear();
            ui->algorithmTextField->getQLineEdit()->clear();
        }
    }

    dataHelper->infoLabelWidget = new CDE90Widget(ui->additionContent);
    dataHelper->infoLabelLayout = new QGridLayout(dataHelper->infoLabelWidget);
    dataHelper->infoLabelLayout->setContentsMargins(3, 3, 3, 3);
    dataHelper->infoLabelLayout->setHorizontalSpacing(20);
    dataHelper->infoLabelLayout->setAlignment(Qt::AlignLeft);
    dataHelper->infoLabelWidget->setLayout(dataHelper->infoLabelLayout);
    ui->additionContent->setWidget(dataHelper->infoLabelWidget);
    dataHelper->infoLabelWidget->show();

    dataHelper->showAllLabels();
}
