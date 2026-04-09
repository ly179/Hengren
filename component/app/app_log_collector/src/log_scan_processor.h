#ifndef LOG_SCAN_PROCESSOR_H
#define LOG_SCAN_PROCESSOR_H

#include "log_content_format.h"
#include "log_saver.h"

class LogRecordEntity;
class LogFileScanEntity;
class LogScanProcessor {

private:
    LogContentParser contentParser;

    static QRegularExpression filePathReg;

    QString logDir;
    QString fileNameReg;
    qint64 waitTime = 1000;

    LogSaver* saver;

    bool doSave(QList<LogRecordEntity>&, LogFileScanEntity&, const QDate& date);

    //0 - success
    //1 - err occern
    //2 - path not exists
    int doRead(const QString& path, LogFileScanEntity& entity);

    void tailHandle();

public:
    LogScanProcessor();

    void start();
};

#endif // LOG_SCAN_PROCESSOR_H
