#include "log_view_model.h"
#include <QHash>
#include "mutex"

const QString logview::ALL_LEVEL = "all";
const QString logview::DEBUG_LEVEL = "debug";
const QString logview::INFO_LEVEL = "info";
const QString logview::WARN_LEVEL = "warn";
const QString logview::ERROR_LEVEL = "error";

std::once_flag flag;
QHash<QString, logview::LogViewLevelEnum> nameToEnum;
QHash<logview::LogViewLevelEnum, QString> enumToName;
void initData() {
    enumToName.insert(logview::LVLE_ALL, logview::ALL_LEVEL);
    enumToName.insert(logview::LVLE_DEBUG, logview::DEBUG_LEVEL);
    enumToName.insert(logview::LVLE_INFO, logview::INFO_LEVEL);
    enumToName.insert(logview::LVLE_WARN, logview::WARN_LEVEL);
    enumToName.insert(logview::LVLE_ERROR, logview::ERROR_LEVEL);

    nameToEnum.insert(logview::ALL_LEVEL, logview::LVLE_ALL);
    nameToEnum.insert(logview::DEBUG_LEVEL, logview::LVLE_DEBUG);
    nameToEnum.insert(logview::INFO_LEVEL, logview::LVLE_INFO);
    nameToEnum.insert(logview::WARN_LEVEL, logview::LVLE_WARN);
    nameToEnum.insert(logview::ERROR_LEVEL, logview::LVLE_ERROR);
}

bool logview::isLegalLevel(const QString& name) {
    std::call_once(flag, initData);
    return nameToEnum.contains(name);
}

bool logview::isLegalLevel(LogViewLevelEnum level) {
    std::call_once(flag, initData);
    return enumToName.contains(level);
}

QString logview::getLevelName(logview::LogViewLevelEnum e) {
    std::call_once(flag, initData);
    return enumToName.value(e, "");
}

logview::LogViewLevelEnum logview::getLevelEnum(const QString& name, bool* found) {
    std::call_once(flag, initData);
    auto it = nameToEnum.find(name);
    if (it == nameToEnum.end()) {
        if (found) *found = false;
        return logview::LVLE_ALL;
    }
    if (found) *found = true;
    return *it;
}
