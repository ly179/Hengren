#include "log_config_helper.h"
#include "log_util.h"
#include "sem_file_reader.h"
#include "log_flusher.h"
#include "log_view_model.h"
#include "cache_log_flusher.h"
#include "log_helper.h"
#include <QColor>
#include <QDebug>
#include "sem_common.h"
using namespace logview;

inline void setFlusherColor(CacheLogFlusher* flusher, int group, const QString& value) {
    flusher->setColor(group, value);
}

void logview::readAndCreateLogConfig(QHash<logview::LogViewLevelEnum, std::shared_ptr<CacheLogFlusher>>& flushers, const QString& logConfigPath) {
    QStringList lines;
    {
        QString configText;
        if (logConfigPath.isEmpty()) {
            configText = sem::readFileAsQString(":/configs/log_highlight_config.properties");
        } else {
            configText = sem::readFileAsQString(logConfigPath);
        }

        if (configText.isEmpty()) {
            return;
        }
        lines = configText.split("\n");
    }
    QList<std::shared_ptr<CacheLogFlusher>> flusherList;
    QtMsgType minLevel = QtInfoMsg;
    int kvSpliterIndex;
    QString key;
    QString value;
    QStringList notes;
    bool foundLevelEnum;
    bool groupOk;
    for (QString &line : lines) {
        line = line.trimmed();
        if (line.isEmpty() || line.startsWith("#") || line.startsWith(";")) {
            continue;
        }
        kvSpliterIndex = line.indexOf("=");
        key = line.sliced(0, kvSpliterIndex);
        value = line.sliced(kvSpliterIndex + 1).trimmed();
        notes = key.split(".");
        if (notes[0] != "log" || notes.size() < 4) {
            continue;
        }
        QString node1 = notes[1].trimmed();
        if (node1 != "logview") {
            continue;
        }
        QString level = notes[2].trimmed();
        if (level.isEmpty()) {
            continue;
        }
        logview::LogViewLevelEnum levelEnum = logview::getLevelEnum(level, &foundLevelEnum);
        //屏蔽levelEnum不为ALL的，即其他类型暂时作废
        if (!foundLevelEnum || levelEnum != logview::LVLE_ALL) {
            continue;
        }
        std::shared_ptr<CacheLogFlusher> flusher;
        auto it = flushers.find(levelEnum);
        QString flusherName = QString("logview::%1").arg(level);
        if (it == flushers.end()) {
            std::shared_ptr<LogFlusher> _flusher = LogHelper::getLogInstance(flusherName);
            if (_flusher) {
                flusher = std::dynamic_pointer_cast<CacheLogFlusher>(_flusher);
                if (flusher)
                    flushers.insert(levelEnum, flusher);
                continue;
            }
            if (!flusher) {
                flusher = std::make_shared<CacheLogFlusher>();
                flusher->setName(flusherName);
            }
            flusherList.append(flusher);
            flushers.insert(levelEnum, flusher);
            if (levelEnum != logview::LVLE_ALL) {
                flusher->strictLevel();
                flusher->setMinLevel(LogUtil::getLevelByStr(level));
            }
        } else {
            flusher = *it;
        }
        if (!flusher) {
            continue;
        }
        QString iKey = notes[3].trimmed();
        if (iKey == "format") {
            flusher->setFormat(value);
        } else if (iKey == "date-format") {
            flusher->setDateFormat(value);
        } else if (iKey == "color-reg"){
            flusher->setColorReg(value);
        } else {
            if (levelEnum == logview::LVLE_ALL && iKey == "min-level") {
                flusher->setMinLevel(LogUtil::getLevelByStr(value));
                continue;
            }
            if (notes.size() <= 4) {
                continue;
            }
            if (levelEnum == logview::LVLE_ALL) {
                if (notes.size() > 5) {
                    level = notes[3].trimmed();
                    levelEnum = logview::getLevelEnum(level, &foundLevelEnum);
                    if (!foundLevelEnum) {
                        continue;
                    }
                    iKey = notes[4].trimmed();
                    if (iKey != "color") {
                        continue;
                    }
                    iKey = notes[5].trimmed();
                    int group = iKey.toInt(&groupOk);
                    if (!groupOk) {
                        continue;
                    }
                    flusher->setColor(levelEnum, group, QColor::fromString(value));
                }
            } else if (iKey == "color") {
                iKey = notes[4].trimmed();
                int group = iKey.toInt(&groupOk);
                if (!groupOk) {
                    continue;
                }
                flusher->setColor(group, QColor::fromString(value));
            }
        }
    }
    for (std::shared_ptr<CacheLogFlusher>& flusher : flusherList) {
        if (flusher->getMinLevel() < 0) {
            flusher->setMinLevel(minLevel);
        }
        LogHelper::addLogInstance(flusher);
    }
}
