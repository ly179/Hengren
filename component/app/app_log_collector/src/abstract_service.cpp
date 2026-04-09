#include "abstract_service.h"
#include "global_connection_factory.h"

QSharedPointer<SqlConnectionFactory> AbstractService::getConnectionFactory() {
    return GlobalConnectionFactory::instance()->getLogSqlConnectionFactory();
}
