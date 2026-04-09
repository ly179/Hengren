//
// Created by Shisan on 2026/1/22.
//

#include "ap_wizard_components.h"

#include <cde90_radio_button.h>

#include "wizard_list_data.h"
#include "ui_ap_wizard_content_controller.h"
#include <QLayout>
#include <sem_global.h>

#include "cde90_message_box.h"
#include "image_operation_data_model.h"
#include "image_operation_relation.h"
#include "cdsem_global_data.h"
#include "image_file_temp_handler.h"
#include "template_data_model.h"

#include "cbox_config_service.h"
#include "base_service_manager.h"
#include "cbox_global_keys.h"
#include "cbox_config_entity.h"
#include "cdsem_control_helper.h"
#include "env_pattern_param_convert.h"
#include "image_type_defition.h"

//code: 0 - OM 1 - SEM 2 - SEM AF 3 - Chip Move
inline int getStepCode(int code, bool omAvailable) {
    return omAvailable ? code : code - 1;
};

enum AllIdpApWizardSteps {
    AP_WIZARD_OM_1,
    AP_WIZARD_OM_2,
    AP_WIZARD_OM_3,
    AP_WIZARD_OM_SAVE_IMG,

    AP_WIZARD_SEM_1,
    AP_WIZARD_SEM_2,
    AP_WIZARD_SEM_3,
    AP_WIZARD_SEM_SAVE_IMG,
    AP_WIZARD_SEM_AF,
    AP_WIZARD_SEM_AF_SAVE,

    AP_WIZARD_CHIP_ORIGIN,
};

struct ApWizardComponentsDataHelper {
    ApWizardDataResult param;
    cdsem::PyShotChipCoordinate tmpCoord;
    IDW_PointF fieldPos;
    QRect boxArea;
    QList<QWidget*> groupItems;
    ApWizardComponents* d;
    Integer32 skipCode;
    Integer32 prevCode;

    bool omAvailable = false;

    void copyParam(ApWizardDataParam& settingParam, const ImageAddtionDataModel& addition, const QString& imgPath) {
        settingParam.param->imagePath = imgPath;
        settingParam.param->infoParam.imageAdditionData = addition;
        settingParam.param->infoParam.modelArea = boxArea;
        settingParam.valid = true;
        settingParam.dPoint.x = fieldPos.x - tmpCoord.cx;
        settingParam.dPoint.y = fieldPos.y - tmpCoord.cy;
        semEnvToPatternParam(*settingParam.param, addition);
        settingParam.param->infoParam.kind = cdsem::MODEL_PATTERN_AP_UNIQUE;

        //TODO
        settingParam.param->infoParam.score = 0;
        settingParam.param->infoParam.quality = 0;
        settingParam.param->infoParam.detectedPatterns = 0;
        settingParam.param->infoParam.alg = cdsem::MODEL_PATTERN_ALGORITHM_VERSATILE;
        settingParam.param->infoParam.processor = 0;
        settingParam.param->infoParam.lineSpaceDetection = 0;
    }

    void toOmDataTmpSave() {
        d->setDisabled(true);
        QPointer<ApWizardComponents> _this(d);
        cdsem::CDSEM_GlobalDataHelper::get()->getImage(IMG_OPT_TYPE_OM, [_this](const cdsem::TmpImageData& imgData, const ImageAddtionDataModel& addition) {
            if (imgData.data.isEmpty()) {
                MainThreadInvoker::instance()->runLater([_this]() {
                    if (!_this) return;
                    _this->setDisabled(false);
                    simpleMessageBox(_this.data(), "Error", "OM Image not exists.");
                });
                return;
            }
            //虽然有图像遗留在磁盘不能被清理的风险，但问题不大。
            QString imgPath = _this->genGroupPath();
            cdsem::saveImage(imgData, imgPath);
            MainThreadInvoker::instance()->runLater([_this, imgPath, addition]() {
                if (!_this) return;
                _this->setDisabled(false);

                //TODO if similar export warning and do not auto toNext.

                if (!_this->dataHelper->param.omParam.param) _this->dataHelper->param.omParam.param = std::make_unique<PatternRecognitionParamSettingParam>();
                _this->dataHelper->param.chipCoord = _this->dataHelper->tmpCoord;
                _this->dataHelper->copyParam(_this->dataHelper->param.omParam, addition, imgPath);
                _this->dataHelper->param.omParam.param->imgType = cdsem::PIT_OM;
                _this->dataHelper->param.omParam.param->infoParam.model = cdsem::PRPOM_AP_OM;

                _this->toFinished(getStepCode(0, _this->dataHelper->omAvailable));
                _this->toNext();
            });
        });
    }
    void toSemDataTmpSave() {
        d->setDisabled(true);
        QPointer<ApWizardComponents> _this(d);
        cdsem::CDSEM_GlobalDataHelper::get()->getImage(IMG_OPT_TYPE_SEM, [_this](const cdsem::TmpImageData& imgData, const ImageAddtionDataModel& addition) {
            if (imgData.data.isEmpty()) {
                MainThreadInvoker::instance()->runLater([_this]() {
                    if (!_this) return;
                    _this->setDisabled(false);
                    simpleMessageBox(_this.data(), "Error", "SEM Image not exists.");
                });
                return;
            }
            //虽然有图像遗留在磁盘不能被清理的风险，但问题不大。
            QString imgPath = _this->genGroupPath();
            cdsem::saveImage(imgData, imgPath);
            MainThreadInvoker::instance()->runLater([_this, imgPath, addition]() {
                if (!_this) return;
                _this->setDisabled(false);
                //TODO if similar export warning and do not auto toNext.

                if (!_this->dataHelper->param.semParam.param) _this->dataHelper->param.semParam.param = std::make_unique<PatternRecognitionParamSettingParam>();
                _this->dataHelper->copyParam(_this->dataHelper->param.semParam, addition, imgPath);
                _this->dataHelper->param.semParam.param->imgType = cdsem::PIT_SEM;
                _this->dataHelper->param.semParam.param->infoParam.model = cdsem::PRPOM_AP_SEM;
                if (!_this->dataHelper->omAvailable)
                    _this->dataHelper->param.chipCoord = _this->dataHelper->tmpCoord;
                _this->toFinished(getStepCode(1, _this->dataHelper->omAvailable));
                _this->toNext();
            });
        });
    }
    void initSteps() {
        param.omParam.valid = false;
        param.semParam.valid = false;
        if (param.omParam.param) *param.omParam.param = PatternRecognitionParamSettingParam {};
        if (param.semParam.param) *param.semParam.param = PatternRecognitionParamSettingParam {};
        if (param.semFocusParam) *param.semFocusParam = AutoFocusSettingParam{};
        //确立id 0为OM 1 - SEM 2 AF 3 Chip Origin，无论OM是否可用
        QList<WizardStepData>& dataList = d->list();
        int begin = int(omAvailable);
        dataList.resize(3 + begin);
        for (int i = 0; i < dataList.size(); i++) {
            dataList[i].finished = false;
        }
        if (omAvailable)
            dataList[0].text = "OM Pattern";
        dataList[begin].text = "SEM Pattern";
        dataList[begin + 1].text = "SEM AF";
        dataList[begin + 2].text = "Chip Origin";
        d->allSteps.clear();
        if (omAvailable) {
            d->allSteps.append(AP_WIZARD_OM_1, std::bind(&ApWizardComponentsDataHelper::toOmStep1, this));
            d->allSteps.append(AP_WIZARD_OM_2, std::bind(&ApWizardComponentsDataHelper::toOmStep2, this));
            d->allSteps.append(AP_WIZARD_OM_3, std::bind(&ApWizardComponentsDataHelper::toOmStep3, this));
            d->allSteps.append(AP_WIZARD_OM_SAVE_IMG, std::bind(&ApWizardComponentsDataHelper::toOmDataTmpSave, this));
        }
        d->allSteps.append(AP_WIZARD_SEM_1, std::bind(&ApWizardComponentsDataHelper::toSemStep1, this));
        d->allSteps.append(AP_WIZARD_SEM_2, std::bind(&ApWizardComponentsDataHelper::toSemStep2, this));
        d->allSteps.append(AP_WIZARD_SEM_3, std::bind(&ApWizardComponentsDataHelper::toSemStep3, this));
        d->allSteps.append(AP_WIZARD_SEM_SAVE_IMG, std::bind(&ApWizardComponentsDataHelper::toSemDataTmpSave, this));
        d->allSteps.append(AP_WIZARD_SEM_AF, std::bind(&ApWizardComponentsDataHelper::toSemStepAF, this));
        d->allSteps.append(AP_WIZARD_SEM_AF_SAVE, std::bind(&ApWizardComponentsDataHelper::toSaveAFParamStep, this));
        d->allSteps.append(AP_WIZARD_CHIP_ORIGIN, std::bind(&ApWizardComponentsDataHelper::toChipOriginStep, this));

        d->notifyUpdated();
        d->toNext();
    }


    bool toTmpCoord() {
        cdsem::PyStageShotChipCoordinate coordinate = cdsem::WaferStageCoordinateConvert::instance()->getShotChipWaferUm();
        if (coordinate.list.isEmpty()) {
            emit d->titleChanged("Specified coordinates are the outside of the shot that can be registered.\nPlease register in the shot that moved by the automatic operation.");
            emit d->nextStatusChanged(false);
            return false;
        }
        //这里就不做多chip判断了，默认就取第一个发现的，如果需求不足再说。
        for (int i = 0; i < coordinate.list.size(); ++ i) {
            cdsem::PyShotChipCoordinate& c = coordinate.list[i];
            if (c.chip == -1) {
                if (i == coordinate.list.size() - 1) {
                    emit d->titleChanged("Specified coordinates are the outside of the shot that can be registered.\nPlease register in the shot that moved by the automatic operation.");
                    emit d->nextStatusChanged(false);
                    return false;
                }
                continue;
            }
            tmpCoord = c;
            break;
        }
        fieldPos = cdsem::WaferStageCoordinateConvert::instance()->waferPosUm();
        return true;
    }

    void continueCalcSemFieldPos() {
        QPointF beamPos = SEM_PY_ENV.getBeamPos();
        fieldPos = cdsem::WaferStageCoordinateConvert::instance()->fieldPos(fieldPos, IDW_PointF{beamPos.x() * 1000'000, beamPos.y() * 1000'000});
    }

    void toOmStep1() {
        emit d->skipStatusChanged(2);
        emit d->nextStatusChanged(true);
        emit d->titleChanged("Please register OM Image in this step.\nSpecify an appropriate pattern by cross cursor.");
        d->select(getStepCode(0, omAvailable));
        d->ui->autoPatternArea->hide();
        d->ui->groupBoxArea->hide();
        d->ui->btnArea->show();
        d->ui->scanArea->hide();
        MsgReceiveStatus<RelationImageOperationController*> status = image_operation::openSingleImageOperation(true);
        if (!status.result) {
            simpleMessageBox(d, "Error", "Image Operation Window is not available.");
            return;
        }
        IOPT->setImageOperationType(IMG_OPT_TYPE_OM);
        status.result->localCrossCursorMode();
    }

    void toOmStep2() {
        d->ui->autoPatternArea->show();
        d->ui->btnArea->hide();
        d->ui->groupBoxArea->hide();
        emit d->nextStatusChanged(true);
        emit d->skipStatusChanged(2);
        emit d->titleChanged("Specify an appropriate pattern by area cursor.");
        IOPT->setImageOperationType(IMG_OPT_TYPE_OM);
        MsgReceiveStatus<RelationImageOperationController*> status = image_operation::openSingleImageOperation(true);
        if (status.result) {
            if (!d->ui->autoCBox->isChecked()) {
                if (param.omParam.param && param.omParam.param->infoParam.modelArea.isValid()) {
                    QRect& rect = param.omParam.param->infoParam.modelArea;
                    status.result->rectCursorMode(MeasurementRect {MeasurementPoint {rect.x(), rect.y()}, MeasurementSize {rect.width(), rect.height()}});
                    MoveStageParam stageParam;
                    auto& waferPos = param.omParam.param->infoParam.imageAdditionData.waferPos;
                    cdsem::WaferStageCoordinateConvert::instance()->umToStagePos(IDW_PointF {waferPos.x, waferPos.y}, stageParam);
                    CDSEM_ControlHelper::instance()->moveStage(d, stageParam);
                } else
                    status.result->rectCursorMode();
            } else {
                status.result->blankCursorMode();
            }
        }
        d->select(getStepCode(0, omAvailable));
        d->toFinished(getStepCode(0, omAvailable), false);
    }

    void toOmStep3() {
        emit d->skipStatusChanged(2);
        d->ui->autoPatternArea->hide();
        MsgReceiveStatus<RelationImageOperationController*> status = image_operation::openSingleImageOperation(true);
        if (status.result) {
            boxArea = status.result->getRectCursorRange();
            status.result->blankCursorMode();
        }
        if (d->ui->autoCBox->isChecked()) {
            //TODO rect
        }
        if (!toTmpCoord()) return;
        bool similar = !boxArea.isValid();
        if (similar) {
            emit d->titleChanged("Similar pattern was detected.\nPress Back button to specify an unique pattern.");
        } else {
            d->toNext();
        }
    }

    void toSemStep1() {
        d->select(getStepCode(1, omAvailable));
        MsgReceiveStatus<RelationImageOperationController*> status = image_operation::openSingleImageOperation(true);
        if (!status.result) {
            simpleMessageBox(d, "Error", "Image Operation Window is not available.");
            return;
        }
        IOPT->setImageOperationType(IMG_OPT_TYPE_SEM);
        status.result->localCrossCursorMode();
        d->ui->autoPatternArea->hide();
        d->ui->groupBoxArea->hide();
        d->ui->btnArea->show();
        d->ui->scanArea->show();
        emit d->nextStatusChanged(true);
        emit d->skipStatusChanged(omAvailable ? 1 : 2);
        emit d->titleChanged("Please register SEM image in this step.\nSpecify an appropriate SEM image by cross cursor.");
    }

    void toSemStep2() {
        d->select(getStepCode(1, omAvailable));
        d->toFinished(getStepCode(1, omAvailable), false);
        emit d->nextStatusChanged(true);
        emit d->titleChanged("Specify an unique pattern by area cursor.\nMagnification must be less than 15k.");
        d->ui->autoPatternArea->show();
        d->ui->btnArea->show();
        d->ui->scanArea->show();
        d->ui->groupBoxArea->hide();
        MsgReceiveStatus<RelationImageOperationController*> status = image_operation::openSingleImageOperation(true);
        if (status.result) {
            if (!d->ui->autoCBox->isChecked()) {
                if (param.semParam.param && param.semParam.param->infoParam.modelArea.isValid()) {
                    QRect& rect = param.semParam.param->infoParam.modelArea;
                    status.result->rectCursorMode(MeasurementRect {MeasurementPoint {rect.x(), rect.y()}, MeasurementSize {rect.width(), rect.height()}});
                    MoveStageParam stageParam;
                    auto& waferPos = param.semParam.param->infoParam.imageAdditionData.waferPos;
                    auto& beamPos = param.semParam.param->infoParam.imageAdditionData.beamPos;
                    cdsem::WaferStageCoordinateConvert::instance()->umToStagePos(IDW_PointF {waferPos.x, waferPos.y}, stageParam);
                    CDSEM_ControlHelper::instance()->moveStage(d, stageParam);
                    CDSEM_ControlHelper::instance()->beamOffsetX(d, beamPos.x());
                    CDSEM_ControlHelper::instance()->beamOffsetY(d, beamPos.y());
                } else
                    status.result->rectCursorMode();
            } else {
                status.result->blankCursorMode();
            }
        }
        emit d->skipStatusChanged(2);
    }

    void toSemStep3() {
        emit d->skipStatusChanged(2);
        emit d->nextStatusChanged(true);
        d->ui->autoPatternArea->hide();
        MsgReceiveStatus<RelationImageOperationController*> status = image_operation::openSingleImageOperation(true);
        if (status.result) {
            boxArea = status.result->getRectCursorRange();
            status.result->blankCursorMode();
        }
        if (!omAvailable && !toTmpCoord()) {
            return;
        }
        continueCalcSemFieldPos();
        if (d->ui->autoCBox->isChecked()) {
            //TODO rect
        }
        bool similar = !boxArea.isValid();
        if (similar) {
            emit d->titleChanged("Similar pattern was detected.\nPress Back button to specify an unique pattern.");
        } else {
            d->toNext();
        }
    }

    void toSemStepAF() {
        d->select(getStepCode(2, omAvailable));
        d->toFinished(getStepCode(2, omAvailable), false);
        emit d->nextStatusChanged(true);
        emit d->titleChanged("Specify the Auto Focus position by area cursor.");
        MsgReceiveStatus<RelationImageOperationController*> status = image_operation::openSingleImageOperation(true);
        if (status.result) {
            status.result->rectCursorMode(image_operation::RECT_CURSOR_32_192);
        }
        QLayout* layout = d->ui->itemGBox->layout();
        for (QWidget* item : groupItems) {
            layout->removeWidget(item);
        }
        qDeleteAll(groupItems);
        groupItems.clear();
        d->ui->itemGBox->setTitle("Method");
        d->setDisabled(true);
        d->caller()->push(d, [this](TSC) {
            sem::AfterGc gc([this]() {
                MainThreadInvoker::getInvoker().runLater([this]() {
                    d->setDisabled(false);
                });
            });
            CboxConfigService* service = BaseServiceManager::instance()->getDefaultService<CboxConfigService>();
            service->cacheCBoxConfigList(CBOX_AP_AUTO_FOCUS_METHOD);
            const QList<CBoxConfigEntity>& list = service->getCBoxConfigList(CBOX_AP_AUTO_FOCUS_METHOD);
            if (list.size() > 0) {
                MainThreadInvoker::instance()->runLater([this, list]() {
                    groupItems.reserve(list.size());
                    for (const CBoxConfigEntity& entity: list) {
                        CDE90RadioButton* radio = new CDE90RadioButton(d->ui->groupBoxArea);
                        radio->setText(entity.getText());
                        radio->setValue(entity.getValue());
                        groupItems.append(radio);
                        d->ui->itemGBox->layout()->addWidget(radio);
                    }
                    static_cast<CDE90RadioButton*>(groupItems.first())->setChecked(true);
                    emit d->nextStatusChanged(true);
                });
            }
        });
        d->ui->autoPatternArea->hide();
        d->ui->btnArea->hide();
        d->ui->groupBoxArea->show();
        if (param.semFocusParam) param.semFocusParam->enable = false;
        emit d->nextStatusChanged(false);
        emit d->skipStatusChanged(1);
    }

    void toSaveAFParamStep() {
        MsgReceiveStatus<RelationImageOperationController*> status = image_operation::openSingleImageOperation(true);
        if (!status.result) {
            d->toPrev();
        }
        QRect rect = status.result->getRectCursorRange();
        if (!rect.isValid()) {
            d->toPrev();
        }

        if (!param.semFocusParam) param.semFocusParam = std::make_unique<AutoFocusSettingParam>();
        //TODO计算中心点间距，即当前选择框与上一步拍图框之间的距离，取的是框的中心点间距？ 还是说该步骤的选框禁止拖动？
        // param.semFocusParam->offset = ?
        param.semFocusParam->enable = true;
        param.semFocusParam->mag = SEM_PY_ENV.getMag();
        for (QWidget*& w : groupItems) {
            auto* radio = dynamic_cast<CDE90RadioButton*>(w);
            if (radio && radio->isChecked()) {
                param.semFocusParam->method = radio->value().toInt();
                break;
            }
        }
        d->toNext();
    }

    void toChipOriginStep() {
        d->toFinished(getStepCode(2, omAvailable));
        d->select(getStepCode(3, omAvailable));
        MsgReceiveStatus<RelationImageOperationController*> status = image_operation::openSingleImageOperation(true);
        if (status.result) {
            status.result->localCrossCursorMode();
        }
        emit d->titleChanged("Specify a chip origin by cross cursor.");
        d->ui->autoPatternArea->hide();
        d->ui->btnArea->show();
        d->ui->scanArea->hide();
        d->ui->groupBoxArea->hide();
        emit d->skipStatusChanged(2);
    }


};

ApWizardComponents::ApWizardComponents(QWidget* parent) : WizardComponent(parent) {
    dataHelper = std::make_unique<ApWizardComponentsDataHelper>();
    dataHelper->d = this;
    ui = new cdsem::UiApWizardContentController;
    ui->init(this);
    createAction();
}

ApWizardComponents::~ApWizardComponents() noexcept {
    delete ui;
}

void ApWizardComponents::createAction() {
    connect(ui->autoCBox, &QCheckBox::toggled, this, [this](bool checked) {
        MsgReceiveStatus<RelationImageOperationController*> status = image_operation::openSingleImageOperation(true);
        if (status.result) {
            if (checked) status.result->blankCursorMode();
            else status.result->rectCursorMode();
        }
    });

    connect(ui->moveChipBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        cdsem::PyStageShotChipCoordinate coordinate = cdsem::WaferStageCoordinateConvert::instance()->getShotChipWaferUm();
        cdsem::PyShotChipCoordinate* coord = nullptr;
        if (coordinate.list.size() > 0) {
            for (int i = 0; i < coordinate.list.size(); ++ i) {
                cdsem::PyShotChipCoordinate& c = coordinate.list[i];
                if (c.chip == -1) {
                    continue;
                }
                coord = &c;
                break;
            }
        }
        if (!coord) {
            simpleMessageBox(this, "Error", "Error to check shot and chip info.");
            return;
        }
        MoveStageParam stageParam;
        cdsem::WaferStageCoordinateConvert::instance()->umToStagePos(IDW_PointF{coord->cx, coord->cy}, stageParam);
        CDSEM_ControlHelper::instance()->moveStage(this, stageParam);
    });
}

void ApWizardComponents::initSteps() {
    WizardComponent::initSteps();
    dataHelper->initSteps();
}

//点击Skip按钮时触发的动作
void ApWizardComponents::getSkip(int& code) {
    if (dataHelper->skipCode != nullptr) {
        code = dataHelper->skipCode;
    }
}
//点击back按钮时要返回到的步骤，传入的int为当前步骤的上一步，如果需要返回到更多，则修改该值
void ApWizardComponents::getPrev(int& code) {
    if (dataHelper->prevCode != nullptr) {
        code = dataHelper->prevCode;
    }
}

void ApWizardComponents::stepFinished() {
    finishedWizard();
}

void ApWizardComponents::stepCodeRefresh(int currentStep) {
    if (currentStep == AP_WIZARD_SEM_1 || currentStep == AP_WIZARD_SEM_AF) {
        dataHelper->skipCode = AP_WIZARD_CHIP_ORIGIN;
    } else {
        dataHelper->skipCode.setNull();
    }
    if (dataHelper->omAvailable && currentStep == AP_WIZARD_SEM_1) {
        dataHelper->prevCode = AP_WIZARD_OM_2;
    } else if (currentStep == AP_WIZARD_SEM_AF) {
        dataHelper->prevCode = AP_WIZARD_SEM_2;
    }  else if (currentStep == AP_WIZARD_CHIP_ORIGIN) {
        dataHelper->prevCode = AP_WIZARD_SEM_AF;
    } else {
        dataHelper->prevCode.setNull();
    }
}

const ApWizardDataResult& ApWizardComponents::getResult() const {
    return dataHelper->param;
}