#ifndef CDSEM_AUTH_ENTITY_H
#define CDSEM_AUTH_ENTITY_H

#include <QDateTime>
#include <QString>
#include "cdsem_pack_base.h"
#include "model_setget.h"

struct SearchRoleDTO {
    QString roleName;
    quint64 userId = 0;
};

struct SearchAuthDTO {
    QString authName;
    quint64 roleId = 0;
};

struct SearchUserDTO {
    QString userName;
    QString realName;
    QList<quint64> userIds;
    Char8 authType;
    Char8 invalid;
};

class AuthEntity {
    CDSEMPSTRING(
        AuthName, authName)
    CDSEMPSTRING(
        AuthKey, authKey)
    CDSEMPSTRING(
        AuthDescription, authDescription)
    CDSEMPDATETIME(
        CreateTime, createTime)
    CDSEMPDATETIME(
        EditTime, editTime)
    CDSEMPUINT64(
        Id, id, 0)
    CDSEMPChar8(
        AuthType, authType)
    CDSEMPChar8(
        Invalid, invalid)
    CDSEMPChar8(
        Readable, readable)
    CDSEMPChar8(
        Writable, writable)
    CDSEMPChar8(
        Executable, executable)
    CDSEMPChar8(
        Deleted, deleted)
};

class RoleEntity {
    CDSEMPDATETIME(
        CreateTime, createTime)
    CDSEMPDATETIME(
        EditTime, editTime)
    CDSEMPSTRING(
        RoleName, roleName)
    CDSEMPHSTRING(
        RoleDes, roleDes)
    CDSEMPUINT64(
        Id, id, 0)
    //1 - 反转角色，即角色拥有的权限用户不可访问
    CDSEMPChar8(
        Reverse, reverse)
    //角色类型0 - 普通 1 - 超管
    CDSEMPChar8(
        RoleType, roleType)
    //1 - 角色不可用
    CDSEMPChar8(
        Invalid, invalid)
    //1 - 删除
    CDSEMPCHAR(
        Deleted, deleted, 0)
};

class RoleAuthEntity {
    CDSEMPDATETIME(
        CreateTime, createTime)
    CDSEMPDATETIME(
        EditTime, editTime)
    CDSEMPUINT64(
        Id, id, 0)
    CDSEMPUINT64(
        AuthId, authId, 0)
    CDSEMPUINT64(
        RoleId, roleId, 0)
    CDSEMPChar8(
        Invalid, invalid)
    CDSEMPChar8(
        Readable, readable)
    CDSEMPChar8(
        Writable, writable)
    CDSEMPChar8(
        Executable, executable)
    CDSEMPChar8(
        Deleted, deleted)
};

class UserEntity {
    CDSEMPDATETIME(
        LastLoginTime, lastLoginTime)
    CDSEMPDATETIME(
        CreateTime, createTime)
    CDSEMPDATETIME(
        EditTime, editTime)
    CDSEMPSTRING(
        UserName, userName)
    CDSEMPHSTRING(
        UserDes, userDes)
    CDSEMPSTRING(
        RealName, realName)
    CDSEMPSTRING(
        Password, password)
    CDSEMPUINT64(
        Id, id, 0)
    CDSEMPChar8(
        Invalid, invalid)
    CDSEMPChar8(
        AuthType, authType)
    CDSEMPChar8(
        Deleted, deleted)
};

class UserRoleEntity {
    CDSEMPDATETIME(
        CreateTime, createTime)
    CDSEMPDATETIME(
        EditTime, editTime)
    CDSEMPUINT64(
        Id, id, 0)
    CDSEMPUINT64(
        UserId, UserId, 0)
    CDSEMPUINT64(
        RolelId, roleId, 0)
    CDSEMPChar8(
        Invalid, invalid)
    CDSEMPChar8(
        Deleted, deleted)
};

//连表结果存储
//在Java中，比如Hibernate框架，通常需要重新定义DTO然后做类型嵌套（遇到不规范的就是在Entity中直接嵌套另一个Entity）
//在MyBatis和互联网项目中，通常是按需查询，定义一个新的DTO(或较为通用DTO)来关注需要的字段
//这里使用这种简单粗暴的方式解决，既不破坏原来的Entity，也能满足项目需求
struct RoleAuthResultDTO {
    QList<RoleAuthEntity> roleAuths;
    QList<AuthEntity> auths;
};
//使用角色ID作为导向的结果查询
struct UserRoleResultDTO {
    QList<UserRoleEntity> userRoles;
    //key:roleId
    QList<RoleEntity> roles;
};

#endif // CDSEM_AUTH_ENTITY_H
