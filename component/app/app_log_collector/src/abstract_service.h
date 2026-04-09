#ifndef ABSTRACT_SERVICE_H
#define ABSTRACT_SERVICE_H

#include "idao.h"
#include "sql_connection_factory.h"

class AbstractService : public IDao {
protected:
    virtual QSharedPointer<SqlConnectionFactory> getConnectionFactory() override;
};

#endif // ABSTRACT_SERVICE_H
