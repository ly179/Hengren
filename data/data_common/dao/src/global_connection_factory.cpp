#include "global_connection_factory.h"
#include "sql_connection_property_reader.h"
#include "base_connection_factory.h"
#include "core_connection_factory.h"

GlobalConnectionFactory::GlobalConnectionFactory() {
    QHash<QString, SqlDatabaseConfigProperties> configs
        = readDatabaseConfig(QList<QString>()<< "auth" << "log");
    this->authSqlConnectionFactory.reset(new SqlConnectionFactory(configs["auth"]));
    this->logSqlConnectionFactory.reset(new SqlConnectionFactory(configs["log"]));
}
GlobalConnectionFactory::~GlobalConnectionFactory() noexcept {
    this->authSqlConnectionFactory.clear();
    this->logSqlConnectionFactory.clear();
}

QSharedPointer<SqlConnectionFactory> GlobalConnectionFactory::getDefaultSqlConnectionFactory() const {
    return BaseConnectionFactory::instance()->getDefaultSqlConnectionFactory();
}

QSharedPointer<SqlConnectionFactory> GlobalConnectionFactory::getFileSqlConnectionFactory() const {
    return CoreConnectionFactory::instance()->getFileSqlConnectionFactory();
}

QSharedPointer<SqlConnectionFactory> GlobalConnectionFactory::getAuthSqlConnectionFactory() const {
    return authSqlConnectionFactory;
}

QSharedPointer<SqlConnectionFactory> GlobalConnectionFactory::getLogSqlConnectionFactory() const {
    return logSqlConnectionFactory;
}
