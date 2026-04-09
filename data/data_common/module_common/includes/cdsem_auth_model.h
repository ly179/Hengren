#ifndef CDSEM_AUTH_MODEL_H
#define CDSEM_AUTH_MODEL_H
#include <QString>

//权限列表TableView的模型
struct AuthTableData {
    QString authName;
    //权限ID(Function id)
    quint64 id = 0;
    bool readable = false;
    bool writable = false;
    bool executable = false;

    bool couldReadAuth = false;
    bool couldWriteAuth = false;
    bool couldExecAuth = false;

    bool selected = false;
};

//用户修改数据模型
struct AuthUserDataModel {
    QString userName;
    QString realName;
    QString userDescription;
    quint64 id = 0;
    bool isSuperAdmin = false;
};

//角色创建与修改的数据模型
struct AuthRoleDataModel {
    QString roleName;
    QString roleDes;
    quint64 id = 0;
    char roleType = 0;
    bool permissionInversion = false;
    bool selected = false;
};

#endif // CDSEM_AUTH_MODEL_H
