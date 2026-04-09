#ifndef SERVICE_MANAGER_H
#define SERVICE_MANAGER_H

#include <QHash>
#include "general_database_service_global.h"
#include "sem_singleton_template.h"
#include "typeindex"
#include <QSharedPointer>

class IService;

class SEM_SERVICE_EXPORT ServiceManager : public Singleton<ServiceManager> {
    friend class Singleton<ServiceManager>;

private:
    QHash<std::type_index, IService*> allServices;

    ServiceManager();

public:
    // ServiceManager交给操作系统来回收资源
    ~ServiceManager() noexcept;

    ServiceManager(const ServiceManager&) = delete;

    ServiceManager& operator=(const ServiceManager&) = delete;

    // static void beginDefaultTransaction(bool addCurrent = false);
    // static void beginFileImageTransaction(bool addCurrent = false);
    // static void beginUserTransaction(bool addCurrent = false);
    // static void beginLogTransaction(bool addCurrent = false);

    template<class R>
    R* getDefaultService() {
        std::type_index tid = std::type_index(typeid(R));
        auto it = allServices.find(tid);

        if (it != allServices.end()) {
            return dynamic_cast<R*>(it.value());
        }
        return nullptr;
    }

    template<class R>
    static R* defaultService() {
        return instance()->getDefaultService<R>();
    }
};

#endif // SERVICE_MANAGER_H
