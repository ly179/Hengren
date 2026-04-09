#include "transaction_manager_service.h"
#include "transaction_manager.h"
#include "global_connection_factory.h"

struct TransactionObjectDataHelper {
    Factory factory;
    bool succ = false;
};

TransactionObject::TransactionObject() {
    dataHelper = new TransactionObjectDataHelper;
}

TransactionObject::~TransactionObject() noexcept {
    delete dataHelper;
}

TransactionObject::TransactionObject(const TransactionObject& other) {
    *this = other;
}
TransactionObject& TransactionObject::operator=(const TransactionObject& other) {
    if (this == &other) return *this;
    dataHelper->factory = other.dataHelper->factory;
    dataHelper->succ = other.dataHelper->succ;
    return *this;
}

bool TransactionObject::commit() {
    if (!dataHelper->factory) return false;
    dataHelper->succ = false;
    bool succ = TransactionManager::commit(dataHelper->factory);
    dataHelper->factory.clear();
    return succ;
}

bool TransactionObject::rollback() {
    if (!dataHelper->factory) return false;
    dataHelper->succ = false;
    bool succ = TransactionManager::rollback(dataHelper->factory);
    dataHelper->factory.clear();
    return succ;
}

bool TransactionObject::hasTransaction() const {
    return dataHelper->factory && TransactionManager::hasTransaction(dataHelper->factory);
}

bool TransactionObject::isOpenSucc() const {
    return dataHelper->succ;
}

struct TransactionManagerServiceDataHelper {
    TransactionObject beginTransaction(const Factory& factory, bool addCurrent) {
        TransactionObject tObj;
        tObj.dataHelper->factory = factory;
        tObj.dataHelper->succ = TransactionManager::beginTransaction(factory, addCurrent);
        return tObj;
    }
};



TransactionManagerService::TransactionManagerService() {
    dataHelper = new TransactionManagerServiceDataHelper;
}

TransactionManagerService::~TransactionManagerService() noexcept {
    delete dataHelper;
}

TransactionObject TransactionManagerService::beginDefaultTransaction(bool addCurrent) {
    return instance()->dataHelper->beginTransaction(GlobalConnectionFactory::instance()->getDefaultSqlConnectionFactory(), addCurrent);
}
TransactionObject TransactionManagerService::beginFileTransaction(bool addCurrent) {
    return instance()->dataHelper->beginTransaction(GlobalConnectionFactory::instance()->getFileSqlConnectionFactory(), addCurrent);
}
TransactionObject TransactionManagerService::beginUserTransaction(bool addCurrent) {
    return instance()->dataHelper->beginTransaction(GlobalConnectionFactory::instance()->getAuthSqlConnectionFactory(), addCurrent);
}
TransactionObject TransactionManagerService::beginLogTransaction(bool addCurrent) {
    return instance()->dataHelper->beginTransaction(GlobalConnectionFactory::instance()->getLogSqlConnectionFactory(), addCurrent);
}

bool TransactionManagerService::hasDefaultTransaction() {
    return TransactionManager::hasTransaction(GlobalConnectionFactory::instance()->getDefaultSqlConnectionFactory());
}
bool TransactionManagerService::hasFileTransaction() {
    return TransactionManager::hasTransaction(GlobalConnectionFactory::instance()->getFileSqlConnectionFactory());
}
bool TransactionManagerService::hasUserTransaction() {
    return TransactionManager::hasTransaction(GlobalConnectionFactory::instance()->getAuthSqlConnectionFactory());
}
bool TransactionManagerService::hasLogTransaction() {
    return TransactionManager::hasTransaction(GlobalConnectionFactory::instance()->getLogSqlConnectionFactory());
}
