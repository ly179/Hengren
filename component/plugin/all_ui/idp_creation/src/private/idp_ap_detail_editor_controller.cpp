#include "idp_ap_detail_editor_controller.h"
#include "cde90_message_box.h"
#include "cde90_process_box.h"
#include "cdsem_number_validator_helper.h"
#include "relation_idp_ap_template_edit_controller.h"
#include "template_relation.h"
#include "idp_message_box_helper.h"
#include "service_manager.h"
#include "template_info_entity.h"
#include "template_info_model_convert.h"
#include "template_info_service.h"
#include "template_save_box_controller.h"
#include "template_selector_controller.h"
#include "ui_idp_ap_detail_editor_controller.h"
#include "view_data_mode.h"
#include "sem_common_data_model.h"
#include "cdsem_global_data.h"
#include "cdsem_control_helper.h"
#include "multi_shot_chip_select_controller.h"
#include "template_handler_helper.h"
#include "image_file_temp_handler.h"
#include "image_save_model_convert.h"

struct IdpApDetailEditorDataHelper {
    QSharedPointer<CDE90MessageBox> questionBox;
    //用于比对动态载入的模板是否发生过改变
    QScopedPointer<ApTemplateParam> dynamicOriginData;
    RelationIDPAPTemplateEditController* templateController = nullptr;
    TemplateSelectorController* templateSelectorController = nullptr;
    TemplateSaveBoxController* saveBoxController = nullptr;
    IdpSaveBoxSaveCaller<ApTemplateParam>* apSaveBoxCaller = nullptr;
    MultiShotChipSelectController* multiShotChipSelector = nullptr;
    IdpApDetailEditorController* d;

    //0 - 默认，点击Reg Origin按钮回调
    //1 - 点击Reg Coordinate按钮回调
    //2 - 点击Move Coordinate按钮回调
    int multiSelectedChipType = 0;

    //0 - dynamic 1 - static
    char templateSelectStatus;

    bool pyHandlerDisabled = false;

    void _resetComponents() {
        resetGroup();
        d->ui->titleNo->setText(d->param.index + 1);
        d->ui->originXTextField->setText(d->param.chipX.toString());
        d->ui->originYTextField->setText(d->param.chipY.toString());
        d->ui->dxTextField->setText(d->param.param.dPoint.x.toQStringAndKeepFullDecimal());
        d->ui->dyTextField->setText(d->param.param.dPoint.y.toQStringAndKeepFullDecimal());
        templateController->resetToComponent();
        templateController->toDynamicRefStyle(d->param.param.apTemplate.templateId > 0
                                              && d->param.param.apTemplate.templateRefType == 0);
        changeDPos();
    }

    void resetGroup() {
        if (templateController)
            templateController->setGroup(
                QString("idp_ap_detail_%1").arg(d->param.id == 0 ? "create" : QString::number(d->param.id)));
    }

    void createTemplateSelector() {
        if (templateSelectorController) {
            return;
        }
        templateSelectorController = new TemplateSelectorController(d);
        templateSelectorController->initAsyncCaller(d->getAsyncCaller());
        templateSelectorController->setWindowTitle("Template Selector");
        templateSelectorController->setTitle("AP");
        templateSelectorController->setOkCallback([this](const ListViewData& data) {
            if (data.getId() == 0) {
                simpleMessageBox(d, "Info", "Please select exactly one file.");
                return false;
            }
            if (templateSelectStatus != 0 && templateSelectStatus != 1) {
                return true;
            }
            d->param.param.apTemplate.templateId = data.getId();
            d->param.param.apTemplate.templateRefType = templateSelectStatus;
            overloadTemplateData();
            return false;
        });
    }
    void openTemplateSelector() {
        createTemplateSelector();
        templateSelectorController->show();
        TemplateInfoSearchDTO param;
        param.setInvalid(0);
        param.setTemplateType(TEMP_ALIGN);
        templateSelectorController->load(param);
    }
    void createSaveBox() {
        if (saveBoxController) {
            return;
        }
        saveBoxController = new TemplateSaveBoxController(d);
        saveBoxController->setCheckBoxText("Save using the same name as Alignment Template");
        saveBoxController->setResizable(false);
        saveBoxController->setWindowTitle("Template Save");
        saveBoxController->setWindowModality(Qt::WindowModal);
        saveBoxController->setOkCallback([this](const TemplateSaveBoxData& data) {
            if (d->param.param.apTemplate.templateId == 0 && data.text.isEmpty()) {
                simpleMessageBox(saveBoxController, "Info", "Please input name.");
                return false;
            }
            QString errMsg = templateController->collectData();
            if (errMsg.length() > 0) {
                simpleMessageBox(saveBoxController, "Error", errMsg);
                return false;
            }
            int refType = d->param.param.apTemplate.templateRefType;
            bool checked = data.checked;
            ApTemplateInfoModel param;
            idpPrepareSaveBoxCall(param,
                                  d->param.param.apTemplate.templateParam,
                                  d->param.param.apTemplate.templateId,
                                  data.text,
                                  TEMP_ALIGN);
            bool succ = d->getAsyncCaller()->push(d, [this, param, refType, checked](TSC) mutable {
                apSaveBoxCaller->idpSaveBoxCall(param, refType, checked);
            });

            if (succ) {
                loadingBoxHelper(d, "Working", "Saving...");
            }
            return false;
        });

        apSaveBoxCaller = new IdpSaveBoxSaveCaller<ApTemplateParam>;
        apSaveBoxCaller->setRelationType(IRDT_TEMPLATE);
        apSaveBoxCaller->setSuccCallback([this](quint64 tid, const TemplateInfoModel<ApTemplateParam>& param, const QList<quint64>& imageIds) {
            Q_UNUSED(param)
            ApTemplateLinkedModel& model = d->param.param.apTemplate;
            model.templateId = param.id;
            if (dynamicOriginData) *dynamicOriginData = model.templateParam;
            if (imageIds.size() > 0) {
                model.templateParam.patternRegParam.imageId = imageIds[0];
                model.templateParam.patternRegParam.imagePath = "";
            }
            simpleMessageBox(d,
                             "Info",
                             QString("Save AP template completely."));
        });
        apSaveBoxCaller->setSaveImageCallback(
            [this](TemplateInfoModel<ApTemplateParam>& param, QList<quint64>& imageIds) {
                if (param.data.patternRegParam.imageId == 0 && param.data.patternRegParam.imagePath.length() > 0) {
                    ImageInfoService* service = ServiceManager::instance()
                    ->getDefaultService<ImageInfoService>();
                    QList<FileImageDuplicateSaveData> list;
                    list.resize(1);
                    FileImageDuplicateSaveData& imgEntity = list[0];
                    QString json;
                    toImageDetailDataModelJson(json, param.data.patternRegParam.infoParam.imageAdditionData);
                    imgEntity.entity.setImageInfo(json);
                    cdsem::TmpImageData image;
                    imgEntity.path = param.data.patternRegParam.imagePath;
                    OperationResult result;
                    service->save(list, &result);
                    if (result.errCode) {
                        simpleMessageBox(saveBoxController, "Error", "Error to save image data.");
                        return false;
                    }
                    imageIds.append(imgEntity.entity.getId());
                    param.data.patternRegParam.imageId = imageIds[0];
                }
                return true;
            });
        apSaveBoxCaller->setSaveBox(saveBoxController);
    }

    void overloadTemplateData(bool overview = false) {
        char refType = d->param.param.apTemplate.templateRefType;
        quint64 templateId = d->param.param.apTemplate.templateId;
        bool succ = d->getAsyncCaller()->push(d, [this, templateId, refType, overview](TSC) mutable {
            TemplateInfoService* service = ServiceManager::instance()
            ->getDefaultService<TemplateInfoService>();
            int errCode;
            //按需求，import(reload)要载入的是load时的旧数据！
            //目前在该函数里，getById足以，理论上getDataByIdForNewVersion作用不大(但是可确保工作绝对正确)
            TemplateInfoEntity entity = refType == 0 && !overview
                                            ? service->getDataByIdForNewVersion(templateId, &errCode)
                                            : service->getById(templateId, &errCode);
            if (errCode) {
                openErrMsgBox("Load template data error.");
                return;
            }
            if (entity.getId() == 0 || entity.getTemplateType() != TEMP_ALIGN) {
                openErrMsgBox("This linked data is not exists.");
                return;
            }
            ApTemplateInfoModel model;
            errCode = templateInfoModelFromEntity(entity, model);
            if (errCode) {
                openErrMsgBox("Load template data error.");
                return;
            }
            MainThreadInvoker::getInvoker().blockRun([this, &model, overview]() {
                QSharedPointer<IdpApParamListData> oldParam;
                if (overview) {
                    //如果是临时图片，调用该函数让其保存临时图片
                    templateController->collectData();
                    oldParam.reset(new IdpApParamListData);
                    *oldParam.data() = d->param.param;
                    //import会强制转静态模板
                    d->param.param.apTemplate.templateRefType = 1;
                }
                d->param.param.apTemplate.templateId = model.id;
                d->param.param.apTemplate.templateParam = model.data;
                d->dataHelper->_resetComponents();
                if (templateSelectorController && templateSelectorController->isVisible()) {
                    templateSelectorController->close();
                }
                if (overview) {
                    questionBox
                        = idpTemplateShowYesNoBox(d,
                                                  "Question",
                                                  "Linked template information is imported.\nIs it OK?",
                                                  false,
                                                  [this, oldParam](int r) {
                                                      if (r) {
                                                          d->param.param = *oldParam.data();
                                                          d->dataHelper->_resetComponents();
                                                      } else if (d->dataHelper->dynamicOriginData) {
                                                          d->dataHelper->dynamicOriginData.reset();
                                                      }
                                                      return true;
                                                  });
                } else if (d->param.param.apTemplate.templateRefType == 0) {
                    d->dataHelper->dynamicOriginData.reset(new ApTemplateParam(d->param.param.apTemplate.templateParam));
                } else if (d->dataHelper->dynamicOriginData) {
                    d->dataHelper->dynamicOriginData.reset();
                }
            });
        });
        if (succ) {
            loadingBoxHelper(d, "Working", "Loading");
        }
    }
    void openErrMsgBox(const QString& text) {
        MainThreadInvoker::getInvoker().blockRun([&text, this]() {
            QWidget* widget;
            if (templateSelectorController && templateSelectorController->isVisible()) {
                widget = templateSelectorController;
            } else {
                widget = d;
            }
            simpleMessageBox(widget, "Error", text);
        });
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
                    return false;
                }
                cdsem::PyShotChipCoordinate* data = multiShotChipSelector->getSelectedData();
                if (!data) {
                    simpleMessageBox(multiShotChipSelector, "Error", "Please select one chip.");
                    return false;
                }
                if (multiSelectedChipType == 0)
                    changePosChipToParam(*data);
                else if (multiSelectedChipType == 1)
                    dRegCoordinateToParam(data->globalChip);
                else
                    dMovePos(data->globalChip);
            }
            multiShotChipSelector->clearListData();
            return true;
        });
    }

    void changePosChipToParam(const cdsem::PyShotChipCoordinate& c) {
        bool succ;
        QPoint chipRuler = cdsem::CDSEM_GlobalDataHelper::get()->getChipRuler(c.globalChip, &succ);
        if (!succ) {
            simpleMessageBox(d, "Error", "Error to get chip position.");
            return;
        }
        d->param.chipNumber = c.globalChip;
        d->param.chipX = chipRuler.x();
        d->param.chipY = chipRuler.y();
        d->param.oPoint.x = c.cx;
        d->param.oPoint.y = c.cy;
        d->ui->originXTextField->setText(d->param.chipX.value());
        d->ui->originYTextField->setText(d->param.chipY.value());
    }

    void changeDPos(bool changeSubContent = true) {
        IDW_FloatNumber dx = d->ui->dxTextField->text();
        IDW_FloatNumber dy = d->ui->dyTextField->text();
        if (dx == nullptr) dx = 0;
        if (dy == nullptr) dy = 0;
        dx += d->param.oPoint.x;
        dy += d->param.oPoint.y;
        if (!changeSubContent) return;
        templateController->setAPos(IDW_PointF{dx, dy});
    }

    void dRegCoordinateToParam(int chip) {
        bool succ;
        cdsem::PyShotChipCoordinate coordinate = cdsem::WaferStageCoordinateConvert::instance()->getShotChipUm(chip, &succ);
        if (!succ) {
            simpleMessageBox(d, "Error", "Error to check shot and chip info.");
            return;
        }
        IDW_PointF pos = cdsem::WaferStageCoordinateConvert::instance()->waferPosUm();
        QPointF beamPos = SEM_PY_ENV.getBeamPos();
        pos = cdsem::WaferStageCoordinateConvert::instance()->fieldPos(pos, IDW_PointF{beamPos.x() * 1000'000, beamPos.y() * 1000'000});
        d->ui->dxTextField->setText((pos.x - coordinate.cx).toQStringAndKeepFullDecimal());
        d->ui->dyTextField->setText((pos.y - coordinate.cy).toQStringAndKeepFullDecimal());
        d->param.oPoint.x = coordinate.cx;
        d->param.oPoint.y = coordinate.cy;
        changeDPos();
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
        if (confirmChip(chip, 1))
            dRegCoordinateToParam(chip);
    }

    void dMovePos(int chip) {
        if (chip < 0) return;
        bool succ;
        cdsem::PyShotChipCoordinate coordinate = cdsem::WaferStageCoordinateConvert::instance()->getShotChipUm(chip, &succ);
        if (!succ) {
            simpleMessageBox(d, "Error", "Error to check shot and chip info.");
            return;
        }
        IDW_FloatNumber x = d->ui->dxTextField->text();
        IDW_FloatNumber y = d->ui->dyTextField->text();
        if (x == nullptr) x = coordinate.cx;
        else x += coordinate.cx;
        if (y == nullptr) y = coordinate.cy;
        else y += coordinate.cy;
        MoveStageParam stageParam;
        cdsem::WaferStageCoordinateConvert::instance()->umToStagePos(IDW_PointF{x, y}, stageParam);
        CDSEM_ControlHelper::instance()->moveStage(d, stageParam);
    }

    void dMovePos() {
        if (d->param.chipNumber < 0) {
            simpleMessageBox(d, "Error", "Empty chip data.");
            return;
        }
        int chip;
        if (confirmChip(chip, 2)) {
            dMovePos(chip);
        }
    }
};

IdpApDetailEditorController::IdpApDetailEditorController(
    QWidget* parent)
    : CDE90Window(parent) {
    //如果你没有异步任务需求，请将connect清理掉，保留create和createAction函数
    connect(this, &IdpApDetailEditorController::asyncCallerChanged, this, [this](){
        create();
        createAction();
    }, Qt::SingleShotConnection);
}

IdpApDetailEditorController::~IdpApDetailEditorController() noexcept {
    if (ui) {
        delete ui;
    }
    if (dataHelper) {
        if (dataHelper->apSaveBoxCaller) {
            delete dataHelper->apSaveBoxCaller;
        }
        delete dataHelper;
    }
}

void IdpApDetailEditorController::closeEvent(
    QCloseEvent* event) {
    CDE90Window::closeEvent(event);
    if (event->isAccepted()) {
        dataHelper->questionBox.reset();
        dataHelper->dynamicOriginData.reset();
    }
}

void IdpApDetailEditorController::create() {
    dataHelper = new IdpApDetailEditorDataHelper;
    dataHelper->d = this;
    setContent(new QWidget(this));
    ui = new cdsem::UiIdpApDetailEditorController;
    CDE90LayoutHelper helper(getContent());
    helper.registUnknowType("alignmentTemplateContent",
                            [this](QObject* parent,
                                   const QHash<QString, QObject*>&,
                                   QHash<QString, QList<QStringList>>&) -> QObject* {
                                QWidget* widget = dynamic_cast<QWidget*>(parent);
                                if (!widget) {
                                    return nullptr;
                                }
                                MsgReceiveStatus<RelationIDPAPTemplateEditController*> status = idp_template::createApTemplateComponent(widget);
                                if (!status.ok) {
                                    return nullptr;
                                }
                                dataHelper->templateController = status.result;
                                dataHelper->templateController->setLineWidth(1);
                                dataHelper->templateController->setFrameStyle(QFrame::Sunken | QFrame::Box);
                                dataHelper->templateController->initParam(&param.param.apTemplate.templateParam);
                                return dataHelper->templateController;
                            });
    ui->init(helper);
    
}

void IdpApDetailEditorController::createAction() {
    bindCloseBtn(ui->cancelBtn->getQPushButton());
    if (!dataHelper->templateController) return;
    {
        CDSemNumberValidatorHelper helper = CDSemNumberValidatorHelper::doubleFree<2>();
        helper.put(ui->dxTextField).put(ui->dyTextField).autoFixMode();
    }
    itsChildAndBeManaged(dataHelper->templateController, this);
    dataHelper->templateController->initAsyncCaller(ACaller);
    SEM_OK_ACTION(ui->okBtn->getQPushButton())

    connect(dataHelper->templateController, &RelationIDPAPTemplateEditController::openDynamicLoader, this, [this]() {
        dataHelper->templateSelectStatus = 0;
        dataHelper->openTemplateSelector();
    });
    connect(dataHelper->templateController, &RelationIDPAPTemplateEditController::openStaticLoader, this, [this]() {
        dataHelper->templateSelectStatus = 1;
        dataHelper->openTemplateSelector();
    });
    connect(dataHelper->templateController, &RelationIDPAPTemplateEditController::reloadTemplate, this, [this]() {
        if (param.param.apTemplate.templateId == 0) {
            return;
        }
        dataHelper->overloadTemplateData(true);
    });
    connect(dataHelper->templateController, &RelationIDPAPTemplateEditController::saveTemplate, this, [this]() {
        dataHelper->createSaveBox();
        idpOpenSaveBox(this, ACaller, param.param.apTemplate, dataHelper->saveBoxController);
    });
    connect(dataHelper->templateController, &RelationIDPAPTemplateEditController::shouldHideParent, this, [this](bool start) {
        setVisible(!start);
    });

    connect(ui->dxTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        dataHelper->changeDPos();
    });
    connect(ui->dyTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        dataHelper->changeDPos();
    });

    connect(ui->dRegBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        dataHelper->dRegCoordinate();
    });
    connect(ui->dMoveBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        dataHelper->dMovePos();
    });
    connect(ui->originRegBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        IDW_PointF waferPos = cdsem::WaferStageCoordinateConvert::instance()->waferPosUm();
        param.offsetOPoint = IDW_PointF {waferPos.x - param.oPoint.x, waferPos.y - param.oPoint.y};
    });
    connect(ui->originMoveBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        bool succ;
        cdsem::PyShotChipCoordinate coordinate = cdsem::WaferStageCoordinateConvert::instance()->getShotChipUm(param.chipNumber, &succ);
        if (!succ) {
            simpleMessageBox(this, "Error", "Error to check shot and chip info.");
            return;
        }
        MoveStageParam stageParam;
        cdsem::WaferStageCoordinateConvert::instance()->umToStagePos(IDW_PointF{coordinate.cx, coordinate.cy}, stageParam);
        CDSEM_ControlHelper::instance()->moveStage(this, stageParam);
    });
    connect(dataHelper->templateController, &RelationIDPAPTemplateEditController::posChanged, this, [this](const IDW_PointF& pos){
        ui->dxTextField->setText((pos.x - param.oPoint.x).toQStringAndKeepFullDecimal());
        ui->dyTextField->setText((pos.y - param.oPoint.y).toQStringAndKeepFullDecimal());
        dataHelper->changeDPos(false);
    });
}

void IdpApDetailEditorController::resetToComponent() {
    if (param.param.apTemplate.templateId > 0 && param.param.apTemplate.templateRefType == 0) {
        dataHelper->dynamicOriginData.reset(new ApTemplateParam(param.param.apTemplate.templateParam));
    } else if (dataHelper->dynamicOriginData)
        dataHelper->dynamicOriginData.reset();
    dataHelper->_resetComponents();
}

QString IdpApDetailEditorController::collectData() {
    QString text = ui->dxTextField->text();
    if (text.isEmpty()) {
        text = "0";
    }
    param.param.dPoint.x = text;
    text = ui->dyTextField->text();
    if (text.isEmpty()) {
        text = "0";
    }
    param.param.dPoint.y = text;
    return dataHelper->templateController->collectData();
}

void IdpApDetailEditorController::disablePyHandlers(bool b) {
    ui->originMoveBtn->getQPushButton()->setDisabled(b);
    ui->originRegBtn->getQPushButton()->setDisabled(b);
    ui->dMoveBtn->getQPushButton()->setDisabled(b);
    ui->dRegBtn->getQPushButton()->setDisabled(b);
    if (dataHelper->templateController) {
        dataHelper->templateController->disablePyHandlers(b);
    }
    if (dataHelper->multiShotChipSelector && dataHelper->multiShotChipSelector->isVisible()) {
        dataHelper->multiShotChipSelector->close();
    }
    ui->testBtn->setDisabled(b);
    dataHelper->pyHandlerDisabled = b;
}

bool IdpApDetailEditorController::checkDynamicTemplateStatus() {
    return param.param.apTemplate.templateId > 0 && param.param.apTemplate.templateRefType == 0
        && dataHelper->dynamicOriginData && param.param.apTemplate.templateParam != *dataHelper->dynamicOriginData;
}

void IdpApDetailEditorController::recoverToDynamicTemplateData() {
    if (param.param.apTemplate.templateId > 0 && param.param.apTemplate.templateRefType == 0 && dataHelper->dynamicOriginData) {
        param.param.apTemplate.templateParam = *dataHelper->dynamicOriginData.get();
    }
}