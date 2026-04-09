#ifndef LOG_CONTENT_FORMAT_H
#define LOG_CONTENT_FORMAT_H

#include "log_content_struct.h"

class LogContentParser {

public:
    LogContentParser() = default;
    LogContentStruct parseLogLine(const QString& content, bool *ok = nullptr);
};

#endif // LOG_CONTENT_FORMAT_H
