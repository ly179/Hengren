#include "history_view_controller.h"
#include "ui_history_view_controller.h"
#include "view_data_mode.h"
#include <QDateTime>
#include "cde90_message_box.h"
#include "cde90_process_box.h"
#include "service_manager.h"
#include "idw_wafer_service.h"
#include "idp_data_service.h"
#include "recipe_data_service.h"

struct HistoryVersionData {
    QString deviceName;
    QDateTime createTime;
    quint64 id;
    int version;
};
class HistoryVersionDataModel : public SimpleRowTableModel<HistoryVersionData>{
private:
    QVariant getItemData(qint32 row, qint32 column) const {
        if (row < 0) return QVariant();
        const QList<HistoryVersionData>& list = getDataList();
        if (row >= list.size()) {
            return QVariant();
        }
        const HistoryVersionData& data = list[row];
        switch (column) {
        case 0:
            return data.version;
        case 1:
            return data.createTime.toString("yyyy/MM/dd HH:mm:ss");
        case 2:
            return data.deviceName;
        default:
            return QVariant();
        }
    };

public:
    explicit HistoryVersionDataModel(QObject* parent = nullptr) : SimpleRowTableModel(parent) {}
};
struct HistoryViewDataHelper {
    HistoryVersionDataModel* currentDataModel;
    HistoryVersionDataModel* historyDataModel;
    HistoryVersionDataType dataType;
};

HistoryViewController::HistoryViewController(QWidget* parent) : CDE90Window(parent) {
    //如果你没有异步任务需求，请将connect清理掉，保留create和createAction函数
    connect(this, &HistoryViewController::asyncCallerChanged, this, [this](){
        dataHelper = new HistoryViewDataHelper;
        create();
        createAction();
    }, Qt::SingleShotConnection);
}

HistoryViewController::~HistoryViewController() noexcept {
    if (ui) {
        delete ui;
    }
    if (dataHelper) delete dataHelper;
}

void HistoryViewController::create() {
    QWidget* content = new QWidget;
    setContent(content);
    ui = new cdsem::UiHistoryViewController;
    ui->init(content);
}

void HistoryViewController::createAction() {
    bindCloseBtn(ui->closeBtn->getQPushButton());
    dataHelper->currentDataModel = new HistoryVersionDataModel(ui->currentVersionTable);
    dataHelper->currentDataModel->insertHead("Version");
    dataHelper->currentDataModel->insertHead("Create Time");
    dataHelper->currentDataModel->insertHead("Device");
    ui->currentVersionTable->replaceDataModel(dataHelper->currentDataModel);

    dataHelper->historyDataModel = new HistoryVersionDataModel(ui->historyTable);
    dataHelper->historyDataModel->insertHead("Version");
    dataHelper->historyDataModel->insertHead("Create Time");
    dataHelper->historyDataModel->insertHead("Device");
    ui->historyTable->replaceDataModel(dataHelper->historyDataModel);

    connect(ui->currEditBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        int row = ui->currentVersionTable->getSelectedRow();
        if (row < 0) return;
        HistoryVersionData& data = dataHelper->currentDataModel->getDataList()[row];
        emit openEditor(dataHelper->dataType, data.id, true);
    });
    connect(ui->historyEditBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        int row = ui->historyTable->getSelectedRow();
        if (row < 0) return;
        HistoryVersionData& data = dataHelper->historyDataModel->getDataList()[row];
        emit openEditor(dataHelper->dataType, data.id, false);
    });
}

void HistoryViewController::localLoadIDW(quint64 id) {
    IDW_WaferService* service = ServiceManager::defaultService<IDW_WaferService>();
    int errCode;
    IDW_WaferInfoEntity entity = service->getSimpleInfoById(id, &errCode);
    if (errCode) {
        simpleMessageBox(this, "Error", "Error to load IDW data.");
        return;
    }
    if (entity.getId() == 0) {
        return;
    }
    IDW_WaferSearchDTO param;
    param.setTopId(entity.getTopId());
    QList<IDW_WaferInfoEntity> list = service->searchList(param, &errCode);
    if (errCode) {
        simpleMessageBox(this, "Error", "Error to load IDW list.");
        return;
    }
    if (list.isEmpty()) {
        return;
    }
    QList<HistoryVersionData> historyList;
    QList<HistoryVersionData> currentList;
    historyList.reserve(list.size() - 1);
    currentList.reserve(1);
    HistoryVersionData resultData;
    QString fileName;
    for (IDW_WaferInfoEntity& lEntity : list) {
        resultData.version = lEntity.getVersion();
        resultData.createTime = lEntity.getCreateTime();
        resultData.id = lEntity.getId();
        if (lEntity.getInvalid() == 0) {
            currentList.append(resultData);
            fileName = lEntity.getWaferName();
        } else {
            historyList.append(resultData);
        }
    }
    dataHelper->currentDataModel->insertDatas(currentList);
    dataHelper->historyDataModel->insertDatas(historyList);
    MainThreadInvoker::getInvoker().blockRun([this, fileName](){
        ui->fileNameTextField->setText(fileName);
    });
}

void HistoryViewController::localLoadIDP(quint64 id) {
    IdpDataService* service = ServiceManager::defaultService<IdpDataService>();
    int errCode;
    IdpDataEntity entity = service->getSimpleInfo(id, &errCode);
    if (errCode) {
        simpleMessageBox(this, "Error", "Error to load IDP data.");
        return;
    }
    if (entity.getId() == 0) {
        return;
    }
    IdpSearchDTO param;
    param.setTopId(entity.getTopId());
    QList<IdpDataEntity> list = service->searchList(param, &errCode);
    if (errCode) {
        simpleMessageBox(this, "Error", "Error to load IDP list.");
        return;
    }
    if (list.isEmpty()) {
        return;
    }
    QList<HistoryVersionData> historyList;
    QList<HistoryVersionData> currentList;
    historyList.reserve(list.size() - 1);
    currentList.reserve(1);
    HistoryVersionData resultData;
    QString fileName;
    for (IdpDataEntity& lEntity : list) {
        resultData.version = lEntity.getVersion();
        resultData.createTime = lEntity.getCreateTime();
        resultData.id = lEntity.getId();
        if (lEntity.getInvalid() == 0) {
            currentList.append(resultData);
            fileName = lEntity.getIdpName();
        } else {
            historyList.append(resultData);
        }
    }
    dataHelper->currentDataModel->insertDatas(currentList);
    dataHelper->historyDataModel->insertDatas(historyList);
    MainThreadInvoker::getInvoker().blockRun([this, fileName](){
        ui->fileNameTextField->setText(fileName);
    });
}
void HistoryViewController::localLoadRecipe(quint64 id) {
    RecipeDataService* service = ServiceManager::defaultService<RecipeDataService>();
    int errCode;
    RecipeDataEntity entity = service->getSimpleInfo(id, &errCode);
    if (errCode) {
        simpleMessageBox(this, "Error", "Error to load Recipe data.");
        return;
    }
    if (entity.getId() == 0) {
        return;
    }
    RecipeSearchDTO param;
    param.setTopId(entity.getTopId());
    QList<RecipeDataEntity> list = service->searchList(param, &errCode);
    if (errCode) {
        simpleMessageBox(this, "Error", "Error to load Recipe list.");
        return;
    }
    if (list.isEmpty()) {
        return;
    }
    QList<HistoryVersionData> historyList;
    QList<HistoryVersionData> currentList;
    historyList.reserve(list.size() - 1);
    currentList.reserve(1);
    HistoryVersionData resultData;
    QString fileName;
    for (RecipeDataEntity& lEntity : list) {
        resultData.version = lEntity.getVersion();
        resultData.createTime = lEntity.getCreateTime();
        resultData.id = lEntity.getId();
        if (lEntity.getInvalid() == 0) {
            currentList.append(resultData);
            fileName = lEntity.getRecipeName();
        } else {
            historyList.append(resultData);
        }
    }
    dataHelper->currentDataModel->insertDatas(currentList);
    dataHelper->historyDataModel->insertDatas(historyList);
    MainThreadInvoker::getInvoker().blockRun([this, fileName](){
        ui->fileNameTextField->setText(fileName);
    });
}

void HistoryViewController::loadData(quint64 id, HistoryVersionDataType dataType) {
    dataHelper->dataType = dataType;
    dataHelper->currentDataModel->clear();
    dataHelper->historyDataModel->clear();
    if (dataType == HVDT_IDW) {
        bool succ = ACaller->push(this, [this, id](TSC){
            localLoadIDW(id);
        });
        if (succ) {
            loadingBox(this, "Working", "Now IDW's history list is loading...");
        }
    } else if (dataType == HVDT_IDP) {
        bool succ = ACaller->push(this, [this, id](TSC){
            localLoadIDP(id);
        });
        if (succ) {
            loadingBox(this, "Loading", "Now IDP's history list is loading...");
        }
    } else if (dataType == HVDT_RECIPE) {
        bool succ = ACaller->push(this, [this, id](TSC){
            localLoadRecipe(id);
        });
        if (succ) {
            loadingBox(this, "Loading", "Now Recipe's history list is loading...");
        }
    }
}

HistoryVersionDataType HistoryViewController::getLastDataType() const {
    return dataHelper->dataType;
}

void HistoryViewController::enableEditBtn(bool b) {
    ui->historyEditBtn->setEnabled(b);
    ui->currEditBtn->setEnabled(b);
}
