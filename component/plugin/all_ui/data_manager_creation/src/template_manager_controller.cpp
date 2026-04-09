#include "template_manager_controller.h"
#include "cde90_button.h"
#include "cde90_label.h"
#include "cde90_list_view.h"
#include "cde90_menu.h"
#include "cde90_message_box.h"
#include "cde90_text_field.h"
#include "service_manager.h"
#include "template_info_service.h"
#include "list_view_main_data_model.h"
#include "private/data_manager_controller_helper.h"

constexpr int refreshGId = 1;
#define DM dataHelper->dataManagerHelper

struct TemplateManagerControllerDataHelper {
    DataManagerControllerHelper* dataManagerHelper;
    int totalLimitCount = 5000;

    QAction* apCreateAction;
    QAction* apEditAction;
    QAction* msCreateAction;
    QAction* msEditAction;
    QAction* mpCreateAciton;
    QAction* mpEditAction;
    QAction* mpipCreateAction;
    QAction* mpipEditAction;
};

TemplateManagerController::TemplateManagerController(
    QWidget* parent)
    : RelationTemplateManager(parent) {
    dataHelper = new TemplateManagerControllerDataHelper;
    dataHelper->dataManagerHelper = new DataManagerControllerHelper;
    DM->create();
    setContent(DM->content);
    initAsyncCaller();
    create();
}
TemplateManagerController::~TemplateManagerController() noexcept {
    delete dataHelper->dataManagerHelper;
    delete dataHelper;
}

void TemplateManagerController::create() {
    DM->details[0].label_action->setText("Alignment");
    DM->details[1].label_action->setText("Addressing");
    DM->details[2].label_action->setText("Measurement");
    DM->details[3].label_action->setText("MP/IP Information");
    DM->details[0].label_item->setText("AP: ");
    DM->details[1].label_item->setText("MP: ");
    DM->details[2].label_item->setText("MS: ");
    DM->details[3].label_item->setText("MP/IP Information: ");
    DM->details[0].label_selected->setText("Select AP Template");
    DM->details[1].label_selected->setText("Select MP Template");
    DM->details[2].label_selected->setText("Select MS Template");
    DM->details[3].label_selected->setText("Select MP/IP Information");

    createMenu();
    createActionMenu();
    for (int index = 0; index < 4; ++index) {
        DetailWidget& widgets = DM->details[index];
        connect(widgets.listView->selectionModel(),
                &QItemSelectionModel::selectionChanged,
                this,
                [&widgets](const QItemSelection& selected, const QItemSelection& deselected) {
                    Q_UNUSED(deselected)
                    if (selected.size() != 1) {
                        return;
                    }
                    QModelIndexList list = selected[0].indexes();
                    if (list.size() != 1) {
                        return;
                    }
                    widgets.textfield_selected->getQLineEdit()->setText(list[0].data().toString());
                });
    }
    refreshAlignmentList();
    refreshAddressingList();
    refreshMPList();
    refreshMPIPList();
}

void TemplateManagerController::createMenu() {
    QAction* exitAction = DM->fileMenu->addAction("Exit");
    CDE90Menu* initializeMenu = new CDE90Menu("Initialize");
    menuStyle(DM->deviceMenu);
    DM->deviceMenu->addMenu(initializeMenu);
    /*QAction* moAction = */initializeMenu->addAction("MO");
    /*QAction* errLogAction = */DM->showMenu->addAction("Error Log");
    connect(exitAction, &QAction::triggered, this, [this]() { close(); });
    // connect(moAction, &QAction::triggered, this, [this]() {});
    // connect(errLogAction, &QAction::triggered, this, [this]() {});

    /*QAction* helpCnDocAction = */DM->helpeMenu->addAction("中文");
    /*QAction* helpEnDocAction = */DM->helpeMenu->addAction("English");
    // connect(helpCnDocAction, &QAction::triggered, this, [this]() {});
    // connect(helpEnDocAction, &QAction::triggered, this, [this]() {});
}

void TemplateManagerController::createActionMenu() {
    createAlignmentMenu();
    createAddressMenu();
    createMpMenu();
    createMpIpMenu();
}
void TemplateManagerController::createAlignmentMenu() {
    CDE90Menu* menu = new CDE90Menu(DM->details[0].btn_action);
    QAction* action;
    menu->setProperty("MenuArrowAndSeq", "arrow");
    DM->details[0].btn_action->getQPushButton()->setMenu(menu);
    action = menu->addAction("Edit");
    dataHelper->apEditAction = action;
    connect(action, &QAction::triggered, this, [this]() {
        ListViewMainData* data = DM->getSelectedData(0);
        if (!data) {
            return;
        }
        emit openAlignmentEditor(data->getId(), "");
    });
    menu->addAction("Copy");
    menu->addAction("Move");
    action = menu->addAction("Delete");
    connect(action, &QAction::triggered, this, [this]() { deleteTemplate(TEMP_ALIGN); });
    action = menu->addAction("Rename");
    connect(action, &QAction::triggered, this, [this]() {
        ListViewMainData* data = DM->getSelectedData(0);
        if (!data) {
            return;
        }
        openRenameEditor(data->getId(), data->getText(), TEMP_ALIGN);
    });
    action = menu->addAction("Create");
    dataHelper->apCreateAction = action;
    connect(action, &QAction::triggered, this, [this](bool) {
        if (DM->checkIsOverTheItemCountAndNotify(0, dataHelper->totalLimitCount)) {
            return;
        }
        openCreateEditor("Create Alignment Template", TEMP_ALIGN);
    });
    CDE90Menu* subMenu = new CDE90Menu("Search", menu);
    action = subMenu->addAction("Name");
    connect(action, &QAction::triggered, this, [this]() { openSearchView(0, 0); });
    action = subMenu->addAction("Comment");
    connect(action, &QAction::triggered, this, [this]() { openSearchView(0, 1); });
    menu->addMenu(subMenu);
    action = menu->addAction("Update");
    connect(action, &QAction::triggered, this, [this](){
        refreshAlignmentList();
    });
    menu->addAction("Version");
    menu->addAction("History");
}
void TemplateManagerController::createAddressMenu() {
    CDE90Menu* menu = new CDE90Menu(DM->details[1].btn_action);
    QAction* action;
    menu->setProperty("MenuArrowAndSeq", "arrow");
    DM->details[1].btn_action->getQPushButton()->setMenu(menu);
    action = menu->addAction("Edit");
    dataHelper->msEditAction = action;
    connect(action, &QAction::triggered, this, [this]() {
        ListViewMainData* data = DM->getSelectedData(1);
        if (!data) {
            return;
        }
        emit openAddressingEditor(data->getId(), "");
    });
    menu->addAction("Copy");
    menu->addAction("Move");
    action = menu->addAction("Delete");
    connect(action, &QAction::triggered, this, [this]() { deleteTemplate(TEMP_MS); });
    action = menu->addAction("Rename");
    connect(action, &QAction::triggered, this, [this]() {
        ListViewMainData* data = DM->getSelectedData(1);
        if (!data) {
            return;
        }
        openRenameEditor(data->getId(), data->getText(), TEMP_MS);
    });
    action = menu->addAction("Create");
    dataHelper->msCreateAction = action;
    connect(action, &QAction::triggered, this, [this](bool) {
        if (DM->checkIsOverTheItemCountAndNotify(1, dataHelper->totalLimitCount)) {
            return;
        }
        openCreateEditor("Create Addressing Template", TEMP_MS);
    });
    CDE90Menu* subMenu = new CDE90Menu("Search", menu);
    action = subMenu->addAction("Name");
    connect(action, &QAction::triggered, this, [this]() { openSearchView(1, 0); });
    action = subMenu->addAction("Comment");
    connect(action, &QAction::triggered, this, [this]() { openSearchView(1, 1); });
    menu->addMenu(subMenu);
    action = menu->addAction("Update");
    connect(action, &QAction::triggered, this, [this](){
        refreshAddressingList();
    });
    menu->addAction("Version");
    menu->addAction("History");
}
void TemplateManagerController::createMpMenu() {
    CDE90Menu* menu = new CDE90Menu(DM->details[2].btn_action);
    menu->setProperty("MenuArrowAndSeq", "arrow");
    DM->details[2].btn_action->getQPushButton()->setMenu(menu);
    QAction* action;
    action = menu->addAction("Edit");
    dataHelper->mpEditAction = action;
    connect(action, &QAction::triggered, this, [this]() {
        ListViewMainData* data = DM->getSelectedData(2);
        if (!data) {
            return;
        }
        emit openMpEditor(data->getId(), "");
    });
    menu->addAction("Copy");
    menu->addAction("Move");
    action = menu->addAction("Delete");
    connect(action, &QAction::triggered, this, [this]() { deleteTemplate(TEMP_MP); });
    action = menu->addAction("Rename");
    connect(action, &QAction::triggered, this, [this]() {
        ListViewMainData* data = DM->getSelectedData(2);
        if (!data) {
            return;
        }
        openRenameEditor(data->getId(), data->getText(), TEMP_MP);
    });
    action = menu->addAction("Create");
    dataHelper->mpCreateAciton = action;
    connect(action, &QAction::triggered, this, [this](bool) {
        if (DM->checkIsOverTheItemCountAndNotify(2, dataHelper->totalLimitCount)) {
            return;
        }
        openCreateEditor("Create MP Template", TEMP_MP);
    });
    menu->addAction("Display All");
    menu->addAction("Cross Reference");
    CDE90Menu* subMenu = new CDE90Menu("Search", menu);
    action = subMenu->addAction("Name");
    connect(action, &QAction::triggered, this, [this]() { openSearchView(2, 0); });
    action = subMenu->addAction("Comment");
    connect(action, &QAction::triggered, this, [this]() { openSearchView(2, 1); });
    menu->addMenu(subMenu);
    action = menu->addAction("Update");
    connect(action, &QAction::triggered, this, [this](){
        refreshMPList();
    });
    menu->addAction("Version");
    menu->addAction("History");
}
void TemplateManagerController::createMpIpMenu() {
    CDE90Menu* menu = new CDE90Menu(DM->details[3].btn_action);
    menu->setProperty("MenuArrowAndSeq", "arrow");
    DM->details[3].btn_action->getQPushButton()->setMenu(menu);
    QAction* action;
    action = menu->addAction("Edit");
    dataHelper->mpipEditAction = action;
    connect(action, &QAction::triggered, this, [this]() {
        ListViewMainData* data = DM->getSelectedData(3);
        if (!data) {
            return;
        }
        emit openMpIpEditor(data->getId(), "");
    });
    menu->addAction("Copy");
    menu->addAction("Move");
    action = menu->addAction("Delete");
    connect(action, &QAction::triggered, this, [this]() { deleteTemplate(TEMP_MPIP); });
    action = menu->addAction("Rename");
    connect(action, &QAction::triggered, this, [this]() {
        ListViewMainData* data = DM->getSelectedData(3);
        if (!data) {
            return;
        }
        openRenameEditor(data->getId(), data->getText(), TEMP_MPIP);
    });
    action = menu->addAction("Create");
    dataHelper->mpipCreateAction = action;
    connect(action, &QAction::triggered, this, [this](bool) {
        if (DM->checkIsOverTheItemCountAndNotify(3, dataHelper->totalLimitCount)) {
            return;
        }
        openCreateEditor("Create MP/IP Template", TEMP_MPIP);
    });
    CDE90Menu* subMenu = new CDE90Menu("Search", menu);
    action = subMenu->addAction("Name");
    connect(action, &QAction::triggered, this, [this]() { openSearchView(3, 0); });
    action = subMenu->addAction("Comment");
    connect(action, &QAction::triggered, this, [this]() { openSearchView(3, 1); });
    menu->addMenu(subMenu);
    action = menu->addAction("Update");
    connect(action, &QAction::triggered, this, [this](){
        refreshMPIPList();
    });
    menu->addAction("Version");
    menu->addAction("History");
}

void TemplateManagerController::openCreateEditor(
    const QString& title, int type) {
    CDE90InputBox box(this);
    box.setInputLength(FILE_NAME_LENGTH);
    box.setInputCallback([this](const QString& text) {
        if (text.isEmpty()) {
            simpleMessageBox(this, "Message", "Please input file name.");
            return false;
        }
        return true;
    });
    box.setWindowTitle("Create");
    box.setEditTitle(title);
    box.resize(300, 200);
    QString fileName = box.waitInput();
    if (fileName.length() > 0) {
        if (type == TEMP_ALIGN) {
            emit openAlignmentEditor(0, fileName);
        } else if (type == TEMP_MS) {
            emit openAddressingEditor(0, fileName);
        } else if (type == TEMP_MP) {
            emit openMpEditor(0, fileName);
        } else if (type == TEMP_MPIP) {
            emit openMpIpEditor(0, fileName);
        }
    }
}

void TemplateManagerController::refreshAlignmentList(quint64 selectedId) {
    refreshList(TEMP_ALIGN, selectedId);
}

void TemplateManagerController::refreshAddressingList(quint64 selectedId) {
    refreshList(TEMP_MS, selectedId);
}

void TemplateManagerController::refreshMPList(quint64 selectedId) {
    refreshList(TEMP_MP, selectedId);
}
void TemplateManagerController::refreshMPIPList(quint64 selectedId) {
    refreshList(TEMP_MPIP, selectedId);
}
void TemplateManagerController::deleteTemplate(
    int type) {
    ListViewMainData* data = DM->getSelectedData(type);
    if (!data) {
        return;
    }
    int r = showYesNoBox(this, "Message", "Are you sure to delete this template ?", true);
    if (r) {
        return;
    }
    quint64 id = data->getId();

    bool succ = getAsyncCaller()->push(this, [this, id, type](TSC) {
        sem::AfterGc gc([this, type]() {
            MainThreadInvoker::getInvoker().runLater(
                [this, type]() { DM->details[type].btn_action->overLoading(); });
        });
        TemplateInfoService* service = ServiceManager::instance()
                                           ->getDefaultService<TemplateInfoService>();
        int errCode;
        service->deleteTemplate(id, &errCode);
        if (errCode) {
            simpleMessageBox(this, "Error", "Error to delete template.");
            return;
        }
        TemplateInfoSearchDTO param;
        param.setTemplateType(type);
        param.setInvalid(0);
        localRefreshList(param);
    });
    if (succ) {
        DM->details[type].btn_action->loading();
    }
}

void TemplateManagerController::enableApAction(bool b) {
    dataHelper->apCreateAction->setEnabled(b);
    dataHelper->apEditAction->setEnabled(b);
}

void TemplateManagerController::enableMsAction(bool b) {
    dataHelper->msCreateAction->setEnabled(b);
    dataHelper->msEditAction->setEnabled(b);
}

void TemplateManagerController::enableMpAction(bool b) {
    dataHelper->mpCreateAciton->setEnabled(b);
    dataHelper->mpEditAction->setEnabled(b);
}

void TemplateManagerController::enableMpIpAction(bool b) {
    dataHelper->mpipCreateAction->setEnabled(b);
    dataHelper->mpipEditAction->setEnabled(b);
}

void TemplateManagerController::localRefreshList(
    const TemplateInfoSearchDTO& searchParam, quint64 selectedId) {
    const Char8& type = searchParam.getTemplateType();
    if (type == nullptr || type.value() < 0
        || type.value() >= sizeof(DM->details) / sizeof(DetailWidget)) {
        return;
    }
    TemplateInfoService* service = ServiceManager::instance()
                                       ->getDefaultService<TemplateInfoService>();
    int errCode;
    QList<TemplateInfoEntity> list = service->searchList(searchParam, &errCode);
    if (errCode) {
        simpleMessageBox(this, "Error", "Error to load template list.");
        return;
    }
    if (list.isEmpty()) {
        MainThreadInvoker::getInvoker().blockRun([this, type]() {
            ListViewMainModel* mode = dynamic_cast<ListViewMainModel*>(
                DM->details[type].listView->model());
            mode->clear();
            DM->details[type].textfield_item->setText(QString("%1/%2").arg(0).arg(dataHelper->totalLimitCount));
        });
        return;
    }
    int selectedIndex = -1;
    QList<ListViewMainData> viewList(list.size());
    for (int i = 0; i < list.size(); ++i) {
        viewList[i].setId(list[i].getId());
        viewList[i].setText(list[i].getTemplateName());
        if (selectedIndex == -1 && selectedId == list[i].getId()) {
            selectedIndex = i;
        }
    }
    MainThreadInvoker::getInvoker().blockRun([this, &viewList, type, selectedIndex]() {
        ListViewMainModel* mode = dynamic_cast<ListViewMainModel*>(DM->details[type].listView->model());
        mode->clear();
        mode->insertDatas(viewList);
        DM->details[type].textfield_selected->getQLineEdit()->clear();
        DM->details[type].textfield_item->setText(
            QString("%1/%2").arg(viewList.size()).arg(dataHelper->totalLimitCount));
        DM->details[type].listView->selectRow(selectedIndex);
    });
}

void TemplateManagerController::refreshList(
    int type, quint64 selectedId) {
    TemplateInfoSearchDTO param;
    param.setTemplateType(type);
    param.setInvalid(0);
    refreshList(param, selectedId);
}

void TemplateManagerController::refreshList(
    const TemplateInfoSearchDTO& param, quint64 selectedId) {
    const Char8& type = param.getTemplateType();
    if (type == nullptr || type.value() < 0
        || type.value() >= sizeof(DM->details) / sizeof(DetailWidget)) {
        return;
    }
    DM->details[type.value()].btn_action->loading();
    bool succ = ACaller->push(this, refreshGId, [this, param, selectedId](TSC) {
        sem::AfterGc gc([this, &param]() {
            MainThreadInvoker::getInvoker().blockRun([this, &param]() {
                DM->details[param.getTemplateType().value()].btn_action->overLoading();
            });
        });
        localRefreshList(param, selectedId);
    });
    if (!succ) {
        DM->details[type].btn_action->overLoading();
    }
}

void TemplateManagerController::openRenameEditor(
    quint64 id, const QString& text, int type) {
    CDE90InputBox* box = new CDE90InputBox(this);
    box->setInputLength(FILE_NAME_LENGTH);
    box->setInputCallback([this, box, id, type](const QString& text) {
        if (text.isEmpty()) {
            simpleMessageBox(this, "Message", "Please input template name.");
        }
        box->loadingStatus(true);
        getAsyncCaller()->push(this, [this, id, text, box, type](TSC) {
            TemplateInfoService* service = ServiceManager::instance()
                                               ->getDefaultService<TemplateInfoService>();
            int errCode = service->updateNameOnly(id, text);
            if (errCode) {
                MainThreadInvoker::getInvoker().runLater([box]() { box->loadingStatus(false); });
                simpleMessageBox(this, "Error", "Rename Failed!");
                return;
            }
            TemplateInfoSearchDTO searchParam;
            searchParam.setTemplateType(type);
            searchParam.setInvalid(0);
            localRefreshList(searchParam);
            MainThreadInvoker::getInvoker().runLater([this, box, type]() {
                box->loadingStatus(false);
                box->close();
                ListViewMainData* view = DM->getSelectedData(0);
                if (view) {
                    refreshList(type);
                }
            });
        });
        return false;
    });
    box->setWindowTitle("Rename");
    if (type == TEMP_ALIGN) {
        box->setEditTitle("Rename AP template");
    } else if (type == TEMP_MS) {
        box->setEditTitle("Rename Addressing template");
    } else if (type == TEMP_MP) {
        box->setEditTitle("Rename MP template");
    } else if (type == TEMP_MPIP) {
        box->setEditTitle("Rename MP/IP template");
    }
    box->setInitText(text);
    box->resize(300, 200);
    box->setAttribute(Qt::WA_DeleteOnClose);
    box->show();
}

void TemplateManagerController::openSearchView(
    char type, char input) {
    CDE90InputBox inputBox(this);
    QString title = "Search ";
    if (type == 0) {
        title += "Alignment";
    } else if (type == 1) {
        title += "Addressing";
    } else if (type == 2) {
        title += "Measurement";
    } else if (type == 3) {
        title += "MP/IP";
    } else {
        return;
    }
    inputBox.setWindowTitle(title);
    if (input == 0) {
        inputBox.setEditTitle("Search by filename");
    } else if (input == 1) {
        inputBox.setEditTitle("Search by comment");
    }
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
    TemplateInfoSearchDTO param;
    param.setTemplateType(type);
    if (input == 0) {
        param.setTemplateName(text);
    } else if (input == 1) {
        param.setTemplateComment(text);
    }
    refreshList(param);
}
