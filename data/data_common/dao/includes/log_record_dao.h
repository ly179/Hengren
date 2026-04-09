#ifndef LOG_RECORD_DAO_H
#define LOG_RECORD_DAO_H
#include "idao.h"
#include "log_record_entity.h"
#include "general_database_dao_global.h"

class SqlConnectionFactory;
class SEM_DAO_EXPORT LogRecordDao : public IDao{
private:
    QSharedPointer<SqlConnectionFactory> getConnectionFactory() override;

public:
    LogRecordSearchResult searchLogs(const LogRecordSearchDTO&, int* errCode = nullptr);
};

#endif // LOG_RECORD_DAO_H
