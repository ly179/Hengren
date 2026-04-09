#ifndef LOG_SCANNER_H
#define LOG_SCANNER_H
#include "functional"
#include <QRegularExpression>

using LogScannerFileCallback = std::function<void(const QString&)>;
class LogScanner {
private:
    LogScannerFileCallback callback;

    QString logDir;
    QRegularExpression fileNameReg;
    qint64 waitTime = 1000;

public:
    LogScanner(const QString& logDir, const QString& fileNameReg, qint64 waitTime);

    void setCallback(const LogScannerFileCallback&);

    void beginScan();

};

#endif // LOG_SCANNER_H
