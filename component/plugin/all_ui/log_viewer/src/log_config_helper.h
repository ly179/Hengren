#ifndef LOG_CONFIG_HELPER_H
#define LOG_CONFIG_HELPER_H

#include "log_view_model.h"
class CacheLogFlusher;
namespace logview{
void readAndCreateLogConfig(QHash<logview::LogViewLevelEnum, std::shared_ptr<CacheLogFlusher>>& flushers, const QString& logConfigPath = "");
}

#endif // LOG_CONFIG_HELPER_H
