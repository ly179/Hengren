#ifndef DAO_MANAGER_H
#define DAO_MANAGER_H
#include "general_database_dao_global.h"
#include <QHash>

#include "idao.h"
#include "sem_singleton_template.h"
#include <typeindex>

enum SqlConnectionFactoryType { SQL_DEFAULT_FACOTRY, SQL_FILE_FACTORY, SQL_AUTH_FACTORY, SQL_LOG_FACTORY };

class SEM_DAO_EXPORT DaoManager : public Singleton<DaoManager> {
    friend class Singleton<DaoManager>;

private:
    QHash<SqlConnectionFactoryType, QHash<std::type_index, IDao*>> allDaos;

    DaoManager();

    template<class R>
    R* getFactoryDao(
        SqlConnectionFactoryType type) {
        std::type_index tid = std::type_index(typeid(R));
        auto& hash = this->allDaos[type];
        auto it = hash.find(tid);

        if (it != hash.end()) {
            return dynamic_cast<R*>(it.value());
        }
        return nullptr;
    }

public:

    // ServiceManager交给操作系统来回收资源
    ~DaoManager() noexcept;

    DaoManager(const DaoManager&) = delete;

    DaoManager& operator=(const DaoManager&) = delete;

    template<class R>
    R* getDefaultFactoryDao() {
        return getFactoryDao<R>(SQL_DEFAULT_FACOTRY);
    }

    template<class R>
    R* getFileFactoryDao() {
        return getFactoryDao<R>(SQL_FILE_FACTORY);
    }

    template<class R>
    R* getAuthFactoryDao() {
        return getFactoryDao<R>(SQL_AUTH_FACTORY);
    }

    template<class R>
    R* getLogFactoryDao() {
        return getFactoryDao<R>(SQL_LOG_FACTORY);
    }
};

#endif // DAO_MANAGER_H
