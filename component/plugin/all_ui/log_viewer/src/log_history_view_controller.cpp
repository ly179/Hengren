#include "log_history_view_controller.h"
#include "ui_log_history_view_controller.h"
#include "view_data_mode.h"
#include "sem_global.h"
#include "service_manager.h"
#include "log_record_service.h"
#include <QDateTime>
#include <QLayout>
#include "cde90_message_box.h"
#include "cde90_process_box.h"

class LogHistoryDataModel;
struct PageData {
    QDateTime startTime;
    quint64 startId = 0;
};
struct LogHistoryViewDataHelper {
    QList<PageData> pageList;
    LogHistoryDataModel* historyDataModel;
    //从0开始
    int currentPage = 0;
};

class LogHistoryDataModel : public SimpleRowTableModel<LogRecordEntity> {
private:
    QVariant data(
        const QModelIndex& index, int role = Qt::DisplayRole) const override {
        if (role == Qt::TextAlignmentRole) {
            return QVariant(Qt::Alignment(Qt::AlignLeft | Qt::AlignVCenter));
        } else if (role == Qt::ForegroundRole && index.column() == 0) {
            const QList<LogRecordEntity>& list = getDataList();
            if (index.row() >= 0 && index.row() < list.size()) {
                const QString& level = list[index.row()].getLogLevel();
                if (level == "debug") {
                    return QColor(0, 143, 35);
                } else if (level == "info") {
                    return QColor(0, 126, 176);
                } else if (level == "warn") {
                    return QColor(214, 180, 0);
                } else if (level == "critical") {
                    return QColor(214, 0, 0);
                }
            }
        }
        return SimpleRowTableModel::data(index, role);
    }
    QVariant getItemData(qint32 row, qint32 column) const override {
        if (row < 0) {
            return QVariant();
        }
        const QList<LogRecordEntity>& list = getDataList();
        if (row >= list.size()) {
            return QVariant();
        }
        const LogRecordEntity& entity = list[row];
        switch (column) {
        case 0:
            return entity.getLogLevel();
        case 1:
            return entity.getCatalog();
        case 2:
            return entity.getFileName();
        case 3:
            return entity.getFileLine();
        case 4:
            if (entity.getLogCreateTime().isValid())
                return entity.getLogCreateTime().toString("yyyy/MM/dd HH:mm:ss");
            return QVariant();
        case 5:
            return entity.getLogContent();
        default:
            return QVariant();
        }
    }
public:
    LogHistoryDataModel(QObject* parent = nullptr) : SimpleRowTableModel(parent) {};
    ~LogHistoryDataModel() noexcept {}
};

LogHistoryViewController::LogHistoryViewController(QWidget* parent) : QWidget(parent) {
    dataHelper = new LogHistoryViewDataHelper;
    create();
    createAction();
}

LogHistoryViewController::~LogHistoryViewController() noexcept {
    if (ui) {
        delete ui;
    }
    delete dataHelper;
}

void LogHistoryViewController::create() {
    ui = new cdsem::UiLogHistoryViewController;
    ui->init(this);
}

void LogHistoryViewController::createAction() {
    dataHelper->historyDataModel = new LogHistoryDataModel(ui->contentTable);
    dataHelper->historyDataModel->insertHead("Level");
    dataHelper->historyDataModel->insertHead("Catalog");
    dataHelper->historyDataModel->insertHead("File");
    dataHelper->historyDataModel->insertHead("Line");
    dataHelper->historyDataModel->insertHead("DateTime");
    dataHelper->historyDataModel->insertHead("Content");
    ui->contentTable->replaceDataModel(dataHelper->historyDataModel);
    connect(ui->searchBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        dataHelper->pageList.clear();
        dataHelper->currentPage = 0;
        if (!caller) {
            return;
        }
        dataHelper->pageList.append({QDateTime(), 0});
        doLoad();
    });
    connect(ui->nextPageBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        if (dataHelper->currentPage < dataHelper->pageList.size() - 1) {
            ++ dataHelper->currentPage;
            doLoad();
        }
    });
    connect(ui->prevPageBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        if (dataHelper->currentPage > 0) {
            -- dataHelper->currentPage;
            doLoad();
        }
    });
    connect(ui->reloadBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        doLoad();
    });
}

void LogHistoryViewController::doLoad() {
    if (dataHelper->currentPage >= dataHelper->pageList.size()) {
        return;
    }
    LogRecordSearchDTO param;
    param.setCatalog(ui->catTextField->text());
    param.setFile(ui->fileTextField->text());
    param.setContent(ui->contentTextField->text());
    param.setStartTime(ui->startDateTime->getDateTimeEdit()->dateTime());
    param.setEndTime(ui->endDateTime->getDateTimeEdit()->dateTime());
    PageData& pageData = dataHelper->pageList[dataHelper->currentPage];
    param.setPageId(pageData.startId);
    param.setPageTime(pageData.startTime);
    {
        int pageSizeIndex = ui->pageSizeItemBox->currentIndex();
        param.setLimit(50);
        if (pageSizeIndex == 1) {
            param.setLimit(100);
        } else if (pageSizeIndex == 2) {
            param.setLimit(200);
        }
    }
    {
        int levelIndex = ui->levelItemBox->currentIndex();
        if (levelIndex == 0) {
            param.setLevel("");
        } else if (levelIndex == 1) {
            param.setLevel("debug");
        } else if (levelIndex == 2) {
            param.setLevel("info");
        } else if (levelIndex == 3) {
            param.setLevel("warn");
        } else if (levelIndex == 4) {
            param.setLevel("critical");
        }
    }
    bool succ = caller->push(this, [this, param](TSC cmd){
        localSearch(param);
    });
    if (succ) {
        loadingBox(this, "Info", "Loading...");
    }
}

void LogHistoryViewController::localSearch(const LogRecordSearchDTO& param) {
    LogRecordService* service = ServiceManager::defaultService<LogRecordService>();
    int errCode;
    LogRecordSearchResult result = service->searchLogs(param, &errCode);
    if (errCode) {
        simpleMessageBox(this, "Error", "Error to load history log");
        return;
    }
    MainThreadInvoker::getInvoker().blockRun([this, &result](){
        if (result.startId > 0 && result.startTime.isValid() && dataHelper->currentPage == dataHelper->pageList.size() - 1) {
            dataHelper->pageList.append({result.startTime, result.startId});
        }
        dataHelper->historyDataModel->clear();
        dataHelper->historyDataModel->insertDatas(result.list);
    });
}

void LogHistoryViewController::initAsyncCaller(AsyncCaller* caller) {
    if (this->caller) {
        return;
    }
    this->caller = caller;
}

AsyncCaller* LogHistoryViewController::getAsyncCaller() {
    return caller;
}
