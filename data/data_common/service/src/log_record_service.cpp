#include "log_record_service.h"
#include "log_record_dao.h"
#include "dao_manager.h"

LogRecordService::LogRecordService() {
    logRecordDao = DaoManager::instance()->getLogFactoryDao<LogRecordDao>();
}

LogRecordService::~LogRecordService() noexcept {}

LogRecordSearchResult LogRecordService::searchLogs(const LogRecordSearchDTO& param, int* errCode) {
    return logRecordDao->searchLogs(param, errCode);
}
