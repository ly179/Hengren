#include "pattern_recognition_param_controller.h"
#include "cbox_global_keys.h"
#include "cde90_message_box.h"
#include "idp_ap_add_reg_helper.h"
#include "image_operation_image_canvas.h"
#include "pattern_recognition_controller.h"
#include "pattern_reg_info_controller.h"
#include "ui_pattern_recognition_param_controller.h"
#include "cdsem_number_validator_helper.h"
#include "cdsem_global_data.h"
#include "env_pattern_param_convert.h"
#include "amp_editor_relation.h"
#include "image_operation_relation.h"
#include "pattern_alg_itembox_helper.h"

struct PatternRecognitionParamControllerDataHelper {
    //仅MP可用
    QImage measureImage;
    //仅MP可用
    AmpCommonData* ampData = nullptr;
    //仅MP可用
    ImageAddtionDataModel measureAddition;

    PatternRecognitionParamController* d;
    ImageOperationImageCanvas* canvasPainter = nullptr;
    PatternRecognitionController* patternRegController = nullptr;
    PatternRegInfoController* patternRegInfoController = nullptr;

    cdsem::PatternRegParamOpenMode mode = cdsem::PRPOM_AP_OM;

    int defaultDNumber = 1;

    void beginRegImage(int kind) {
        SemRegModel regMode;
        if (mode == cdsem::PRPOM_MP_M) {
            regMode = SRM_MP;
        } else if (mode == cdsem::PRPOM_AP_OM || mode == cdsem::PRPOM_AP_SEM) {
            regMode = SRM_AP;
        } else if (mode == cdsem::PRPOM_MP_ADD) {
            regMode = SRM_ADDRESS;
        } else {
            return;
        }
        d->hide();
        if (regMode != SRM_MP) {
            idpRegImage(regMode, d->ui->autoRadio->isChecked(), d, canvasPainter, d->ui->canvas, [this, regMode](NormalImageRegResult& result){
                afterRegImage(result, regMode);
            });
        } else {
            //ifrem暂时不支持
            if (kind == 4) return;
            {
                MsgReceiveStatus<RelatoinAmpEditController*> status = amp_editor::openSingleAMPEditor();
                if (!status.result) {
                    simpleMessageBox(d, "Error", "Error to open AMP Window");
                    return;
                }
            }
            {
                MsgReceiveStatus<RelationImageOperationController*> status = image_operation::openSingleImageOperation(true);
                if (!status.result) {
                    simpleMessageBox(d, "Error", "Error to open Image Operation Window");
                    return;
                }
            }
            int r = showYesNoBox(
                nullptr, "Waiting input", QString("Waiting for Auto Measurement Position to complete...\n"
                        "After the measurement results are obtained, please click 'OK' to confirm.\n"
                        "To cancel the operation, please click 'Cancel'."));
            if (r) return;
            DEFAULT_DELETE_POINTER_IN_CLASS(ampData)
            this->ampData = new AmpCommonData(cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpData());
            //Line / Hole / Entire
            //kind == 2 || kind == 3 || kind == 5
            //Unique / Struct
            //kind == 0 || kind == 1
            //skip = 1表示跳过beam shift步骤
            //skip = 2表示跳过box area步骤
            //skip = 3表示跳过score/quality计算步骤
            int skip = 3;
            if (kind == 2 || kind == 3 || kind == 5) {
                skip = 2;
            }
            idpRegImage(SRM_MP, d->ui->autoRadio->isChecked(), d, canvasPainter, d->ui->canvas, [this, skip](NormalImageRegResult& result){
                if (result.error == CANCELED_ERROR_CODE) {
                    DEFAULT_DELETE_POINTER_IN_CLASS(ampData)
                    return;
                }
                if (result.error != 0) {
                    DEFAULT_DELETE_POINTER_IN_CLASS(ampData)
                    d->show();
                    simpleMessageBox(d, "Error", "Empty image or not freezen.");
                    return;
                }
                if (result.additionParam.photoImagetype != cdsem::PIT_SEM) {
                    DEFAULT_DELETE_POINTER_IN_CLASS(ampData)
                    d->show();
                    simpleMessageBox(d, "Error", "It's not a SEM image.");
                    return;
                }
                if (skip == 3) {
                    measureImage = canvasPainter->getImage();
                    canvasPainter->clearImage();
                    ImageAddtionDataModel& measureAddition = result.additionParam;
                    idpRegImage(SRM_MP, d->ui->autoRadio->isChecked(), d, canvasPainter, d->ui->canvas, [this, measureAddition](NormalImageRegResult& result){
                        d->hide();
                        if (result.error == CANCELED_ERROR_CODE) {
                            DEFAULT_DELETE_POINTER_IN_CLASS(ampData)
                            measureImage = QImage();
                            return;
                        }
                        if (result.error != 0) {
                            DEFAULT_DELETE_POINTER_IN_CLASS(ampData)
                            measureImage = QImage();
                            simpleMessageBox(d, "Error", "Empty image or not freezen.");
                            return;
                        }
                        this->measureAddition = measureAddition;
                        bool succ = afterRegImage(result, SRM_MP);
                        if (!succ) {
                            DEFAULT_DELETE_POINTER_IN_CLASS(ampData)
                            measureImage = QImage();
                        }
                    }, 1);
                } else {
                    d->hide();
                    this->measureAddition = result.additionParam;
                    afterRegImage(result, SRM_MP);
                }
            }, skip);
        }
    }

    bool afterRegImage(NormalImageRegResult& result, int regMode) {
        d->show();
        SemRegModel model = SemRegModel(regMode);
        if (result.error != 0) {
            DEFAULT_DELETE_POINTER_IN_CLASS(ampData)
            if (result.error == UNKNOW_ERR_CODE) {
                simpleMessageBox(d, "Error", "Failed to check image.");
            } else if (result.error == NULL_POINTER_ERR_CODE) {
                simpleMessageBox(d, "Error", "Empty image");
            } else if (result.error != CANCELED_ERROR_CODE){
                simpleMessageBox(d, "Error", "Something goes to wrong.");
            }
            d->window()->activateWindow();
            return false;
        }
        if (model == SRM_MP || model == SRM_ADDRESS) {
            if (result.additionParam.photoImagetype != cdsem::PIT_SEM) {
                DEFAULT_DELETE_POINTER_IN_CLASS(ampData)
                simpleMessageBox(d, "Error", "It's not a SEM image.");
                d->window()->activateWindow();
                return false;
            }
        } else if (model != SRM_AP) {
            simpleMessageBox(d, "Error", "No support reg type.");
            d->window()->activateWindow();
            return false;
        }
        d->param.patternRegParam.infoParam.modelArea = result.area;
        copyRegParamToInfo(result);

        if (canvasPainter) {
            d->param.image = canvasPainter->getImage();
        }
        d->ui->infoAct->setDisabled(false);
        d->ui->clearImageAct->setDisabled(false);
        int kind = d->param.patternRegParam.algSettingParam.kind;
        d->ui->kindItemBox->safeSetCurrentData(kind, true);
        d->ui->algItemBox->safeSetCurrentData(d->param.patternRegParam.algSettingParam.algorithm, true);
        kindChanged(kind);
        showRegInfo(d->param.patternRegParam.infoParam);
        d->param.imgUpdated = true;
        return true;
    }

    void createPatternRegInfoController() {
        if (!patternRegInfoController) {
            patternRegInfoController = new PatternRegInfoController(d);
            patternRegInfoController->initAsyncCaller(d->ACaller);
            patternRegInfoController->setResizable(false);
            if (mode == cdsem::PRPOM_AP_OM || mode == cdsem::PRPOM_AP_SEM) {
                patternRegInfoController->setWindowTitle("Information (Alignment)");
            } else if (mode == cdsem::PRPOM_MP_ADD) {
                patternRegInfoController->setWindowTitle("Information (Addressing)");
            } else if (mode == cdsem::PRPOM_MP_M) {
                patternRegInfoController->setWindowTitle("Information (Measurement)");
            }
            patternRegInfoController->setWindowModality(Qt::WindowModal);
        }
    }

    void showRegInfo(const PatternRegInfoParam& param) {
        createPatternRegInfoController();
        patternRegInfoController->show();
        MainThreadInvoker::getInvoker().moveToScreenCenter(patternRegInfoController);
        patternRegInfoController->setParam(param);
    }

    void copyRegParamToInfo(const NormalImageRegResult& result) {
        {
            PatternRegInfoParam& param = d->param.patternRegParam.infoParam;
            param.model = this->mode;
            param.modelArea = result.area;
            param.kind = d->param.patternRegParam.algSettingParam.kind;
            param.score = result.score;
            param.quality = result.quality;
            param.detectedPatterns = 0;
            param.alg = d->param.patternRegParam.algSettingParam.algorithm;
            param.processor = d->param.patternRegParam.algSettingParam.processor;
            param.lineSpaceDetection = d->param.patternRegParam.algSettingParam.lineSpaceDetection;
            param.imageAdditionData = result.additionParam;
        }
            if (result.additionParam.photoImagetype == cdsem::PIT_OM) {
            if (mode == cdsem::PRPOM_AP_SEM) {
                d->setMode(cdsem::PRPOM_AP_OM);
            }
            d->param.patternRegParam.manualParam.brightness = result.additionParam.brightness;
            d->ui->bTextField->setText(d->param.patternRegParam.manualParam.brightness.toMinString(1));
        } else {
            if (mode == cdsem::PRPOM_AP_OM) {
                d->setMode(cdsem::PRPOM_AP_SEM);
            }
            semEnvToPatternParam(d->param.patternRegParam, result.additionParam);
            d->param.patternRegParam.autoParam.acceptance = d->param.patternRegParam.algSettingParam.lowLimit;

            d->ui->autoBTextField->setText(result.additionParam.getBrightness().toMinString(1));
            d->ui->autoCTextField->setText(result.additionParam.getContrast().toMinString(1));
            d->ui->autoFrameDetectionItemBox->safeSetCurrentData(result.additionParam.frame);
            d->ui->autoScanRateItemBox->safeSetCurrentData(result.additionParam.scanRate);
            d->ui->autoAcceptanceTextField->setText(d->param.patternRegParam.autoParam.acceptance.value());

            d->ui->bTextField->setText(d->ui->autoBTextField->text());
            d->ui->cTextField->setText(d->ui->autoCTextField->text());
            d->ui->magTextField->setText(result.additionParam.mag);
            d->ui->rotTextField->setText(result.additionParam.getImageRot().toMinString());
            d->ui->manualFrameItemBox->safeSetCurrentData(result.additionParam.frame);
            if (result.additionParam.imageType == cdsem::SIT_LIVE) {
                d->ui->viewRadio->setChecked(true);
            } else {
                d->ui->integRadio->setChecked(true);
            }
            d->ui->manualAcceptanceTextField->setText(d->param.patternRegParam.manualParam.acceptance.value());
        }
    }


    void openAlgController(){
        int v = d->ui->kindItemBox->currentValue();
        if (v >= 0)
            d->param.patternRegParam.algSettingParam.kind = v;
        v = d->ui->kindItemBox->currentValue();
        if (v >= 0)
            d->param.patternRegParam.algSettingParam.algorithm = v;
        if (!patternRegController) {
            patternRegController = new PatternRecognitionController(mode, d);
            patternRegController->initAsyncCaller(d->ACaller);
            patternRegController->hideDefault(mode == cdsem::PRPOM_MP_ADD);
            patternRegController->setResizable(false);
            patternRegController->setOkCallback(
                [this](const PatternRecognitionAlgorithmSettingParam& data) {
                    d->param.patternRegParam.algSettingParam = data;
                    MainThreadInvoker::getInvoker().runLater([this](){
                        beginRegImage(d->param.patternRegParam.algSettingParam.kind);
                    }, true);
                    return true;
                });
        }
        if (mode == cdsem::PRPOM_AP_OM || mode == cdsem::PRPOM_AP_SEM) {
            patternRegController->setWindowTitle("Pattern Recognition (Alignment)");
        } else if (mode == cdsem::PRPOM_MP_ADD) {
            patternRegController->setWindowTitle("Pattern Recognition (Addressing)");
        } else if (mode == cdsem::PRPOM_MP_M) {
            patternRegController->setWindowTitle("Pattern Recognition (Measurement)");
        }
        patternRegController->setParam(d->param.patternRegParam.algSettingParam);
        patternRegController->show();
        MainThreadInvoker::getInvoker().moveToParentCenter(d, patternRegController);
    }

    void kindChanged(int cData = -1) {
        algKindChanged(&mode, -1, d->ui->kindItemBox, d->ui->algItemBox);
        if (cData == -1) cData = d->ui->kindItemBox->currentValue();
        if (cData < 0) return;
        bool disabled;
        if (mode == cdsem::PRPOM_AP_OM || mode == cdsem::PRPOM_AP_SEM) {
            disabled = cData != cdsem::MODEL_PATTERN_AP_STRUCT;
        } else if (mode == cdsem::PRPOM_MP_ADD) {
            disabled = cData != cdsem::MODEL_PATTERN_ADDRESS_STRUCT;
        } else if (mode == cdsem::PRPOM_MP_M) {
            disabled = cData != cdsem::MODEL_PATTERN_MP_STRUCT;
        } else {
            return;
        }
        d->ui->autoMinNumber1TextField->setDisabled(disabled);
        d->ui->autoMinNumber2TextField->setDisabled(disabled);
    }
};

PatternRecognitionParamController::PatternRecognitionParamController(QWidget* parent)
    : CDE90Dialog(parent) {
    connect(this, &PatternRecognitionParamController::asyncCallerChanged, this, [this](){
        create();
        setMode(dataHelper->mode);
        createAction();
    }, Qt::SingleShotConnection);
}

PatternRecognitionParamController::~PatternRecognitionParamController() noexcept {
    if (dataHelper->ampData) delete dataHelper->ampData;
    if (ui) {
        delete ui;
    }
    if (dataHelper) delete dataHelper;
}

void PatternRecognitionParamController::setMode(cdsem::PatternRegParamOpenMode mode) {
    dataHelper->mode = mode;
    bool notApOm = mode != cdsem::PRPOM_AP_OM;
    ui->cTextField->setVisible(notApOm);
    ui->bLabelContent->setVisible(notApOm);
    ui->cLabelContent->setVisible(notApOm);
    ui->magLabel->setVisible(notApOm);
    ui->rotLabel->setVisible(notApOm);
    ui->rotUnitLabel->setVisible(notApOm);
    ui->frameLabel->setVisible(notApOm);
    ui->filterLabel->setVisible(notApOm);
    ui->scanLabel->setVisible(notApOm);
    ui->magTextField->setVisible(notApOm);
    ui->rotTextField->setVisible(notApOm);
    ui->manualFrameItemBox->setVisible(notApOm);
    ui->manualFilterItemBox->setVisible(notApOm);
    ui->viewLabel->setVisible(notApOm);
    ui->viewItemsBtn->setVisible(notApOm);
    ui->viewRadio->setVisible(notApOm);
    ui->integRadio->setVisible(notApOm);
    ui->autoRadio->setVisible(notApOm);
    ui->manualRadio->setVisible(notApOm);

    ui->energyFilterLabelContent->setVisible(notApOm);
    ui->energyFilterParamContent->setVisible(notApOm);
    ui->energySpliter->setVisible(notApOm);

    ui->detailWidget->setTabEnabled(0, notApOm);
    if (!notApOm) {
        ui->detailWidget->setCurrentIndex(1);
    }

    bool isApMode = mode == cdsem::PRPOM_AP_OM || mode == cdsem::PRPOM_AP_SEM;
    ui->omTitleLabel->setVisible(isApMode);
    ui->semTitleCbox->setHidden(isApMode);
    ui->energyFilterLabelContent->setHidden(isApMode);
    ui->energyFilterParamContent->setHidden(isApMode);
    ui->energySpliter->setHidden(isApMode);
    ui->autoFilterEContent->setHidden(isApMode);
    ui->autoOffsetLabelContent->setHidden(isApMode);
    ui->autoOffsetContent->setHidden(isApMode);


    bool isApOrMs = mode == cdsem::PRPOM_AP_OM || mode == cdsem::PRPOM_AP_SEM || mode == cdsem::PRPOM_MP_ADD;
    bool isMsOrMp = mode == cdsem::PRPOM_MP_ADD || mode == cdsem::PRPOM_MP_M;
    ui->autoFrameDetectionLabelContent->setHidden(isApOrMs);
    ui->autoFrameDMContent->setHidden(isApOrMs);
    ui->autoEntireModeLabelContent->setHidden(isApOrMs);
    ui->autoEntireContent->setHidden(isApOrMs);
    ui->autoDcModeLabelContent->setHidden(isApOrMs);
    ui->autoDcModeContent->setHidden(isApOrMs);
    ui->autoFilterLabel->setHidden(isApOrMs);
    ui->autoFlatModeItemBox->setHidden(isApOrMs);
    ui->centerLabelContent->setHidden(isApOrMs);
    ui->centerMinNoLabelContent->setHidden(isApOrMs);
    ui->autoRotRageNoteContent->setHidden(isApOrMs);
    ui->lsContrastModeCBox->setHidden(isApOrMs);
    ui->dLabel->setVisible(isMsOrMp);
    ui->dLeftLabel->setVisible(isMsOrMp);
    ui->dRightLabel->setVisible(isMsOrMp);
    ui->dCenterLabel->setVisible(isMsOrMp);
    ui->dPosXTextField->setVisible(isMsOrMp);
    ui->dPosYTextField->setVisible(isMsOrMp);
    ui->dRegPosBtn->setVisible(isMsOrMp);
    ui->dSplitLine->setVisible(isMsOrMp);


    bool isMpOrMs = mode == cdsem::PRPOM_MP_ADD;
    ui->autoEFilterItemBox->setHidden(isMpOrMs);
    ui->autoOffsetRegBtn->setHidden(isMpOrMs);

    bool isMp = mode == cdsem::PRPOM_MP_M;
    ui->autoFrameLabelContent->setHidden(isMp);
    ui->autoFrameFilterContent->setHidden(isMp);
    ui->searchAroundCBox->setHidden(isMp);
    ui->minNumberLabelContent->setHidden(isMp);
    ui->minNumberContent->setHidden(isMp);

    if (dataHelper->mode == cdsem::PRPOM_AP_OM || dataHelper->mode == cdsem::PRPOM_AP_SEM) {
        ui->kindItemBox->loadItemFromDatabase(CBOX_TYPE_AP_MODEL_KIND, true, [this]() {
            dataHelper->kindChanged();
        });
    } else if (dataHelper->mode == cdsem::PRPOM_MP_ADD) {
        ui->kindItemBox->loadItemFromDatabase(CBOX_TYPE_ADDRESS_MODEL_KIND,
                                          true,
                                          [this]() { dataHelper->kindChanged(); });
    } else {
        ui->kindItemBox->loadItemFromDatabase(CBOX_TYPE_MP_MODEL_KIND, true, [this]() {
            dataHelper->kindChanged();
        });
    }
}

void PatternRecognitionParamController::create() {
    dataHelper = new PatternRecognitionParamControllerDataHelper;
    dataHelper->d = this;
    ui = new cdsem::UiPatternRecognitionParamController;
    setContent(new QWidget);
    ui->init(getContent());
}

void PatternRecognitionParamController::createAction() {
    CDSemNumberValidatorHelper::range<1>(0, 10).put(ui->autoWaitTextField->getQLineEdit())
        .autoFixMode();
    CDSemNumberValidatorHelper::range(0, 1000).put(ui->autoAcceptanceTextField->getQLineEdit())
        .put(ui->manualAcceptanceTextField->getQLineEdit())
        .autoFixMode();
    CDSemNumberValidatorHelper::doubleFree<3>().put(ui->autoOffsetXTextField->getQLineEdit())
        .put(ui->autoOffsetYTextField->getQLineEdit())
        .autoFixMode();
    CDSemNumberValidatorHelper::doubleFree<2>().put(ui->dPosXTextField->getQLineEdit())
        .put(ui->dPosYTextField->getQLineEdit())
        .autoFixMode();
    CDSemNumberValidatorHelper::range(0, 3000000).put(ui->magTextField->getQLineEdit())
        .autoFixMode();
    CDSemNumberValidatorHelper::range<1>("0", "359.9").put(ui->rotTextField->getQLineEdit())
        .autoFixMode();
    CDSemNumberValidatorHelper::range(0, 100).put(ui->bTextField->getQLineEdit())
        .put(ui->cTextField->getQLineEdit())
        .put(ui->autoBTextField->getQLineEdit())
        .put(ui->autoCTextField->getQLineEdit())
        .autoFixMode();

    itsChildAndBeManaged(ui->manualFilterItemBox, this);
    itsChildAndBeManaged(ui->manualFrameItemBox, this);
    itsChildAndBeManaged(ui->autoFilterItemBox, this);
    itsChildAndBeManaged(ui->autoFrameDetectionItemBox, this);
    itsChildAndBeManaged(ui->autoFrameMeasurementItemBox, this);
    itsChildAndBeManaged(ui->autoEFilterItemBox, this);
    itsChildAndBeManaged(ui->autoEntireModeItemBox, this);
    itsChildAndBeManaged(ui->autoDcModeItemBox, this);
    itsChildAndBeManaged(ui->autoCenterItemBox, this);
    itsChildAndBeManaged(ui->autoFrameItemBox, this);
    itsChildAndBeManaged(ui->autoScanRateItemBox, this);
    itsChildAndBeManaged(ui->kindItemBox, this);
    itsChildAndBeManaged(ui->algItemBox, this);

    ui->manualFilterItemBox->setAsyncCaller(getAsyncCaller());
    ui->manualFrameItemBox->setAsyncCaller(getAsyncCaller());
    ui->autoFilterItemBox->setAsyncCaller(getAsyncCaller());
    ui->autoFrameDetectionItemBox->setAsyncCaller(getAsyncCaller());
    ui->autoFrameMeasurementItemBox->setAsyncCaller(getAsyncCaller());
    ui->autoEFilterItemBox->setAsyncCaller(getAsyncCaller());
    ui->autoEntireModeItemBox->setAsyncCaller(getAsyncCaller());
    ui->autoDcModeItemBox->setAsyncCaller(getAsyncCaller());
    ui->autoCenterItemBox->setAsyncCaller(getAsyncCaller());
    ui->autoFrameItemBox->setAsyncCaller(getAsyncCaller());
    ui->autoScanRateItemBox->setAsyncCaller(getAsyncCaller());
    ui->kindItemBox->setAsyncCaller(ACaller);
    ui->algItemBox->setAsyncCaller(ACaller);

    ui->manualFilterItemBox->loadItemFromDatabase(CBOX_TYPE_SEM_IMG_FILTER);
    ui->manualFrameItemBox->loadItemFromDatabase(CBOX_TYPE_SEM_IMG_FRAME);
    ui->autoFilterItemBox->loadItemFromDatabase(CBOX_TYPE_SEM_IMG_FILTER);
    ui->autoFrameDetectionItemBox->loadItemFromDatabase(CBOX_TYPE_SEM_IMG_FRAME);
    ui->autoFrameMeasurementItemBox->loadItemFromDatabase(CBOX_TYPE_SEM_IMG_FRAME);
    ui->autoEFilterItemBox->loadItemFromDatabase(CBOX_TYPE_SEM_IMG_FILTER);
    ui->autoEntireModeItemBox->loadItemFromDatabase(CBOX_PR_AUTO_ENTIRE);
    ui->autoDcModeItemBox->loadItemFromDatabase(CBOX_TYPE_SEM_IMG_DC);
    ui->autoCenterItemBox->loadItemFromDatabase(CBOX_PR_MEASURE_CENTER);
    ui->autoScanRateItemBox->loadItemFromDatabase(CBOX_TYPE_SEM_IMG_SCAN_RATE);
    ui->autoFrameItemBox->loadItemFromDatabase(CBOX_TYPE_SEM_IMG_FRAME);
    SEM_OK_ACTION(ui->okBtn->getQPushButton())
    SEM_STEP_BACK_ACTION(ui->backBtn->getQPushButton())
    SEM_STEP_NEXT_ACTION(ui->nextBtn->getQPushButton())
    bindCloseBtn(ui->cancelBtn->getQPushButton());

    connect(ui->energyFilterCBox, &CDE90CheckBox::toggled, this, [this](bool checked) {
        ui->energyFilterTextField->setDisabled(!checked);
        ui->energyFilterBseTextField->setDisabled(!checked);
    });
    connect(ui->autoEvaluationCBox, &CDE90CheckBox::toggled, this, [this](bool checked) {
        ui->autoEvaluationTextField->setDisabled(!checked);
    });
    connect(ui->regImageAct, &QAction::triggered, this, [this]() { dataHelper->openAlgController(); });
    connect(ui->clearImageAct, &QAction::triggered, this, [this]() {
        if (dataHelper->canvasPainter) {
            dataHelper->canvasPainter->clearAndDeleteMeasurementTools();
            dataHelper->canvasPainter->clearImage();
        }
        ui->canvas->clearImage();
        param.image = QImage();
        param.imgUpdated = true;
        ui->infoAct->setDisabled(true);
        ui->clearImageAct->setDisabled(true);
        param.patternRegParam.infoParam = {};
    });
    connect(ui->infoAct, &QAction::triggered, this, [this]() {
        dataHelper->createPatternRegInfoController();
        dataHelper->patternRegInfoController->show();
        MainThreadInvoker::getInvoker().moveToScreenCenter(dataHelper->patternRegInfoController);
        dataHelper->patternRegInfoController->setParam(this->param.patternRegParam.infoParam);
    });
    connect(ui->readBcBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        if (dataHelper->mode == cdsem::PRPOM_AP_OM) {
            ui->bTextField->setText(FloatNumber1(OM_PY_ENV.getBrightness()).toMinString(1));
            return;
        }
        ui->bTextField->setText(FloatNumber1(SEM_PY_ENV.getBrightness()).toMinString(1));
        ui->cTextField->setText(FloatNumber1(SEM_PY_ENV.getContrast()).toMinString(1));
    });
    connect(ui->kindItemBox->getQComboBox(), &UComboBox::currentIndexChanged, this, [this](int) {
        dataHelper->kindChanged();
    });
}

void PatternRecognitionParamController::resetToComponent() {
    param.imgUpdated = false;
    if (!param.image.isNull()) {
        idpToCreateAndDrawThumbnailCanvas(this, dataHelper->canvasPainter, ui->canvas, param.image, param.patternRegParam.infoParam.modelArea);
        ui->infoAct->setDisabled(false);
        ui->clearImageAct->setDisabled(false);
    } else {
        ui->infoAct->setDisabled(true);
        ui->clearImageAct->setDisabled(true);
        ui->canvas->clearImage();
        if (dataHelper->canvasPainter) {
            dataHelper->canvasPainter->clearAndDeleteMeasurementTools();
            dataHelper->canvasPainter->clearImage();
        }
    }
    ui->kindItemBox->safeSetCurrentData(param.patternRegParam.algSettingParam.kind, true);
    ui->algItemBox->safeSetCurrentData(param.patternRegParam.algSettingParam.algorithm, true);
    ACaller->push(this, [this](TSC) {
        MainThreadInvoker::instance()->runLater([this]() {
            dataHelper->kindChanged(param.patternRegParam.algSettingParam.kind);
        });
    });
    if (dataHelper->mode == cdsem::PRPOM_AP_OM) {
        ui->bTextField->setText(this->param.patternRegParam.manualParam.brightness.toMinString(1));
        ui->manualAcceptanceTextField->setText(this->param.patternRegParam.manualParam.acceptance.toString());
        return;
    }

    if (dataHelper->mode == cdsem::PRPOM_MP_ADD || dataHelper->mode == cdsem::PRPOM_MP_M) {
        ui->semTitleCbox->setChecked(this->param.patternRegParam.enable);
    }
    if (this->param.patternRegParam.useAuto) {
        ui->autoRadio->setChecked(true);
        ui->detailWidget->setCurrentIndex(0);
    } else {
        ui->manualRadio->setChecked(true);
        ui->detailWidget->setCurrentIndex(1);
    }

    {
        PatternRecognitionParamSettingAutoParam& param = this->param.patternRegParam.autoParam;
        ui->autoBTextField->setText(FloatNumber3(param.brightness).toMinString());
        ui->autoCTextField->setText(FloatNumber3(param.contrast).toMinString());
        if (param.acceptance != nullptr)
            ui->autoAcceptanceTextField->setText(param.acceptance.value());
        else
            ui->autoAcceptanceTextField->getQLineEdit()->clear();
        ui->autoWaitTextField->setText(param.waitSeconds.toQStringAndKeepFullDecimal());
        if (param.minNumber1 != nullptr)
            ui->autoMinNumber1TextField->setText(param.minNumber1.value());
        else
            ui->autoMinNumber1TextField->getQLineEdit()->clear();
        if (param.minNumber2 != nullptr)
            ui->autoMinNumber2TextField->setText(param.minNumber2.value());
        else
            ui->autoMinNumber2TextField->getQLineEdit()->clear();
        ui->autoRotationTextField->setText(param.rotation.toQStringAndKeepFullDecimal());
        ui->autoScanRateItemBox->safeSetCurrentData(param.scanRate);
        if (dataHelper->mode == cdsem::PRPOM_MP_M) {
            ui->autoFrameMeasurementItemBox->safeSetCurrentData(param.measurementFrame);
            ui->autoFrameDetectionItemBox->safeSetCurrentData(param.detectionFrame);
            ui->autoCenterItemBox->safeSetCurrentData(param.centering);
            ui->autoEFilterItemBox->safeSetCurrentData(param.filter);
        } else {
            ui->autoFrameItemBox->safeSetCurrentData(param.detectionFrame);
            ui->autoFilterItemBox->safeSetCurrentData(param.filter);
        }
        ui->searchAroundCBox->setChecked(param.enableSearchAround);
        ui->lsDetectionItemBox->setCurrentData(param.lsDetection);
        ui->autoMethodItemBox->setCurrentData(param.method);
        ui->autoEdgeDetectionItemBox->setCurrentData(param.edgeDetection);
        ui->imageQualityItemBox->setCurrentData(param.imageQuality);

        if (dataHelper->mode == cdsem::PRPOM_MP_ADD
            || dataHelper->mode == cdsem::PRPOM_MP_M) {
            ui->energyFilterCBox->setChecked(param.enableEnergyFilter);
            if (param.energyFilterValue != nullptr)
                ui->energyFilterTextField->setText(param.energyFilterValue.value());
            else
                ui->energyFilterTextField->getQLineEdit()->clear();
            if (param.bsePercent != nullptr)
                ui->energyFilterBseTextField->setText(param.bsePercent.value());
            else
                ui->energyFilterBseTextField->getQLineEdit()->clear();
            emit ui->energyFilterCBox->toggled(ui->energyFilterCBox->isChecked());

            ui->autoOffsetXTextField->setText(param.offset.x());
            ui->autoOffsetYTextField->setText(param.offset.y());
            ui->autoEvaluationCBox->setChecked(param.enableEvaluation);
            ui->autoEvaluationTextField->setText(param.evaluationValue);
            emit ui->autoEvaluationCBox->toggled(ui->autoEvaluationCBox->isChecked());
            if (dataHelper->mode == cdsem::PRPOM_MP_M) {
                ui->autoEntireModeItemBox->safeSetCurrentData(param.entireMode);
                ui->autoContrastModeCBox->setChecked(param.enableEntireModeContrastMode);
                ui->autoDcModeItemBox->safeSetCurrentData(param.dcMode);
                ui->lsContrastModeCBox->setChecked(param.enableLsDetectionContrastMode);
                ui->autoFlatModeItemBox->setChecked(param.enableFlatMode);
            }
        }
    }

    {
        PatternRecognitionParamSettingManualParam& param = this->param.patternRegParam.manualParam;
        ui->bTextField->setText(param.brightness.toMinString(1));
        ui->cTextField->setText(param.contrast.toMinString(1));
        if (param.mag != nullptr)
            ui->magTextField->setText(param.mag.value());
        else
            ui->magTextField->getQLineEdit()->clear();
        ui->rotTextField->setText(param.rot.toQStringAndKeepFullDecimal());
        ui->manualFrameItemBox->safeSetCurrentData(param.frame);
        ui->manualFilterItemBox->safeSetCurrentData(param.filter);
        if (param.scanMode == 0) {
            ui->viewRadio->setChecked(true);
        } else {
            ui->integRadio->setChecked(true);
        }
        ui->viewItemsBtn->setCurrentData(param.viewScan);
        if (dataHelper->mode == cdsem::PRPOM_MP_ADD || dataHelper->mode == cdsem::PRPOM_MP_M) {
            ui->dPosXTextField->setText(param.relativePos.x.toQStringAndKeepFullDecimal());
            ui->dPosYTextField->setText(param.relativePos.y.toQStringAndKeepFullDecimal());
        }
        ui->manualAcceptanceTextField->setText(param.acceptance.toString());
    }
}

QString PatternRecognitionParamController::collectData() {
    //mode = 0 -> Ap的OM，OM没有auto/manual区分。
    PatternRecognitionParamSettingParam param;
    if (dataHelper->mode == cdsem::PRPOM_AP_OM) {
        param.enable = true;
        param.useAuto = false;
        QString text = ui->bTextField->text();
        if (text.isEmpty()) {
            return "Brightness is not specified.";
        }
        param.manualParam.brightness = text.toInt();
    } else {
        bool isEnable = ui->semTitleCbox->isChecked();
        if (isEnable && !ui->autoRadio->isChecked() && !ui->manualRadio->isChecked()) {
            return "Work mode is not specified.";
        }
        if (dataHelper->mode == cdsem::PRPOM_AP_SEM) {
            param.enable = true;
        } else {
            param.enable = ui->semTitleCbox->isChecked();
        }
        bool useAuto = ui->autoRadio->isChecked();
        param.useAuto = useAuto;
        QString val;
        {
            PatternRecognitionParamSettingAutoParam& autoResult = param.autoParam;
            int intV = ui->kindItemBox->currentValue();
            if (intV < 0 && isEnable && useAuto) {
                return "Kind is not specified.";
            }
            param.algSettingParam.kind = intV;
            intV = ui->algItemBox->currentValue();
            if (intV < 0 && isEnable && useAuto) {
                return "Algorithm is not specified.";
            }
            param.algSettingParam.algorithm = intV;

            if (dataHelper->mode == cdsem::PRPOM_MP_M) {
                val = ui->autoCenterItemBox->currentData();
                if (val.isEmpty()) {
                    if (isEnable && useAuto)
                        return "Centering is not specified.";
                    autoResult.centering = -1;
                } else {
                    autoResult.centering = val.toInt();
                }
            }
            val = ui->autoBTextField->text();
            if (val.isEmpty()) {
                if (isEnable && useAuto)
                    return "Brightness is not specified.";
                autoResult.brightness = 0;
            } else {
                autoResult.brightness = val.toInt();
            }
            val = ui->autoCTextField->text();
            if (val.isEmpty()) {
                if (isEnable && useAuto)
                    return "Contrast is not specified.";
                autoResult.contrast = 0;
            } else {
                autoResult.contrast = val.toInt();
            }
            val = ui->autoAcceptanceTextField->text();
            if (val.isEmpty()) {
                if (isEnable && useAuto)
                    return "Acceptance is not specified.";
                autoResult.acceptance = nullptr;
            } else {
                autoResult.acceptance = val.toInt();
            }
            val = ui->autoWaitTextField->text();
            if (val.isEmpty()) {
                if (isEnable && useAuto)
                    return "Wait Sec. is not specified.";
                autoResult.waitSeconds.clear();
            } else {
                autoResult.waitSeconds = val;
            }

            val = ui->autoMinNumber1TextField->text();
            if (val.isEmpty()) {
                if (isEnable && useAuto && ui->autoMinNumber1TextField->isEnabled())
                    return "Min Number is not specified.";
                autoResult.minNumber1 = nullptr;
            } else {
                autoResult.minNumber1 = val.toInt();
            }

            val = ui->autoMinNumber2TextField->text();
            if (val.isEmpty()) {
                if (isEnable && useAuto && ui->autoMinNumber2TextField->isEnabled())
                    return "Min Number is not specified.";
                autoResult.minNumber2 = nullptr;
            } else {
                autoResult.minNumber2 = val.toInt();
            }

            val = ui->autoRotationTextField->text();
            if (val.isEmpty()) {
                if (isEnable && useAuto && ui->autoRotationTextField->isEnabled())
                    return "Rotation is not specified.";
                autoResult.rotation.clear();
            } else {
                autoResult.rotation = val;
            }

            val = ui->autoScanRateItemBox->currentData();
            if (val.isEmpty()) {
                if (isEnable && useAuto)
                    return "Scan mode is not specified.";
                autoResult.scanRate = 0;
            } else {
                autoResult.scanRate = val.toInt();
            }

            if (dataHelper->mode == cdsem::PRPOM_MP_M) {
                val = ui->autoFrameMeasurementItemBox->currentData();
                if (val.isEmpty()) {
                    if (isEnable && useAuto)
                        return "Frame(Measurement) is not specified.";
                    autoResult.measurementFrame = -1;
                } else {
                    autoResult.measurementFrame = val.toInt();
                }
                val = ui->autoFrameDetectionItemBox->currentData();
            } else {
                val = ui->autoFrameItemBox->currentData();
            }
            if (val.isEmpty()) {
                if (isEnable && useAuto)
                    return dataHelper->mode == cdsem::PRPOM_MP_M ? "Frame(Detection) is not specified."
                                                     : "Frame is not specified.";
                autoResult.detectionFrame = -1;
            } else {
                autoResult.detectionFrame = val.toInt();
            }
            if (dataHelper->mode == cdsem::PRPOM_MP_M) {
                val = ui->autoEFilterItemBox->currentData();
            } else {
                val = ui->autoFilterItemBox->currentData();
            }
            if (val.isEmpty()) {
                if (isEnable && useAuto)
                    return "Filter is not specified.";
                autoResult.filter = 0;
            } else {
                autoResult.filter = val.toInt();
            }
            autoResult.enableSearchAround = ui->searchAroundCBox->isChecked();
            val = ui->lsDetectionItemBox->currentData();
            if (val.isEmpty()) {
                if (isEnable && useAuto)
                    return "L/S Detection is not specified.";
                autoResult.lsDetection = 0;
            } else {
                autoResult.lsDetection = val.toInt();
            }
            val = ui->autoMethodItemBox->currentData();
            if (val.isEmpty()) {
                if (isEnable && useAuto)
                    return "Method is not specified.";
                autoResult.method = -1;
            } else {
                autoResult.method = val.toInt();
            }
            val = ui->autoEdgeDetectionItemBox->currentData();
            if (val.isEmpty()) {
                if (isEnable && useAuto)
                    return "Edge Detection is not specified.";
                autoResult.edgeDetection = -1;
            } else {
                autoResult.edgeDetection = val.toInt();
            }
            val = ui->imageQualityItemBox->currentData();
            if (val.isEmpty()) {
                if (isEnable && useAuto)
                    return "Image Quality is not specified.";
                autoResult.imageQuality = -1;
            } else {
                autoResult.imageQuality = val.toInt();
            }

            if (dataHelper->mode == cdsem::PRPOM_MP_ADD
                || dataHelper->mode == cdsem::PRPOM_MP_M) {
                autoResult.enableEnergyFilter = ui->energyFilterCBox->isChecked();
                val = ui->energyFilterTextField->text();
                if (val.isEmpty()) {
                    if (isEnable && autoResult.enableEnergyFilter && useAuto)
                        return "Energy filter value is not specified.";
                    autoResult.energyFilterValue = nullptr;
                } else {
                    autoResult.energyFilterValue = val.toInt();
                }

                val = ui->energyFilterBseTextField->text();
                if (val.isEmpty()) {
                    if (isEnable && autoResult.enableEnergyFilter && useAuto)
                        return "Energy filter BSE value is not specified.";
                    autoResult.bsePercent = nullptr;
                } else {
                    autoResult.bsePercent = val.toInt();
                }

                val = ui->autoOffsetXTextField->text();
                if (val.isEmpty()) {
                    autoResult.offset.setX(0);
                } else {
                    autoResult.offset.setX(val.toInt());
                }
                val = ui->autoOffsetYTextField->text();
                if (val.isEmpty()) {
                    autoResult.offset.setY(0);
                } else {
                    autoResult.offset.setY(val.toInt());
                }

                autoResult.enableEvaluation = ui->autoEvaluationCBox->isChecked();
                val = ui->autoEvaluationTextField->text();
                if (val.isEmpty()) {
                    if (isEnable && autoResult.enableEvaluation && useAuto)
                        return "Evaluation value is not specified.";
                    autoResult.evaluationValue = 0;
                } else {
                    autoResult.evaluationValue = val.toInt();
                }
                if (dataHelper->mode == cdsem::PRPOM_MP_M) {
                    val = ui->autoEntireModeItemBox->currentData();
                    if (val.isEmpty()) {
                        if (isEnable && useAuto)
                            return "Entire mode is not specified.";
                        autoResult.entireMode = -1;
                    } else {
                        autoResult.entireMode = val.toInt();
                    }
                    autoResult.enableEntireModeContrastMode = ui->autoContrastModeCBox->isChecked();

                    val = ui->autoDcModeItemBox->currentData();
                    if (val.isEmpty()) {
                        if (isEnable && useAuto)
                            return "D.C. mode is not specified.";
                        autoResult.dcMode = -1;
                    } else {
                        autoResult.dcMode = val.toInt();
                    }
                    autoResult.enableLsDetectionContrastMode = ui->lsContrastModeCBox->isChecked();
                    autoResult.enableFlatMode = ui->autoFlatModeItemBox->isChecked();
                }
            }
        }

        {
            PatternRecognitionParamSettingManualParam& manualResult = param.manualParam;
            val = ui->bTextField->text();
            if (val.isEmpty()) {
                if (isEnable && !useAuto)
                    return "Brightness is not specified.";
                manualResult.brightness = 0;
            } else {
                manualResult.brightness = val.toInt();
            }
            val = ui->cTextField->text();
            if (val.isEmpty()) {
                if (isEnable && !useAuto)
                    return "Contrast is not specified.";
                manualResult.contrast = 0;
            } else {
                manualResult.contrast = val.toInt();
            }

            val = ui->magTextField->text();
            if (val.isEmpty()) {
                if (isEnable && !useAuto)
                    return "Magnification is not specified.";
                manualResult.mag = 0;
            } else {
                manualResult.mag = val.toInt();
            }

            val = ui->rotTextField->text();
            if (val.isEmpty()) {
                if (isEnable && !useAuto)
                    return "Rotation is not specified.";
                manualResult.rot.clear();
            } else {
                manualResult.rot = val;
            }
            val = ui->manualFrameItemBox->currentData();
            if (val.isEmpty()) {
                if (isEnable && !useAuto)
                    return "Frame is not specified.";
                manualResult.frame = -1;
            } else {
                manualResult.frame = val.toInt();
            }
            val = ui->manualFilterItemBox->currentData();
            if (val.isEmpty()) {
                if (isEnable && !useAuto)
                    return "Filter is not specified.";
                manualResult.filter = -1;
            } else {
                manualResult.filter = val.toInt();
            }
            if (ui->viewRadio->isChecked()) {
                manualResult.scanMode = 0;
            } else {
                manualResult.scanMode = 1;
            }

            val = ui->viewItemsBtn->currentData();
            if (val.isEmpty()) {
                if (isEnable && manualResult.scanMode == 0 && !useAuto)
                    return "View scan mode is not specified.";
                manualResult.viewScan = -1;
            } else {
                manualResult.viewScan = val.toInt();
            }

            if (dataHelper->mode == cdsem::PRPOM_MP_ADD || dataHelper->mode == cdsem::PRPOM_MP_M) {
                val = ui->dPosXTextField->text();
                if (val.isEmpty()) {
                    manualResult.relativePos.x.clear();
                } else {
                    manualResult.relativePos.x = val;
                }

                val = ui->dPosYTextField->text();
                if (val.isEmpty()) {
                    manualResult.relativePos.y.clear();
                } else {
                    manualResult.relativePos.y = val;
                }
            }
        }
    }

    QString val = ui->manualAcceptanceTextField->text();
    if (val.isEmpty()) {
        if (param.enable && !param.useAuto)
            return "Acceptance is not specified.";
        param.manualParam.acceptance = nullptr;
    } else {
        param.manualParam.acceptance = val.toInt();
    }
    param.infoParam = this->param.patternRegParam.infoParam;
    this->param.patternRegParam = param;
    return "";
}

void PatternRecognitionParamController::hideNextBtn(bool b) {
    ui->nextBtn->setVisible(!b);
}
void PatternRecognitionParamController::hideBackBtn(bool b) {
    ui->backBtn->setVisible(!b);
}

void PatternRecognitionParamController::setDefaultDNumber(int d) {
    dataHelper->defaultDNumber = d;
}

const QImage& PatternRecognitionParamController::getMeasureImage() const {
    return dataHelper->measureImage;
}

const ImageAddtionDataModel& PatternRecognitionParamController::getMeasureImageAddition() const {
    return dataHelper->measureAddition;
}

const AmpCommonData* PatternRecognitionParamController::getAmpCommonData() const {
    return dataHelper->ampData;
}

void PatternRecognitionParamController::setLoadStatus(bool status) {
    if (status) {
        if (ui->actionBtn->isLoading()) return;
        ui->canvas->setText("Loading...");
        ui->actionBtn->loading();
    } else {
        if (!ui->actionBtn->isLoading()) return;
        ui->canvas->setText("");
        ui->actionBtn->overLoading();
    }
}

void PatternRecognitionParamController::setImageOnly(
    const QImage& image) {
    if (image.isNull()) {
        ui->clearImageAct->trigger();
    } else {
        param.image = image;
        idpToCreateAndDrawThumbnailCanvas(this, dataHelper->canvasPainter, ui->canvas, param.image, param.patternRegParam.infoParam.modelArea);
        ui->clearImageAct->setDisabled(false);
        ui->infoAct->setDisabled(false);
    }
}

void PatternRecognitionParamController::setImageAdditionOnly(const ImageAddtionDataModel& param) {
    this->param.patternRegParam.infoParam.imageAdditionData = param;
}
