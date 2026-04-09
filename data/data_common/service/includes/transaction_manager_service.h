#ifndef TRANSACTION_MANAGER_SERVICE_H
#define TRANSACTION_MANAGER_SERVICE_H

#include "sem_singleton_template.h"
#include "general_database_service_global.h"
struct TransactionObjectDataHelper;
class SEM_SERVICE_EXPORT TransactionObject {
    friend struct TransactionManagerServiceDataHelper;
private:
    TransactionObjectDataHelper* dataHelper;
public:
    TransactionObject();
    ~TransactionObject() noexcept;
    TransactionObject(const TransactionObject&);
    TransactionObject& operator=(const TransactionObject&);
    bool commit();
    bool rollback();
    //判断当前有没有事务
    bool hasTransaction() const;
    //判断本次开事务是否开启成功
    bool isOpenSucc() const;
};

struct TransactionManagerServiceDataHelper;
class SEM_SERVICE_EXPORT TransactionManagerService : public Singleton<TransactionManagerService>{
    friend class Singleton<TransactionManagerService>;
    TransactionManagerServiceDataHelper* dataHelper;
    TransactionManagerService();
public:
    ~TransactionManagerService() noexcept;

    static TransactionObject beginDefaultTransaction(bool addCurrent = false);
    static TransactionObject beginFileTransaction(bool addCurrent = false);
    static TransactionObject beginUserTransaction(bool addCurrent = false);
    static TransactionObject beginLogTransaction(bool addCurrent = false);

    static bool hasDefaultTransaction();
    static bool hasFileTransaction();
    static bool hasUserTransaction();
    static bool hasLogTransaction();
};

#endif // TRANSACTION_MANAGER_SERVICE_H
