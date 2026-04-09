//
// Created by Shisan on 2026/3/18.
//

#include "cache_log_flusher.h"
#include "log_util.h"

namespace cache_log_viewer{
    enum SearchType {
        ST_NONE,
        ST_TEXT,
        ST_REG
    };
    struct SearchCondition {
        QString searchText;
        QRegularExpression searchExpression;
        Qt::CaseSensitivity caseSensitivity = Qt::CaseInsensitive;
        SearchType searchType = ST_NONE;
    };
}
using namespace cache_log_viewer;
using namespace logview;

struct CacheLogFlusherDataHelper {
    SearchCondition condition;
    std::vector<LogContent> records;
    LogViewLineCallback callback;
    LogViewFetchCallback f_callback;
    QHash<logview::LogViewLevelEnum, QHash<int, QColor>> colorConfigs;
    QRegularExpression colorReg;
    CasLock lock;
    int maxRecords = 10000;
    int keepRecords = 8000;
    //开启后只打印指定level的bug，而不是范围判定
    bool strictLevel = false;

    void formatHightLight(LogContent& logContent) {
        LogLineData& data = logContent.formatData;
        if (data.init) return;
        data.init = true;
        if (colorReg.isValid() && colorConfigs.size() > 0) {
            QRegularExpressionMatch match = colorReg.match(logContent.text);
            if (match.hasMatch()) {
                data.configs.reserve(match.lastCapturedIndex() + 1);
                ColorConfig config;
                for (int i = 0; i <= match.lastCapturedIndex(); ++ i) {
                    LogViewLevelEnum defaultLevel = logContent.formatData.level;
                    auto it = colorConfigs.find(defaultLevel);
                    if (it == colorConfigs.end()) {
                        if (defaultLevel == LVLE_ALL) {
                            continue;
                        }
                        defaultLevel = LVLE_ALL;
                        it = colorConfigs.find(defaultLevel);
                        if (it == colorConfigs.end()) {
                            continue;
                        }
                    }
                    auto cit = it->find(i);
                    if (cit == it->end()) {
                        if (defaultLevel == LVLE_ALL) {
                            continue;
                        }
                        defaultLevel = LVLE_ALL;
                        it = colorConfigs.find(defaultLevel);
                        if (it == colorConfigs.end()) {
                            continue;
                        }
                        cit = it->find(i);
                        if (cit == it->end()) {
                            continue;
                        }
                    }
                    config.color = *cit;
                    config.start = match.capturedStart(i);
                    config.len = match.capturedLength(i);
                    data.configs.append(config);
                }
            }
        }
    }

    //非懒加载逻辑
    // LogContent dataContent = LogContent {LogLineData {}, result, level};
    // dataHelper->formatHightLight(dataContent);
    //参数2：为了不频繁创建result，使用传入的result。
    void filterByCondition(LogContent& line, LogViewSearchResult& result) {
        if (!callback) return;
        if (condition.searchType == ST_NONE) {
            if (result.sames.size() > 0) result.sames.clear();
            formatHightLight(line);
            result.logData = line;
            callback(result);
            return;
        }
        if (condition.searchType == ST_TEXT) {
            int start = 0;
            if (result.sames.size() > 0) result.sames.clear();
            while (1) {
                start = line.text.indexOf(condition.searchText, start, condition.caseSensitivity);
                if (start == -1) break;
                result.sames.append(std::pair(start, condition.searchText.length()));
                start += condition.searchText.length();
            }
            if (result.sames.size() > 0) {
                formatHightLight(line);
                result.logData = line;
                callback(result);
            }
            return;
        }
        if (condition.searchType == ST_REG) {
            if (!condition.searchExpression.isValid()) return;
            QRegularExpressionMatch match = condition.searchExpression.match(line.text, condition.caseSensitivity);
            if (!match.hasMatch()) {
                return;
            }
            formatHightLight(line);
            result.logData = line;
            for (int i = 0; i <= match.lastCapturedIndex(); ++ i) {
                result.sames.append(std::pair<int, int>(match.capturedStart(i), match.capturedLength(i)));
            }
            callback(result);
        }
    }

    void eachAllLines() {
        CasLocker locker(lock);
        LogViewSearchResult result;
        if (f_callback) f_callback(false, true);
        for (auto it = records.begin(); it != records.end(); ++ it) {
            filterByCondition(*it, result);
        }
        if (f_callback) f_callback(false, false);
    }
};

CacheLogFlusher::CacheLogFlusher() : LogFlusher(), dataHelper(new CacheLogFlusherDataHelper) {
    dataHelper->records.reserve(dataHelper->maxRecords);
}

CacheLogFlusher::~CacheLogFlusher() {

}

void CacheLogFlusher::flushData(const MessageLogContextData& context, const QDateTime& datetime, const QtMsgType& msgType, const QString& content) {
    if (format.isEmpty()) {
        return;
    }
    if (dataHelper->strictLevel && msgType != minLevel) {
        return;
    }
    CasLocker locker(dataHelper->lock);
    QString result = formatMsg(context, datetime, msgType, content);
    result = LogUtil::replaceText(result);
    if (dataHelper->records.size() >= dataHelper->maxRecords) {
        dataHelper->records.erase(dataHelper->records.begin(), dataHelper->records.begin() + (dataHelper->maxRecords - dataHelper->keepRecords));
    }
    logview::LogViewLevelEnum level = logview::LVLE_ALL;
    if (msgType == QtDebugMsg) {
        level = logview::LVLE_DEBUG;
    } else if (msgType == QtInfoMsg) {
        level = logview::LVLE_INFO;
    }else if (msgType == QtWarningMsg) {
        level = logview::LVLE_WARN;
    } else if (msgType == QtCriticalMsg || msgType == QtFatalMsg) {
        level = logview::LVLE_ERROR;
    }
    LogLineData lineData;
    lineData.level = level;
    LogContent logData = LogContent {lineData, result};
    LogViewSearchResult sResult;
    if (dataHelper->f_callback) dataHelper->f_callback(true, true);
    dataHelper->filterByCondition(logData, sResult);
    if (dataHelper->f_callback) dataHelper->f_callback(true, false);
    dataHelper->records.push_back(logData);
}
void CacheLogFlusher::flush() {}
void CacheLogFlusher::close() {
    CasLocker locker(dataHelper->lock);
    dataHelper->records.clear();
}

void CacheLogFlusher::setMaxRecords(int max) {
    CasLocker locker(dataHelper->lock);
    dataHelper->maxRecords = max;
}

void CacheLogFlusher::setKeepRecords(int r) {
    CasLocker locker(dataHelper->lock);
    dataHelper->keepRecords = r;
}

void CacheLogFlusher::setContentCallback(const LogViewLineCallback& callback) {
    CasLocker locker(dataHelper->lock);
    dataHelper->callback = callback;
}

void CacheLogFlusher::setFetchCallback(const LogViewFetchCallback& callback) {
    CasLocker locker(dataHelper->lock);
    dataHelper->f_callback = callback;
}

void CacheLogFlusher::filter(const QString& text, Qt::CaseSensitivity c, bool doCallback) {
    {
        CasLocker locker(dataHelper->lock);
        dataHelper->condition.searchType = ST_TEXT;
        dataHelper->condition.caseSensitivity = c;
        dataHelper->condition.searchText = text;
    }
    if (doCallback)
        dataHelper->eachAllLines();
}

void CacheLogFlusher::filter(const QRegularExpression& reg, Qt::CaseSensitivity c, bool doCallback) {
    {
        CasLocker locker(dataHelper->lock);
        dataHelper->condition.searchType = ST_REG;
        dataHelper->condition.caseSensitivity = c;
        dataHelper->condition.searchExpression = reg;
    }
    if (doCallback)
        dataHelper->eachAllLines();
}

void CacheLogFlusher::clearFilter(bool doCallback) {
    {
        CasLocker locker(dataHelper->lock);
        dataHelper->condition.searchType = ST_NONE;
    }
    if (doCallback)
        dataHelper->eachAllLines();
}

void CacheLogFlusher::strictLevel(bool) {
    dataHelper->strictLevel = true;
}

void CacheLogFlusher::setColor(int group, const QColor& color) {
    setColor(logview::LVLE_ALL, group, color);
}

void CacheLogFlusher::setColor(logview::LogViewLevelEnum level, int group, const QColor& color) {
    dataHelper->colorConfigs[level].insert(group, color);
}

void CacheLogFlusher::setColorReg(const QString& reg) {
    dataHelper->colorReg = QRegularExpression(reg);
}

void CacheLogFlusher::formatHightLight(cache_log_viewer::LogContent& c) {
    dataHelper->formatHightLight(c);
}