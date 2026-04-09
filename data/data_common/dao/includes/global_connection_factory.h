#ifndef GLOBAL_CONNECTION_FACTORY_H
#define GLOBAL_CONNECTION_FACTORY_H

#include "general_database_dao_global.h"
#include "sem_singleton_template.h"
#include "sql_connection_factory.h"

class SEM_DAO_EXPORT GlobalConnectionFactory : public Singleton<GlobalConnectionFactory> {
    friend class Singleton<GlobalConnectionFactory>;

private:
    QSharedPointer<SqlConnectionFactory> authSqlConnectionFactory;
    QSharedPointer<SqlConnectionFactory> logSqlConnectionFactory;
    GlobalConnectionFactory();

public:
    ~GlobalConnectionFactory() noexcept;

    QSharedPointer<SqlConnectionFactory> getDefaultSqlConnectionFactory() const;
    QSharedPointer<SqlConnectionFactory> getFileSqlConnectionFactory() const;
    QSharedPointer<SqlConnectionFactory> getAuthSqlConnectionFactory() const;
    QSharedPointer<SqlConnectionFactory> getLogSqlConnectionFactory() const;
};
#endif // GLOBAL_CONNECTION_FACTORY_H
