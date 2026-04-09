#include "amp_edit_controller.h"
#include <QLayout>
#include "amp_corner_edit_controller.h"
#include "amp_data_model_convert.h"
#include "amp_data_select_controller.h"
#include "amp_data_service.h"
#include "amp_diameter_hole_edit_controller.h"
#include "amp_edge_roughness_edit_controller.h"
#include "amp_ellipse_edit_controller.h"
#include "amp_gap_edit_controller.h"
#include "amp_lwr_analysis_edit_controller.h"
#include "amp_mppc_edit_controller.h"
#include "amp_open_close_hole_edit_controller.h"
#include "amp_width_edit_controller.h"
#include "amp_width_roughness_edit_controller.h"
#include "cde90_message_box.h"
#include "cde90_process_box.h"
#include "service_manager.h"
#include "ui_amp_edit_controller.h"
#include "list_view_main_data_model.h"
#include "cdsem_global_data.h"


AmpEditController::AmpEditController(QWidget* parent)
    : RelatoinAmpEditController(parent) {
    //如果你没有异步任务需求，请将connect清理掉，保留create和createAction函数
    connect(this, &AmpEditController::asyncCallerChanged, this, [this](){
        create();
        createAction();
    }, Qt::SingleShotConnection);
}

AmpEditController::~AmpEditController() noexcept {
    if (ui) {
        delete ui;
    }
}

void AmpEditController::create() {
    setContent(new QWidget);
    ui = new cdsem::UiAmpEditController;
    ui->init(getContent());
}

void AmpEditController::createAction() {
    connect(ui->measurementOptionCBox->getQComboBox(),
            &QComboBox::currentIndexChanged,
            this,
            [this]() { switchTool(cdsem::MeasureType(ui->measurementOptionCBox->currentValue())); });
    connect(ui->loadAct, &QAction::triggered, this, [this](bool) {
        dataSelectorAsSave = false;
        createDataSelector();
        dataSelector->setWindowTitle("File Selector: Load");
        dataSelector->show();
        dataSelector->refreshList();
    });
    connect(ui->saveAct, &QAction::triggered, this, [this](bool) {
        dataSelectorAsSave = true;
        createDataSelector();
        dataSelector->setWindowTitle("File Selector: Save");
        dataSelector->show();
        dataSelector->refreshList();
    });
    bindCloseBtn(ui->cancelBtn->getQPushButton());
    connect(ui->okBtn->getQPushButton(), &QPushButton::clicked, this, [this]() {
        AmpCommonData data;
        QString errMsg = prepareAmpDataToSave(data);
        if (errMsg.length() > 0) {
            simpleMessageBox(this, "Error", errMsg);
            return;
        }
        cdsem::CDSEM_GlobalDataHelper::get()->setGlobalAmpData(data);
        changeCloseStatusOnce(HW_OK);
        close();
    });
    connect(ui->applyBtn->getQPushButton(), &QPushButton::clicked, this, [this]() {
        AmpCommonData data;
        QString errMsg = prepareAmpDataToSave(data);
        if (errMsg.length() > 0) {
            simpleMessageBox(this, "Error", errMsg);
            return;
        }
        cdsem::CDSEM_GlobalDataHelper::get()->setGlobalAmpData(data);
    });
}

void AmpEditController::createDataSelector() {
    if (!dataSelector) {
        dataSelector = new AmpDataSelectController(this);
        dataSelector->initAsyncCaller(ACaller);
        dataSelector->setViewMode(true);
        dataSelector->setSubTitle("AMP");
        dataSelector->setTitle("File Kind:");
        dataSelector->setOkCallback([this]() {
            const ListViewMainData* selected = dataSelector->getSelectedData();
            QString inputText = dataSelector->getInputText();
            if (selected && inputText.size() > 0 && selected->getText() == inputText) {
                quint64 id = selected->getId();
                if (dataSelectorAsSave) {
                    int r = showYesNoBox(this, "Confirm", "Would you like to replace this file?");
                    if (r) {
                        return false;
                    }
                    bool succ = ACaller->push(this, [this, id, inputText](TSC) {
                        saveAmpNoCaller(id, inputText);
                    });
                    if (succ) {
                        loadingBox(this, "Process", "Working");
                    }

                } else {
                    bool succ = ACaller->push(this, [this, id](TSC) { loadAmpNoCaller(id); });
                    if (succ) {
                        loadingBox(this, "Process", "Working");
                    }
                }
                return false;
            }
            if (inputText.isEmpty()) {
                simpleMessageBox(this, "Error", "Please choose or type a file name.");
                return false;
            }
            checkName(inputText);
            return false;
        });
    }
}

void AmpEditController::switchTool(cdsem::MeasureType mType, bool resetComponent) {
    if (currentController) {
        currentController->hide();
        currentController = nullptr;
    }
    ui->addSubContent->setVisible(mType == cdsem::MT_WIDTH || mType == cdsem::MT_WIDTH_ROUGHNESS || mType == cdsem::MT_EDGE_ROUGHNESS);
    cdsem::MeasureObject object = cdsem::MO_UNKNOW;
    if (mType == cdsem::MT_WIDTH) {
        if (!widthController) {
            widthController = new AmpWidthEditController(ui->toolContent);
            ui->toolContent->layout()->addWidget(widthController);
            itsChildAndBeManaged(widthController, this);
            widthController->initAsyncCaller(ACaller);
        }
        if (resetComponent)
            widthController->resetToComponent();
        currentController = widthController;
        object = cdsem::ampObjectToMeasureObject(mType, cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpWidthParam().object);
        //TODO 后续的其他type都需要调用一下，等到都绑定到全局临时AMP变量时
    } else if (mType == cdsem::MT_WIDTH_ROUGHNESS) {
        if (!widthRoughnessController) {
            widthRoughnessController = new AmpWidthRoughnessEditController(ui->toolContent);
            ui->toolContent->layout()->addWidget(widthRoughnessController);
            itsChildAndBeManaged(widthRoughnessController, this);
            widthRoughnessController->initAsyncCaller(ACaller);
        }
        if (resetComponent)
            widthRoughnessController->resetToComponent();
        currentController = widthRoughnessController;
        object = cdsem::ampObjectToMeasureObject(mType, cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpWidthRoughnessParam().object);
    } else if (mType == cdsem::MT_EDGE_ROUGHNESS) {
        if (!edgeRoughnessController) {
            edgeRoughnessController = new AmpEdgeRoughnessEditController(ui->toolContent);
            ui->toolContent->layout()->addWidget(edgeRoughnessController);
            itsChildAndBeManaged(edgeRoughnessController, this);
            edgeRoughnessController->initAsyncCaller(ACaller);
        }
        if (resetComponent)
            edgeRoughnessController->resetToComponent();
        currentController = edgeRoughnessController;
        object = cdsem::ampObjectToMeasureObject(mType, cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpEdgeRoughnessParam().object);
    } else if (mType == cdsem::MT_DIAMETER_HOLE) {
        if (!diameterHoleController) {
            diameterHoleController = new AmpDiameterHoleEditController(ui->toolContent);
            ui->toolContent->layout()->addWidget(diameterHoleController);
            itsChildAndBeManaged(diameterHoleController, this);
            diameterHoleController->initAsyncCaller(ACaller);
        }
        if (resetComponent)
            diameterHoleController->resetToComponent();
        currentController = diameterHoleController;
        object = cdsem::ampObjectToMeasureObject(mType, cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpDiameterHoleParam().object);
    } else if (mType == cdsem::MT_ELLIPSE) {
        if (!ellipseController) {
            ellipseController = new AmpEllipseEditController(ui->toolContent);
            ui->toolContent->layout()->addWidget(ellipseController);
            itsChildAndBeManaged(ellipseController, this);
            ellipseController->initAsyncCaller(ACaller);
        }
        if (resetComponent)
            ellipseController->resetToComponent();
        currentController = ellipseController;
        object = cdsem::ampObjectToMeasureObject(mType, cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpEllipseParam().object);
    } else if (mType == cdsem::MT_GAP) {
        if (!gapController) {
            gapController = new AmpGapEditController(ui->toolContent);
            ui->toolContent->layout()->addWidget(gapController);
            itsChildAndBeManaged(gapController, this);
            gapController->initAsyncCaller(ACaller);
        }
        if (resetComponent)
            gapController->resetToComponent();
        currentController = gapController;
    } else if (mType == cdsem::MT_CORNER) {
        if (!cornerController) {
            cornerController = new AmpCornerEditController(ui->toolContent);
            ui->toolContent->layout()->addWidget(cornerController);
        }
        if (resetComponent)
            cornerController->resetToComponent();
        currentController = cornerController;
    } else if (mType == cdsem::MT_LWR) {
        if (!lwrAnalysisController) {
            lwrAnalysisController = new AmpLwrAnalysisEditController(ui->toolContent);
            itsChildAndBeManaged(lwrAnalysisController, this);
            lwrAnalysisController->initAsyncCaller(ACaller);
            ui->toolContent->layout()->addWidget(lwrAnalysisController);
        }
        if (resetComponent)
            lwrAnalysisController->resetToComponent();
        currentController = lwrAnalysisController;
    } else if (mType == cdsem::MT_MPPC) {
        if (!mppcController) {
            mppcController = new AmpMppcEditController(ui->toolContent);
            itsChildAndBeManaged(mppcController, this);
            mppcController->initAsyncCaller(ACaller);
            ui->toolContent->layout()->addWidget(mppcController);
        }
        if (resetComponent)
            mppcController->resetToComponent();
        currentController = mppcController;
    } else if (mType == cdsem::MT_OPEN_CLOSE_HOLE) {
        if (!openCloseHoleController) {
            openCloseHoleController = new AmpOpenCloseHoleEditController(ui->toolContent);
            ui->toolContent->layout()->addWidget(openCloseHoleController);
        }
        if (resetComponent)
            openCloseHoleController->resetToComponent();
        currentController = openCloseHoleController;
    }
    if (currentController) {
        if (cdsem::CDSEM_GlobalDataHelper::get()->updateTmpAmpMObject()) emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(this, &cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpMeasureObject());
        if (mType != cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpMeasureType()) {
            cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpMeasureType() = mType;
            emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(ui->measurementOptionCBox->getQComboBox()), &cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpMeasureType());
        }
        if (object != cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpMeasureObject()) {
            cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpMeasureObject() = object;
            emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(ui->measurementOptionCBox->getQComboBox()), &cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpMeasureObject());
        }
        currentController->show();
    }
}

void AmpEditController::checkName(const QString& inputText) {
    bool succ = ACaller->push(this, [this, inputText](TSC) {
        AmpDataService* service = ServiceManager::instance()
                                      ->getDefaultService<AmpDataService>();
        int errCode;
        quint64 id = service->checkName(inputText, &errCode);
        if (errCode) {
            simpleMessageBox(this, "Error", "Save/Load Amp data error.");
            return;
        }
        if (dataSelectorAsSave) {
            if (id > 0) {
                int r = showYesNoBox(this, "Confirm", "Would you like to replace this file?");
                if (r) {
                    return;
                }
            }
            saveAmpNoCaller(id, inputText);
        } else {
            if (id == 0) {
                simpleMessageBox(this,
                                 "Error",
                                 QString("No file named %1 was found.").arg(inputText));
                return;
            }
            loadAmpNoCaller(id);
        }
    });
    if (succ) {
        loadingBox(this, "Process", "Working");
    }
}
void AmpEditController::saveAmpNoCaller(quint64 id, const QString& name) {
    AmpDataEntity entity;

    {
        QString errMsg;
        AmpCommonData ampData;
        MainThreadInvoker::getInvoker().blockRun([this, &ampData, &errMsg](){
            errMsg = prepareAmpDataToSave(ampData);
        });
        if (errMsg.size() > 0) {
            simpleMessageBox(this, "Error", errMsg);
            return;
        }
        entity.setAmpType(ampData.getType());
        int errCode;
        ampCommonDataToJson(ampData, entity.getDetailContent(), &errCode);
        if (errCode) {
            simpleMessageBox(this, "Error", "Unsupport Mearure Type.");
            return;
        }
    }

    entity.setId(id);
    entity.setAmpName(name);
    AmpDataService* service = ServiceManager::instance()->getDefaultService<AmpDataService>();
    int errCode = service->saveWithTransaction(entity);
    if (errCode) {
        simpleMessageBox(this, "Error", "Save Amp data error.");
        return;
    }
    MainThreadInvoker::getInvoker().blockRun([this, &entity]() {
        dataId = entity.getId();
        simpleMessageBox(this, "Info", "AMP saved successfully.");
        dataSelector->close();
    });
}
void AmpEditController::loadAmpNoCaller(quint64 id) {
    if (id == 0) {
        return;
    }
    AmpDataService* service = ServiceManager::instance()->getDefaultService<AmpDataService>();
    int errCode;
    AmpDataEntity entity = service->get(id, &errCode);
    if (errCode) {
        simpleMessageBox(this, "Error", "Load AMP data error.");
        return;
    }
    if (entity.getId() == 0) {
        simpleMessageBox(this, "Error", "AMP data not exists.");
        return;
    }
    MainThreadInvoker::getInvoker().blockRun([this, &entity]() {
        loadAmp(cdsem::MeasureType(entity.getAmpType()), entity.getDetailContent());
        dataSelector->close();
    });
}

QString AmpEditController::prepareAmpDataToSave(AmpCommonData& ampData) {
    int mType = ui->measurementOptionCBox->currentValue();
    if (mType == cdsem::MT_WIDTH) {
        QString errMsg;
        AmpWidthParam param;
        if (!widthController) {
            errMsg = "No data specified.";
        } else {
            errMsg = widthController->collectData(param);
        }
        if (errMsg.size() > 0) {
            return errMsg;
        }
        ampData.setMObj(cdsem::ampObjectToMeasureObject(mType, param.object));
        ampData.setData(param);
    } else if (mType == cdsem::MT_WIDTH_ROUGHNESS) {
        QString errMsg;
        AmpWidthRoughnessParam param;
        if (!widthRoughnessController) {
            errMsg = "No data specified.";
        } else {
            errMsg = widthRoughnessController->collectData(param);
        }
        if (errMsg.size() > 0) {
            return errMsg;
        }
        ampData.setData(param);
        ampData.setMObj(cdsem::ampObjectToMeasureObject(mType, param.object));
    } else if (mType == cdsem::MT_EDGE_ROUGHNESS) {
        AmpEdgeRoughnessParam param;
        QString errMsg;
        if (!edgeRoughnessController) {
            errMsg = "No data specified.";
        } else {
            errMsg = edgeRoughnessController->collectData(param);
        }
        if (errMsg.size() > 0) {
            return errMsg;
        }
        ampData.setData(param);
        ampData.setMObj(cdsem::ampObjectToMeasureObject(mType, param.object));
    } else if (mType == cdsem::MT_DIAMETER_HOLE) {
        AmpDiameterHoleParam param;
        QString errMsg;
        if (!diameterHoleController) {
            errMsg = "No data specified.";
        } else {
            errMsg = diameterHoleController->collectData(param);
        }
        if (errMsg.size() > 0) {
            return errMsg;
        }
        ampData.setData(param);
        ampData.setMObj(cdsem::ampObjectToMeasureObject(mType, param.object));
    } else if (mType == cdsem::MT_ELLIPSE) {
        AmpEllipseParam param;
        QString errMsg;
        if (!ellipseController) {
            errMsg = "No data specified.";
        } else {
            errMsg = ellipseController->collectData(param);
        }
        if (errMsg.size() > 0) {
            return errMsg;
        }
        ampData.setData(param);
        ampData.setMObj(cdsem::ampObjectToMeasureObject(mType, param.object));
    } else if (mType == cdsem::MT_GAP) {
        AmpGapParam param;
        QString errMsg;
        if (!gapController) {
            errMsg = "No data specified.";
        } else {
            errMsg = gapController->collectData(param);
        }
        if (errMsg.size() > 0) {
            return errMsg;
        }
        ampData.setData(param);
        ampData.setMObj(cdsem::ampObjectToMeasureObject(mType));
    } else if (mType == cdsem::MT_CORNER) {
        AmpCornerParam param;
        QString errMsg;
        if (!cornerController) {
            errMsg = "No data specified.";
        } else {
            errMsg = cornerController->collectData(param);
        }
        if (errMsg.size() > 0) {
            return errMsg;
        }
        ampData.setData(param);
        ampData.setMObj(cdsem::ampObjectToMeasureObject(mType, param.object));
    } else if (mType == cdsem::MT_LWR) {
        AmpLwrAnalysisParam param;
        QString errMsg;
        if (!lwrAnalysisController) {
            errMsg = "No data specified.";
        } else {
            errMsg = lwrAnalysisController->collectData(param);
        }
        if (errMsg.size() > 0) {
            return errMsg;
        }
        ampData.setData(param);
        ampData.setMObj(cdsem::ampObjectToMeasureObject(mType, param.object));
    } else if (mType == cdsem::MT_MPPC) {
        AmpMppcParam param;
        QString errMsg;
        if (!mppcController) {
            errMsg = "No data specified.";
        } else {
            errMsg = mppcController->collectData(param);
        }
        if (errMsg.size() > 0) {
            return errMsg;
        }
        ampData.setData(param);
        ampData.setMObj(cdsem::ampObjectToMeasureObject(mType, param.object));
    } else if (mType == cdsem::MT_OPEN_CLOSE_HOLE) {
        AmpOpenCloseHoleParam param;
        QString errMsg;
        if (!openCloseHoleController) {
            errMsg = "No data specified.";
        } else {
            errMsg = openCloseHoleController->collectData(param);
        }
        if (errMsg.size() > 0) {
            return errMsg;
        }
        ampData.setData(param);
        ampData.setMObj(cdsem::ampObjectToMeasureObject(mType));
    } else {
        return "This function is not supported yet.";
    }
    return "";
}

void AmpEditController::closeEvent(QCloseEvent* event) {
    CDE90Window::closeEvent(event);
    cdsem::CDSEM_GlobalDataHelper::get()->setAmpWindowStatus(false);
    cdsem::CDSEM_GlobalDataHelper::get()->copyAmpGlobalToTmp();
}

void AmpEditController::showEvent(QShowEvent* event) {
    CDE90Window::showEvent(event);
    cdsem::CDSEM_GlobalDataHelper::get()->setAmpWindowStatus(true);
    resize(QSize(466, 700));
}

void AmpEditController::loadAmp(cdsem::MeasureType mType, const QString& json) {
    if (mType == cdsem::MT_UNKNOW) {
        return;
    }
    if (json.isEmpty()) {
        return;
    }
    ui->measurementOptionCBox->getQComboBox()->blockSignals(true);
    ui->measurementOptionCBox->setCurrentData(int(mType));
    ui->measurementOptionCBox->getQComboBox()->blockSignals(false);
    cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpMeasureType() = mType;
    if (cdsem::CDSEM_GlobalDataHelper::get()->updateTmpAmpMObject()) emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(this, &cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpMeasureObject());
    if (mType == cdsem::MT_WIDTH) {
        int errCode = ampWidthModelFromJson(json, cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpWidthParam());
        if (errCode) {
            simpleMessageBox(this, "Error", "Load width data error, cannot parse data.");
            return;
        }
        cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpMeasureObject() = cdsem::ampObjectToMeasureObject(mType, cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpWidthParam().object);
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), nullptr);
    } else if (mType == cdsem::MT_WIDTH_ROUGHNESS) {
        int errCode = ampWidthRoughnessModelFromJson(json, cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpWidthRoughnessParam());
        if (errCode) {
            simpleMessageBox(this, "Error", "Load width data error, cannot parse data.");
            return;
        }
        cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpMeasureObject() = cdsem::ampObjectToMeasureObject(mType, cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpWidthRoughnessParam().object);
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), nullptr);
    } else if (mType == cdsem::MT_EDGE_ROUGHNESS) {
        int errCode = ampEdgeRoughnessModelFromJson(json, cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpEdgeRoughnessParam());
        if (errCode) {
            simpleMessageBox(this, "Error", "Load Edge roughness data error, cannot parse data.");
            return;
        }
        cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpMeasureObject() = cdsem::ampObjectToMeasureObject(mType, cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpEdgeRoughnessParam().object);
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), nullptr);
    } else if (mType == cdsem::MT_DIAMETER_HOLE) {
        int errCode = ampDiameterHoleModelFromJson(json, cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpDiameterHoleParam());
        if (errCode) {
            simpleMessageBox(this, "Error", "Load diameter hole data error, cannot parse data.");
            return;
        }
        cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpMeasureObject() = cdsem::ampObjectToMeasureObject(mType, cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpDiameterHoleParam().object);
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), nullptr);
    } else if (mType == cdsem::MT_ELLIPSE) {
        int errCode = ampEllipseModelFromJson(json, cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpEllipseParam());
        if (errCode) {
            simpleMessageBox(this, "Error", "Load ellipse data error, cannot parse data.");
            return;
        }
        cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpMeasureObject() = cdsem::ampObjectToMeasureObject(mType, cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpEllipseParam().object);
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), nullptr);
    } else if (mType == cdsem::MT_GAP) {
        int errCode = ampGapModelFromJson(json, cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpGapParam());
        if (errCode) {
            simpleMessageBox(this, "Error", "Load gap data error, cannot parse data.");
            return;
        }
        cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpMeasureObject() = cdsem::ampObjectToMeasureObject(mType);
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), nullptr);
    } else if (mType == cdsem::MT_CORNER) {
        int errCode = ampCornerModelFromJson(json, cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpCornerParam());
        if (errCode) {
            simpleMessageBox(this, "Error", "Load corner data error, cannot parse data.");
            return;
        }
        cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpMeasureObject() = cdsem::ampObjectToMeasureObject(mType, cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpCornerParam().object);
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), nullptr);
    } else if (mType == cdsem::MT_LWR) {
        int errCode = ampLWRAnalysisModelFromJson(json, cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpLwrAnalysisParam());
        if (errCode) {
            simpleMessageBox(this, "Error", "Load LWR data error, cannot parse data.");
            return;
        }
        cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpMeasureObject() = cdsem::ampObjectToMeasureObject(mType, cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpLwrAnalysisParam().object);
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), nullptr);
    } else if (mType == cdsem::MT_MPPC) {
        int errCode = ampMPPCModelFromJson(json, cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpMppcParam());
        if (errCode) {
            simpleMessageBox(this, "Error", "Load MPPC data error, cannot parse data.");
            return;
        }
        cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpMeasureObject() = cdsem::ampObjectToMeasureObject(mType, cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpMppcParam().object);
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), nullptr);
    } else if (mType == cdsem::MT_OPEN_CLOSE_HOLE) {
        int errCode = ampOpenCloseHoleModelFromJson(json, cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpOpenCloseParam());
        if (errCode) {
            simpleMessageBox(this, "Error", "Load open/close hole data error, cannot parse data.");
            return;
        }
        cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpMeasureObject() = cdsem::ampObjectToMeasureObject(mType);
        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(this), nullptr);
    } else {
        simpleMessageBox(this, "Error", "This function is not supported yet.");
        return;
    }
    switchTool(mType, true);
}

void AmpEditController::loadAmp() {
    cdsem::MeasureType mType = cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpMeasureType();
    if (mType == cdsem::MT_UNKNOW) {
        mType = cdsem::MT_WIDTH;
    }
    ui->measurementOptionCBox->getQComboBox()->blockSignals(true);
    ui->measurementOptionCBox->setCurrentData(int(mType));
    ui->measurementOptionCBox->getQComboBox()->blockSignals(false);
    switchTool(mType);
}

