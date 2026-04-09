#include "axis_align_detail_content.h"
#include <QHBoxLayout>
#include "cbox_global_keys.h"
#include "cde90_button.h"
#include "cde90_message_box.h"
#include "cde90_spliter_line.h"
#include "cde90_window.h"
#include "cdsem_number_validator_helper.h"
#include "common_act_param_helper.h"
#include "focus_auto_content.h"
#include "focus_stigma_content.h"
#include "idp_ap_add_reg_helper.h"
#include "ifem_setting_controller.h"
#include "image_file_temp_handler.h"
#include "image_operation_image_canvas.h"
#include "pattern_recognition_controller.h"
#include "pattern_recognition_param_controller.h"
#include "pattern_reg_info_controller.h"
#include "sem_global.h"
#include "tmp_image_data_converter.h"
#include "ui_axis_align_detail_content.h"
#include "z_sensor_setting_controller.h"
#include "env_pattern_param_convert.h"

class AbstractAxisFocusSettingController : public CDE90Window {
    CDSEM_OK_CALL
private:
    void create() {
        QWidget* content = new QWidget;
        setContent(content);
        QVBoxLayout* vLayout = new QVBoxLayout(content);
        vLayout->setContentsMargins(0, 0, 0, 0);
        vLayout->setSpacing(5);
        setLayout(vLayout);

        contentWidget = new QWidget(content);
        QHBoxLayout* contentLayout = new QHBoxLayout(contentWidget);
        contentLayout->setContentsMargins(5, 5, 5, 5);
        contentWidget->setLayout(contentLayout);
        vLayout->addWidget(contentWidget);

        vLayout->addWidget(new CDE90SpliterHLine(content));

        QWidget* bottomContent = new QWidget(content);
        QHBoxLayout* bottomLayout = new QHBoxLayout(bottomContent);
        bottomLayout->setContentsMargins(5, 5, 5, 5);
        bottomLayout->setSpacing(0);
        bottomContent->setLayout(bottomLayout);
        vLayout->addWidget(bottomContent);

        CDE90ButtonBox* okBtn = new CDE90ButtonBox("OK", bottomContent);
        CDE90ButtonBox* cancelBtn = new CDE90ButtonBox("Cancel", bottomContent);
        bottomLayout->addWidget(okBtn);
        bottomLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
        bottomLayout->addWidget(cancelBtn);

        bindCloseBtn(cancelBtn->getQPushButton());
        connect(okBtn->getQPushButton(), &QPushButton::clicked, this, [this]() {
            QString errMsg = collectData();
            if (errMsg.length() > 0) {
                simpleMessageBox(this, "Error", errMsg);
                return;
            }
            if (okCallback && okCallback()) {
                changeCloseStatusOnce(HW_OK);
                close();
            }
        });
    }

protected:
    QWidget* contentWidget;

    virtual void resetToComponent() = 0;
    virtual QString collectData() = 0;

public:
    explicit AbstractAxisFocusSettingController(
        QWidget* parent = nullptr)
        : CDE90Window(parent) {
        create();
    }

    virtual ~AbstractAxisFocusSettingController() noexcept {}
};

class AxisFocusSettingController : public AbstractAxisFocusSettingController {
    SEM_SET_INIT_PARAM(
        AutoFocusSettingParam)
private:
    FocusAutoContent* focusContent = nullptr;
    void create() {
        focusContent = new FocusAutoContent(true, contentWidget);
        focusContent->initParam(&param);
        contentWidget->layout()->addWidget(focusContent);
    }
    void createAction() {
        itsChildAndBeManaged(focusContent, this);
        focusContent->initAsyncCaller(getAsyncCaller());
    }
    void resetToComponent() override { focusContent->resetToComponent(); }
    QString collectData() override { return focusContent->collectData(); }

public:
    explicit AxisFocusSettingController(
        QWidget* parent = nullptr)
        : AbstractAxisFocusSettingController(parent) {
        connect(
            this,
            &AxisFocusSettingController::asyncCallerChanged,
            this,
            [this]() {
                create();
                createAction();
            },
            Qt::SingleShotConnection);
    }

    const AutoFocusSettingParam& getParam() const { return param; }
};

class AxisStigmaSettingController : public AbstractAxisFocusSettingController {
    SEM_SET_INIT_PARAM(
        AutoStigmaSettingParam)
private:
    FocusStigmaContent* focusContent = nullptr;
    void create() {
        focusContent = new FocusStigmaContent(true, contentWidget);
        focusContent->initParam(&param);
        contentWidget->layout()->addWidget(focusContent);
    }
    void createAction() {
        itsChildAndBeManaged(focusContent, this);
        focusContent->initAsyncCaller(getAsyncCaller());
    }
    void resetToComponent() override { focusContent->resetToComponent(); }
    QString collectData() override { return focusContent->collectData(); }

public:
    explicit AxisStigmaSettingController(
        QWidget* parent = nullptr)
        : AbstractAxisFocusSettingController(parent) {
        connect(
            this,
            &AxisFocusSettingController::asyncCallerChanged,
            this,
            [this]() {
                create();
                createAction();
            },
            Qt::SingleShotConnection);
    }

    const AutoStigmaSettingParam& getParam() const { return param; }
};

template<class T>
void _resetToComponent(
    AxisAlignDetailContent* _this, AxisAlignContentModel<T>& param) {
    _this->ui->moveCheckBox->setChecked(param.moveParam.enable);
    _this->ui->coordinateXTextField->setText(param.moveParam.x.toQStringAndKeepFullDecimal());
    _this->ui->coordinateYTextField->setText(param.moveParam.y.toQStringAndKeepFullDecimal());
    _this->ui->coordinateXDeviationTextField->setText(
        param.moveParam.deviationX.toQStringAndKeepFullDecimal());
    _this->ui->coordinateYDeviationTextField->setText(
        param.moveParam.deviationY.toQStringAndKeepFullDecimal());
    _this->ui->zSensorCheckBox->setChecked(param.zParam.enable);
    _this->ui->autoFocusCheckBox->setChecked(param.focusParam.enable);
    _this->ui->patternCheckBox->setChecked(param.patternParam.enable);
    _this->ui->axisAlignCheckBox->setChecked(param.axisParam.enable);
    _this->ui->waitSecond->setText(param.axisParam.waitSeconds.toQStringAndKeepFullDecimal());
    _this->ui->magTextField->setText(param.axisParam.mag.toString());
    _this->ui->axisItemBox->safeSetCurrentData(param.axisParam.item);

    if (_this->canvasPainter) {
        _this->canvasPainter->clearAndDeleteMeasurementTools();
        _this->canvasPainter->clearImage();
    }
    _this->resetAddressImage(param.imagePath,
                             &param.imageId,
                             param.patternParam.infoParam.imageAdditionData,
                             _this->ui->modelCanvas,
                             _this->canvasPainter,
                             param.patternParam.infoParam.modelArea);
}

template<class T>
QString _collectData(
    AxisAlignDetailContent* _this, AxisAlignContentModel<T>& param) {
    param.moveParam.enable = _this->ui->moveCheckBox->isChecked();
    param.moveParam.x = _this->ui->coordinateXTextField->text();
    param.moveParam.y = _this->ui->coordinateYTextField->text();
    param.moveParam.deviationX = _this->ui->coordinateXDeviationTextField->text();
    param.moveParam.deviationY = _this->ui->coordinateYDeviationTextField->text();
    param.zParam.enable = _this->ui->zSensorCheckBox->isChecked();
    param.focusParam.enable = _this->ui->autoFocusCheckBox->isChecked();
    param.patternParam.enable = _this->ui->patternCheckBox->isChecked();
    param.axisParam.enable = _this->ui->axisAlignCheckBox->isChecked();
    param.axisParam.waitSeconds = _this->ui->waitSecond->text();
    QString str = _this->ui->magTextField->text();
    if (str.isEmpty()) {
        param.axisParam.mag = nullptr;
    } else {
        param.axisParam.mag = str.toInt();
    }
    param.axisParam.item = _this->ui->axisItemBox->currentValue();
    if (param.axisParam.item < 0) {
        return _this->mode == 0 ? "Frame is not specified." : "Mode is not specified.";
    }
    if (param.imagePath.isEmpty() && param.imageId == 0 && _this->canvasPainter
        && !_this->canvasPainter->getImage().isNull()) {
        param.imagePath = cdsem::genCommonPath(_this->group);
        //这个动作要不要异步应该问题不大，如果需要后期再改，将collectData本身改为回调即可。
        cdsem::TmpImageData tmpImage;
        qImageToTmp(_this->canvasPainter->getImage(), tmpImage);
        if (!cdsem::saveImage(tmpImage, param.imagePath)) {
            return "Cache address1 image error.";
        }
    }
    return "";
}

void coverZParam(
    void* param, int mode, const ZSensorSettingParam& zParam) {
    if (mode == 0) {
        reinterpret_cast<AxisAlignmentContentModel*>(param)->zParam = zParam;
        return;
    }
    reinterpret_cast<AxisStigmaContentModel*>(param)->zParam = zParam;
}

ZSensorSettingParam& getZParam(
    void* param, int mode) {
    if (mode == 0) {
        return reinterpret_cast<AxisAlignmentContentModel*>(param)->zParam;
    }
    return reinterpret_cast<AxisStigmaContentModel*>(param)->zParam;
}

PatternRecognitionParamSettingParam& getPatternRegParam(
    void* param, int mode) {
    if (mode == 0) {
        return reinterpret_cast<AxisAlignmentContentModel*>(param)->patternParam;
    }
    return reinterpret_cast<AxisStigmaContentModel*>(param)->patternParam;
}

PatternRecognitionAlgorithmSettingParam& getPatternRegAlgParam(
    void* param, int mode) {
    if (mode == 0) {
        return reinterpret_cast<AxisAlignmentContentModel*>(param)->patternParam.algSettingParam;
    }
    return reinterpret_cast<AxisStigmaContentModel*>(param)->patternParam.algSettingParam;
}

// PatternRegInfoParam& getPatternRegInfoParam(
//     void* param, int mode) {
//     if (mode == 0) {
//         return reinterpret_cast<AxisAlignmentContentModel*>(param)->patternParam.infoParam;
//     }
//     return reinterpret_cast<AxisStigmaContentModel*>(param)->patternParam.infoParam;
// }


QString& getPatternRegImagePath(
    void* param, int mode) {
    if (mode == 0) {
        return reinterpret_cast<AxisAlignmentContentModel*>(param)->imagePath;
    }
    return reinterpret_cast<AxisStigmaContentModel*>(param)->imagePath;
}

quint64& getPatternRegImageId(
    void* param, int mode) {
    if (mode == 0) {
        return reinterpret_cast<AxisAlignmentContentModel*>(param)->imageId;
    }
    return reinterpret_cast<AxisStigmaContentModel*>(param)->imageId;
}

AxisAlignDetailContent::AxisAlignDetailContent(
    QWidget* parent)
    : CDE90Frame(parent) {
    create();
}

AxisAlignDetailContent::AxisAlignDetailContent(
    int mode, QWidget* parent)
    : CDE90Frame(parent)
    , mode(mode) {
    create();
}

AxisAlignDetailContent::~AxisAlignDetailContent() noexcept {
    if (ui) {
        delete ui;
    }
    cdsem::removeGroup(this->group);
}

void AxisAlignDetailContent::create() {
    ui = new cdsem::UiAxisAlignDetailContent;
    ui->init(this);
    if (mode == 0) {
        ui->titleLabel->setText("Axis Alignment");
        ui->axisItemLabel->setText("Frame");
        ui->axisItemBox->loadItemFromDatabase(CBOX_TYPE_SEM_IMG_FRAME);
    } else {
        ui->titleLabel->setText("Stigma");
        ui->axisItemLabel->setText("Mode");
        ui->axisItemBox->loadItemFromDatabase(CBOX_AUTO_STIGMA_MODE);
        this->mode = 1;
    }
}

void AxisAlignDetailContent::createAction() {
    CDSemNumberValidatorHelper::doubleFree<2>()
        .put(ui->coordinateXTextField)
        .put(ui->coordinateYTextField)
        .put(ui->coordinateXDeviationTextField)
        .put(ui->coordinateYDeviationTextField)
        .autoFixMode();
    CDSemNumberValidatorHelper::min(0).put(ui->magTextField).autoFixMode();
    CDSemNumberValidatorHelper::range<1>(1, 10).put(ui->waitSecond).autoFixMode();
    if (mode == 0) {
        ui->axisItemBox->loadItemFromDatabase(CBOX_TYPE_SEM_IMG_FRAME);
    } else {
        ui->axisItemBox->loadItemFromDatabase(CBOX_AUTO_STIGMA_MODE);
    }
    connect(ui->zSensorCheckBox, &QCheckBox::toggled, this, [this](bool checked) {
        getZParam(param, mode).enable = checked;
    });
    connect(ui->zSensorMoreBtn->getQPushButton(), &QPushButton::clicked, this, [this]() {
        createZSensorController();
        zSensorController->setParam(getZParam(param, mode));
        zSensorController->show();
        MainThreadInvoker::getInvoker().moveToParentCenter(this, zSensorController);
    });
    connect(ui->autoFocusCheckBox, &QCheckBox::toggled, this, [this](bool checked) {
        if (mode == 0) {
            reinterpret_cast<AxisAlignmentContentModel*>(param)->focusParam.enable = checked;
        } else {
            reinterpret_cast<AxisStigmaContentModel*>(param)->focusParam.enable = checked;
        }
    });
    connect(ui->autoFocusMoreBtn->getQPushButton(), &QPushButton::clicked, this, [this]() {
        showFocusSettingController();
    });

    connect(ui->patternCheckBox, &QCheckBox::toggled, this, [this](bool checked) {
        getPatternRegParam(param, mode).enable = checked;
    });
    connect(ui->patternMoreBtn->getQPushButton(), &QPushButton::clicked, this, [this]() {
        createPatternRecognitionParamController();
        QImage image;
        if (canvasPainter) {
            image = canvasPainter->getImage();
        }
        patternRegParamController->setParam({getPatternRegParam(param, mode),image});
        patternRegParamController->show();
        if (ui->actionBtn->isLoading()) {
            patternRegParamController->setLoadStatus(true);
        }
        MainThreadInvoker::getInvoker().moveToParentCenter(this, patternRegParamController);
        emit shouldHideParent(true);
    });

    connect(ui->regImageAct, &QAction::triggered, this, [this]() {
        createPatternAlgContrller();
        patternAlgController->setParam(getPatternRegAlgParam(param, mode));
        patternAlgController->show();
        MainThreadInvoker::getInvoker().moveToParentCenter(this, patternAlgController);
    });
    connect(ui->regInfoAct, &QAction::triggered, this, [this]() {
        createPatternRegInfoController();
        patternRegInfoController->show();
        MainThreadInvoker::getInvoker().moveToScreenCenter(patternRegInfoController);
        PatternRegInfoParam* infoParam;
        if (mode == 0) {
            AxisAlignmentContentModel* data = reinterpret_cast<AxisAlignmentContentModel*>(param);
            infoParam = &data->patternParam.infoParam;
        } else {
            AxisStigmaContentModel* data = reinterpret_cast<AxisStigmaContentModel*>(param);
            infoParam = &data->patternParam.infoParam;
        }
        patternRegInfoController->setParam(*infoParam);
    });
    connect(ui->regClearAct, &QAction::triggered, this, [this]() {
        quint64* imageId;
        QString* imagePath;
        if (mode == 0) {
            AxisAlignmentContentModel* data = reinterpret_cast<AxisAlignmentContentModel*>(param);
            imageId = &data->imageId;
            imagePath = &data->imagePath;
            data->patternParam.infoParam = {};
        } else {
            AxisStigmaContentModel* data = reinterpret_cast<AxisStigmaContentModel*>(param);
            imageId = &data->imageId;
            imagePath = &data->imagePath;
            data->patternParam.infoParam = {};
        }
        *imageId = 0;
        *imagePath = "";
        if (canvasPainter) {
            canvasPainter->clearImage();
        }
        ui->modelCanvas->clearImage();
        ui->regClearAct->setDisabled(true);
        ui->regInfoAct->setDisabled(true);
    });
}

void AxisAlignDetailContent::createZSensorController() {
    if (!zSensorController) {
        zSensorController = new ZSensorSettingController(this);
        zSensorController->setResizable(false);
        zSensorController->setWindowTitle("Z Sensor");
        zSensorController->hideBackBtn();
        zSensorController->hideNextBtn();
        zSensorController->setOkCallback([this](const ZSensorSettingParam& data) -> bool {
            coverZParam(param, mode, data);
            ui->zSensorCheckBox->setChecked(data.enable);
            return true;
        });
    }
}

void AxisAlignDetailContent::showFocusSettingController() {
    if (mode == 0) {
        AxisFocusSettingController* focusSettingController;
        if (!this->focusSettingController) {
            focusSettingController = new AxisFocusSettingController(this);
            focusSettingController->setWindowTitle("Auto Focus");
            focusSettingController->setWindowModality(Qt::WindowModal);
            focusSettingController->setResizable(false);
            focusSettingController->initAsyncCaller(caller);
            focusSettingController->setOkCallback([this, focusSettingController]() {
                const AutoFocusSettingParam& data = focusSettingController->getParam();
                reinterpret_cast<AxisAlignmentContentModel*>(param)->focusParam = data;
                ui->autoFocusCheckBox->setChecked(data.enable);
                return true;
            });
            this->focusSettingController = focusSettingController;
        } else {
            focusSettingController = dynamic_cast<AxisFocusSettingController*>(
                this->focusSettingController);
        }
        focusSettingController->show();
        focusSettingController->setParam(
            reinterpret_cast<AxisAlignmentContentModel*>(param)->focusParam);
        MainThreadInvoker::getInvoker().moveToScreenCenter(focusSettingController);
    } else {
        AxisStigmaSettingController* stigmaSettingController;
        if (!this->focusSettingController) {
            stigmaSettingController = new AxisStigmaSettingController(this);
            stigmaSettingController->setWindowTitle("Auto Focus");
            stigmaSettingController->setWindowModality(Qt::WindowModal);
            stigmaSettingController->setResizable(false);
            stigmaSettingController->initAsyncCaller(caller);
            stigmaSettingController->setOkCallback([this, stigmaSettingController]() {
                const AutoStigmaSettingParam& data = stigmaSettingController->getParam();
                reinterpret_cast<AxisStigmaContentModel*>(param)->focusParam = data;
                ui->autoFocusCheckBox->setChecked(data.enable);
                return true;
            });
            this->focusSettingController = stigmaSettingController;
        } else {
            stigmaSettingController = dynamic_cast<AxisStigmaSettingController*>(
                this->focusSettingController);
        }
        stigmaSettingController->show();
        stigmaSettingController->setParam(
            reinterpret_cast<AxisStigmaContentModel*>(param)->focusParam);
        MainThreadInvoker::getInvoker().moveToScreenCenter(stigmaSettingController);
    }
}

void AxisAlignDetailContent::createPatternRecognitionParamController() {
    if (patternRegParamController) {
        return;
    }
    patternRegParamController = new PatternRecognitionParamController(this);
    patternRegParamController->initAsyncCaller(caller);
    patternRegParamController->setMode(cdsem::PRPOM_AP_SEM);
    patternRegParamController->setResizable(false);
    patternRegParamController->setWindowTitle("Pattern Recognition");
    patternRegParamController->hideBackBtn();
    patternRegParamController->hideNextBtn();
    patternRegParamController->setCloseCall([this](CDE90WindowCloseStatus){
        emit shouldHideParent(false);
        return true;
    });
    patternRegParamController->setOkCallback([this](const PatternRegParam& data) {
        ui->patternCheckBox->setChecked(data.patternRegParam.enable);
        getPatternRegParam(param, mode) = data.patternRegParam;
        getPatternRegAlgParam(param, mode) = data.patternRegParam.algSettingParam;
        if (data.imgUpdated) {
            if (data.image.isNull()) {
                ui->regClearAct->trigger();
            } else {
                idpToCreateAndDrawThumbnailCanvas(this,
                                                  canvasPainter,
                                                  ui->modelCanvas,
                                                  data.image,
                                                  data.patternRegParam.infoParam.modelArea);
                ui->regClearAct->setDisabled(false);
                ui->regInfoAct->setDisabled(false);
            }
        }

        return true;
    });
}

void AxisAlignDetailContent::createPatternAlgContrller() {
    if (!patternAlgController) {
        patternAlgController = new PatternRecognitionController(cdsem::PRPOM_MP_ADD, this);
        patternAlgController->setWindowTitle("Pattern Recognition (Addressing)");
        patternAlgController->initAsyncCaller(caller);
        patternAlgController->setResizable(false);
        patternAlgController->hideDefault();
        patternAlgController->setOkCallback(
            [this](const PatternRecognitionAlgorithmSettingParam& param) {
                getPatternRegAlgParam(this->param, mode) = param;
                if (param.kind == 3) {
                    createIfemController();
                    ifemController->show();
                    return true;
                }
                MainThreadInvoker::getInvoker().runLater([this](){
                    beginRegImage();
                }, true);
                return true;
            });
    }
}

void AxisAlignDetailContent::createPatternRegInfoController() {
    if (!patternRegInfoController) {
        patternRegInfoController = new PatternRegInfoController(this);
        patternRegInfoController->initAsyncCaller(caller);
        patternRegInfoController->setResizable(false);
        patternRegInfoController->setWindowTitle("Information");
        patternRegInfoController->setWindowModality(Qt::WindowModal);
    }
}

void AxisAlignDetailContent::createIfemController() {
    if (ifemController) {
        return;
    }
    ifemController = new IfemSettingController(this);
    ifemController->setWindowModality(Qt::WindowModal);
    ifemController->setWindowTitle("Design Value");
    ifemController->setResizable(false);
    ifemController->setOkCallback([this](const IfemParam&) {
        beginRegImage();
        return true;
    });
}

void AxisAlignDetailContent::beginRegImage() {
    emit shouldHideParent(true);
    PatternRecognitionParamSettingParam& gParam = getPatternRegParam(this->param, mode);
    idpRegImage(SRM_AP, gParam.useAuto, this, canvasPainter, ui->modelCanvas, [this](NormalImageRegResult& result){
        emit shouldHideParent(false);
        if (result.error != 0) {
            if (result.error == UNKNOW_ERR_CODE) {
                simpleMessageBox(this, "Error", "Failed to check image.");
            } else if (result.error == NULL_POINTER_ERR_CODE) {
                simpleMessageBox(this, "Error", "Empty image.");
            } else if (result.error != CANCELED_ERROR_CODE){
                simpleMessageBox(this, "Error", "Something goes to wrong.");
            }
            this->window()->activateWindow();
            return;
        }
        if (result.additionParam.photoImagetype != cdsem::PIT_SEM) {
            simpleMessageBox(this, "Error", "It's not a SEM image.");
            this->window()->activateWindow();
            return;
        }
        quint64* imageId;
        QString* imagePath;
        PatternRecognitionParamSettingParam* regSettingParam;
        PatternRecognitionAlgorithmSettingParam* regAlgParam;
        if (mode == 0) {
            AxisAlignmentContentModel* data = reinterpret_cast<AxisAlignmentContentModel*>(param);
            imageId = &data->imageId;
            imagePath = &data->imagePath;
            regSettingParam = &data->patternParam;
            regAlgParam = &data->patternParam.algSettingParam;
        } else {
            AxisStigmaContentModel* data = reinterpret_cast<AxisStigmaContentModel*>(param);
            imageId = &data->imageId;
            imagePath = &data->imagePath;
            regSettingParam = &data->patternParam;
            regAlgParam = &data->patternParam.algSettingParam;
        }

        regSettingParam->infoParam.model = 1;
        regSettingParam->infoParam.modelArea = result.area;
        regSettingParam->infoParam.kind = regAlgParam->kind;
        //测试数据
        regSettingParam->infoParam.score = 800;
        regSettingParam->infoParam.quality = 70;
        regSettingParam->infoParam.detectedPatterns = 1;
        regSettingParam->infoParam.alg = regAlgParam->algorithm;
        regSettingParam->infoParam.processor = regAlgParam->processor;
        regSettingParam->infoParam.lineSpaceDetection = regAlgParam->lineSpaceDetection;

        *imageId = 0;
        *imagePath = "";
        regSettingParam->infoParam.imageAdditionData = result.additionParam;
        semEnvToPatternParam(*regSettingParam, result.additionParam);
        regSettingParam->autoParam.acceptance = regAlgParam->lowLimit;
        ui->regClearAct->setDisabled(false);
        ui->regInfoAct->setDisabled(false);
        //TODO 执行图像识别操作来评分，目前没有这个函数。
        ui->regInfoAct->trigger();
    });
}

void AxisAlignDetailContent::resetAddressImage(
    QString& imagePath,
    quint64* imageId,
    ImageAddtionDataModel& imageDetail,
    SemThumbnailWidget* canvas,
    ImageOperationImageCanvas*& canvasPainter,
    const QRect& range) {
    canvas->clearImage();
    if (imagePath.length() > 0) {
        ui->regClearAct->setDisabled(false);
        ui->regInfoAct->setDisabled(false);
        sem::AfterGc gc([canvas]() { canvas->setText(""); });
        cdsem::TmpImageData tmpImage;
        if (!cdsem::readImage(tmpImage, imagePath)) {
            simpleMessageBox(this, "Error", "Load address image data error.");
        } else {
            QImage image;
            tmpToQImage(std::move(tmpImage), image);
            idpToCreateAndDrawThumbnailCanvas(this, canvasPainter, canvas, image, range);
        }
    } else if (*imageId > 0) {
        ui->regClearAct->setDisabled(true);
        ui->regInfoAct->setDisabled(true);
        imagePath = "";
        ui->actionBtn->loading();
        if (patternRegParamController && patternRegParamController->isVisible()) {
            patternRegParamController->setLoadStatus(true);
        }
        quint64 cimageId = *imageId;
        resetImageFromDatabase(this, caller, imageId, imageDetail, canvasPainter, canvas, range, [this, canvasPainter, imageDetail, cimageId, imageId](bool succ) {
            if (cimageId != *imageId) return;
            ui->actionBtn->overLoading();
            if (patternRegParamController) patternRegParamController->setLoadStatus(false);
            if (!succ) return;
            ui->regClearAct->setDisabled(false);
            ui->regInfoAct->setDisabled(false);
            if (patternRegParamController && patternRegParamController->isVisible()) {
                patternRegParamController->setImageOnly(canvasPainter->getImage());
                patternRegParamController->setImageAdditionOnly(imageDetail);
            }
        });
    } else {
        ui->regClearAct->setDisabled(true);
        ui->regInfoAct->setDisabled(true);
    }
}

void AxisAlignDetailContent::initAsyncCaller(AsyncCaller* caller) {
    if (this->caller) {
        return;
    }
    this->caller = caller;
    caller->manage(ui->axisItemBox, this);
    ui->axisItemBox->setAsyncCaller(caller);
    createAction();
}

void AxisAlignDetailContent::initParam(
    AxisAlignmentContentModel* param) {
    if (mode == 0) {
        this->param = param;
    }
}
void AxisAlignDetailContent::initParam(
    AxisStigmaContentModel* param) {
    if (mode == 0) {
        return;
    }
    this->param = param;
}

void AxisAlignDetailContent::resetToComponent() {
    if (mode == 0) {
        _resetToComponent(this, *reinterpret_cast<AxisAlignmentContentModel*>(param));
    } else {
        _resetToComponent(this, *reinterpret_cast<AxisStigmaContentModel*>(param));
    }
}

QString AxisAlignDetailContent::collectData() {
    QString errMsg;
    if (mode == 0) {
        errMsg = _collectData(this, *reinterpret_cast<AxisAlignmentContentModel*>(param));
    } else {
        errMsg = _collectData(this, *reinterpret_cast<AxisStigmaContentModel*>(param));
    }
    return errMsg;
}

void AxisAlignDetailContent::setGroup(
    const QString& group) {
    cdsem::removeGroup(this->group);
    this->group = group;
    cdsem::insertGroup(group);
}
