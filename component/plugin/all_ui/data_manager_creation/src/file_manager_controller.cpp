#include "file_manager_controller.h"
#include <QOverload>
#include "cde90_button.h"
#include "cde90_label.h"
#include "cde90_list_view.h"
#include "cde90_menu.h"
#include "cde90_message_box.h"
#include "cde90_text_field.h"
#include "private/cdsem_class_editor.h"
#include "class_service.h"
#include "idp_data_service.h"
#include "idw_wafer_service.h"
#include "recipe_data_service.h"
#include "service_manager.h"
#include "file_class_copy_controller.h"
#include "file_idw_copy_controller.h"
#include "file_idp_copy_controller.h"
#include "file_recipe_copy_controller.h"
#include "private/history_view_controller.h"
#include "list_view_main_data_model.h"
#include "private/common_data_loader.h"
#include "private/data_manager_controller_helper.h"
#include "log_viewer_relation.h"

constexpr int refreshForIDWGid = 1;
constexpr int refreshForIDPGid = 2;
constexpr int refreshForRecipeGid = 3;

#define MH dataHelper->managerHelper
struct FileManagerDataHelper {
    HistoryViewController* historyViewController = nullptr;
    DataManagerControllerHelper* managerHelper;

    QAction* idwCreateAction;
    QAction* idwEditAction;

    int totalLimitCount = 5000;
};

FileManagerController::FileManagerController(
    QWidget* parent)
    : RelationFileManager(parent) {
    dataHelper = new FileManagerDataHelper;
    dataHelper->managerHelper = new DataManagerControllerHelper;
    dataHelper->managerHelper->create();
    setContent(MH->content);
    initAsyncCaller();
    create();
}

FileManagerController::~FileManagerController() noexcept {
    if (dataHelper) {
        delete dataHelper->managerHelper;
        delete dataHelper;
    }
}

void FileManagerController::createMenu() {
    QAction* exitAction = MH->fileMenu->addAction("Exit");
    CDE90Menu* initializeMenu = new CDE90Menu("Initialize");
    menuStyle(MH->deviceMenu);
    MH->deviceMenu->addMenu(initializeMenu);
    /*QAction* moAction = */initializeMenu->addAction("MO");
    QAction* errLogAction = MH->showMenu->addAction("Error Log");
    connect(errLogAction, &QAction::triggered, this, [this]() {
        auto status = log_viewer::openSingleLogViewer();
        if (!status.result) {
            simpleMessageBox(this, "Error", "Failed to open Log Viewer");
        }
    });
    connect(exitAction, &QAction::triggered, this, [this]() { close(); });
    // connect(moAction, &QAction::triggered, this, []() {});
    // connect(errLogAction, &QAction::triggered, this, []() {});

    /*QAction* helpCnDocAction = */MH->helpeMenu->addAction("中文");
    /*QAction* helpEnDocAction = */MH->helpeMenu->addAction("English");
    // connect(helpCnDocAction, &QAction::triggered, this, []() {});
    // connect(helpEnDocAction, &QAction::triggered, this, []() {});
}

void FileManagerController::createActionMenu() {
    createClassMenu();
    createIDWMenu();
    createIDPMenu();
    createRecipeMenu();
}

void FileManagerController::createClassMenu() {
    CDE90Menu* menu = new CDE90Menu(MH->details[0].btn_action);
    MH->details[0].btn_action->getQPushButton()->setMenu(menu);
    QAction* action;
    action = menu->addAction("Copy");
    connect(action, &QAction::triggered, this, [this](){
        ListViewMainData* data = MH->getSelectedData(0);
        if (!data) {
            simpleMessageBox(this, "Error", "Please select Class first.");
            return;
        }
        openClassCopyEditor(data->getId());
    });
    menu->addAction("Move");
    action = menu->addAction("Delete");
    connect(action, &QAction::triggered, this, [this]() {
        ListViewMainData* data = MH->getSelectedData(0);
        if (!data) {
            return;
        }
        int r = showYesNoBox(this, "Message", "Are you sure to delete this class ?", true);
        if (r) {
            return;
        }
        quint64 id = data->getId();
        MH->details[0].btn_action->loading();
        getAsyncCaller()->push(this, [this, id](TSC) {
            sem::AfterGc gc([this]() {
                MainThreadInvoker::getInvoker().runLater(
                    [this]() { MH->details[0].btn_action->overLoading(); });
            });
            ClassService* classService = ServiceManager::instance()
                                             ->getDefaultService<ClassService>();
            OperationResult result;
            classService->deleteClassAndContent(id, &result);
            if (result.errCode) {
                simpleMessageBox(this, "Error Occern", result.errMsg);
                return;
            }
            refreshClassData();
        });
    });
    action = menu->addAction("Rename");
    connect(action,
            &QAction::triggered,
            this,
            QOverload<>::of(&FileManagerController::openClassEditor));
    action = menu->addAction("Create");
    connect(action, &QAction::triggered, this, [this]() {
        if (MH->checkIsOverTheItemCountAndNotify(0, dataHelper->totalLimitCount)) {
            return;
        }
        openClassEditor(0, "");
    });
    action = menu->addAction("Search");
    connect(action, &QAction::triggered, this, [this]() { openSearchView(0); });
    action = menu->addAction("Update");
    connect(action, &QAction::triggered, this, [this]() { refreshClassData(); });
    menu->addAction("Print...");
}
void FileManagerController::createIDWMenu() {
    CDE90Menu* menu = new CDE90Menu(MH->details[1].btn_action);
    menu->setProperty("MenuArrowAndSeq", "arrow");
    MH->details[1].btn_action->getQPushButton()->setMenu(menu);
    QAction* action;
    action = menu->addAction("Edit");
    dataHelper->idwEditAction = action;
    connect(action, &QAction::triggered, this, [this]() {
        ListViewMainData* classData = MH->getSelectedData(0);
        if (!classData) {
            simpleMessageBox(this, "Message", "Please select a class first.");
            return;
        }
        ListViewMainData* idwData = MH->getSelectedData(1);
        if (!idwData) {
            simpleMessageBox(this, "Message", "Please select a IDW first.");
            return;
        }
        emit openIDWEditor(classData->getId(),
                           classData->getText(),
                           idwData->getId(),
                           idwData->getText());
    });
    action = menu->addAction("Copy");
    connect(action, &QAction::triggered, this, [this](){
        ListViewMainData* data = MH->getSelectedData(1);
        if (!data) {
            simpleMessageBox(this, "Error", "Please select IDW first.");
            return;
        }
        openIDWCopyEditor(data->getId());
    });
    menu->addAction("Move");
    action = menu->addAction("Delete");
    connect(action, &QAction::triggered, this, [this]() {
        ListViewMainData* data = MH->getSelectedData(1);
        if (!data) {
            return;
        }
        int r = showYesNoBox(this, "Message", "Are you sure to delete this IDW ?", true);
        if (r) {
            return;
        }
        quint64 id = data->getId();
        MH->details[0].btn_action->loading();
        getAsyncCaller()->push(this, [this, id](TSC) {
            sem::AfterGc gc([this]() {
                MainThreadInvoker::getInvoker().runLater(
                    [this]() { MH->details[0].btn_action->overLoading(); });
            });
            IDW_WaferService* waferService = ServiceManager::instance()
                                                 ->getDefaultService<IDW_WaferService>();
            int errCode = waferService->deleteWaferById(id);
            if (errCode) {
                simpleMessageBox(this, "Error Occern", "Delete IDW Error");
                return;
            }
            MainThreadInvoker::getInvoker().runLater([this]() {
                ListViewMainData* classData = MH->getSelectedData(0);
                if (classData) {
                    refreshIDWData(classData->getId());
                }
            });
        });
    });
    action = menu->addAction("Rename");
    connect(action, &QAction::triggered, this, [this]() {
        ListViewMainData* idwData = MH->getSelectedData(1);
        if (!idwData) {
            simpleMessageBox(this, "Message", "Please select a IDW first.");
            return;
        }
        openIDWRenameEditor(idwData->getId(), idwData->getText());
    });
    action = menu->addAction("Create");
    dataHelper->idwCreateAction = action;
    connect(action, &QAction::triggered, this, [this]() {
        if (MH->checkIsOverTheItemCountAndNotify(1, dataHelper->totalLimitCount)) {
            return;
        }
        ListViewMainData* classData = MH->getSelectedData(0);
        if (!classData) {
            simpleMessageBox(this, "Message", "Please select a class first.");
            return;
        }
        openIDWCreateEditor(classData->getId(), classData->getText());
    });
    action = menu->addAction("Search");
    connect(action, &QAction::triggered, this, [this]() { openSearchView(1); });
    action = menu->addAction("Update");
    connect(action, &QAction::triggered, this, [this]() {
        ListViewMainData* classData = MH->getSelectedData(0);
        if (!classData) {
            simpleMessageBox(this, "Message", "Please select a class first.");
            return;
        }
        ListViewMainData* idwData = MH->getSelectedData(1);
        refreshIDWData(classData->getId(), idwData ? idwData->getId() : 0);
    });
    CDE90Menu* subMenu = new CDE90Menu("Field Map", menu);
    menu->addMenu(subMenu);
    subMenu->addAction("Display");
    subMenu->addAction("Cut");
    subMenu->addAction("Copy");
    subMenu->addAction("Paste");
    subMenu->addAction("Delete");
    subMenu->addAction("Copy to public");
    menu->addAction("Print...");
    action = menu->addAction("Version && History");
    connect(action, &QAction::triggered, this, [this](){
        ListViewMainData* data = MH->getSelectedData(1);
        if (!data) return;
        createHistoryView();
        dataHelper->historyViewController->show();
        dataHelper->historyViewController->loadData(data->getId(), HVDT_IDW);
    });
}
void FileManagerController::createIDPMenu() {
    CDE90Menu* menu = new CDE90Menu(MH->details[2].btn_action);
    menu->setProperty("MenuArrowAndSeq", "arrow");
    MH->details[2].btn_action->getQPushButton()->setMenu(menu);
    QAction* action;
    action = menu->addAction("Edit");
    connect(action, &QAction::triggered, this, [this](bool) {
        ListViewMainData* classData = MH->getSelectedData(0);
        if (!classData) {
            simpleMessageBox(this, "Message", "Please select a class first.");
            return;
        }
        ListViewMainData* idpData = MH->getSelectedData(2);
        if (!idpData) {
            simpleMessageBox(this, "Message", "Please select a IDP first.");
            return;
        }
        emit openIdpEditor(classData->getId(), idpData->getId(), "", 0);
    });
    action = menu->addAction("Copy");
    connect(action, &QAction::triggered, this, [this](){
        ListViewMainData* data = MH->getSelectedData(2);
        if (!data) {
            simpleMessageBox(this, "Error", "Please select IDP first.");
            return;
        }
        openIDPCopyEditor(data->getId());
    });
    menu->addAction("Move");
    action = menu->addAction("Delete");
    connect(action, &QAction::triggered, this, [this]() {
        ListViewMainData* data = MH->getSelectedData(2);
        if (!data) {
            return;
        }
        int r = showYesNoBox(this, "Message", "Are you sure to delete this IDP ?", true);
        if (r) {
            return;
        }
        quint64 id = data->getId();
        MH->details[0].btn_action->loading();
        getAsyncCaller()->push(this, [this, id](TSC) {
            sem::AfterGc gc([this]() {
                MainThreadInvoker::getInvoker().runLater(
                    [this]() { MH->details[0].btn_action->overLoading(); });
            });
            IdpDataService* service = ServiceManager::instance()
                                          ->getDefaultService<IdpDataService>();
            int errCode = service->deleteIDPById(id);
            if (errCode) {
                simpleMessageBox(this, "Error Occern", "Delete IDP Error");
                return;
            }
            MainThreadInvoker::getInvoker().runLater([this]() {
                ListViewMainData* classData = MH->getSelectedData(0);
                if (classData) {
                    refreshIDPData(classData->getId());
                }
            });
        });
    });
    action = menu->addAction("Rename");
    connect(action, &QAction::triggered, this, [this]() {
        ListViewMainData* idpData = MH->getSelectedData(2);
        if (!idpData) {
            simpleMessageBox(this, "Message", "Please select a IDP first.");
            return;
        }
        openIDPRenameEditor(idpData->getId(), idpData->getText());
    });
    action = menu->addAction("Create");
    connect(action, &QAction::triggered, this, [this](bool) {
        if (MH->checkIsOverTheItemCountAndNotify(2, dataHelper->totalLimitCount)) {
            return;
        }
        ListViewMainData* viewData = MH->getSelectedData(0);
        if (!viewData) {
            simpleMessageBox(this, "Message", "Please select a class first.");
            return;
        }
        quint64 classId = viewData->getId();
        viewData = MH->getSelectedData(1);
        if (!viewData) {
            simpleMessageBox(this, "Message", "Please select a IDW first.");
            return;
        }
        openIdpCreateEditor(classId, viewData->getId());
    });
    action = menu->addAction("Search");
    connect(action, &QAction::triggered, this, [this]() { openSearchView(2); });
    menu->addAction("Modify Ax+B");
    action = menu->addAction("Update");
    connect(action, &QAction::triggered, this, [this]() {
        ListViewMainData* classData = MH->getSelectedData(0);
        if (!classData) {
            simpleMessageBox(this, "Message", "Please select a class first.");
            return;
        }
        refreshIDPData(classData->getId());
    });
    CDE90Menu* subMenu = new CDE90Menu("Field Map", menu);
    menu->addMenu(subMenu);
    subMenu->addAction("Display");
    subMenu->addAction("Cut");
    subMenu->addAction("Copy");
    subMenu->addAction("Paste");
    subMenu->addAction("Delete");
    subMenu->addAction("Copy to public");
    menu->addAction("Print...");
    action = menu->addAction("Version && History");
    connect(action, &QAction::triggered, this, [this](){
        ListViewMainData* data = MH->getSelectedData(2);
        if (!data) return;
        createHistoryView();
        dataHelper->historyViewController->show();
        dataHelper->historyViewController->loadData(data->getId(), HVDT_IDP);
    });
}

void FileManagerController::createRecipeMenu() {
    CDE90Menu* menu = new CDE90Menu(MH->details[3].btn_action);
    menu->setProperty("MenuArrowAndSeq", "arrow");
    MH->details[3].btn_action->getQPushButton()->setMenu(menu);
    QAction* action;
    action = menu->addAction("Edit");
    connect(action, &QAction::triggered, this, [this](bool) {
        ListViewMainData* classData = MH->getSelectedData(0);
        if (!classData) {
            simpleMessageBox(this, "Message", "Please select a class first.");
            return;
        }
        ListViewMainData* recipeData = MH->getSelectedData(3);
        if (!recipeData) {
            simpleMessageBox(this, "Message", "Please select a Recipe first.");
            return;
        }
        emit openRecipeEditor(classData->getId(), recipeData->getId(), "");
    });
    action = menu->addAction("Copy");
    connect(action, &QAction::triggered, this, [this](){
        ListViewMainData* data = MH->getSelectedData(3);
        if (!data) {
            simpleMessageBox(this, "Error", "Please select Recipe first.");
            return;
        }
        openRecipeCopyEditor(data->getId());
    });
    menu->addAction("Move");
    action = menu->addAction("Delete");
    connect(action, &QAction::triggered, this, [this]() {
        ListViewMainData* data = MH->getSelectedData(3);
        if (!data) {
            return;
        }
        int r = showYesNoBox(this, "Message", "Are you sure to delete this Recipe ?", true);
        if (r) {
            return;
        }
        quint64 id = data->getId();
        MH->details[0].btn_action->loading();
        getAsyncCaller()->push(this, [this, id](TSC) {
            sem::AfterGc gc([this]() {
                MainThreadInvoker::getInvoker().runLater(
                    [this]() { MH->details[0].btn_action->overLoading(); });
            });
            RecipeDataService* service = ServiceManager::instance()
                                             ->getDefaultService<RecipeDataService>();
            int errCode = service->deleteRecipeById(id);
            if (errCode) {
                simpleMessageBox(this, "Error Occern", "Delete Recipe Error");
                return;
            }
            MainThreadInvoker::getInvoker().runLater([this]() {
                ListViewMainData* classData = MH->getSelectedData(0);
                if (classData) {
                    refreshRecipeData(classData->getId());
                }
            });
        });
    });
    action = menu->addAction("Rename");
    connect(action, &QAction::triggered, this, [this]() {
        ListViewMainData* recipeData = MH->getSelectedData(3);
        if (!recipeData) {
            simpleMessageBox(this, "Message", "Please select a Recipe first.");
            return;
        }
        openRecipeRenameEditor(recipeData->getId(), recipeData->getText());
    });
    action = menu->addAction("Recipe File Eraser");
    connect(action, &QAction::triggered, this, [this]() {
        ListViewMainData* classData = MH->getSelectedData(0);
        if (!classData) {
            return;
        }
        emit openRecipeEraser(classData->getId());
    });
    action = menu->addAction("Create");
    connect(action, &QAction::triggered, this, [this](bool) {
        if (MH->checkIsOverTheItemCountAndNotify(0, dataHelper->totalLimitCount)) {
            return;
        }
        ListViewMainData* classData = MH->getSelectedData(0);
        if (!classData) {
            simpleMessageBox(this, "Message", "Please select a class first.");
            return;
        }
        openRecipeCreateEditor(classData->getId());
    });
    action = menu->addAction("Search");
    connect(action, &QAction::triggered, this, [this]() { openSearchView(3); });
    CDE90Menu* subMenu = new CDE90Menu("View", menu);
    menu->addMenu(subMenu);
    action = subMenu->addAction("Selected IDW");
    connect(action, &QAction::triggered, this, [this]() {
        ListViewMainData* classData = MH->getSelectedData(0);
        if (!classData) {
            // ListViewMainModel* mode = dynamic_cast<ListViewMainModel*>(details[3].listView->model());
            return;
        }
        ListViewMainData* idwData = MH->getSelectedData(1);
        if (!idwData) {
            return;
        }
        RecipeSearchDTO param;
        param.setClassId(classData->getId());
        param.setIdwId(idwData->getId());
        param.setInvalid(0);
        refreshRecipeData(param);
    });
    action = subMenu->addAction("Selected IDP");
    connect(action, &QAction::triggered, this, [this]() {
        ListViewMainData* classData = MH->getSelectedData(0);
        if (!classData) {
            return;
        }
        ListViewMainData* idpData = MH->getSelectedData(2);
        if (!idpData) {
            return;
        }
        RecipeSearchDTO param;
        param.setClassId(classData->getId());
        param.setIdpId(idpData->getId());
        param.setInvalid(0);
        refreshRecipeData(param);
    });
    action = subMenu->addAction("All");
    connect(action, &QAction::triggered, this, [this]() {
        ListViewMainData* classData = MH->getSelectedData(0);
        if (!classData) {
            return;
        }
        refreshRecipeData(classData->getId());
    });
    action = menu->addAction("Update");
    connect(action, &QAction::triggered, this, [this]() {
        ListViewMainData* classData = MH->getSelectedData(0);
        if (!classData) {
            simpleMessageBox(this, "Message", "Please select a class first.");
            return;
        }
        refreshRecipeData(classData->getId());
    });
    menu->addAction("Print...");
    action = menu->addAction("Version && History");
    connect(action, &QAction::triggered, this, [this](){
        ListViewMainData* data = MH->getSelectedData(3);
        if (!data) return;
        createHistoryView();
        dataHelper->historyViewController->show();
        dataHelper->historyViewController->loadData(data->getId(), HVDT_RECIPE);
    });
}

void FileManagerController::refreshClassData(quint64 selectedId) {
    ClassSearchDTO param;
    refreshClassData(param, selectedId);
}

void FileManagerController::refreshClassData(
    const ClassSearchDTO& searchParam, quint64 selectedId) {
    getAsyncCaller()->push(this, [this, searchParam, selectedId](const TaskStopCmd&) {
        MainThreadInvoker::getInvoker().blockRun([this]() { MH->details[0].btn_action->loading(); });
        sem::AfterGc gc([this]() {
            MainThreadInvoker::getInvoker().blockRun(
                [this]() { MH->details[0].btn_action->overLoading(); });
        });

        FileCommonResult result = refreshClass(searchParam, selectedId);
        if (result.errCode) {
            simpleMessageBox(this, "Error Occern", result.errMsg);
            return;
        }
        if (result.list.isEmpty()) {
            MainThreadInvoker::getInvoker().blockRun([this]() {
                ListViewMainModel* mode = dynamic_cast<ListViewMainModel*>(
                    MH->details[0].listView->model());
                mode->clear();
            });
            return;
        }
        MainThreadInvoker::getInvoker().blockRun([this, &result]() {
            ListViewMainModel* mode = dynamic_cast<ListViewMainModel*>(MH->details[0].listView->model());
            mode->clear();
            mode->insertDatas(result.list);
            MH->details[0].textfield_selected->getQLineEdit()->clear();
            dynamic_cast<ListViewMainModel*>(MH->details[1].listView->model())->clear();
            dynamic_cast<ListViewMainModel*>(MH->details[2].listView->model())->clear();
            dynamic_cast<ListViewMainModel*>(MH->details[3].listView->model())->clear();
            MH->details[0].textfield_item->setText(
                QString("%1/%2").arg(result.list.size()).arg(dataHelper->totalLimitCount));
            if (result.selectedIndex >= 0) {
                QModelIndex index = mode->index(result.selectedIndex);
                MH->details[0].listView->selectionModel()->select(index, QItemSelectionModel::SelectCurrent);
                MH->details[0].listView->scrollTo(index);
            }
        });
    });
}

void FileManagerController::refreshIDWData(
    quint64 classId, quint64 selectedId) {
    IDW_WaferSearchDTO param;
    param.setInvalid(0);
    param.setClassId(classId);
    refreshIDWData(param, selectedId);
}


void FileManagerController::refreshSelectedIDWData(quint64 classId, quint64 selectedId) {
    ListViewMainData* classData = MH->getSelectedData(0);
    if (!classData) {
        return;
    }
    if (classId == 0) {
        classId = classData->getId();
    } else if (classId != classData->getId()) {
        return;
    }
    refreshIDWData(classId, selectedId);
}

void FileManagerController::enableIDWAction(bool d) {
    dataHelper->idwCreateAction->setEnabled(d);
    dataHelper->idwEditAction->setEnabled(d);
    if (dataHelper->historyViewController)
        dataHelper->historyViewController->enableEditBtn(d);
}

void FileManagerController::refreshIDWData(
    const IDW_WaferSearchDTO& searchParam, quint64 selectedId) {
    bool succ = getAsyncCaller()->push(this, refreshForIDWGid, [this, searchParam, selectedId](TSC) {
        IDW_WaferService* service = ServiceManager::instance()
                                        ->getDefaultService<IDW_WaferService>();
        qint32 errCode;
        QList<IDW_WaferInfoEntity> list = service->searchList(searchParam, &errCode);
        if (errCode) {
            simpleMessageBox(this, "Error Occern", "Query IDW List Error!");
            return;
        }
        if (list.isEmpty()) {
            MainThreadInvoker::getInvoker().blockRun([this]() {
                ListViewMainModel* mode = dynamic_cast<ListViewMainModel*>(
                    MH->details[1].listView->model());
                mode->clear();
                MH->details[1].textfield_item->setText(QString("%1/%2").arg(0).arg(dataHelper->totalLimitCount));
            });
            return;
        }
        QList<ListViewMainData> viewList(list.size());
        int selectedIndex = -1;
        for (int i = 0; i < list.size(); ++i) {
            viewList[i].setId(list[i].getId());
            viewList[i].setText(list[i].getWaferName());
            viewList[i].setTopId(list[i].getTopId());
            if (selectedIndex == -1 && selectedId > 0 && selectedId == list[i].getId()) {
                selectedIndex = i;
            }
        }
        MainThreadInvoker::getInvoker().blockRun([this, &viewList, selectedIndex]() {
            ListViewMainModel* mode = dynamic_cast<ListViewMainModel*>(MH->details[1].listView->model());
            mode->clear();
            mode->insertDatas(viewList);
            MH->details[1].textfield_selected->getQLineEdit()->clear();
            MH->details[1].textfield_item->setText(
                QString("%1/%2").arg(viewList.size()).arg(dataHelper->totalLimitCount));
            if (selectedIndex >= 0) {
                QModelIndex index = mode->index(selectedIndex);
                MH->details[1].listView->selectionModel()->select(index, QItemSelectionModel::SelectCurrent);
                MH->details[1].listView->scrollTo(index);
            }
        });
    });
    updateLoadingBtnStatus(1, succ);
}

void FileManagerController::refreshIDPData(
    quint64 classId, quint64 selectedId) {
    IdpSearchDTO param;
    param.setInvalid(0);
    param.setClassId(classId);
    refreshIDPData(param, selectedId);
}

void FileManagerController::refreshSelectedIDPData(quint64 classId, quint64 idwTopId, quint64 selectedId) {
    ListViewMainData* classViewData = MH->getSelectedData(0);
    if (!classViewData) {
        return;
    }
    ListViewMainData* idwViewData = MH->getSelectedData(1);
    if (classId == 0) {
        classId = classViewData->getId();
    } else if (classId != classViewData->getId()) {
        return;
    }
    if (idwTopId == 0) {
        if (idwViewData) idwTopId = idwViewData->getTopId();
    } else if (!idwViewData) {
        idwTopId = 0;
    } else if (idwViewData->getTopId() != idwTopId) {
        return;
    }
    IdpSearchDTO param;
    param.setInvalid(0);
    param.setClassId(classId);
    if (idwTopId > 0)
        param.setIdwTopId(idwTopId);
    refreshIDPData(param, selectedId);
}

void FileManagerController::refreshIDPData(
    const IdpSearchDTO& searchParam, quint64 selectedId) {
    bool succ = ACaller->push(this, refreshForIDPGid, [this, searchParam, selectedId](TSC) {
        IdpDataService* service = ServiceManager::instance()
                                      ->getDefaultService<IdpDataService>();
        qint32 errCode;
        QList<IdpDataEntity> list = service->searchList(searchParam, &errCode);
        if (errCode) {
            simpleMessageBox(this, "Error Occern", "Query IDP List Error!");
            return;
        }
        if (list.isEmpty()) {
            MainThreadInvoker::getInvoker().blockRun([this]() {
                ListViewMainModel* mode = dynamic_cast<ListViewMainModel*>(
                    MH->details[2].listView->model());
                mode->clear();
                MH->details[2].textfield_item->setText(QString("%1/%2").arg(0).arg(dataHelper->totalLimitCount));
            });
            return;
        }
        QList<ListViewMainData> viewList(list.size());
        int selectedIndex = -1;
        for (int i = 0; i < list.size(); ++i) {
            viewList[i].setId(list[i].getId());
            viewList[i].setText(list[i].getIdpName());
            viewList[i].setTopId(list[i].getTopId());
            if (selectedIndex == -1 && selectedId > 0 && selectedId == list[i].getId()) {
                selectedIndex = i;
            }
        }
        MainThreadInvoker::getInvoker().blockRun([this, &viewList, selectedIndex]() {
            ListViewMainModel* mode = dynamic_cast<ListViewMainModel*>(MH->details[2].listView->model());
            mode->clear();
            mode->insertDatas(viewList);
            MH->details[2].textfield_selected->getQLineEdit()->clear();
            MH->details[2].textfield_item->setText(
                QString("%1/%2").arg(viewList.size()).arg(dataHelper->totalLimitCount));
            if (selectedIndex >= 0) {
                QModelIndex index = mode->index(selectedIndex);
                MH->details[2].listView->selectionModel()->select(index, QItemSelectionModel::SelectCurrent);
                MH->details[2].listView->scrollTo(index);
            }
        });
    });
    updateLoadingBtnStatus(2, succ);
}

void FileManagerController::enableIDPAction(bool d) {
    MH->details[2].btn_action->setEnabled(d);
}

void FileManagerController::refreshRecipeData(
    quint64 classId, quint64 selectedId) {
    RecipeSearchDTO param;
    param.setClassId(classId);
    param.setInvalid(0);
    refreshRecipeData(param, selectedId);
}

void FileManagerController::refreshSelectedRecipeData(quint64 classId, quint64 selectedId) {
    ListViewMainData* classData = MH->getSelectedData(0);
    if (!classData) {
        return;
    }
    if (classId == 0) {
        classId = classData->getId();
    } else if (classId != classData->getId()) {
        return;
    }
    refreshRecipeData(classId, selectedId);
}

void FileManagerController::refreshRecipeData(
    const RecipeSearchDTO& searchParam, quint64 selectedId) {
    bool succ = ACaller->push(this, refreshForRecipeGid, [this, searchParam, selectedId](TSC) {
        RecipeDataService* service = ServiceManager::instance()
                                         ->getDefaultService<RecipeDataService>();

        qint32 errCode;
        QList<RecipeDataEntity> list = service->searchList(searchParam, &errCode);
        if (errCode) {
            simpleMessageBox(this, "Error Occern", "Query IDP List Error!");
            return;
        }
        if (list.isEmpty()) {
            MainThreadInvoker::getInvoker().blockRun([this]() {
                ListViewMainModel* mode = dynamic_cast<ListViewMainModel*>(
                    MH->details[3].listView->model());
                mode->clear();
                MH->details[3].textfield_item->setText(QString("%1/%2").arg(0).arg(dataHelper->totalLimitCount));
            });
            return;
        }
        QList<ListViewMainData> viewList(list.size());
        int selectedIndex = -1;
        for (int i = 0; i < list.size(); ++i) {
            viewList[i].setId(list[i].getId());
            viewList[i].setText(list[i].getRecipeName());
            viewList[i].setTopId(list[i].getTopId());
            if (selectedIndex == -1 && selectedId > 0 && selectedId == list[i].getId()) {
                selectedIndex = i;
            }
        }
        MainThreadInvoker::getInvoker().blockRun([this, &viewList, selectedIndex]() {
            ListViewMainModel* mode = dynamic_cast<ListViewMainModel*>(MH->details[3].listView->model());
            mode->clear();
            mode->insertDatas(viewList);
            MH->details[3].textfield_selected->getQLineEdit()->clear();
            MH->details[3].textfield_item->setText(
                QString("%1/%2").arg(viewList.size()).arg(dataHelper->totalLimitCount));
            if (selectedIndex >= 0) {
                QModelIndex index = mode->index(selectedIndex);
                MH->details[3].listView->selectionModel()->select(index, QItemSelectionModel::SelectCurrent);
                MH->details[3].listView->scrollTo(index);
            }
        });
    });
    updateLoadingBtnStatus(3, succ);
}

void FileManagerController::enableRecipeAction(bool d) {
    MH->details[3].btn_action->setEnabled(d);
}

void FileManagerController::openClassEditor(
    quint64 id, const QString& str) {
    if (!MH->classEditor) {
        MH->classEditor = new CDSemClassEditor(this);
        MH->classEditor->setWindowModality(Qt::WindowModal);
        MH->classEditor->setResizable(false);
        MH->classEditor->resize(300, 200);
        MH->classEditor->initAsyncCaller(getAsyncCaller());
    }
    MH->classEditor->setWindowTitle(id > 0 ? "Rename" : "Create");
    MH->classEditor->setOkCallback([this](quint64 cid) { refreshClassData(cid); });
    MH->classEditor->show();
    MainThreadInvoker::getInvoker().moveToScreenCenter(MH->classEditor);
    MH->classEditor->setInitData(id, str);
}

void FileManagerController::openClassCopyEditor(quint64 id) {
    SimpleDataCopyParam param;
    param.fromId = id;
    FileClassCopyController* classCopier = new FileClassCopyController(this);
    classCopier->setAttribute(Qt::WA_DeleteOnClose);
    classCopier->setWindowTitle("Class Copy");
    classCopier->initAsyncCaller(ACaller);
    classCopier->setCopyParam(param);
    classCopier->setCopyTitle("Class");
    classCopier->setWindowModality(Qt::WindowModal);
    classCopier->setCloseCall([this](CDE90WindowCloseStatus status){
        if (status == HW_OK) {
            refreshClassData();
        }
        return true;
    });
    classCopier->show();
}

void FileManagerController::openIDWCreateEditor(
    quint64 classId, const QString& className) {
    CDE90InputBox box(this);
    box.setInputLength(FILE_NAME_LENGTH);
    box.setInputCallback([this](const QString& text) {
        if (text.isEmpty()) {
            simpleMessageBox(this, "Message", "Please input IDW name.");
            return false;
        }
        return true;
    });
    box.setWindowTitle("Create");
    box.setEditTitle("Create IDW");
    box.resize(300, 200);
    QString waferName = box.waitInput();
    if (waferName.length() > 0) {
        emit openIDWEditor(classId, className, 0, waferName);
    }
}

void FileManagerController::openIDWRenameEditor(
    quint64 idwId, const QString& text) {
    CDE90InputBox* box = new CDE90InputBox(this);
    box->setInputLength(FILE_NAME_LENGTH);
    box->setInputCallback([this, box, idwId](const QString& text) {
        if (text.isEmpty()) {
            simpleMessageBox(this, "Message", "Please input IDW name.");
            return false;
        }
        box->loadingStatus(true);
        bool succ = getAsyncCaller()->pushFocus(this, [this, idwId, text, box](TSC) {
            IDW_WaferService* service = ServiceManager::instance()
                                            ->getDefaultService<IDW_WaferService>();
            int errCode;
            quint64 classId = service->getClassIdByWaferId(idwId, &errCode);
            if (errCode || classId == 0) {
                MainThreadInvoker::getInvoker().runLater([box]() { box->loadingStatus(false); });
                simpleMessageBox(this, "Error", "Faile to rename IDW, error occern during check class info.");
                return;
            }
            quint64 existsId = service->getWaferIdByName(text, classId, &errCode);
            if (existsId > 0) {
                MainThreadInvoker::getInvoker().runLater([box]() { box->loadingStatus(false); });
                if (existsId != idwId) {
                    simpleMessageBox(this, "Error", "IDW file name already exists.");
                    return;
                }
                MainThreadInvoker::getInvoker().runLater([box]() {
                    box->loadingStatus(false);
                    box->close();
                });
                return;
            }
            errCode = service->updateWaferNameOnly(idwId, text);
            if (errCode) {
                MainThreadInvoker::getInvoker().runLater([box]() { box->loadingStatus(false); });
                simpleMessageBox(this, "Error", "Rename Failed!");
                return;
            }
            MainThreadInvoker::getInvoker().runLater([this, box]() {
                box->loadingStatus(false);
                box->close();
                ListViewMainData* view = MH->getSelectedData(0);
                if (view) {
                    refreshIDWData(view->getId());
                }
            });
        });
        if (!succ) {
            simpleMessageBox(this, "Message", "Task thread is closed, process fail.");
            box->loadingStatus(false);
            box->close();
        }
        return false;
    });
    box->setWindowTitle("Rename");
    box->setEditTitle("Rename IDW");
    box->setInitText(text);
    box->resize(300, 200);
    box->setAttribute(Qt::WA_DeleteOnClose);
    box->show();
}

void FileManagerController::openIDWCopyEditor(quint64 id) {
    IDW_CopyDataParam param;
    param.fromId = id;
    FileIdwCopyController* idwCopier = new FileIdwCopyController(this);
    idwCopier->setAttribute(Qt::WA_DeleteOnClose);
    idwCopier->setWindowTitle("IDW Copy");
    idwCopier->initAsyncCaller(ACaller);
    idwCopier->setParam(param);
    idwCopier->setWindowModality(Qt::WindowModal);
    idwCopier->setCloseCall([this, idwCopier](CDE90WindowCloseStatus status){
        if (status == HW_OK) {
            ListViewMainData* selectedClass = MH->getSelectedData(0);
            if (selectedClass == nullptr) {
                return true;
            }
            quint64 idwId = idwCopier->getAfterCopyId();
            if (idwId <= 0) {
                return true;
            }
            quint64 classId = idwCopier->getAfterCopyClassId();
            if (selectedClass->getId() == classId) {
                refreshIDWData(classId, idwId);
                return true;
            }
            MH->details[0].listView->selectionModel()->clearSelection();
            refreshClassData(classId);
        }
        return true;
    });
    idwCopier->show();
}

void FileManagerController::openIDPRenameEditor(
    quint64 idpId, const QString& text) {
    CDE90InputBox* box = new CDE90InputBox(this);
    box->setInputLength(FILE_NAME_LENGTH);
    box->setInputCallback([this, box, idpId](const QString& text) {
        if (text.isEmpty()) {
            simpleMessageBox(this, "Message", "Please input IDP name.");
        }
        box->loadingStatus(true);
        getAsyncCaller()->push(this, [this, idpId, text, box](TSC) {
            IdpDataService* service = ServiceManager::instance()
                                          ->getDefaultService<IdpDataService>();

            int errCode;
            IdpDataEntity simpleIdpEntity = service->getSimpleInfo(idpId, &errCode);
            if (errCode || simpleIdpEntity.getClassId() == 0) {
                MainThreadInvoker::getInvoker().runLater([box]() { box->loadingStatus(false); });
                simpleMessageBox(this, "Error", "Faile to rename IDP, error occern during check class info.");
                return;
            }
            quint64 existsId = service->getIdpIdByName(simpleIdpEntity.getClassId(), text, &errCode);
            if (existsId > 0) {
                MainThreadInvoker::getInvoker().runLater([box]() { box->loadingStatus(false); });
                if (existsId != idpId) {
                    simpleMessageBox(this, "Error", "IDP file name already exists.");
                    return;
                }
                MainThreadInvoker::getInvoker().runLater([box]() {
                    box->loadingStatus(false);
                    box->close();
                });
                return;
            }

            errCode = service->updateIdpNameOnly(idpId, text);
            if (errCode) {
                MainThreadInvoker::getInvoker().runLater([box]() { box->loadingStatus(false); });
                simpleMessageBox(this, "Error", "Rename Failed!");
                return;
            }
            MainThreadInvoker::getInvoker().runLater([this, box]() {
                box->loadingStatus(false);
                box->close();
                ListViewMainData* view = MH->getSelectedData(0);
                if (view) {
                    refreshIDPData(view->getId());
                }
            });
        });
        return false;
    });
    box->setWindowTitle("Rename");
    box->setEditTitle("Rename IDP");
    box->setInitText(text);
    box->resize(300, 200);
    box->setAttribute(Qt::WA_DeleteOnClose);
    box->show();
}

void FileManagerController::openIDPCopyEditor(quint64 id) {
    IDP_CopyDataParam param;
    param.fromId = id;
    FileIdpCopyController* idpCopier = new FileIdpCopyController(this);
    idpCopier->setAttribute(Qt::WA_DeleteOnClose);
    idpCopier->setWindowTitle("IDP Copy");
    idpCopier->initAsyncCaller(ACaller);
    idpCopier->setParam(param);
    idpCopier->setWindowModality(Qt::WindowModal);
    idpCopier->setCloseCall([this, idpCopier](CDE90WindowCloseStatus status){
        if (status == HW_OK) {
            ListViewMainData* selectedClass = MH->getSelectedData(0);
            if (selectedClass == nullptr) {
                return true;
            }
            quint64 idpId = idpCopier->getAfterCopyId();
            if (idpId <= 0) {
                return true;
            }
            quint64 classId = idpCopier->getAfterCopyClassId();
            if (selectedClass->getId() == classId) {
                refreshIDPData(classId, idpId);
                return true;
            }
            MH->details[0].listView->selectionModel()->clearSelection();
            refreshClassData(classId);
        }
        return true;
    });
    idpCopier->show();
}

void FileManagerController::openRecipeRenameEditor(
    quint64 recipeId, const QString& text) {
    CDE90InputBox* box = new CDE90InputBox(this);
    box->setInputLength(FILE_NAME_LENGTH);
    box->setInputCallback([this, box, recipeId](const QString& text) {
        if (text.isEmpty()) {
            simpleMessageBox(this, "Message", "Please input Recipe name.");
        }
        box->loadingStatus(true);
        getAsyncCaller()->push(this, [this, recipeId, text, box](TSC) {
            RecipeDataService* service = ServiceManager::instance()
                                             ->getDefaultService<RecipeDataService>();

            int errCode;
            RecipeDataEntity simpleIdpEntity = service->getSimpleInfo(recipeId, &errCode);
            if (errCode || simpleIdpEntity.getClassId() == 0) {
                MainThreadInvoker::getInvoker().runLater([box]() { box->loadingStatus(false); });
                simpleMessageBox(this, "Error", "Faile to rename Recipe, error occern during check class info.");
                return;
            }
            quint64 existsId = service->getRecipeIdByName(simpleIdpEntity.getClassId(), text, &errCode);
            if (existsId > 0) {
                MainThreadInvoker::getInvoker().runLater([box]() { box->loadingStatus(false); });
                if (existsId != recipeId) {
                    simpleMessageBox(this, "Error", "Recipe file name already exists.");
                    return;
                }
                MainThreadInvoker::getInvoker().runLater([box]() {
                    box->loadingStatus(false);
                    box->close();
                });
                return;
            }

            errCode = service->updateRecipeNameOnly(recipeId, text);
            if (errCode) {
                MainThreadInvoker::getInvoker().runLater([box]() { box->loadingStatus(false); });
                simpleMessageBox(this, "Error", "Rename Failed!");
                return;
            }
            MainThreadInvoker::getInvoker().runLater([this, box]() {
                box->loadingStatus(false);
                box->close();
                ListViewMainData* view = MH->getSelectedData(0);
                if (view) {
                    refreshRecipeData(view->getId());
                }
            });
        });
        return false;
    });
    box->setWindowTitle("Rename");
    box->setEditTitle("Rename Recipe");
    box->setInitText(text);
    box->resize(300, 200);
    box->setAttribute(Qt::WA_DeleteOnClose);
    box->show();
}

void FileManagerController::openIdpCreateEditor(
    quint64 classId, quint64 idwId) {
    CDE90InputBox box(this);
    box.setInputLength(FILE_NAME_LENGTH);
    box.setInputCallback([this](const QString& text) {
        if (text.isEmpty()) {
            simpleMessageBox(this, "Message", "Please input IDP name.");
            return false;
        }
        return true;
    });
    box.setWindowTitle("Create");
    box.setEditTitle("Create IDP");
    box.resize(300, 200);
    QString idpName = box.waitInput();
    if (idpName.length() > 0) {
        emit openIdpEditor(classId, 0, idpName, idwId);
    }
}

void FileManagerController::openRecipeCreateEditor(
    quint64 classId) {
    CDE90InputBox box(this);
    box.setInputLength(FILE_NAME_LENGTH);
    box.setInputCallback([this](const QString& text) {
        if (text.isEmpty()) {
            simpleMessageBox(this, "Message", "Please input Recipe name.");
            return false;
        }
        return true;
    });
    box.setWindowTitle("Create");
    box.setEditTitle("Create Recipe");
    box.resize(300, 200);
    QString recipeName = box.waitInput();
    if (recipeName.length() > 0) {
        emit openRecipeEditor(classId, 0, recipeName);
    }
}

void FileManagerController::openRecipeCopyEditor(quint64 recipeId) {
    Recipe_CopyDataParam param;
    param.fromId = recipeId;
    FileRecipeCopyController* recipeCopier = new FileRecipeCopyController(this);
    recipeCopier->setAttribute(Qt::WA_DeleteOnClose);
    recipeCopier->setWindowTitle("IDP Copy");
    recipeCopier->initAsyncCaller(ACaller);
    recipeCopier->setParam(param);
    recipeCopier->setWindowModality(Qt::WindowModal);
    recipeCopier->setCloseCall([this, recipeCopier](CDE90WindowCloseStatus status){
        if (status == HW_OK) {
            ListViewMainData* selectedClass = MH->getSelectedData(0);
            if (selectedClass == nullptr) {
                return true;
            }
            quint64 recipeId = recipeCopier->getAfterCopyId();
            if (recipeId <= 0) {
                return true;
            }
            quint64 classId = recipeCopier->getAfterCopyClassId();
            if (selectedClass->getId() == classId) {
                refreshRecipeData(classId, recipeId);
                return true;
            }
            MH->details[0].listView->selectionModel()->clearSelection();
            refreshClassData(classId);
        }
        return true;
    });
    recipeCopier->show();
}

void FileManagerController::openSearchView(
    char type) {
    CDE90InputBox inputBox(this);
    QString title = "Search ";
    if (type == 0) {
        title += "Class";
    } else if (type == 1) {
        title += "IDW";
    } else if (type == 2) {
        title += "IDP";
    } else if (type == 3) {
        title += "Recipe";
    }
    inputBox.setWindowTitle(title);
    inputBox.setEditTitle("Search by file name");
    inputBox.setInputCallback([](const QString& str) {
        if (str.isEmpty()) {
            return false;
        }
        return true;
    });
    QString text = inputBox.waitInput();
    if (text.isEmpty()) {
        return;
    }
    if (type == 0) {
        ClassSearchDTO param;
        param.setClassName(text);
        refreshClassData(param);
        return;
    }
    ListViewMainData* classData = MH->getSelectedData(0);
    if (!classData) {
        return;
    }
    if (type == 1) {
        IDW_WaferSearchDTO param;
        param.setInvalid(0);
        param.setClassId(classData->getId());
        param.setWaferName(text);
        refreshIDWData(param);
    } else if (type == 2) {
        IdpSearchDTO param;
        param.setInvalid(0);
        param.setClassId(classData->getId());
        param.setIdpName(text);
        refreshIDPData(param);
    } else if (type == 3) {
        RecipeSearchDTO param;
        param.setClassId(classData->getId());
        param.setInvalid(0);
        param.setRecipeName(text);
        refreshRecipeData(param);
    }
}

void FileManagerController::createHistoryView() {
    if (dataHelper->historyViewController) return;
    dataHelper->historyViewController = new HistoryViewController(this);
    dataHelper->historyViewController->setWindowTitle("History & Version");
    dataHelper->historyViewController->setWindowModality(Qt::WindowModal);
    dataHelper->historyViewController->initAsyncCaller(ACaller);
    dataHelper->historyViewController->enableEditBtn(dataHelper->idwCreateAction->isEnabled());
    connect(dataHelper->historyViewController, &HistoryViewController::openEditor, this, [this](HistoryVersionDataType type, quint64 id, bool allowCoverSave){
        dataHelper->historyViewController->close();
        if (type == HVDT_IDW) {
            ListViewMainData* data = MH->getSelectedData(0);
            emit openIDWEditor(data ? data->getId() : 0, data ? data->getText() : "", id, "", allowCoverSave);
        } else if (type == HVDT_IDP) {
            ListViewMainData* data = MH->getSelectedData(0);
            emit openIdpEditor(data ? data->getId() : 0, id, "", allowCoverSave, 0);
        }  else if (type == HVDT_RECIPE) {
            ListViewMainData* data = MH->getSelectedData(0);
            emit openRecipeEditor(data ? data->getId() : 0, id, "", allowCoverSave);
        }
    });
    // dataHelper->historyViewController->setCloseCall([this](CDE90WindowCloseStatus status){
    //     if (dataHelper->historyViewController->getLastDataType() == HVDT_IDW) {
    //         ListViewMainData* data = getSelectedData(0);
    //         if (data)
    //             refreshIDWData(data->getId());
    //     }
    //     return true;
    // });
}

void FileManagerController::create() {
    MH->details[0].label_action->setText("Class");
    MH->details[1].label_action->setText("IDW");
    MH->details[2].label_action->setText("IDP");
    MH->details[3].label_action->setText("Recipe");
    MH->details[0].label_item->setText("Class Item: ");
    MH->details[1].label_item->setText("IDW Item: ");
    MH->details[2].label_item->setText("IDP Item: ");
    MH->details[3].label_item->setText("Recipe Item: ");
    MH->details[0].label_selected->setText("Select Class");
    MH->details[1].label_selected->setText("Select IDW");
    MH->details[2].label_selected->setText("Select IDP");
    MH->details[3].label_selected->setText("Select Recipe");
    createMenu();
    createActionMenu();
    for (int index = 0; index < 4; ++index) {
        DetailWidget& widgets = MH->details[index];
        connect(widgets.listView->selectionModel(),
                &QItemSelectionModel::selectionChanged,
                this,
                [this, &widgets, index](const QItemSelection& selected,
                                        const QItemSelection& deselected) {
                    Q_UNUSED(deselected)
                    if (selected.size() != 1) {
                        return;
                    }
                    QModelIndexList list = selected[0].indexes();
                    if (list.size() != 1) {
                        return;
                    }
                    widgets.textfield_selected->getQLineEdit()->setText(list[0].data().toString());
                    if (index == 0) {
                        ListViewMainData* view = reinterpret_cast<ListViewMainData*>(
                            list[0].internalPointer());
                        if (view) {
                            ACaller->stop(refreshForIDWGid);
                            ACaller->stop(refreshForIDPGid);
                            ACaller->stop(refreshForRecipeGid);
                            refreshIDWData(view->getId());
                            refreshIDPData(view->getId());
                            refreshRecipeData(view->getId());
                        }
                    } else if (index == 1) {
                        ListViewMainData* view = reinterpret_cast<ListViewMainData*>(
                            list[0].internalPointer());
                        if (view) {
                            ACaller->stop(refreshForIDPGid);
                            IdpSearchDTO param;
                            param.setInvalid(0);
                            param.setIdwTopId(view->getTopId());
                            refreshIDPData(param);
                        }
                    }
                });
    }
    refreshClassData();
}

void FileManagerController::updateLoadingBtnStatus(int i, bool succ) {
    if (succ) {
        int gid = 0;
        if (i == 1) {
            gid = refreshForIDWGid;
        } else if (i == 2) {
            gid = refreshForIDPGid;
        } else if (i == 3) {
            gid = refreshForRecipeGid;
        } else {
            return;
        }
        MH->details[i].btn_action->loading();
        succ = ACaller->pushFocus(this, gid, [this, i](TSC cmd){
            if (cmd.isStop()) return;
            MainThreadInvoker::getInvoker().runLater([this, i](){
                MH->details[i].btn_action->overLoading();
            });
        });
        if (!succ) MH->details[i].btn_action->overLoading();
    }
}

void FileManagerController::openClassEditor() {
    ListViewMainData* data = MH->getSelectedData(0);
    if (data) {
        openClassEditor(data->getId(), data->getText());
    }
}
