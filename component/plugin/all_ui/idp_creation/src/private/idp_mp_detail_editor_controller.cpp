#include "idp_mp_detail_editor_controller.h"
#include "cde90_check_box.h"
#include "cde90_message_box.h"
#include "cde90_process_box.h"
#include "cde90_text_field.h"
#include "cdsem_number_validator_helper.h"
#include "relation_idp_multi_ms_template_edit_controller.h"
#include "relation_idp_mp_template_edit_controller.h"
#include "template_relation.h"
#include "idp_message_box_helper.h"
#include "idp_mp_coordinate_setting_controller.h"
#include "mpip_template_select_controller.h"
#include "sem_global.h"
#include "service_manager.h"
#include "template_info_model_convert.h"
#include "template_info_service.h"
#include "template_save_box_controller.h"
#include "template_selector_controller.h"
#include "ui_idp_mp_detail_editor_controller.h"
#include "list_view_main_data_model.h"
#include "sem_common_data_model.h"
#include "cdsem_global_data.h"
#include "template_handler_helper.h"
#include "multi_shot_chip_select_controller.h"
#include "cdsem_control_helper.h"
#include "recipe_procedure.h"
#include "image_save_model_convert.h"
#include "image_file_temp_handler.h"

//! 为了应对部分数据的实时性
//! param中的dPoint是实时刷新的，即有修改就刷新，其他数据仅调用collectData时才更新！
struct IdpMpDetailEditorDataHelper {
    std::function<bool(const ListViewMainData&)> titleSelectorCallback;
    QSharedPointer<CDE90MessageBox> questionBox;
    QScopedPointer<MultiAddressTemplateParam> dynamicOriginMSData;
    QScopedPointer<MpTemplateParam> dynamicOriginMPData;
    IdpMpDetailEditorController* d;
    IdpMpCoordinateSettingController* mpCoordinateSettingController = nullptr;
    RelationIDPMultiMSTemplateEditController* addressTemplateController = nullptr;
    RelationIDPMPTemplateEditController* mpTemplateController = nullptr;
    TemplateSelectorController* templateSelectorController = nullptr;
    TemplateSaveBoxController* saveBoxController = nullptr;
    IdpSaveBoxSaveCaller<MultiAddressTemplateParam>* msSaveBoxCaller = nullptr;
    IdpSaveBoxSaveCaller<MpTemplateParam>* mpSaveBoxCaller = nullptr;
    MPIPTemplateSelectController* mpipSelectController = nullptr;
    MultiShotChipSelectController* multiShotChipSelector = nullptr;
    //0 - 默认，点击Reg 按钮回调
    //1 - 点击 Move按钮回调
    //2 - Test按钮回调
    int multiSelectedChipType = 0;
    //0 - 动态 1 - 静态
    char templateSelectStatus;
    //1 - MS 2 - MP
    char templateSelectType;
    //0 - Dynamic Load 1 - Static Load 2 - Save
    char mpipSelectorStatus;

    bool pyHandlerDisabled = false;

    void resetGroup() {
        if (addressTemplateController)
            addressTemplateController->setGroup(
                QString("idp_ms_detail_%1").arg(d->param.id == 0 ? "create" : QString::number(d->param.id)));
        if (mpTemplateController)
            mpTemplateController->setGroup(
                QString("idp_mp_detail_%1").arg(d->param.id == 0 ? "create" : QString::number(d->param.id)));
    }

    void createMultiChipSelector() {
        if (multiShotChipSelector) return;
        multiShotChipSelector = new MultiShotChipSelectController(true, d);
        multiShotChipSelector->setWindowTitle("Overlapping Chips Selector");
        multiShotChipSelector->setWindowModality(Qt::WindowModal);
        multiShotChipSelector->setCloseCall([this](CDE90WindowCloseStatus status){
            if (status == HW_OK) {
                if (pyHandlerDisabled) {
                    simpleMessageBox(d, "Info", "Current IDW has been unloaded.");
                    return true;
                }
                cdsem::PyShotChipCoordinate* data = multiShotChipSelector->getSelectedData();
                if (!data) {
                    simpleMessageBox(multiShotChipSelector, "Error", "Please select one chip.");
                    return false;
                }
                if (multiSelectedChipType == 0) {
                    dRegCoordinateToParam(data->globalChip);
                } else if (multiSelectedChipType == 1){
                    dMovePos(data->globalChip);
                } else if (multiSelectedChipType == 2){
                    doTest(data->globalChip);
                }
            }
            multiShotChipSelector->clearListData();
            return true;
        });
    }

    void updateDPointXFromTextField() {
        int val = d->ui->posItembox->currentValue();
        QString text = d->ui->dxTextField->text();
        if (val == 0) {
            d->param.param.dPoint.x = text;
        } else if (d->param.prevCoordinate.x != nullptr){
            if (text.isEmpty()) {
                text = "0";
            }
            d->param.param.dPoint.x = d->param.prevCoordinate.x + text;
        }
        IDW_PointF pos = d->param.param.dPoint;
        if (pos.x == nullptr) pos.x = 0;
        if (pos.y == nullptr) pos.y = 0;

        pos.x += d->param.oPoint.x;
        pos.y += d->param.oPoint.y;
        addressTemplateController->setAPos(pos);
        mpTemplateController->setAPos(pos);
    }

    void updateDPointYFromTextField() {
        int val = d->ui->posItembox->currentValue();
        QString text = d->ui->dyTextField->text();
        if (val == 0) {
            d->param.param.dPoint.y = text;
        } else if (d->param.prevCoordinate.y != nullptr){
            if (text.isEmpty()) {
                text = "0";
            }
            d->param.param.dPoint.y = d->param.prevCoordinate.y + text;
        }
        IDW_PointF pos = d->param.param.dPoint;
        if (pos.x == nullptr) pos.x = 0;
        if (pos.y == nullptr) pos.y = 0;

        pos.x += d->param.oPoint.x;
        pos.y += d->param.oPoint.y;
        addressTemplateController->setAPos(pos);
        mpTemplateController->setAPos(pos);
    }

    //changedFrom == 0 默认
    //1 - MS
    //2 - MP
    void setDTextFieldValue(char changedFrom = 0) {
        int val = d->ui->posItembox->currentValue();
        if (val == 0) {
            d->ui->dxTextField->setText(d->param.param.dPoint.x.toQString());
            d->ui->dyTextField->setText(d->param.param.dPoint.y.toQString());
        } else {
            if (d->param.param.dPoint.x == nullptr) {
                d->ui->dxTextField->setText((-d->param.prevCoordinate.x).toQString());
            } else {
                d->ui->dxTextField->setText((d->param.param.dPoint.x - d->param.prevCoordinate.x).toQString());
            }
            if (d->param.param.dPoint.y == nullptr) {
                d->ui->dyTextField->setText((-d->param.prevCoordinate.y).toQString());
            } else {
                d->ui->dyTextField->setText((d->param.param.dPoint.y - d->param.prevCoordinate.y).toQString());
            }
        }
        IDW_PointF pos = d->param.param.dPoint;
        if (pos.x == nullptr) pos.x = 0;
        if (pos.y == nullptr) pos.y = 0;

        pos.x += d->param.oPoint.x;
        pos.y += d->param.oPoint.y;
        if (changedFrom != 1)
            addressTemplateController->setAPos(pos);
        if (changedFrom != 2)
            mpTemplateController->setAPos(pos);
    }

    void dRegCoordinateToParam(int chip) {
        bool succ;
        cdsem::PyShotChipCoordinate coordinate = cdsem::WaferStageCoordinateConvert::instance()->getShotChipUm(chip, &succ);
        if (!succ) {
            simpleMessageBox(d, "Error", "Error to check shot and chip info.");
            return;
        }
        IDW_PointF pos = cdsem::WaferStageCoordinateConvert::instance()->waferPosUm();
        // QPointF beamPos = SEM_PY_ENV.getBeamPos();
        // pos = cdsem::WaferStageCoordinateConvert::instance()->fieldPos(pos, IDW_PointF{beamPos.x() * 1000'000, beamPos.y() * 1000'000});
        d->param.param.dPoint.x = pos.x - coordinate.cx;
        d->param.param.dPoint.y = pos.y - coordinate.cy;
        d->param.oPoint = IDW_PointF{coordinate.cx, coordinate.cy};

        setDTextFieldValue();
    }


    bool confirmChip(int& chip, int multiType) {
        if (d->param.chipNumber < 0) {
            simpleMessageBox(d, "Error", "Empty chip data.");
            return false;
        }
        chip = -1;
        cdsem::PyStageShotChipCoordinate currentChips = cdsem::WaferStageCoordinateConvert::instance()->getShotChipWaferUm(false);
        QList<cdsem::PyShotChipCoordinate> chips;
        if (currentChips.list.size() > 0) {
            chips.reserve(currentChips.list.size());
            for (cdsem::PyShotChipCoordinate& p : currentChips.list) {
                if (p.globalChip < 0) continue;
                chips.append(p);
                if (p.globalChip == d->param.chipNumber) {
                    chip = p.globalChip;
                }
            }
        }
        if (chip == -1) {
            if (chips.isEmpty()) {
                //使用指定的Chip来注册坐标
                simpleMessageBox(d, "Info", "No chip data detected. Defaulting to assigned registration coordinates.");
                if (pyHandlerDisabled) {
                    simpleMessageBox(d, "Info", "Current IDW has been unloaded.");
                    return false;
                }
                chip = d->param.chipNumber;
            } else {
                //询问使用机台当前chip还是指定的chip来注册坐标
                int r = showYesNoBox(d, "Info", "Chip mismatch detected. Use registration coordinates from:", false, false, "Current Machine", "Specified");
                if (pyHandlerDisabled) {
                    simpleMessageBox(d, "Info", "Current IDW has been unloaded.");
                    return false;
                }
                if (r) {
                    chip = d->param.chipNumber;
                } else {
                    if (chips.size() == 1) {
                        chip = chips.first().globalChip;
                    } else {
                        createMultiChipSelector();
                        multiSelectedChipType = multiType;
                        multiShotChipSelector->setListData(chips);
                        multiShotChipSelector->show();
                        return false;
                    }
                }
            }
        }
        return true;
    }

    void dRegCoordinate() {
        int chip;
        if (confirmChip(chip, 0)) {
            dRegCoordinateToParam(chip);
        }
    }

    void dMovePos(int chip) {
        if (chip < 0) return;
        bool succ;
        cdsem::PyShotChipCoordinate coordinate = cdsem::WaferStageCoordinateConvert::instance()->getShotChipUm(chip, &succ);
        if (!succ) {
            simpleMessageBox(d, "Error", "Error to check shot and chip info.");
            return;
        }
        IDW_PointF pos = d->param.param.dPoint;
        if (pos.x == nullptr) pos.x = coordinate.cx;
        else pos.x += coordinate.cx;
        if (pos.y == nullptr) pos.y = coordinate.cy;
        else pos.y += coordinate.cy;
        MoveStageParam stageParam;
        cdsem::WaferStageCoordinateConvert::instance()->umToStagePos(pos, stageParam);
        CDSEM_ControlHelper::instance()->moveStage(d, stageParam);
    }

    void dMovePos() {
        if (d->param.chipNumber < 0) {
            simpleMessageBox(d, "Error", "Empty chip data.");
            return;
        }
        int chip;
        if (confirmChip(chip, 1)) {
            dMovePos(chip);
        }
    }

    //注意，MP中测量图和模式匹配图，如果是同一张图，则测量图path会丢弃，或imageId相同。
    void doTest(int chip) {
        bool succ;
        cdsem::PyShotChipCoordinate coordinate = cdsem::WaferStageCoordinateConvert::instance()->getShotChipUm(chip, &succ);
        if (!succ) {
            simpleMessageBox(d, "Error", "Error to check shot and chip info.");
            return;
        }

        QSharedPointer<IdpDetailData> data(new IdpDetailData);
        QSharedPointer<IdpMpPosListData> posData(new IdpMpPosListData);
        posData->oPoint = IDW_PointF {coordinate.cx, coordinate.cy};
        posData->chipX = d->param.chipX;
        posData->chipY = d->param.chipY;
        posData->chipNumber = d->param.chipNumber;
        posData->paramIndex = 0;
        QList<IdpMExeParamDetail>& subList = d->param.param.mpTemplate.templateParam.exeParam.list;
        posData->dNumbers.resize(subList.size());
        for (int i = 0; i < subList.size(); ++ i) {
            posData->dNumbers.replace(i, subList[i].dNumbers);
        }
        data->mpData.posList.reserve(1);
        data->mpData.posList.append(posData);
        data->mpData.paramList.reserve(1);
        data->mpData.paramList.append(QSharedPointer<IdpMpParamListData>(new IdpMpParamListData(d->param.param)));
        QSharedPointer<IdpMpParamListData> detail = data->mpData.paramList.first();
        RecipeProcedureConfig config;
        config.procedureType = RPT_MP_TEST;
        RecipeProcedureHelper::get()->startProcedure(data, config);
    }

    void testTrigger() {
        QString errMsg = d->collectData();
        if (errMsg.length() > 0) {
            simpleMessageBox(d, "Error", errMsg);
            return;
        }

        int chip;
        if (!confirmChip(chip, 2)) {
            return;
        }
        doTest(chip);
    }

    void _resetMSComponent() {
        addressTemplateController->resetToComponent();
        if (d->param.param.addressRotate == 0) {
            addressTemplateController->rotateTemplateCheckBox()->setChecked(false);
            addressTemplateController->rotateTemplateTextField()->getQLineEdit()->clear();
        } else {
            addressTemplateController->rotateTemplateCheckBox()->setChecked(true);
            addressTemplateController->rotateTemplateTextField()->setText(
                d->param.param.addressRotate.toQStringAndKeepFullDecimal());
        }
        bool isDynamic = d->param.param.addressTemplate.templateId > 0
                         && d->param.param.addressTemplate.templateRefType == 0;
        addressTemplateController->toDynamicRefStyle(isDynamic);
        addressTemplateController->rotateTemplateCheckBox()->setDisabled(!isDynamic);
        addressTemplateController->rotateTemplateTextField()->setDisabled(
            !addressTemplateController->rotateTemplateCheckBox()->isChecked() || !isDynamic);
    }

    void _resetMPComponent() {
        mpTemplateController->setDefaultDNumber(d->param.param.pNumber);
        mpTemplateController->resetToComponent();
        if (d->param.param.measurementRotate == 0) {
            mpTemplateController->rotateTemplateCheckBox()->setChecked(false);
            mpTemplateController->rotateTemplateTextField()->getQLineEdit()->clear();
        } else {
            mpTemplateController->rotateTemplateCheckBox()->setChecked(true);
            mpTemplateController->rotateTemplateTextField()->setText(
                d->param.param.measurementRotate.toQStringAndKeepFullDecimal());
        }

        bool isDynamic = d->param.param.mpTemplate.templateId > 0
                         && d->param.param.mpTemplate.templateRefType == 0;
        mpTemplateController->toDynamicRefStyle(isDynamic);
        mpTemplateController->rotateTemplateCheckBox()->setDisabled(!isDynamic);
        mpTemplateController->rotateTemplateTextField()->setDisabled(
            !mpTemplateController->rotateTemplateCheckBox()->isChecked() || !isDynamic);
    }
};

IdpMpDetailEditorController::IdpMpDetailEditorController(QWidget* parent)
    : CDE90Window(parent) {
    connect(this, &IdpMpDetailEditorController::asyncCallerChanged, this, [this](){
        create();
        createAction();
    }, Qt::SingleShotConnection);
}

IdpMpDetailEditorController::~IdpMpDetailEditorController() noexcept {
    if (ui) {
        delete ui;
    }
    if (dataHelper) {
        if (dataHelper->msSaveBoxCaller) {
            delete dataHelper->msSaveBoxCaller;
        }
        if (dataHelper->mpSaveBoxCaller) {
            delete dataHelper->mpSaveBoxCaller;
        }
        delete dataHelper;
    }
}

void IdpMpDetailEditorController::create() {
    dataHelper = new IdpMpDetailEditorDataHelper;
    dataHelper->d = this;
    ui = new cdsem::UiIdpMpDetailEditorController;
    setContent(new QWidget);
    CDE90LayoutHelper helper(getContent());
    helper.registUnknowType("addressTemplateContent",
                            [this](QObject* parent,
                                   const QHash<QString, QObject*>&,
                                   QHash<QString, QList<QStringList>>&) -> QObject* {
                                QWidget* widget = dynamic_cast<QWidget*>(parent);
                                if (!widget) {
                                    return nullptr;
                                }
                                MsgReceiveStatus<RelationIDPMultiMSTemplateEditController*> status = idp_template::createMultiMsTemplateComponent(widget);
                                if (!status.ok) return nullptr;
                                dataHelper->addressTemplateController = status.result;
                                dataHelper->addressTemplateController->setLineWidth(1);
                                dataHelper->addressTemplateController->setFrameStyle(QFrame::Plain
                                                                         | QFrame::Box);
                                dataHelper->addressTemplateController->initParam(
                                    &param.param.addressTemplate.templateParam);
                                dataHelper->addressTemplateController->setRotTempContentVisible(true);
                                return dataHelper->addressTemplateController;
                            });
    helper.registUnknowType("mpTemplateContent",
                            [this](QObject* parent,
                                   const QHash<QString, QObject*>&,
                                   QHash<QString, QList<QStringList>>&) -> QObject* {
                                QWidget* widget = dynamic_cast<QWidget*>(parent);
                                if (!widget) {
                                    return nullptr;
                                }
                                MsgReceiveStatus<RelationIDPMPTemplateEditController*> status = idp_template::createMpTemplateComponent(widget);
                                if (!status.ok) return nullptr;
                                dataHelper->mpTemplateController = status.result;
                                dataHelper->mpTemplateController->setLineWidth(1);
                                dataHelper->mpTemplateController->setFrameStyle(QFrame::Plain | QFrame::Box);
                                dataHelper->mpTemplateController->initParam(
                                    &param.param.mpTemplate.templateParam);
                                dataHelper->mpTemplateController->setRotTempContentVisible(true);
                                return dataHelper->mpTemplateController;
                            });

    ui->init(helper);
    
}

void IdpMpDetailEditorController::createAction() {
    bindCloseBtn(ui->cancelBtn->getQPushButton());
    if (!dataHelper->addressTemplateController || !dataHelper->mpTemplateController) return;
    CDSemNumberValidatorHelper::doubleFree<2>()
        .put(ui->dxTextField->getQLineEdit())
        .put(ui->dyTextField->getQLineEdit())
        .autoFixMode();
    itsChildAndBeManaged(dataHelper->addressTemplateController, this);
    itsChildAndBeManaged(dataHelper->mpTemplateController, this);
    dataHelper->addressTemplateController->initAsyncCaller(ACaller);
    dataHelper->mpTemplateController->initAsyncCaller(ACaller);
    dataHelper->addressTemplateController->hideNextBtn(false);
    dataHelper->addressTemplateController->hidePrevBtn(false);
    dataHelper->mpTemplateController->hidePrevBtn(false);
    SEM_OK_ACTION(ui->okBtn->getQPushButton())
    dataHelper->titleSelectorCallback = std::bind(&IdpMpDetailEditorController::titleSelectCall,
                                      this,
                                      std::placeholders::_1);
    connect(ui->regPosMoreBtn->getQPushButton(), &CDE90Button::clicked, this, [this]() {
        createCoordinateSettingController();
        IdpMpCoordinateSettingParam param;
        param.dPoint = this->param.param.dPoint;
        param.prevCoordinate = this->param.prevCoordinate;
        param.chipX = this->param.chipX;
        param.chipY = this->param.chipY;
        param.chipNumber = this->param.chipNumber;
        param.dPointType = ui->posItembox->currentValue();
        param.usePrevPoint = this->param.param.usePrevPoint;
        dataHelper->mpCoordinateSettingController->setParam(param);
        dataHelper->mpCoordinateSettingController->show();
        MainThreadInvoker::getInvoker().moveToParentCenter(this, dataHelper->mpCoordinateSettingController);
    });

    connect(dataHelper->addressTemplateController,
            &RelationIDPMultiMSTemplateEditController::openDynamicLoader,
            this,
            [this]() {
                dataHelper->templateSelectStatus = 0;
                openTemplateSelector(TEMP_MS);
            });
    connect(dataHelper->addressTemplateController,
            &RelationIDPMultiMSTemplateEditController::openStaticLoader,
            this,
            [this]() {
                dataHelper->templateSelectStatus = 1;
                openTemplateSelector(TEMP_MS);
            });
    connect(dataHelper->addressTemplateController, &RelationIDPMultiMSTemplateEditController::reloadTemplate, this, [this]() {
        if (param.param.addressTemplate.templateId == 0) {
            return;
        }
        overloadMSTemplateData(true);
    });
    connect(dataHelper->addressTemplateController, &RelationIDPMultiMSTemplateEditController::saveTemplate, this, [this]() {
        dataHelper->templateSelectType = TEMP_MS;
        createSaveBox();
        dataHelper->saveBoxController->setCheckBoxText("Save using the same name as Address Template");
        idpOpenSaveBox(this, ACaller, param.param.addressTemplate, dataHelper->saveBoxController);
    });

    connect(dataHelper->mpTemplateController, &RelationIDPMPTemplateEditController::openDynamicLoader, this, [this]() {
        dataHelper->templateSelectStatus = 0;
        openTemplateSelector(TEMP_MP);
    });
    connect(dataHelper->mpTemplateController, &RelationIDPMPTemplateEditController::openStaticLoader, this, [this]() {
        dataHelper->templateSelectStatus = 1;
        openTemplateSelector(TEMP_MP);
    });
    connect(dataHelper->mpTemplateController, &RelationIDPMPTemplateEditController::reloadTemplate, this, [this]() {
        if (param.param.mpTemplate.templateId == 0) {
            return;
        }
        overloadMPTemplateData(true);
    });
    connect(dataHelper->mpTemplateController, &RelationIDPMPTemplateEditController::saveTemplate, this, [this]() {
        dataHelper->templateSelectType = TEMP_MP;
        createSaveBox();
        dataHelper->saveBoxController->setCheckBoxText("Save using the same name as Measurement Template");
        idpOpenSaveBox(this, ACaller, param.param.mpTemplate, dataHelper->saveBoxController);
    });

    connect(ui->handleTypeItembox->getQComboBox(),
            &QComboBox::currentIndexChanged,
            this,
            [this](int index) {
                dataHelper->mpTemplateController->setHandleType(index);
            });

    connect(ui->posItembox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int){
        int val = ui->posItembox->currentValue();
        if (val == 1 && (param.prevCoordinate.x == nullptr || param.prevCoordinate.y == nullptr)) {
            simpleMessageBox(this, "Error", "Previous Pattern Group No. not found. Relative Position unavailable.");
            ui->posItembox->setCurrentData(0);
            return;
        }
        param.param.dPointType = val;
        dataHelper->setDTextFieldValue();
    });

    connect(ui->titleTempDLoadAct, &QAction::triggered, this, [this]() {
        createMpipSelector();
        dataHelper->mpipSelectorStatus = 0;
        dataHelper->mpipSelectController->setWindowTitle("File Selector: Load");
        dataHelper->mpipSelectController->show();
        MainThreadInvoker::getInvoker().moveToScreenCenter(dataHelper->mpipSelectController);
        dataHelper->mpipSelectController->initMpIpList(TEMP_MPIP);
    });
    connect(ui->titleTempSLoadAct, &QAction::triggered, this, [this]() {
        createMpipSelector();
        dataHelper->mpipSelectorStatus = 1;
        dataHelper->mpipSelectController->setWindowTitle("File Selector: Load");
        dataHelper->mpipSelectController->show();
        MainThreadInvoker::getInvoker().moveToScreenCenter(dataHelper->mpipSelectController);
        dataHelper->mpipSelectController->initMpIpList(TEMP_MPIP);
    });
    connect(ui->titleSaveAct, &QAction::triggered, this, [this]() {
        createMpipSelector();
        dataHelper->mpipSelectorStatus = 2;
        dataHelper->mpipSelectController->setWindowTitle("File Selector: Save");
        dataHelper->mpipSelectController->show();
        MainThreadInvoker::getInvoker().moveToScreenCenter(dataHelper->mpipSelectController);
        dataHelper->mpipSelectController->initMpIpList(TEMP_MPIP);
    });

    connect(dataHelper->addressTemplateController, &RelationIDPMultiMSTemplateEditController::toPrevEditor, this, [this](){
        ui->regPosMoreBtn->getQPushButton()->click();
    });
    connect(dataHelper->addressTemplateController, &RelationIDPMultiMSTemplateEditController::toNextEditor, this, [this](){
        dataHelper->mpTemplateController->openFirstEditor();
    });
    connect(dataHelper->addressTemplateController, &RelationIDPMultiMSTemplateEditController::shouldHideParent, this, [this](bool d){
        setVisible(!d);
    });
    connect(dataHelper->mpTemplateController, &RelationIDPMPTemplateEditController::toPrevEditor, this, [this](){
        dataHelper->addressTemplateController->openEndEditor();
    });
    connect(dataHelper->mpTemplateController, &RelationIDPMPTemplateEditController::shouldHideParent, this, [this](bool d){
        setVisible(!d);
    });
    connect(ui->dxTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        dataHelper->updateDPointXFromTextField();
    });
    connect(ui->dyTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        dataHelper->updateDPointYFromTextField();
    });
    connect(ui->regPosBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        dataHelper->dRegCoordinate();
    });
    connect(ui->movePosBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        dataHelper->dMovePos();
    });

    connect(ui->testBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        dataHelper->testTrigger();
    });

    connect(dataHelper->mpTemplateController, &RelationIDPMPTemplateEditController::posChanged, this, [this](const IDW_PointF& pos){
        param.param.dPoint.x = pos.x - param.oPoint.x;
        param.param.dPoint.y = pos.y - param.oPoint.y;
        dataHelper->setDTextFieldValue(2);
    });
}

void IdpMpDetailEditorController::resetToComponent() {
    dataHelper->resetGroup();
    ui->seqTitleTextield->setText(param.index + 1);
    if (param.param.dPointType == 1 && (param.prevCoordinate.x == nullptr || param.prevCoordinate.y == nullptr)) {
        simpleMessageBox(this, "Error", "The previous Pattern Group No. is missing. The displayed coordinates will be automatically corrected.");
        param.param.dPointType = 0;
    }
    ui->posItembox->setCurrentData(param.param.dPointType);
    dataHelper->setDTextFieldValue();
    ui->handleTypeItembox->setCurrentIndex(param.param.seqType);
    ui->templateTitleTextField->setText(param.param.mpipTemplateName);
    if (param.param.mpipTemplateId > 0 && param.param.mpipTemplateRefType == 0) {
        ui->handleTypeItembox->getQComboBox()->setDisabled(true);
        addCssClass(*ui->handleTypeItembox->getQComboBox(), "BlueText");
    } else {
        ui->handleTypeItembox->getQComboBox()->setDisabled(false);
        removeCssClass(*ui->handleTypeItembox->getQComboBox(), "BlueText");
    }
    ui->handleTypeItembox->getQComboBox()->style()->unpolish(ui->handleTypeItembox->getQComboBox());
    ui->handleTypeItembox->getQComboBox()->style()->polish(ui->handleTypeItembox->getQComboBox());
    resetMsToComponent();
    resetMpToComponent();
}

void IdpMpDetailEditorController::resetMsToComponent() {
    dataHelper->_resetMSComponent();
    if (param.param.addressTemplate.templateId > 0 && param.param.addressTemplate.templateRefType == 0) {
        dataHelper->dynamicOriginMSData.reset(new MultiAddressTemplateParam(param.param.addressTemplate.templateParam));
    } else if (dataHelper->dynamicOriginMSData)
        dataHelper->dynamicOriginMSData.reset();
}
void IdpMpDetailEditorController::resetMpToComponent() {
    dataHelper->_resetMPComponent();
    if (param.param.mpTemplate.templateId > 0 && param.param.mpTemplate.templateRefType == 0) {
        dataHelper->dynamicOriginMPData.reset(new MpTemplateParam(param.param.mpTemplate.templateParam));
    } else if (dataHelper->dynamicOriginMPData)
        dataHelper->dynamicOriginMPData.reset();
}

QString IdpMpDetailEditorController::collectData() {
    param.param.dPointType = ui->posItembox->currentValue();
    if (param.param.dPointType == 1 && (param.prevCoordinate.x == nullptr || param.prevCoordinate.y == nullptr)) {
        return "Previous Pattern Group No. not found. Relative Position unavailable.";
    }
    QString val;
    if (dataHelper->addressTemplateController->rotateTemplateCheckBox()->isEnabled()
        && dataHelper->addressTemplateController->rotateTemplateCheckBox()->isChecked()) {
        val = dataHelper->addressTemplateController->rotateTemplateTextField()->text();
        if (val.isEmpty()) {
            val = "0";
        }
        param.param.addressRotate = val;
    } else {
        param.param.addressRotate = 0;
    }
    if (dataHelper->mpTemplateController->rotateTemplateCheckBox()->isEnabled()
        && dataHelper->mpTemplateController->rotateTemplateCheckBox()->isChecked()) {
        val = dataHelper->mpTemplateController->rotateTemplateTextField()->text();
        if (val.isEmpty()) {
            val = "0";
        }
        param.param.measurementRotate = val;
    } else {
        param.param.measurementRotate = 0;
    }
    val = ui->dxTextField->text();
    if (val.isEmpty()) {
        val = "0";
    }
    param.param.dPoint.x = val;
    val = ui->dyTextField->text();
    if (val.isEmpty()) {
        val = "0";
    }
    param.param.dPoint.y = val;
    if (param.param.dPointType == 1) {
        param.param.dPoint.x += param.prevCoordinate.x;
        param.param.dPoint.y += param.prevCoordinate.y;
    }
    param.param.seqType = ui->handleTypeItembox->currentIndex();
    param.param.mpipTemplateName = ui->templateTitleTextField->text();
    QString errMsg = dataHelper->addressTemplateController->collectData();
    if (errMsg.length() > 0) {
        return errMsg;
    }
    return dataHelper->mpTemplateController->collectData();
}

void IdpMpDetailEditorController::createCoordinateSettingController() {
    if (!dataHelper->mpCoordinateSettingController) {
        dataHelper->mpCoordinateSettingController = new IdpMpCoordinateSettingController(this);
        dataHelper->mpCoordinateSettingController->setResizable(false);
        dataHelper->mpCoordinateSettingController->initAsyncCaller(ACaller);
        dataHelper->mpCoordinateSettingController->setWindowTitle("Coordinate");
        dataHelper->mpCoordinateSettingController->disablePyHandlers(dataHelper->pyHandlerDisabled);
        dataHelper->mpCoordinateSettingController->setCloseCall([this](CDE90WindowCloseStatus status){
            if (status == HW_NEXT) {
                dataHelper->addressTemplateController->openFirstEditor();
            } else if (status == HW_CLOSE || status == HW_OK) {
                if (isHidden()) show();
            }
            return true;
        });
        dataHelper->mpCoordinateSettingController->setOkCallback(
            [this](const IdpMpCoordinateSettingParam& param) {
                this->param.param.dPoint = param.dPoint;
                this->param.param.dPointType = param.dPointType;
                ui->posItembox->setCurrentData(param.dPointType);
                dataHelper->setDTextFieldValue();
                this->param.param.usePrevPoint = param.usePrevPoint;
                //TODO 待完善
                return true;
            });
    }
}

void IdpMpDetailEditorController::createTemplateSelector() {
    if (dataHelper->templateSelectorController) {
        return;
    }
    dataHelper->templateSelectorController = new TemplateSelectorController(this);
    dataHelper->templateSelectorController->initAsyncCaller(ACaller);
    dataHelper->templateSelectorController->setWindowTitle("Template Selector");
    dataHelper->templateSelectorController->setTitle("AP");
    dataHelper->templateSelectorController->setOkCallback([this](const ListViewData& data) {
        if (data.getId() == 0) {
            simpleMessageBox(this, "Info", "Please select exactly one file.");
            return false;
        }
        if (dataHelper->templateSelectStatus != 0 && dataHelper->templateSelectStatus != 1) {
            return true;
        }
        if (dataHelper->templateSelectType == TEMP_MS) {
            param.param.addressTemplate.templateId = data.getId();
            param.param.addressTemplate.templateRefType = dataHelper->templateSelectStatus;
        } else if (dataHelper->templateSelectType == TEMP_MP) {
            param.param.mpTemplate.templateId = data.getId();
            param.param.mpTemplate.templateRefType = dataHelper->templateSelectStatus;
        } else {
            return true;
        }
        if (dataHelper->templateSelectType == TEMP_MS) {
            overloadMSTemplateData();
        } else {
            overloadMPTemplateData();
        }
        return false;
    });
}
void IdpMpDetailEditorController::openTemplateSelector(
    char type) {
    dataHelper->templateSelectType = type;
    createTemplateSelector();
    dataHelper->templateSelectorController->show();
    TemplateInfoSearchDTO param;
    param.setInvalid(0);
    param.setTemplateType(type);
    dataHelper->templateSelectorController->load(param);
}
void IdpMpDetailEditorController::createSaveBox() {
    if (dataHelper->saveBoxController) {
        return;
    }
    dataHelper->saveBoxController = new TemplateSaveBoxController(this);
    dataHelper->saveBoxController->setResizable(false);
    dataHelper->saveBoxController->setWindowTitle("Template Save");
    dataHelper->saveBoxController->setWindowModality(Qt::WindowModal);
    dataHelper->saveBoxController->setOkCallback([this](const TemplateSaveBoxData& data) {
        int refType;
        if (dataHelper->templateSelectType == TEMP_MS) {
            if (param.param.addressTemplate.templateId == 0 && data.text.isEmpty()) {
                simpleMessageBox(dataHelper->saveBoxController, "Info", "Please input name.");
                return false;
            }
            QString errMsg = dataHelper->addressTemplateController->collectData();
            if (errMsg.length() > 0) {
                simpleMessageBox(dataHelper->saveBoxController, "Error", errMsg);
                return false;
            }
            refType = this->param.param.addressTemplate.templateRefType;
        } else if (dataHelper->templateSelectType == TEMP_MP) {
            if (param.param.mpTemplate.templateId == 0 && data.text.isEmpty()) {
                simpleMessageBox(dataHelper->saveBoxController, "Info", "Please input name.");
                return false;
            }
            QString errMsg = dataHelper->mpTemplateController->collectData();
            if (errMsg.length() > 0) {
                simpleMessageBox(dataHelper->saveBoxController, "Error", errMsg);
                return false;
            }
            refType = this->param.param.mpTemplate.templateRefType;
        } else {
            return true;
        }
        bool checked = data.checked;
        bool succ;
        if (dataHelper->templateSelectType == TEMP_MS) {
            MultiAddressTemplateInfoModel param;
            idpPrepareSaveBoxCall(param,
                                  this->param.param.addressTemplate.templateParam,
                                  this->param.param.addressTemplate.templateId,
                                  data.text,
                                  TEMP_MS);
            succ = ACaller->push(this, [this, param, refType, checked](TSC) mutable {
                dataHelper->msSaveBoxCaller->idpSaveBoxCall(param, refType, checked);
            });
        } else {
            MpTemplateInfoModel param;
            idpPrepareSaveBoxCall(param,
                                  this->param.param.mpTemplate.templateParam,
                                  this->param.param.mpTemplate.templateId,
                                  data.text,
                                  TEMP_MP);
            succ = ACaller->push(this, [this, param, refType, checked](TSC) mutable {
                dataHelper->mpSaveBoxCaller->idpSaveBoxCall(param, refType, checked);
            });
        }

        if (succ) {
            loadingBox(dataHelper->saveBoxController, "Working", "Saving...");
        }
        return false;
    });

    dataHelper->msSaveBoxCaller = new IdpSaveBoxSaveCaller<MultiAddressTemplateParam>;
    dataHelper->msSaveBoxCaller->setRelationType(IRDT_TEMPLATE);
    dataHelper->msSaveBoxCaller->setSuccCallback(
        [this](quint64 tid, const TemplateInfoModel<MultiAddressTemplateParam>& param, const QList<quint64>& imageIds) {
            AddressTemplateLinkedModel& model = this->param.param.addressTemplate;
            if (model.templateParam.addressList.size() != param.data.addressList.size()) {
                simpleMessageBox(this, "Error", "Data changed while MS is saving, but data still saved.");
                return;
            }
            for (int i = 0; i < param.data.addressList.size(); ++ i) {
                MultiAddressDetailTemplateParam& c1 = this->param.param.addressTemplate.templateParam.addressList[i];
                const MultiAddressDetailTemplateParam& c2 = param.data.addressList[i];
                if (c1.patternRegParam.imagePath != c2.patternRegParam.imagePath) {
                    simpleMessageBox(this, "Error", "Data changed while MS is saving, but data still saved.");
                    return;
                }
            }
            model.templateId = param.id;
            if (dataHelper->dynamicOriginMSData) *dataHelper->dynamicOriginMSData = model.templateParam;
            for (int i = 0; i < param.data.addressList.size(); ++ i) {
                MultiAddressDetailTemplateParam& c1 = this->param.param.addressTemplate.templateParam.addressList[i];
                const MultiAddressDetailTemplateParam& c2 = param.data.addressList[i];
                if (c1.patternRegParam.imageId == 0 && c1.patternRegParam.imagePath.length() > 0 && c2.patternRegParam.imageId > 0) {
                    c1.patternRegParam.imageId = c2.patternRegParam.imageId;
                }
            }
        });

    dataHelper->msSaveBoxCaller->setSaveImageCallback(
        [this](TemplateInfoModel<MultiAddressTemplateParam>& param, QList<quint64>& imageIds) {
            cdsem::TmpImageData image;
            QList<FileImageDuplicateSaveData> list;
            list.reserve(param.data.addressList.size());
            FileImageDuplicateSaveData saveData;
            QString json;
            for (int i = 0; i < param.data.addressList.size(); ++ i) {
                MultiAddressDetailTemplateParam& address = param.data.addressList[i];
                if (address.patternRegParam.imageId == 0 && address.patternRegParam.imagePath.length() > 0) {
                    saveData.path = address.patternRegParam.imagePath;
                    toImageDetailDataModelJson(json, address.patternRegParam.infoParam.imageAdditionData);
                    saveData.entity.setImageInfo(json);
                    list.append(saveData);
                }
            }
            if (list.isEmpty()) return true;
            ImageInfoService* service = ServiceManager::instance()
                                            ->getDefaultService<ImageInfoService>();
            OperationResult result;
            service->save(list, &result);
            if (result.errCode) {
                simpleMessageBox(dataHelper->saveBoxController, "Error", "Error to save image data.");
                return false;
            }
            for (FileImageDuplicateSaveData& f : list) {
                imageIds.append(f.entity.getId());
            }
            int i = 0;
            for (MultiAddressDetailTemplateParam& address : param.data.addressList) {
                if (address.patternRegParam.imageId == 0 && address.patternRegParam.imagePath.length() > 0) {
                    address.patternRegParam.imageId = imageIds[i];
                    ++ i;
                }
            }
            return true;
        });
    dataHelper->msSaveBoxCaller->setSaveBox(dataHelper->saveBoxController);

    dataHelper->mpSaveBoxCaller = new IdpSaveBoxSaveCaller<MpTemplateParam>;
    dataHelper->mpSaveBoxCaller->setRelationType(IRDT_TEMPLATE);
    dataHelper->mpSaveBoxCaller->setSuccCallback([this](quint64 tid, const TemplateInfoModel<MpTemplateParam>& param, const QList<quint64>& imageIds) {
        Q_UNUSED(param)
        MpTemplateLinkedModel& model = this->param.param.mpTemplate;
        model.templateId = param.id;
        if (dataHelper->dynamicOriginMPData) *dataHelper->dynamicOriginMPData = model.templateParam;
        if (imageIds.size() > 0) {
            model.templateParam.exeParam.imageId = imageIds[imageIds.size() - 1];
            model.templateParam.patternRegParam.imageId = imageIds[0];
            model.templateParam.patternRegParam.imagePath = "";
            model.templateParam.exeParam.imagePath = "";
        }
        simpleMessageBox(this,"Info",
                             QString("Save MP template completely."));
    });
    dataHelper->mpSaveBoxCaller->setSaveImageCallback(
        [this](TemplateInfoModel<MpTemplateParam>& param, QList<quint64>& imageIds) {
            QList<FileImageDuplicateSaveData> list;
            list.reserve(2);
            if (param.data.patternRegParam.imageId == 0 && param.data.patternRegParam.imagePath.length() > 0) {
                FileImageDuplicateSaveData saveData;
                QString json;
                toImageDetailDataModelJson(json, param.data.patternRegParam.infoParam.imageAdditionData);
                saveData.path = param.data.patternRegParam.imagePath;
                saveData.entity.setImageInfo(json);
                saveData.entity.setDataType(cdsem::IDT_MP_PATTERN);
                list.append(saveData);
                if (param.data.exeParam.imageId == 0 && param.data.exeParam.imagePath.length() > 0 && param.data.patternRegParam.imagePath != param.data.exeParam.imagePath) {
                    FileImageDuplicateSaveData saveData;
                    saveData.path = param.data.exeParam.imagePath;
                    QString json;
                    toImageDetailDataModelJson(json, param.data.exeParam.imageAddition);
                    saveData.entity.setImageInfo(json);
                    saveData.entity.setDataType(cdsem::IDT_MP_EXE_PATTERN);
                    list.append(saveData);
                }
                ImageInfoService* service = ServiceManager::instance()
                                                ->getDefaultService<ImageInfoService>();
                OperationResult result;
                service->save(list, &result);
                if (result.errCode) {
                    simpleMessageBox(dataHelper->saveBoxController, "Error", "Error to save image data.");
                    return false;
                }
                for (FileImageDuplicateSaveData& f : list) {
                    imageIds.append(f.entity.getId());
                }
                if (imageIds.size() > 0) {
                    param.data.patternRegParam.imageId = imageIds[0];
                    if (imageIds.size() > 1) {
                        param.data.exeParam.imageId = imageIds[1];
                    }
                }
            }
            return true;
        });
    dataHelper->mpSaveBoxCaller->setSaveBox(dataHelper->saveBoxController);
}

void IdpMpDetailEditorController::createMpipSelector() {
    if (dataHelper->mpipSelectController) {
        return;
    }
    dataHelper->mpipSelectController = new MPIPTemplateSelectController(this);
    dataHelper->mpipSelectController->initAsyncCaller(ACaller);
    dataHelper->mpipSelectController->setSubTitle("MP/IP Information");
    dataHelper->mpipSelectController->setOkCallback([this]() {
        dataHelper->mpipSelectController->simpleGetSelectedData(dataHelper->titleSelectorCallback);
        return false;
    });
}

void IdpMpDetailEditorController::localOverloadMSTemplateData(
    quint64 templateId, char refType, bool overview) {
    TemplateInfoService* service = ServiceManager::instance()
                                       ->getDefaultService<TemplateInfoService>();
    int errCode;
    TemplateInfoEntity entity = refType == 0 && !overview
                                    ? service->getDataByIdForNewVersion(templateId, &errCode)
                                    : service->getById(templateId, &errCode);
    if (errCode) {
        openErrMsgBox("Load MS template data error.");
        return;
    }
    if (entity.getId() == 0 || entity.getTemplateType() != TEMP_MS) {
        openErrMsgBox("This linked MS data is not exists.");
        return;
    }
    MultiAddressTemplateInfoModel model;
    errCode = templateInfoModelFromEntity(entity, model);
    if (errCode) {
        openErrMsgBox("Load MS template data error.");
        return;
    }
    MainThreadInvoker::getInvoker().blockRun([this, &model, overview]() {
        QSharedPointer<AddressTemplateLinkedModel> oldParam;
        if (overview) {
            //如果是临时图片，调用该函数让其保存临时图片
            dataHelper->addressTemplateController->collectData();
            oldParam.reset(new AddressTemplateLinkedModel);
            *oldParam.data() = param.param.addressTemplate;
            //import会强制转静态模板
            param.param.addressTemplate.templateRefType = 1;
        }
        param.param.addressTemplate.templateId = model.id;
        param.param.addressTemplate.templateParam = model.data;
        dataHelper->_resetMSComponent();
        if (dataHelper->templateSelectorController && dataHelper->templateSelectorController->isVisible()) {
            dataHelper->templateSelectorController->close();
        }
        if (overview) {
            dataHelper->questionBox
                = idpTemplateShowYesNoBox(this,
                                          "Question",
                                          "Linked template information is imported.\nIs it OK?",
                                          false,
                                          [this, oldParam](int r) {
                                              if (r) {
                                                  param.param.addressTemplate = *oldParam.data();
                                                  dataHelper->_resetMSComponent();
                                              } else if (dataHelper->dynamicOriginMSData) {
                                                  dataHelper->dynamicOriginMSData.reset();
                                              }
                                              return true;
                                          });
        }else if (param.param.addressTemplate.templateRefType == 0) {
            dataHelper->dynamicOriginMSData.reset(new MultiAddressTemplateParam(param.param.addressTemplate.templateParam));
        } else if (dataHelper->dynamicOriginMSData) {
            dataHelper->dynamicOriginMSData.reset();
        }
    });
}

void IdpMpDetailEditorController::overloadMSTemplateData(
    bool overview) {
    char refType = param.param.addressTemplate.templateRefType;
    quint64 templateId = param.param.addressTemplate.templateId;
    bool succ = ACaller->push(this, [this, templateId, refType, overview](TSC) {
        localOverloadMSTemplateData(templateId, refType, overview);
    });
    if (succ) {
        loadingBox(this, "Working", "Loading");
    }
}

void IdpMpDetailEditorController::localOverloadMPTemplateData(
    quint64 templateId, char refType, bool overview) {
    TemplateInfoService* service = ServiceManager::instance()
                                       ->getDefaultService<TemplateInfoService>();
    int errCode;
    TemplateInfoEntity entity = refType == 0 && !overview
                                    ? service->getDataByIdForNewVersion(templateId, &errCode)
                                    : service->getById(templateId, &errCode);

    if (errCode) {
        openErrMsgBox("Load MP template data error.");
        return;
    }
    if (entity.getId() == 0 || entity.getTemplateType() != TEMP_MP) {
        openErrMsgBox("This linked MP data is not exists.");
        return;
    }
    MpTemplateInfoModel model;
    errCode = templateInfoModelFromEntity(entity, model);
    if (errCode) {
        openErrMsgBox("Load MP template data error.");
        return;
    }
    MainThreadInvoker::getInvoker().blockRun([this, &model, overview]() {
        QSharedPointer<MpTemplateLinkedModel> oldParam;
        if (overview) {
            //如果是临时图片，调用该函数让其保存临时图片
            dataHelper->mpTemplateController->collectData();
            oldParam.reset(new MpTemplateLinkedModel);
            *oldParam.data() = param.param.mpTemplate;
            //import会强制转静态模板
            param.param.mpTemplate.templateRefType = 1;
        }
        param.param.mpTemplate.templateId = model.id;
        param.param.mpTemplate.templateParam = model.data;
        dataHelper->_resetMPComponent();
        if (dataHelper->templateSelectorController && dataHelper->templateSelectorController->isVisible()) {
            dataHelper->templateSelectorController->close();
        }
        if (overview) {
            dataHelper->questionBox
                = idpTemplateShowYesNoBox(this,
                                          "Question",
                                          "Linked template information is imported.\nIs it OK?",
                                          false,
                                          [this, oldParam](int r) {
                                              if (r) {
                                                  param.param.mpTemplate = *oldParam.data();
                                                  dataHelper->_resetMPComponent();
                                              } else if (dataHelper->dynamicOriginMPData) {
                                                  dataHelper->dynamicOriginMPData.reset();
                                              }
                                              return true;
                                          });
        }else if (param.param.mpTemplate.templateRefType == 0) {
            dataHelper->dynamicOriginMPData.reset(new MpTemplateParam(param.param.mpTemplate.templateParam));
        } else if (dataHelper->dynamicOriginMPData) {
            dataHelper->dynamicOriginMPData.reset();
        }
    });
}

void IdpMpDetailEditorController::overloadMPTemplateData(
    bool overview) {
    char refType = param.param.mpTemplate.templateRefType;
    quint64 templateId = param.param.mpTemplate.templateId;
    bool succ = ACaller->push(this, [this, templateId, refType, overview](TSC) {
        localOverloadMPTemplateData(templateId, refType, overview);
    });
    if (succ) {
        loadingBox(this, "Working", "Loading");
    }
}
void IdpMpDetailEditorController::openErrMsgBox(
    const QString& text) {
    MainThreadInvoker::getInvoker().blockRun([&text, this]() {
        QWidget* widget;
        if (dataHelper->templateSelectorController && dataHelper->templateSelectorController->isVisible()) {
            widget = dataHelper->templateSelectorController;
        } else {
            widget = this;
        }
        simpleMessageBox(widget, "Error", text);
    });
}

bool IdpMpDetailEditorController::titleSelectCall(
    const ListViewMainData& data) {
    if (dataHelper->mpipSelectorStatus == 0 || dataHelper->mpipSelectorStatus == 1) {
        if (data.getId() == 0) {
            simpleMessageBox(this, "Error", "Please select a template file.");
        } else {
            loadTitleTemplate(data);
        }

        return false;
    }
    if (data.getText().isEmpty()) {
        simpleMessageBox(this, "Error", "Please specify file name.");
        return false;
    }
    saveTitleTemplate(data);
    return false;
}

void IdpMpDetailEditorController::loadTitleTemplate(
    const ListViewMainData& data) {
    char refType = dataHelper->mpipSelectorStatus;
    bool succ = ACaller->push(this, [this, data, refType](TSC) {
        bool success = false;
        sem::AfterGc gc([this, &success]() {
            MainThreadInvoker::getInvoker().runLater([this, success]() {
                if (success && dataHelper->mpipSelectController) {
                    dataHelper->mpipSelectController->close();
                }
            });
        });
        TemplateInfoService* service = ServiceManager::instance()
                                           ->getDefaultService<TemplateInfoService>();
        int errCode;
        TemplateInfoEntity entity = service->getById(data.getId(), &errCode);
        if (errCode) {
            simpleMessageBox(this, "Error", "Error to load MP/IP information.");
            return;
        }
        if (entity.getId() == 0) {
            simpleMessageBox(this, "Error", "MP/IP information is not exists.");
            return;
        }
        MPIPTemplateInfoModel model;
        errCode = templateInfoModelDataFromEntity(entity, model);
        if (errCode) {
            simpleMessageBox(this, "Error", "Error to load MP/IP information.");
            return;
        }
        success = true;
        MainThreadInvoker::getInvoker().blockRun([this, &data, &model, refType]() {
            param.param.dPointType = 0;
            param.param.mpipTemplateId = data.getId();
            param.param.mpipTemplateRefType = refType;
            param.param.addressTemplate.templateId = model.msId;
            param.param.addressTemplate.templateRefType = refType;
            param.param.mpTemplate.templateId = model.mpId;
            param.param.mpTemplate.templateRefType = refType;
            param.param.dPoint = {model.dx, model.dy};
            ui->posItembox->setCurrentData(param.param.dPointType);
            ui->posItembox->setCurrentData(0);
            param.param.usePrevPoint = false;
            ui->templateTitleTextField->setText(model.name);
            dataHelper->setDTextFieldValue();
        });
        if (model.msId > 0) {
            localOverloadMSTemplateData(model.msId, refType);
        }
        if (model.mpId > 0) {
            localOverloadMPTemplateData(model.mpId, refType);
        }
        MainThreadInvoker::getInvoker().blockRun([this, refType]() {
            if (refType == 0) {
                ui->handleTypeItembox->getQComboBox()->setDisabled(true);
                addCssClass(*ui->handleTypeItembox->getQComboBox(), "BlueText");
            } else {
                ui->handleTypeItembox->getQComboBox()->setDisabled(false);
                removeCssClass(*ui->handleTypeItembox->getQComboBox(), "BlueText");
            }
            ui->handleTypeItembox->getQComboBox()->style()->unpolish(
                ui->handleTypeItembox->getQComboBox());
            ui->handleTypeItembox->getQComboBox()->style()->polish(
                ui->handleTypeItembox->getQComboBox());
        });
    });
    if (succ) {
        loadingBox(this, "Working", "Loading");
    }
}

void IdpMpDetailEditorController::saveTitleTemplate(
    const ListViewMainData& data) {
    quint64 currId = param.param.mpipTemplateId;
    bool succ = ACaller->push(this, [this, data, currId](TSC) mutable {
        TemplateInfoService* service = ServiceManager::instance()
                                           ->getDefaultService<TemplateInfoService>();

        if (data.getId() > 0 && currId != data.getId()) {
            int r = showYesNoBox(
                this,
                "Question",
                QString("The template file \"%1\" is exists, do you want to overwrite it?")
                    .arg(data.getText()));
            if (r) {
                return;
            }
        }

        TemplateInfoEntity entity;
        MPIPTemplateInfoModel model;
        MainThreadInvoker::getInvoker().blockRun(
            [this, &model]() { collectTemplateTitleData(model); });
        model.name = data.getText();
        templateInfoModelDataToEntity(entity, model);
        int errCode;
        if (currId > 0) {
            TemplateInfoEntity tmpEntity = service->getSimpleInfoById(currId, &errCode);
            if (errCode) {
                simpleMessageBox(this, "Error", "Error to load MP/IP information.");
                return;
            }
            entity.setTemplateComment(tmpEntity.getTemplateComment());
        }
        errCode = service->overrideAndSave(entity, data.getId());
        if (errCode) {
            simpleMessageBox(this, "Error", "Error to save MP/IP information.");
            return;
        }
        currId = entity.getId();
        MainThreadInvoker::getInvoker().blockRun([this, &data, &currId]() {
            simpleMessageBox(this, "Info", "Save MP/IP information successfully.");
            if (dataHelper->mpipSelectController && dataHelper->mpipSelectController)
                dataHelper->mpipSelectController->close();
            ui->templateTitleTextField->setText(data.getText());
            param.param.mpipTemplateId = currId;
        });
    });
    if (succ) {
        loadingBox(this, "Working", "Loading");
    }
}

void IdpMpDetailEditorController::collectTemplateTitleData(
    MPIPTemplateInfoModel& model) {
    model.dx = ui->dxTextField->text();
    model.dy = ui->dyTextField->text();
    model.msId = param.param.addressTemplate.templateId;
    model.mpId = param.param.mpTemplate.templateId;
    model.name = ui->templateTitleTextField->text();
}

void IdpMpDetailEditorController::closeEvent(QCloseEvent *event) {
    CDE90Window::closeEvent(event);
    if (event->isAccepted()) {
        dataHelper->questionBox.reset();
        dataHelper->dynamicOriginMSData.reset();
        dataHelper->dynamicOriginMPData.reset();
    }
}

//禁用一切与机台互动相关的操作
void IdpMpDetailEditorController::disablePyHandlers(bool b) {
    dataHelper->pyHandlerDisabled = b;
    ui->regPosBtn->getQPushButton()->setDisabled(b);
    ui->movePosBtn->getQPushButton()->setDisabled(b);
    if (dataHelper->addressTemplateController) {
        dataHelper->addressTemplateController->disablePyHandlers(b);
    }
    if (dataHelper->mpTemplateController) {
        dataHelper->mpTemplateController->disablePyHandlers(b);
    }
    if (dataHelper->mpCoordinateSettingController) {
        dataHelper->mpCoordinateSettingController->disablePyHandlers(b);
    }
    ui->testBtn->setDisabled(b);
    ui->wizardBtn->setDisabled(b);
}

bool IdpMpDetailEditorController::checkDynamicTemplateStatus() {
    return (param.param.addressTemplate.templateId > 0 && param.param.addressTemplate.templateRefType == 0
        && dataHelper->dynamicOriginMSData && param.param.addressTemplate.templateParam != *dataHelper->dynamicOriginMSData)
    || (param.param.mpTemplate.templateId > 0 && param.param.mpTemplate.templateRefType == 0
        && dataHelper->dynamicOriginMPData && param.param.mpTemplate.templateParam != *dataHelper->dynamicOriginMPData);
}

void IdpMpDetailEditorController::recoverToDynamicTemplateData() {
    if (param.param.addressTemplate.templateId > 0 && param.param.addressTemplate.templateRefType == 0 && dataHelper->dynamicOriginMSData) {
        param.param.addressTemplate.templateParam = *dataHelper->dynamicOriginMSData.get();
    }
    if (param.param.mpTemplate.templateId > 0 && param.param.mpTemplate.templateRefType == 0 && dataHelper->dynamicOriginMPData) {
        param.param.mpTemplate.templateParam = *dataHelper->dynamicOriginMPData.get();
    }
}