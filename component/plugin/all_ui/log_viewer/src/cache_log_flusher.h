//
// Created by Shisan on 2026/3/18.
//

#ifndef HENGREN_CD_SEM_CACHE_LOG_FLUSHER_H
#define HENGREN_CD_SEM_CACHE_LOG_FLUSHER_H

#include "log_flusher.h"
#include "log_view_model.h"
#include <QColor>

namespace cache_log_viewer {
    struct ColorConfig {
        QColor color;
        int start;
        int len;
    };
    struct LogLineData {
        QList<ColorConfig> configs;
        logview::LogViewLevelEnum level = logview::LVLE_ALL;
        bool init = false;
    };
    struct LogContent {
        LogLineData formatData;
        QString text;
    };
};

struct LogViewSearchResult {
    QList<std::pair<int, int>> sames;
    cache_log_viewer::LogContent logData;
};
using LogViewLineCallback = std::function<void(const LogViewSearchResult&)>;
//param1: true-append false-reload
//param2: true-begin false-end
//当param2=false时，不需要检查param1
using LogViewFetchCallback = std::function<void(bool, bool)>;
class QRegularExpression;

struct CacheLogFlusherDataHelper;
class CacheLogFlusher : public LogFlusher {
private:
    QScopedPointer<CacheLogFlusherDataHelper> dataHelper;
public:
    CacheLogFlusher();
    ~CacheLogFlusher() override;
    void flushData(const MessageLogContextData&, const QDateTime&, const QtMsgType& , const QString&) override;
    void flush() override;
    void close() override;

    //到达最大值开始清理
    void setMaxRecords(int);
    //清理后保留多少条
    void setKeepRecords(int);

    void setContentCallback(const LogViewLineCallback&);
    void setFetchCallback(const LogViewFetchCallback&);
    void filter(const QString& text, Qt::CaseSensitivity, bool doCallback = true);
    void filter(const QRegularExpression& reg, Qt::CaseSensitivity, bool doCallback = true);
    void clearFilter(bool doCallback = true);

    void strictLevel(bool = true);

    void setColor(int group, const QColor&);
    void setColor(logview::LogViewLevelEnum, int group, const QColor&);
    void setColorReg(const QString&);

    void formatHightLight(cache_log_viewer::LogContent&);
};


#endif //HENGREN_CD_SEM_CACHE_LOG_FLUSHER_H