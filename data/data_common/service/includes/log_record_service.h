#ifndef LOG_RECORD_SERVICE_H
#define LOG_RECORD_SERVICE_H
#include "general_database_service_global.h"
#include "iservice.h"
#include "log_record_entity.h"

class LogRecordDao;

class SEM_SERVICE_EXPORT LogRecordService : public IService{
private:
    LogRecordDao* logRecordDao;
public:
    LogRecordService();
    ~LogRecordService() noexcept;

    LogRecordSearchResult searchLogs(const LogRecordSearchDTO&, int* errCode = nullptr);
};

#endif // LOG_RECORD_SERVICE_H
