#ifndef LOG_VIEW_MODEL_H
#define LOG_VIEW_MODEL_H

#include <QString>

namespace logview {

enum LogViewLevelEnum {
    LVLE_ALL,
    LVLE_DEBUG,
    LVLE_INFO,
    LVLE_WARN,
    LVLE_ERROR
};

extern const QString ALL_LEVEL;
extern const QString DEBUG_LEVEL;
extern const QString INFO_LEVEL;
extern const QString WARN_LEVEL;
extern const QString ERROR_LEVEL;

bool isLegalLevel(const QString&);
bool isLegalLevel(LogViewLevelEnum);
QString getLevelName(LogViewLevelEnum);
//如果找不到，found将为false
LogViewLevelEnum getLevelEnum(const QString&, bool* found = nullptr);
};

#endif // LOG_VIEW_MODEL_H
