#include "recipe_editor_controller.h"
#include "cde90_message_box.h"
#include "cde90_process_box.h"
#include "cdsem_number_validator_helper.h"
#include "class_service.h"
#include "fem_measurement_controller.h"
#include "idp_data_service.h"
#include "idw_wafer_service.h"
#include "maintenance_controller.h"
#include "recipe_data_convert.h"
#include "recipe_data_service.h"
#include "recipe_environment_controller.h"
#include "recipe_idp_data_select_controller.h"
#include "recipe_idw_data_select_controller.h"
#include "service_manager.h"
#include "system_default_config_global_keys.h"
#include "system_default_config_service.h"
#include "ui_recipe_editor_controller.h"
#include "wafer_slot_enum.h"
#include "list_view_main_data_model.h"

struct RecipeSaveAdditionParam {
    QString name;
    quint64 id;
};

RecipeEditorController::RecipeEditorController(QWidget* parent) : CDE90Window(parent) {
    //如果你没有异步任务需求，请将connect清理掉，保留create和createAction函数
    connect(this, &RecipeEditorController::asyncCallerChanged, this, [this](){
        create();
        createAction();
    }, Qt::SingleShotConnection);
}

RecipeEditorController::~RecipeEditorController() noexcept {
    if (ui) {
        delete ui;
    }
}

void RecipeEditorController::create() {
    ui = new cdsem::UiRecipeEditorController;
    setContent(new QWidget);
    ui->init(getContent());

    ui->cassetteWidget->hideStr();
}

void RecipeEditorController::createAction() {
    CDSemNumberValidatorHelper::range(0, 9).put(ui->semConditionNoTextField).autoFixMode();
    connect(ui->exitAct, &QAction::triggered, this, [this]() {
        changeCloseStatusOnce(HW_CLOSE);
        close();
    });
    connect(ui->idwBtn->getQPushButton(), &QPushButton::clicked, this, [this]() {
        createIdwSelector();
        idwSelector->show();
        idwSelector->initIdwList(baseData.classId);
        MainThreadInvoker::getInvoker().moveToParentCenter(this, idwSelector);
    });

    connect(ui->idpBtn->getQPushButton(), &QPushButton::clicked, this, [this]() {
        createIdpSelector();
        idpSelector->show();
        idpSelector->initIdpList(baseData.classId, param.idwId);
        MainThreadInvoker::getInvoker().moveToParentCenter(this, idpSelector);
    });

    connect(ui->cassetteWidget,
            &WaferCassetteWidget::slotChecked,
            this,
            [this](bool selected, qint32 slot) {
                if (selected)
                    cassetteSlots.append(slot);
                else
                    cassetteSlots.remove(slot);
            });

    connect(ui->saveAct, &QAction::triggered, this, [this]() {
        QString errMsg = collectData(param);
        if (errMsg.length() > 0) {
            simpleMessageBox(this, "Error", errMsg);
            return;
        }
        saveData(param, {ui->recipeTextField->text(), param.recipeId});
    });
    connect(ui->saveAsAct, &QAction::triggered, this, [this]() {
        if (!saveAsBox) {
            saveAsBox = new CDE90InputBox(this);
            saveAsBox->setWindowTitle("Save as...");
            saveAsBox->setInputLength(FILE_NAME_LENGTH);
            saveAsBox->setEditTitle("Create Recipe");
            saveAsBox->setInputCallback([this](const QString& text) {
                if (text.isEmpty()) {
                    simpleMessageBox(this, "Message", "Please input Recipe name.");
                    return false;
                }
                saveData(param, {text, 0}, true);
                return false;
            });
            saveAsBox->resize(300, 200);
        }
        saveAsBox->setInitText(ui->recipeTextField->text());
        saveAsBox->show();
    });

    connect(ui->femMeasureBtn->getQPushButton(), &QPushButton::clicked, this, [this]() {
        createFemMeasurementEditor();
        femEditorController->show();
        femEditorController->resetToComponent(param.fem);
        MainThreadInvoker::getInvoker().moveToParentCenter(this, femEditorController);
    });

    connect(ui->mainBtn->getQPushButton(), &QPushButton::clicked, this, [this]() {
        createMaintenanceEditor();
        maintenanceController->show();
        maintenanceController->resetToComponent(param.maintenance);
        MainThreadInvoker::getInvoker().moveToParentCenter(this, maintenanceController);
    });

    connect(ui->envAct, &QAction::triggered, this, [this]() {
        createEnvironmentEditor();
        environmentController->show();
        environmentController->resetToComponent(param.env);
        MainThreadInvoker::getInvoker().moveToScreenCenter(environmentController);
    });
}

void RecipeEditorController::createIdwSelector() {
    if (idwSelector) {
        return;
    }
    idwSelector = new RecipeIdwDataSelectController(this);
    idwSelector->setWindowTitle("IDW Selector");
    idwSelector->setWindowModality(Qt::WindowModal);
    idwSelector->initAsyncCaller(ACaller);
    idwSelector->setSubTitle("IDW");
    idwSelector->setOkCallback([this]() {
        const ListViewMainData* data = idwSelector->getSelectedData();
        if (!data) {
            simpleMessageBox(this, "Info", "It is required to specify a IDW data.");
            return;
        }
        if (param.idwId > 0  && data->getId() != param.idwId && param.idpId > 0) {
            param.idpId = 0;
            ui->idpTextField->getQLineEdit()->clear();
        }
        ui->idwTextField->setText(data->getText());
        param.idwId = data->getId();
        param.idwTopId = data->getTopId();
        idwSelector->close();
    });
}
void RecipeEditorController::createIdpSelector() {
    if (idpSelector) {
        return;
    }
    idpSelector = new RecipeIdpDataSelectController(this);
    idpSelector->setWindowTitle("IDP Selector");
    idpSelector->setWindowModality(Qt::WindowModal);
    idpSelector->initAsyncCaller(ACaller);
    idpSelector->setSubTitle("IDP");
    idpSelector->setOkCallback([this]() {
        const ListViewMainData* dataPtr = idpSelector->getSelectedData();
        if (!dataPtr) {
            simpleMessageBox(this, "Info", "It is required to specify a IDP data.");
            return;
        }
        ListViewMainData data = *dataPtr;
        bool succ = ACaller->push(this, [this, data](TSC){
            int errCode;
            IdpDataService* idpService = ServiceManager::defaultService<IdpDataService>();
            IdpDataEntity entity = idpService->getSimpleInfo(data.getId(), &errCode);
            if (errCode) {
                simpleMessageBox(this, "Error", "Error to check IDP info.");
                return;
            }
            if (entity.getId() == 0) {
                simpleMessageBox(this, "Error", "IDP data is not exists.");
                return;
            }
            if (entity.getIdwId() == 0) {
                simpleMessageBox(this, "Error", "No IDW refrerence to IDP.");
                return;
            }
            IDW_WaferService* idwService = ServiceManager::defaultService<IDW_WaferService>();
            IDW_WaferInfoEntity idwEntity = idwService->getSimpleInfoById(entity.getIdwId(), &errCode);
            if (errCode) {
                simpleMessageBox(this, "Error", "Error to check IDW info.");
                return;
            }
            if (idwEntity.getId() == 0) {
                simpleMessageBox(this, "Error", "IDW data is not exists.");
                return;
            }
            QString idwName = idwEntity.getWaferName();
            quint64 idwId = idwEntity.getId();
            quint64 idwTopId = idwEntity.getTopId();
            MainThreadInvoker::getInvoker().runLater([this, data, idwName, idwId, idwTopId](){
                ui->idpTextField->setText(data.getText());
                param.idpId = data.getId();
                param.idpTopId = data.getTopId();
                ui->idwTextField->setText(idwName);
                param.idwId = idwId;
                param.idwTopId = idwTopId;
                idpSelector->close();
            });
        });
        if (succ) {
            loadingBox(this, "Working", "Checking IDW...");
        }
    });
}

void RecipeEditorController::createFemMeasurementEditor() {
    if (femEditorController) {
        return;
    }
    femEditorController = new FemMeasurementController(this);
    femEditorController->setWindowTitle("FEM Assignment");
    femEditorController->setWindowModality(Qt::WindowModal);
    femEditorController->setResizable(false);
    femEditorController->setOkCallback([this](const FemMeasureModel& param) {
        this->param.fem = param;
        return true;
    });
}

void RecipeEditorController::createMaintenanceEditor() {
    if (maintenanceController) {
        return;
    }
    maintenanceController = new MaintenanceController(this);
    maintenanceController->setWindowModality(Qt::WindowModal);
    maintenanceController->setResizable(false);
    maintenanceController->setWindowTitle("Maintenance");
    maintenanceController->setOkCallback([this](const MaintenanceDataModel& param) {
        this->param.maintenance = param;
        return true;
    });
}

void RecipeEditorController::createEnvironmentEditor() {
    if (environmentController) {
        return;
    }
    environmentController = new RecipeEnvironmentController(this);
    environmentController->setWindowTitle("Measurement Environment");
    environmentController->setWindowModality(Qt::WindowModal);
    environmentController->setResizable(false);
    environmentController->initAsyncCaller(ACaller);
    environmentController->setOkCallback([this](const RecipeEnvironmentModel& param) {
        this->param.env = param;
        return true;
    });
}

QString RecipeEditorController::collectData(
    RecipeDataModel& data) {
    data.classId = baseData.classId;
    data.recipeId = baseData.recipeId;
    if (&data != &param) {
        data.idwId = param.idwId;
        data.idpId = param.idpId;
    }
    QString text = ui->semConditionNoTextField->text();
    if (text.length() > 0) {
        data.semConditionNum = text.toInt();
    } else {
        data.semConditionNum = nullptr;
    }
    if (ui->highOxideRadio->isChecked()) {
        data.waferProperty = WP_HighOxide;
    } else if (ui->normalRadio->isChecked()) {
        data.waferProperty = WP_Normal;
    } else if (ui->arfRadio->isChecked()) {
        data.waferProperty = WP_ArF;
    } else {
        return "Wafer Property is not specified.";
    }
    if (ui->naRadio->isChecked()) {
        data.special = 0;
    } else if (ui->femMeasureRadio->isChecked()) {
        data.special = 1;
    } else if (ui->mainRadio->isChecked()) {
        data.special = 2;
    } else if (ui->microscaleRadio->isChecked()) {
        data.special = 3;
    } else if (ui->nanoscaleRadio->isChecked()) {
        data.special = 4;
    } else {
        return "It is required to specify a special item.";
    }
    data.autoCalibration = ui->autoCalibrationCBox->isChecked();

    data.selectedSlots = cassetteSlots.getAllDatas();
    return "";
}
void RecipeEditorController::saveData(const RecipeDataModel& data,
                                      const RecipeSaveAdditionParam& additionParam, bool saveAs) {
    RecipeDataEntity entity;
    entity.setClassId(data.classId);
    entity.setId(additionParam.id);
    entity.setIdwId(data.idwId);
    entity.setIdpId(data.idpId);
    entity.setRecipeName(additionParam.name);
    entity.setIdwTopId(param.idwTopId);
    entity.setIdpTopId(param.idpTopId);
    QString json;
    recipeDataToJson(json, data);
    entity.setDetail(json);
    bool succ = ACaller->push(this, [this, entity, saveAs](TSC) mutable {
        RecipeDataService* service = ServiceManager::instance()
                                         ->getDefaultService<RecipeDataService>();
        int errCode;
        quint64 existsId = service->getRecipeIdByName(entity.getClassId(), entity.getRecipeName(), &errCode);
        if (errCode) {
            simpleMessageBox(this, "Error", "Error to check Recipe name.");
            return;
        }
        if ((entity.getId() == 0 && existsId) || (entity.getId() > 0 && existsId > 0 && entity.getId() != existsId)) {
            if (saveAs) {
                simpleMessageBox(this, "Error", "Recipe name already exists.");
            } else {
                simpleMessageBox(this, "Error", "Recipe name already exists, please use \"Save as\" instead.");
            }
            return;
        }
        errCode = service->saveWithTransaction(entity);
        if (errCode) {
            if (errCode == FILE_NAME_REPEAT_ERROR_CODE) {
                if (saveAs) {
                    simpleMessageBox(this, "Error", "Recipe name already exists.");
                } else {
                    simpleMessageBox(this, "Error", "Recipe name already exists, please use \"Save as\" instead.");
                }
            } else {
                simpleMessageBox(this, "Error", "Error to save recipe data.");
            }
            return;
        }
        simpleMessageBox(this, "Info", "Save Recipe data successfully.");
        MainThreadInvoker::getInvoker().blockRun([this, &entity, saveAs]() {
            baseData.recipeId = entity.getId();
            ui->recipeTextField->setText(entity.getRecipeName());
            if (saveAs && saveAsBox) {
                saveAsBox->close();
            }
            if (saveAs) ui->saveAct->setVisible(true);
        });
    });
    if (succ) {
        loadingBox(this, "Working", "Saving...");
    }
}

void RecipeEditorController::loadExistsData(
    const Recipe_BaseData& data) {
    baseData.recipeId = data.recipeId;
    param.recipeId = data.recipeId;
    bool succ = ACaller->push(this, [this, data](TSC) {
        RecipeDataService* service = ServiceManager::instance()
                                         ->getDefaultService<RecipeDataService>();
        int errCode;
        RecipeDataEntity entity = service->get(data.recipeId, &errCode);
        if (errCode) {
            simpleMessageBox(this, "Error", "Error to load Recipe data.");
            return;
        }
        if (entity.getId() == 0) {
            simpleMessageBox(this, "Error", "Recipe data is not exists.");
            return;
        }
        if (data.classId > 0 && entity.getClassId() > 0 && data.classId != entity.getClassId()) {
            simpleMessageBox(this,
                             "Error",
                             "Loading class of Recipe data is different from current class.");
            return;
        }
        RecipeDataModel dataModel;
        errCode = recipeDataFromJson(entity.getDetail(), dataModel);
        if (errCode) {
            simpleMessageBox(this, "Error", "Error to load Recipe data.");
            return;
        }
        dataModel.classId = entity.getClassId();
        dataModel.idwId = entity.getIdwId();
        dataModel.idpId = entity.getIdpId();
        dataModel.recipeId = entity.getId();
        dataModel.idwTopId = entity.getIdwTopId();
        dataModel.idpTopId = entity.getIdpTopId();
        QString idwName;
        QString idpName;
        QString className;
        if (dataModel.classId > 0) {
            ClassService* classService = ServiceManager::instance()
                                             ->getDefaultService<ClassService>();
            ClassEntity classEntity = classService->getById(dataModel.classId, &errCode);
            if (errCode) {
                simpleMessageBox(this, "Error", "Error to load Class data.");
                return;
            }
            className = classEntity.getClassName();
        }

        if (dataModel.idwId > 0) {
            IDW_WaferService* idwService = ServiceManager::instance()
                                               ->getDefaultService<IDW_WaferService>();
            idwName = idwService->getWaferName(dataModel.idwId, &errCode);
            if (errCode) {
                simpleMessageBox(this, "Error", "Error to load IDW file name.");
                return;
            }
        }
        if (dataModel.idpId > 0) {
            IdpDataService* idpService = ServiceManager::instance()
                                             ->getDefaultService<IdpDataService>();
            idpName = idpService->getIdpName(dataModel.idpId, &errCode);
            if (errCode) {
                simpleMessageBox(this, "Error", "Error to load IDP file name.");
                return;
            }
        }
        MainThreadInvoker::getInvoker().blockRun(
            [this, &dataModel, &entity, &className, &idwName, &idpName]() {
                if (baseData.recipeId == dataModel.recipeId) {
                    param = dataModel;
                    baseData.classId = dataModel.classId;
                    ui->classTextField->setText(className);
                    ui->idwTextField->setText(idwName);
                    ui->idpTextField->setText(idpName);
                    ui->recipeTextField->setText(entity.getRecipeName());
                    ui->semConditionNoTextField->setText(dataModel.semConditionNum.toString());
                    if (dataModel.waferProperty == WP_HighOxide) {
                        ui->highOxideRadio->setChecked(true);
                    } else if (dataModel.waferProperty == WP_Normal) {
                        ui->normalRadio->setChecked(true);
                    } else if (dataModel.waferProperty == WP_ArF) {
                        ui->arfRadio->setChecked(true);
                    } else {
                        if (ui->highOxideRadio->isChecked()) {
                            ui->highOxideRadio->setChecked(false);
                        } else if (ui->normalRadio->isChecked()) {
                            ui->normalRadio->setChecked(false);
                        } else if (ui->arfRadio->isChecked()) {
                            ui->arfRadio->setChecked(false);
                        }
                    }
                    if (dataModel.special == 0) {
                        ui->naRadio->setChecked(true);
                    } else if (dataModel.special == 1) {
                        ui->femMeasureRadio->setChecked(true);
                    } else if (dataModel.special == 2) {
                        ui->mainRadio->setChecked(true);
                    } else if (dataModel.special == 3) {
                        ui->microscaleRadio->setChecked(true);
                    } else if (dataModel.special == 4) {
                        ui->nanoscaleRadio->setChecked(true);
                    } else {
                        if (ui->naRadio->isChecked()) {
                            ui->naRadio->setChecked(false);
                        } else if (ui->femMeasureRadio->isChecked()) {
                            ui->femMeasureRadio->setChecked(false);
                        } else if (ui->mainRadio->isChecked()) {
                            ui->mainRadio->setChecked(false);
                        } else if (ui->microscaleRadio->isChecked()) {
                            ui->microscaleRadio->setChecked(false);
                        } else if (ui->nanoscaleRadio->isChecked()) {
                            ui->nanoscaleRadio->setChecked(false);
                        }
                    }
                    ui->autoCalibrationCBox->setChecked(dataModel.autoCalibration);
                    ui->cassetteWidget->clearCheckedSlot(false);
                    if (dataModel.selectedSlots.isEmpty()) {
                        ui->cassetteWidget->update();
                    } else {
                        for (const int& s : dataModel.selectedSlots) {
                            ui->cassetteWidget->setCheckedSlot(s, {WP_Normal, WS_Reserved}, false);
                        }
                        ui->cassetteWidget->update();
                    }
                }
            });
    });
    if (succ) {
        loadingBox(this, "Working", "Loading");
    }
}

void RecipeEditorController::newData(
    quint64 classId, const QString& recipeName) {
    baseData.classId = classId;
    ui->recipeTextField->setText(recipeName);
    param = {};
    param.classId = classId;
    bool succ = ACaller->push(this, [this, classId](TSC) {
        int errCode;
        {
            ClassService* service = ServiceManager::instance()
                                        ->getDefaultService<ClassService>();
            ClassEntity entity = service->getById(classId, &errCode);
            if (errCode) {
                simpleMessageBox(this, "Error", "Load class info error.");
                return;
            }
            MainThreadInvoker::getInvoker().blockRun(
                [this, &entity]() { ui->classTextField->setText(entity.getClassName()); });
        }

        SystemDefaultConfigService* service
            = ServiceManager::instance()->getDefaultService<SystemDefaultConfigService>();
        SystemDefaultConfigEntity entity = service->getByKey(sysConf::DEF_CONFIG_MEASURE_ENV,
                                                             &errCode);
        if (entity.getId() == 0) {
            return;
        }
        if (errCode) {
            simpleMessageBox(this, "Error", "Load default data error.");
            return;
        }
        QString json = entity.getSValue();
        RecipeEnvironmentModel model;
        errCode = recipeEnvironmentFromJson(json, model);
        if (errCode) {
            simpleMessageBox(this, "Error", "Load default measurement environment data error.");
            return;
        }
        MainThreadInvoker::getInvoker().blockRun([this, &model]() { param.env = model; });
    });
    if (succ) {
        loadingBox(this, "Info", "Please wait");
    }
}

void RecipeEditorController::disableCoverSave(bool d) {
    ui->saveAct->setVisible(!d);
}

quint64 RecipeEditorController::getRecipeId() const {
    return baseData.recipeId;
}
