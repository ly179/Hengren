#ifndef LOG_FILE_READER_H
#define LOG_FILE_READER_H
#include <QString>
#include "functional"

using LogLineCallback = std::function<bool(const QString&, qint64, qint64)>;

class LogFileReader {

private:
    QString path;

    LogLineCallback callback;

public:
    explicit LogFileReader(const QString& path);

    void setLineCallback(const LogLineCallback& callback);

    qint64 beginRead(qint64 skip = 0);
};

#endif // LOG_FILE_READER_H
