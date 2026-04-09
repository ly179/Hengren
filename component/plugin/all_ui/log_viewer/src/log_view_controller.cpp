#include "log_view_controller.h"
#include "ui_log_view_controller.h"
#include "log_config_helper.h"
#include <QHBoxLayout>
#include <QRegularExpression>

#include "highlight_log_textarea.h"
#include "cache_log_flusher.h"
#include "log_history_view_controller.h"
#include "log_helper.h"
#include "log_content_view_controller.h"

struct LogViewControllerDataHelper {
    std::shared_ptr<CacheLogFlusher> flusher;
    LogViewController* d;
    LogContentViewController* logContent;

    void createTextPlainEdit(CacheLogFlusher* flusher) {
        HighlightLogTextarea* textarea = new HighlightLogTextarea(flusher, nullptr);
        textarea->setWordWrapMode(QTextOption::NoWrap);
        textarea->setReadOnly(false);
        logContent->setLogView(textarea);
        flusher->clearFilter(true);
    }


    void initLogFlushers() {
        QHash<logview::LogViewLevelEnum, std::shared_ptr<CacheLogFlusher>> flushers;
        logview::readAndCreateLogConfig(flushers);

        if (flushers.size() > 0) {
            auto it = flushers.find(logview::LVLE_ALL);
            if (it != flushers.end()) {
                flusher = *it;
                createTextPlainEdit(it->get());
            }
        }
    }

    void deinitFlushers() {
        //不再从flusher 实例中移除
        // if (flushers.isEmpty()) return;
        // for (auto it = flushers.begin(); it != flushers.end(); ++ it) {
        //     QString str = (*it)->getName();
        //     LogHelper::removeInstance(str);
        // }
    }
};

LogViewController::LogViewController(QWidget* parent) : RelationLogViewerController(parent), dataHelper(new LogViewControllerDataHelper) {
    dataHelper->d = this;
    connect(this, &CDE90Window::asyncCallerChanged, this, [this](){
        create();
        createAction();
    });
}

LogViewController::~LogViewController() noexcept {
    if (dataHelper) dataHelper->deinitFlushers();
    if (ui) {
        delete ui;
    }
}

void LogViewController::create() {
    QWidget* content = new QWidget;
    setContent(content);
    ui = new cdsem::UiLogViewController;
    ui->init(content);
    dataHelper->logContent = new LogContentViewController(this);
    ui->logContent->addTab(dataHelper->logContent, "Watch");
}

void LogViewController::createAction() {
    connect(ui->exitAct, &QAction::triggered, this, [this](){close();});
    dataHelper->initLogFlushers();
    historyController = new LogHistoryViewController(ui->logContent);
    itsChildAndBeManaged(historyController, this);
    historyController->initAsyncCaller(ACaller);
    ui->logContent->addTab(historyController, "History");

    connect(dataHelper->logContent, &LogContentViewController::doFilter, this, [this](const QString& text, bool enableCase, bool enableRegular) {
        if (dataHelper->flusher) {
            if (text.isEmpty()) {
                dataHelper->flusher->clearFilter(true);
            } else if (enableRegular) {
                dataHelper->logContent->pushFilterHistory(text);
                dataHelper->flusher->filter(QRegularExpression(text), enableCase ? Qt::CaseSensitive : Qt::CaseInsensitive, true);
            } else {
                dataHelper->logContent->pushFilterHistory(text);
                dataHelper->flusher->filter(text, enableCase ? Qt::CaseSensitive : Qt::CaseInsensitive, true);
            }
        }
    });

    connect(dataHelper->logContent, &LogContentViewController::doSearch, dataHelper->logContent, &LogContentViewController::searchText);
}