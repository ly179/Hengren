#ifndef LOG_SAVER_H
#define LOG_SAVER_H
#include <QDateTime>
#include <QList>
#include "abstract_service.h"
#include "log_file_scan_entity.h"
#include "log_record_entity.h"

class LogSaver : public AbstractService {
public:

    int save(LogRecordEntity& line);

    int save(const QList<LogRecordEntity>& list, const QDate& date);

    int update(const LogRecordEntity& entity, const QDate& date);

    int generalTable(const QDate&);
    int generalTable(const QDateTime&);

    int saveFilePos(LogFileScanEntity&, const QDate&);
    LogFileScanEntity getFilePos(const QString& path, int* errCode = nullptr);

    QList<LogFileScanEntity> allFilePos(qint32* errCode = nullptr);

    int deleteFilePosRecord(quint64 id);
};

#endif // LOG_SAVER_H
