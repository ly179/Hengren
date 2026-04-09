#include "job_recipe_setting_controller.h"
#include "cde90_message_box.h"
#include "cde90_process_box.h"
#include "idp_data_service.h"
#include "idw_list_selector_controller.h"
#include "idw_wafer_service.h"
#include "job_class_select_controller.h"
#include "list_view_data_query_helper.h"
#include "recipe_data_service.h"
#include "service_manager.h"
#include "ui_job_recipe_setting_controller.h"
#include "list_view_main_data_model.h"

#define RECIPE_PROCESS_GID 1

struct JobRecipeheckedSlots {
    QString text;
    quint64 id = 0;
    bool checked = false;
};

JobRecipeSettingController::JobRecipeSettingController(
    QWidget* parent)
    : CDE90Window(parent) {
    //如果你没有异步任务需求，请将connect清理掉，保留create和createAction函数
    connect(this, &JobRecipeSettingController::asyncCallerChanged, this, [this](){
        create();
        createAction();
    }, Qt::SingleShotConnection);
}

JobRecipeSettingController::~JobRecipeSettingController() noexcept {
    if (ui) {
        delete ui;
    }
}

void JobRecipeSettingController::create() {
    ui = new cdsem::UiJobRecipeSettingController;
    setContent(new QWidget);
    ui->init(getContent());
    ui->cassetteWidget->setCheckedStrAlign(Qt::AlignVCenter | Qt::AlignLeft);
}

void JobRecipeSettingController::createAction() {
    listViewModel = new ListViewMainModel(ui->recipeListView);
    // ui->cassetteWidget->setDisabled(true);
    ui->recipeListView->replaceDataModel(listViewModel);
    ui->cassetteWidget->setWaferCassetteCheckedStrCallback(
        [this](WaferLoadingStatus status, int slot, QString& value) {
            Q_UNUSED(status)
            if (slot < 1 || slot > checkedSlots.size()) {
                value = "";
                return;
            }
            JobRecipeheckedSlots& data = checkedSlots[slot - 1];
            if (!data.checked) {
                value = "";
                return;
            }
            value = data.text;
        });

    bindCloseBtn(ui->closeBtn->getQPushButton());
    connect(ui->classSelectBtn->getQPushButton(), &QPushButton::clicked, this, [this]() {
        createClassSelector();
        classSelector->show();
        classSelector->loadClassList();
        MainThreadInvoker::getInvoker().moveToScreenCenter(classSelector);
    });
    connect(ui->recipeListView->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this,
            [this](const QItemSelection& selected, const QItemSelection& deselected) {
                Q_UNUSED(selected)
                Q_UNUSED(deselected)
                resetCassetteSlots();
                QModelIndexList index = ui->recipeListView->selectionModel()->selectedRows(0);
                if (index.isEmpty()) {
                    // ui->cassetteWidget->setDisabled(true);
                    return;
                }
                // if (!ui->cassetteWidget->isEnabled()) {
                //     ui->cassetteWidget->setDisabled(false);
                // }
                baseData.recipeId = reinterpret_cast<ListViewData*>(index[0].internalPointer())
                                        ->getId();
                refreshRecipeData(baseData.recipeId);
            });
    connect(ui->cassetteWidget,
            &WaferCassetteWidget::slotChecked,
            this,
            [this](bool selected, int slot) {
                JobRecipeheckedSlots& v = checkedSlots[slot - 1];
                if (selected) {
                    v.checked = true;
                    int index = ui->recipeListView->getLimitSelectedRow();
                    if (index < 0) {
                        return;
                    }
                    ListViewData& recipe = listViewModel->getDataList()[index];
                    v.id = recipe.getId();
                    v.text = recipe.getText();
                    ui->cassetteWidget->setCheckedSlot(slot, {WP_Normal, WS_Reserved});
                } else {
                    v.id = 0;
                    v.text = "";
                    v.checked = false;
                    ui->cassetteWidget->removeCheckedSlot(slot);
                }
            });
    connect(ui->recipeUintItemBox->getQComboBox(),
            &QComboBox::currentIndexChanged,
            this,
            [this](int) { resetCassetteSlots(); });
    connect(ui->viewAllAct, &QAction::triggered, this, [this]() { refreshRecipeList(); });
    connect(ui->filterByIdwAct, &QAction::triggered, this, [this]() {
        IdwListSelectorController* idwSelector = new IdwListSelectorController(this);
        idwSelector->setAttribute(Qt::WA_DeleteOnClose);
        idwSelector->setWindowTitle("File Selector Load");
        idwSelector->initAsyncCaller(ACaller);
        idwSelector->initIdwList(baseData.classId);
        idwSelector->setOkCallback([idwSelector, this]() {
            idwSelector->simpleGetSelectedData([this](const ListViewMainData& data) {
                if (data.getId() == 0) {
                    if (data.getText().isEmpty())
                        simpleMessageBox(this, "Info", "Please select a IDW file.");
                    else
                        simpleMessageBox(this,
                                         "Info",
                                         QString("File %1 is not exists.").arg(data.getText()));
                    return false;
                }
                refreshRecipeList(data.getId());
                return true;
            });
            return false;
        });
        idwSelector->show();
    });
    checkedSlots.resize(ui->cassetteWidget->getSlotCount());
}

void JobRecipeSettingController::createClassSelector() {
    if (classSelector) {
        return;
    }
    classSelector = new JobClassSelectController(this);
    classSelector->setWindowTitle("Class Selector");
    classSelector->setWindowModality(Qt::WindowModal);
    classSelector->setResizable(false);
    classSelector->initAsyncCaller(ACaller);
    classSelector->setOkCallback([this](const ListViewMainData& data) {
        ui->classTextField->setText(data.getText());
        baseData.classId = data.getId();
        refreshRecipeList();
        return true;
    });
}

void JobRecipeSettingController::refreshRecipeList(
    quint64 idwId) {
    ui->recipeTextField->getQLineEdit()->clear();
    ui->idwTextField->getQLineEdit()->clear();
    ui->idpTextField->getQLineEdit()->clear();
    listViewModel->clear();
    bool succ = ACaller->push(this, [this, idwId](TSC) {
        localLoadRecipeList(baseData.classId, *listViewModel, idwId);
    });
    if (succ) {
        loadingBox(this, "Working", "Loading...");
    }
}

void JobRecipeSettingController::refreshRecipeData(
    quint64 recipeId) {
    ui->recipeTextField->setText("Loading");
    ui->idwTextField->setText("Loading");
    ui->idpTextField->setText("Loading");
    ACaller->stop(RECIPE_PROCESS_GID);
    ACaller->push(this, RECIPE_PROCESS_GID, [this, recipeId](TSC cmd) {
        if (cmd.isStop()) {
            return;
        }
        int errCode;
        QString recipeName, idwName, idpName;
        RecipeDataService* recipeService = ServiceManager::instance()
                                               ->getDefaultService<RecipeDataService>();
        RecipeDataEntity recipeEntity = recipeService->getSimpleInfo(recipeId, &errCode);
        if (errCode) {
            simpleMessageBox(this, "Error", "Error to load Recipe Data.");
        } else if (recipeEntity.getId() > 0) {
            recipeName = recipeEntity.getRecipeName();
            if (recipeEntity.getIdpId() > 0) {
                IdpDataService* idpService = ServiceManager::instance()
                                                 ->getDefaultService<IdpDataService>();
                idpName = idpService->getIdpName(recipeEntity.getIdpId(), &errCode);
                if (errCode) {
                    simpleMessageBox(this, "Error", "Error to load IDP Data.");
                }
            }
            if (recipeEntity.getIdwId() > 0) {
                IDW_WaferService* idwService = ServiceManager::instance()
                                                   ->getDefaultService<IDW_WaferService>();
                idwName = idwService->getWaferName(recipeEntity.getIdwId(), &errCode);
                if (errCode) {
                    simpleMessageBox(this, "Error", "Error to load IDW Data.");
                }
            }
        }
        MainThreadInvoker::getInvoker().blockRun([&cmd, this, &recipeName, &idwName, &idpName]() {
            if (!cmd.isStop()) {
                ui->recipeTextField->setText(recipeName);
                ui->idwTextField->setText(idwName);
                ui->idpTextField->setText(idpName);
            }
        });
    });
}

void JobRecipeSettingController::resetCassetteSlots() {
    int index = ui->recipeUintItemBox->currentIndex();
    if (index > 0) {
        return;
    }
    ListViewData* recipe = nullptr;
    int selectedRecipeIndex = ui->recipeListView->getLimitSelectedRow();
    if (selectedRecipeIndex >= 0) {
        recipe = &listViewModel->getDataList()[selectedRecipeIndex];
    }

    for (JobRecipeheckedSlots& d : checkedSlots) {
        if (!d.checked) {
            continue;
        }
        if (recipe) {
            d.id = recipe->getId();
            d.text = recipe->getText();
        } else {
            d.id = 0;
            d.text = "";
        }
    }
    ui->cassetteWidget->update();
}

void JobRecipeSettingController::setPort(
    char c) {
    baseData.port = c;
    ui->portTextField->setText(QChar(baseData.port));
}
