#include "image_operation_view_setter_controller.h"
#include <QSpacerItem>
#include "cbox_config_service.h"
#include "cbox_global_keys.h"
#include "cde90_button.h"
#include "cde90_dialog.h"
#include "cde90_label.h"
#include "cde90_menu.h"
#include "cde90_message_box.h"
#include "cde90_process_box.h"
#include "cde90_spliter_line.h"
#include "cde90_table_view.h"
#include "scan_mode_data_model.h"
#include "scan_model_data_convert.h"
#include "sem_scan_mode_config_service.h"
#include "service_manager.h"
#include "table_cell_menu_helper.h"
#include "image_scan_live_mode_helper.h"
#include "cdsem_global_data.h"
//两个tableMode公用一个缓存变量

struct ViewScanDataModeHelper {
    QBrush disableBrush;
};

ViewScanDataMode::ViewScanDataMode(bool viewMode,
                                   QHash<CBoxMixKey, QString>* textValue,
                                   QObject* parent)
    : SimpleRowTableModel(parent)
    , viewMode(viewMode) {
    helper = new ViewScanDataModeHelper;
    this->valueToText = textValue;
    insertHead("No.");
    insertHead("Scan");
    insertHead("Frame");
    insertHead("Pixel X");
    insertHead("Pixel Y");
    insertHead("Filter");
    insertHead("Flat");
    insertHead("D.C.");
    helper->disableBrush = QBrush(QColor(204, 204, 204));
}

ViewScanDataMode::~ViewScanDataMode() noexcept {
    delete helper;
}

void ViewScanDataMode::updateView() {
    MainThreadInvoker::getInvoker().runLater([this](){
        notifyDataChanged(0, getDataList().size() - 1);
    });
}

QVariant ViewScanDataMode::data(const QModelIndex &index, int role) const {
    if (role == Qt::ForegroundRole && viewMode && index.column() >= 3) {
        return helper->disableBrush;
    }
    return SimpleRowTableModel::data(index, role);
}

QVariant ViewScanDataMode::getItemData(qint32 row, qint32 column) const {
    if (row < 0 || row >= getDataList().size()) {
        return QVariant();
    }
    const ScanModeTableData& data = getDataList()[row];
    switch (column) {
    case 0:
        return row + 1;
    case 1:
        return data.scanRateText;
    case 2:
        return data.data.getFrame();
    case 3:
        return data.data.getPixelX();
    case 4:
        return data.data.getPixelY();
    case 5:
        return data.filterText;
    case 6:
        return data.flatText;
    case 7:
        return data.dcText;

        break;
    }
    return QVariant();
}

ImageOperationViewSetterController::ImageOperationViewSetterController(QWidget* parent): CDE90Dialog(parent) {
    create();
    createAction();
}
ImageOperationViewSetterController::~ImageOperationViewSetterController() noexcept {
    qDebug() << "ImageOperationViewSetterController release";
}

void ImageOperationViewSetterController::create() {
    content = new QWidget;
    contentLayout = new QVBoxLayout(content);
    contentLayout->setContentsMargins(0,0,0,0);
    contentLayout->setSpacing(5);
    content->setLayout(contentLayout);

    topContent = new QWidget(content);
    topLayout = new QHBoxLayout(topContent);
    topLayout->setContentsMargins(5,5,5,5);
    topLayout->setSpacing(5);
    topLayout->setAlignment(Qt::AlignLeft);
    topContent->setLayout(topLayout);
    contentLayout->addWidget(topContent);

    defaultLabel = new CDE90Label("Default", topContent);
    saveBtn = new CDE90ButtonBox("Save", topContent);
    loadBtn = new CDE90ButtonBox("Load", topContent);
    topLayout->addWidget(defaultLabel);
    topLayout->addWidget(saveBtn);
    topLayout->addWidget(loadBtn);

    contentLayout->addWidget(new CDE90SpliterHLine(content));

    viewModeContent  = new QWidget(content);
    viewModeLayout = new QVBoxLayout(viewModeContent);
    viewModeLayout->setContentsMargins(5,5,5,5);
    viewModeLayout->setSpacing(5);
    viewModeContent->setLayout(viewModeLayout);
    contentLayout->addWidget(viewModeContent);

    viewModeTitleContent = new QWidget(viewModeContent);
    viewModeTitleLayout = new QHBoxLayout(viewModeTitleContent);
    viewModeTitleLayout->setContentsMargins(0,0,0,0);
    viewModeTitleLayout->setSpacing(5);
    viewModeTitleContent->setLayout(viewModeTitleLayout);
    viewModeLayout->addWidget(viewModeTitleContent);
    viewModeLabel = new CDE90Label("View Mode", viewModeTitleContent);
    viewScanBtn = new CDE90ButtonBox("Scan", viewModeTitleContent);
    viewModeTitleLayout->addWidget(viewModeLabel);
    viewModeTitleLayout->addItem(new QSpacerItem(10,10, QSizePolicy::Expanding, QSizePolicy::Preferred));
    viewModeTitleLayout->addWidget(viewScanBtn);

    viewDataMode = new ViewScanDataMode(true, &valueToText);

    viewTable = new CDE90TableView(viewDataMode, viewModeContent);
    viewTable->setSelectionMode(QTableView::SingleSelection);
    viewTable->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    viewTable->setFixedHeight(115);
    viewModeLayout->addWidget(viewTable);

    contentLayout->addWidget(new CDE90SpliterHLine(content));

    integModeContent  = new QWidget(content);
    integModeLayout = new QVBoxLayout(integModeContent);
    integModeLayout->setContentsMargins(5,5,5,5);
    integModeLayout->setSpacing(5);
    integModeContent->setLayout(integModeLayout);
    contentLayout->addWidget(integModeContent);

    integModeTitleContent = new QWidget(integModeContent);
    integModeTitleLayout = new QHBoxLayout(integModeTitleContent);
    integModeTitleLayout->setContentsMargins(0,0,0,0);
    integModeTitleLayout->setSpacing(5);
    integModeTitleContent->setLayout(integModeTitleLayout);
    integModeLayout->addWidget(integModeTitleContent);
    integModeLabel = new CDE90Label("Integ Mode", integModeTitleContent);
    integScanBtn = new CDE90ButtonBox("Scan", integModeTitleContent);
    integModeTitleLayout->addWidget(integModeLabel);
    integModeTitleLayout->addItem(new QSpacerItem(10,10, QSizePolicy::Expanding, QSizePolicy::Preferred));
    integModeTitleLayout->addWidget(integScanBtn);

    integDataMode = new ViewScanDataMode(false, &valueToText);
    integTable = new CDE90TableView(integDataMode, integModeContent);
    integTable->setSelectionMode(QTableView::SingleSelection);
    integTable->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    integTable->setFixedHeight(175);
    integModeLayout->addWidget(integTable);

    contentLayout->addWidget(new CDE90SpliterHLine(content));

    bottomContent = new QWidget(content);
    bottomLayout = new QHBoxLayout(bottomContent);
    bottomLayout->setContentsMargins(5,5,5,5);
    bottomLayout->setSpacing(5);
    bottomContent->setLayout(bottomLayout);
    contentLayout->addWidget(bottomContent);

    applyBtn = new CDE90ButtonBox("Apply", bottomContent);
    closeBtn = new CDE90ButtonBox("Close", bottomContent);
    bottomLayout->addWidget(applyBtn);
    bottomLayout->addItem(new QSpacerItem(10,10, QSizePolicy::Expanding, QSizePolicy::Preferred));
    bottomLayout->addWidget(closeBtn);
    setContent(content);
}

void ImageOperationViewSetterController::createAction() {
    connect(closeBtn->getQPushButton(), &CDE90Button::clicked, this, [this](){
        this->close();
    });
    viewTableMenuHelper = new TableCellMenuHelper(viewTable);
    integTableMenuHelper = new TableCellMenuHelper(integTable);
    connect(loadBtn->getQPushButton(), &QPushButton::clicked, this, [this]() {
        bool succ = ACaller->push(this, [this](TSC) {
            if (localLoadDataFromDatabase()) {
                simpleMessageBox(this,
                                 "Error",
                                 "Load default data error, current data is not changed.");
            }
        });
        if (succ) {
            loadingBox(this, "Working", "Loading");
        }
    });
    connect(saveBtn->getQPushButton(), &QPushButton::clicked, this, [this]() { saveData(); });
    connect(applyBtn->getQPushButton(), &QPushButton::clicked, this, [this]() {
        bool succ = ACaller->push(this, [this](TSC){
            localCopyTableDataToGlobal();
            MainThreadInvoker::getInvoker().runLater([this](){
                emit dataApplied();
            });
        });
        if (succ) {
            loadingBox(this, "Working", "Saving...");
        }
    });
    connect(viewScanBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        int i = viewTable->getSelectedRow();
        QList<ScanModeTableData>& list = viewDataMode->getDataList();
        if (i < 0 || i >= list.size()) {
            return;
        }
        ImageScanLiveModeHelper::get()->setGlobal(ISM_CURRENT, list[i].data, true);
    });
    connect(integScanBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        int i = integTable->getSelectedRow();
        QList<ScanModeTableData>& list = integDataMode->getDataList();
        if (i < 0 || i >= list.size()) {
            return;
        }
        ImageScanLiveModeHelper::get()->setGlobal(ISM_CURRENT, list[i].data, true);
    });

    connect(cdsem::CDSEM_GlobalDataHelper::get(), &cdsem::CDSEM_GlobalData::procedureRunningStatusChanged, this, [this]() {
        setProcedureRunningStatus();
    });
    setProcedureRunningStatus();
}

void ImageOperationViewSetterController::setProcedureRunningStatus() {
    bool disabled = cdsem::CDSEM_GlobalDataHelper::get()->procedureRunningStatus() == cdsem::PRS_RUNNING;
    viewScanBtn->setDisabled(disabled);
    integScanBtn->setDisabled(disabled);
    applyBtn->setDisabled(disabled);
}

bool ImageOperationViewSetterController::localRefreshView(QList<ScanModeTableData>& list) {
    bool dChanged = false;
    for (ScanModeTableData& data: list) {
        dChanged |= localRefreshOneView(data);
    }
    localLoadMenu();
    return dChanged;
}

bool ImageOperationViewSetterController::localRefreshOneView(ScanModeTableData& data) {
    CboxConfigService* service = ServiceManager::defaultService<CboxConfigService>();
    CBoxMixKey key = CBoxMixKey(CBOX_TYPE_SEM_IMG_SCAN_RATE, data.data.getScanRate());
    bool dChanged = false;
    bool cached;
    data.scanRateText = service->getText(key, &cached);
    dChanged |= cached;
    if (!cached) {
        if (service->cacheText(key)) {
            data.scanRateText = "Error";
        } else {
            data.scanRateText = service->getText(key);
        }
    }
    key = CBoxMixKey(CBOX_TYPE_SEM_IMG_FILTER, data.data.getFilterType());
    data.filterText = service->getText(key, &cached);
    dChanged |= cached;
    if (!cached) {
        if (service->cacheText(key)) {
            data.filterText = "Error";
        } else {
            data.filterText = service->getText(key);
        }
    }
    key = CBoxMixKey(CBOX_TYPE_SEM_IMG_FLAT, data.data.getFlat());
    data.flatText = service->getText(key, &cached);
    dChanged |= cached;
    if (!cached) {
        if (service->cacheText(key)) {
            data.flatText = "Error";
        } else {
            data.flatText = service->getText(key);
        }
    }
    key = CBoxMixKey(CBOX_TYPE_SEM_IMG_DC, data.data.getDcType());
    data.dcText = service->getText(key, &cached);
    dChanged |= cached;
    if (!cached) {
        if (service->cacheText(key)) {
            data.dcText = "Error";
        } else {
            data.dcText = service->getText(key);
        }
    }
    return dChanged;
}

void ImageOperationViewSetterController::localLoadMenu() {
    CboxConfigService* service = ServiceManager::instance()
                                     ->getDefaultService<CboxConfigService>();
    int errCode;
    bool cached;
    const QList<CBoxConfigEntity>* scanList
        = &service->getCBoxConfigList(CBOX_TYPE_SEM_IMG_SCAN_RATE,
                                                &cached);
    if (!cached) {
        errCode = service->cacheCBoxConfigList(CBOX_TYPE_SEM_IMG_SCAN_RATE);
        if (errCode) {
            simpleMessageBox(this, "Error", "Error to load \"Scan Rate\" menu.");
        } else {
            scanList
                = &service->getCBoxConfigList(CBOX_TYPE_SEM_IMG_SCAN_RATE);
        }
    }

    const QList<CBoxConfigEntity>* frameList
        = &service->getCBoxConfigList(CBOX_TYPE_SEM_IMG_FRAME, &cached);
    if (!cached) {
        errCode = service->cacheCBoxConfigList(CBOX_TYPE_SEM_IMG_FRAME);
        if (errCode) {
            simpleMessageBox(this, "Error", "Error to load \"Frame\" menu.");
        } else {
            frameList
                = &service->getCBoxConfigList(CBOX_TYPE_SEM_IMG_FRAME);
        }
    }

    const QList<CBoxConfigEntity>* pixelXList
        = &service->getCBoxConfigList(CBOX_TYPE_SEM_IMG_X_PIX_COUNT,
                                                &cached);
    if (!cached) {
        errCode = service->cacheCBoxConfigList(CBOX_TYPE_SEM_IMG_X_PIX_COUNT);
        if (errCode) {
            simpleMessageBox(this, "Error", "Error to load \"Pixel X\" menu.");
        } else {
            pixelXList
                = &service->getCBoxConfigList(CBOX_TYPE_SEM_IMG_X_PIX_COUNT);
        }
    }

    const QList<CBoxConfigEntity>* pixelYList
        = &service->getCBoxConfigList(CBOX_TYPE_SEM_IMG_Y_PIX_COUNT,
                                                &cached);

    if (!cached) {
        errCode = service->cacheCBoxConfigList(CBOX_TYPE_SEM_IMG_Y_PIX_COUNT);
        if (errCode) {
            simpleMessageBox(this, "Error", "Error to load \"Pixel Y\" menu.");
        } else {
            pixelYList
                = &service->getCBoxConfigList(CBOX_TYPE_SEM_IMG_Y_PIX_COUNT);
        }
    }

    const QList<CBoxConfigEntity>* filterList
        = &service->getCBoxConfigList(CBOX_TYPE_SEM_IMG_FILTER, &cached);

    if (!cached) {
        errCode = service->cacheCBoxConfigList(CBOX_TYPE_SEM_IMG_FILTER);
        if (errCode) {
            simpleMessageBox(this, "Error", "Error to load \"Filter\" menu.");
        } else {
            filterList
                = &service->getCBoxConfigList(CBOX_TYPE_SEM_IMG_FILTER);
        }
    }

    const QList<CBoxConfigEntity>* flatList
        = &service->getCBoxConfigList(CBOX_TYPE_SEM_IMG_FLAT, &cached);
    if (!cached) {
        errCode = service->cacheCBoxConfigList(CBOX_TYPE_SEM_IMG_FLAT);
        if (errCode) {
            simpleMessageBox(this, "Error", "Error to load \"Flat\" menu.");
        } else {
            flatList
                = &service->getCBoxConfigList(CBOX_TYPE_SEM_IMG_FLAT);
        }
    }


    const QList<CBoxConfigEntity>* dcList
        = &service->getCBoxConfigList(CBOX_TYPE_SEM_IMG_DC, &cached);
    if (!cached) {
        errCode = service->cacheCBoxConfigList(CBOX_TYPE_SEM_IMG_DC);
        if (errCode) {
            simpleMessageBox(this, "Error", "Error to load \"DC\" menu.");
        } else {
            dcList
                = &service->getCBoxConfigList(CBOX_TYPE_SEM_IMG_DC);
        }
    }


    if (localNeedLoadMenu) {
        localNeedLoadMenu = false;
    }
    MainThreadInvoker::getInvoker().blockRun(
        [this, &scanList, &frameList, &pixelXList, &pixelYList, &filterList, &flatList, &dcList]() {
            CDE90Menu* viewMenu = new CDE90Menu(viewTable);
            CDE90Menu* integMenu = new CDE90Menu(integTable);
            QAction* action;
            int value;
            for (const CBoxConfigEntity& e : *scanList) {
                value = e.getValue().toInt();
                action = viewMenu->addAction(e.getText());
                connect(action, &QAction::triggered, this, [value, this]() {
                    doSelectScan(viewTable, value);
                });
                action = integMenu->addAction(e.getText());
                connect(action, &QAction::triggered, this, [value, this]() {
                    doSelectScan(integTable, value);
                });
            }
            viewTableMenuHelper->setMenu(viewMenu, 1, -1);
            integTableMenuHelper->setMenu(integMenu, 1, -1);

            viewMenu = new CDE90Menu(viewTable);
            integMenu = new CDE90Menu(integTable);
            for (const CBoxConfigEntity& e : *frameList) {
                value = e.getValue().toInt();
                action = viewMenu->addAction(e.getText());
                connect(action, &QAction::triggered, this, [value, this]() {
                    doSelectFrame(viewTable, value);
                });
                action = integMenu->addAction(e.getText());
                connect(action, &QAction::triggered, this, [value, this]() {
                    doSelectFrame(integTable, value);
                });
            }
            viewTableMenuHelper->setMenu(viewMenu, 2, -1);
            integTableMenuHelper->setMenu(integMenu, 2, -1);

            integMenu = new CDE90Menu(integTable);
            for (const CBoxConfigEntity& e : *pixelXList) {
                value = e.getValue().toInt();
                action = integMenu->addAction(e.getText());
                connect(action, &QAction::triggered, this, [value, this]() {
                    doSelectPixelX(integTable, value);
                });
            }
            integTableMenuHelper->setMenu(integMenu, 3, -1);

            integMenu = new CDE90Menu(integTable);
            for (const CBoxConfigEntity& e : *pixelYList) {
                value = e.getValue().toInt();
                action = integMenu->addAction(e.getText());
                connect(action, &QAction::triggered, this, [value, this]() {
                    doSelectPixelY(integTable, value);
                });
            }
            integTableMenuHelper->setMenu(integMenu, 4, -1);

            integMenu = new CDE90Menu(integTable);
            for (const CBoxConfigEntity& e : *filterList) {
                value = e.getValue().toInt();
                action = integMenu->addAction(e.getText());
                connect(action, &QAction::triggered, this, [value, this]() {
                    doSelectFilter(integTable, value);
                });
            }
            integTableMenuHelper->setMenu(integMenu, 5, -1);

            integMenu = new CDE90Menu(integTable);
            for (const CBoxConfigEntity& e : *flatList) {
                value = e.getValue().toInt();
                action = integMenu->addAction(e.getText());
                connect(action, &QAction::triggered, this, [value, this]() {
                    doSelectFlat(integTable, value);
                });
            }
            integTableMenuHelper->setMenu(integMenu, 6, -1);

            integMenu = new CDE90Menu(integTable);
            for (const CBoxConfigEntity& e : *dcList) {
                value = e.getValue().toInt();
                action = integMenu->addAction(e.getText());
                connect(action, &QAction::triggered, this, [value, this]() {
                    doSelectDC(integTable, value);
                });
            }
            integTableMenuHelper->setMenu(integMenu, 7, -1);
        });
}


void ImageOperationViewSetterController::doSelectScan(
    CDE90TableView* table, int value) {
    int index = table->getLimitSelectedRow();
    if (index < 0) {
        return;
    }
    CboxConfigService* service = ServiceManager::defaultService<CboxConfigService>();
    QPointer<ViewScanDataMode> dataModel = dynamic_cast<ViewScanDataMode*>(table->model());
    ScanModeTableData& data = dataModel->getDataList()[index];
    data.data.setScanRate(value);
    CBoxMixKey key(CBOX_TYPE_SEM_IMG_SCAN_RATE, value);
    bool cached;
    data.scanRateText = service->getText(key, &cached);
    if (!cached) {
        data.scanRateText = "Loading...";
        ACaller->push(this, [service, key, index, dataModel](TSC) {
            QString text;
            if (service->cacheText(key)) {
                text = "Error";
            } else {
                text = service->getText(key);
            }
            MainThreadInvoker::getInvoker().runLater(
                [index, dataModel, text, key]() {
                    if (!dataModel) return;
                    ScanModeTableData& data = dataModel->getDataList()[index];
                    if (data.data.getScanRate() != key.getValue().toInt()) return;
                    data.scanRateText = text;
                    dataModel->notifyDataChanged(index, index);
                });
        });
    }
    dataModel->notifyDataChanged(index, index);
}

void ImageOperationViewSetterController::doSelectFrame(
    CDE90TableView* table, int value) {
    int index = table->getLimitSelectedRow();
    if (index < 0) {
        return;
    }
    QPointer<ViewScanDataMode> dataModel = dynamic_cast<ViewScanDataMode*>(table->model());
    ScanModeTableData& data = dataModel->getDataList()[index];
    data.data.setFrame(value);
    dataModel->notifyDataChanged(index, index);
}
void ImageOperationViewSetterController::doSelectPixelX(
    CDE90TableView* table, int value) {
    int index = table->getLimitSelectedRow();
    if (index < 0) {
        return;
    }
    QPointer<ViewScanDataMode> dataModel = dynamic_cast<ViewScanDataMode*>(table->model());
    ScanModeTableData& data = dataModel->getDataList()[index];
    data.data.setPixelX(value);
    dataModel->notifyDataChanged(index, index);
}
void ImageOperationViewSetterController::doSelectPixelY(
    CDE90TableView* table, int value) {
    int index = table->getLimitSelectedRow();
    if (index < 0) {
        return;
    }
    QPointer<ViewScanDataMode> dataModel = dynamic_cast<ViewScanDataMode*>(table->model());
    ScanModeTableData& data = dataModel->getDataList()[index];
    data.data.setPixelY(value);
    dataModel->notifyDataChanged(index, index);
}
void ImageOperationViewSetterController::doSelectFilter(
    CDE90TableView* table, int value) {
    int index = table->getLimitSelectedRow();
    if (index < 0) {
        return;
    }
    CboxConfigService* service = ServiceManager::defaultService<CboxConfigService>();
    QPointer<ViewScanDataMode> dataModel = dynamic_cast<ViewScanDataMode*>(table->model());
    ScanModeTableData& data = dataModel->getDataList()[index];
    data.data.setFilterType(value);
    CBoxMixKey key(CBOX_TYPE_SEM_IMG_FILTER, value);
    bool cached;
    data.filterText = service->getText(key, &cached);
    if (!cached) {
        data.filterText = "Loading...";
        ACaller->push(this, [service, key, index, dataModel](TSC) {
            QString text;
            if (service->cacheText(key)) {
                text = "Error";
            } else {
                text = service->getText(key);
            }
            MainThreadInvoker::getInvoker().runLater(
                [index, dataModel, text, key]() {
                    if (!dataModel) return;
                    ScanModeTableData& data = dataModel->getDataList()[index];
                    if (data.data.getFilterType() != key.getValue().toInt()) return;
                    data.filterText = text;
                    dataModel->notifyDataChanged(index, index);
                });
        });
    }
    dataModel->notifyDataChanged(index, index);
}
void ImageOperationViewSetterController::doSelectFlat(
    CDE90TableView* table, int value) {
    int index = table->getLimitSelectedRow();
    if (index < 0) {
        return;
    }
    CboxConfigService* service = ServiceManager::defaultService<CboxConfigService>();
    QPointer<ViewScanDataMode> dataModel = dynamic_cast<ViewScanDataMode*>(table->model());
    ScanModeTableData& data = dataModel->getDataList()[index];
    data.data.setFlat(value);
    CBoxMixKey key(CBOX_TYPE_SEM_IMG_FLAT, value);
    bool cached;
    data.flatText = service->getText(key, &cached);
    if (!cached) {
        data.flatText = "Loading...";
        ACaller->push(this, [service, key, index, dataModel](TSC) {
            QString text;
            if (service->cacheText(key)) {
                text = "Error";
            } else {
                text = service->getText(key);
            }
            MainThreadInvoker::getInvoker().runLater(
                [index, dataModel, text, key]() {
                    if (!dataModel) return;
                    ScanModeTableData& data = dataModel->getDataList()[index];
                    if (data.data.getFlat() != key.getValue().toInt()) return;
                    data.flatText = text;
                    dataModel->notifyDataChanged(index, index);
                });
        });
    }
    dataModel->notifyDataChanged(index, index);
}
void ImageOperationViewSetterController::doSelectDC(
    CDE90TableView* table, int value) {
    int index = table->getLimitSelectedRow();
    if (index < 0) {
        return;
    }
    CboxConfigService* service = ServiceManager::defaultService<CboxConfigService>();
    QPointer<ViewScanDataMode> dataModel = dynamic_cast<ViewScanDataMode*>(table->model());
    ScanModeTableData& data = dataModel->getDataList()[index];
    data.data.setDcType(value);
    CBoxMixKey key(CBOX_TYPE_SEM_IMG_DC, value);
    bool cached;
    data.dcText = service->getText(key, &cached);
    if (!cached) {
        data.dcText = "Loading...";
        ACaller->push(this, [service, key, index, dataModel](TSC) {
            QString text;
            if (service->cacheText(key)) {
                text = "Error";
            } else {
                text = service->getText(key);
            }
            MainThreadInvoker::getInvoker().runLater(
                [index, dataModel, text, key]() {
                    if (!dataModel) return;
                    ScanModeTableData& data = dataModel->getDataList()[index];
                    if (data.data.getDcType() != key.getValue().toInt()) return;
                    data.dcText = text;
                    dataModel->notifyDataChanged(index, index);
                });
        });
    }
    dataModel->notifyDataChanged(index, index);
}

int ImageOperationViewSetterController::localLoadDataFromDatabase() {
    SemScanModeConfigService* service = ServiceManager::instance()
                                            ->getDefaultService<SemScanModeConfigService>();
    int errCode;
    QList<ScanModeEntity> list = service->get(&errCode);
    if (errCode) {
        return errCode;
    }
    if (list.isEmpty()) {
        return 0;
    }
    QList<ScanModeDataModel> modelList;
    modelList.resize(list.size());
    qsizetype size = std::min(list.size(), qsizetype(6));
    for (int i = 0; i < size; ++i) {
        ScanModeEntity& entity = list[i];
        errCode = scanModeDataFromEntity(entity.getDetail(), modelList[i]);
        if (errCode) {
            return errCode;
        }
    }
    localCopyDataToTable(modelList);
    return 0;
}

int ImageOperationViewSetterController::localLoadDataFromGlobal() {
    localCopyDataToTable(ImageScanLiveModeHelper::get()->getGlobal());
    return 0;
}

void ImageOperationViewSetterController::saveData() {
    bool succ = ACaller->push(this, [this](TSC) {
        SemScanModeConfigService* service
            = ServiceManager::instance()->getDefaultService<SemScanModeConfigService>();
        QList<ScanModeEntity> list;
        list.resize(6);
        QList<ScanModeTableData>& viewList = viewDataMode->getDataList();
        QList<ScanModeTableData>& integList = integDataMode->getDataList();
        qsizetype size = std::min(list.size(), viewList.size() + integList.size());
        QString detail;
        for (int i = 0; i < size; ++i) {
            if (i < 2) {
                scanModeDataToEntity(detail, viewList[i].data);
                list[i].setId(viewList[i].data.getId());
            } else {
                scanModeDataToEntity(detail, integList[i - 2].data);
                list[i].setId(integList[i - 2].data.getId());
            }
            list[i].setDetail(detail);
        }
        int errCode = service->saveWithTransaction(list);
        if (errCode) {
            simpleMessageBox(this, "Error", "Error to save scan data.");
        } else {
            simpleMessageBox(this, "Info", "Save successfully.");
        }
    });
    if (succ) {
        loadingBox(this, "Working", "Saving...");
    }
}

void ImageOperationViewSetterController::localCopyDataToTable(const QList<ScanModeDataModel>& modelList) {
    QList<ScanModeTableData>& viewList = viewDataMode->getDataList();
    QList<ScanModeTableData>& integList = integDataMode->getDataList();
    viewList.resize(2);
    integList.resize(4);
    for (int i = 0; i < modelList.size(); ++ i) {
        if (i < 2) {
            viewList[i] = ScanModeTableData{modelList[i], "Loading"};
        } else {
            integList[i - 2] = ScanModeTableData{modelList[i], "Loading"};
        }
    }
    viewDataMode->resizeViewList(2);
    integDataMode->resizeViewList(4);
    localRefreshView(viewList);
    localRefreshView(integList);
    viewDataMode->updateView();
    integDataMode->updateView();
}

void ImageOperationViewSetterController::localCopyTableDataToGlobal() {
    QList<ScanModeDataModel> list;
    list.reserve(6);
    for (ScanModeTableData& data: viewDataMode->getDataList()) {
        list.append(data.data);
    }
    for (ScanModeTableData& data: integDataMode->getDataList()) {
        list.append(data.data);
    }
    ImageScanLiveModeHelper::get()->setGlobal(list);
}

void ImageOperationViewSetterController::initData() {
    bool succ = ACaller->push(this, [this](TSC){
        SemScanModeConfigService* service = ServiceManager::defaultService<SemScanModeConfigService>();
        if (service->initGlobal()) {
            MainThreadInvoker::getInvoker().runLater([this](){
                simpleMessageBox(this, "Error", "Error to load scan params.");
                close();
            });
        }
        localLoadDataFromGlobal();
    });
    if (succ) {
        loadingBox(this, "Working", "Loading...");
    }
}
