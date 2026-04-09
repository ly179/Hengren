#include "system_default_config_entity.h"

int SystemDefaultConfigEntity::getVersion() const
{
    return version;
}

void SystemDefaultConfigEntity::setVersion(int newVersion)
{
    version = newVersion;
}

SystemDefaultConfigEntity::SystemDefaultConfigEntity() {}

SystemDefaultConfigEntity::SystemDefaultConfigEntity(const SystemDefaultConfigEntity& other) {
    *this = other;
}
SystemDefaultConfigEntity& SystemDefaultConfigEntity::operator=(const SystemDefaultConfigEntity& other) {
    if (this == &other) {
        return *this;
    }
    id = other.id;
    sKey = other.sKey;
    sValue = other.sValue;
    deleted = other.deleted;
    createTime = other.createTime;
    editTime = other.editTime;
    version = other.version;
    return *this;
}

quint64 SystemDefaultConfigEntity::getId() const
{
    return id;
}

void SystemDefaultConfigEntity::setId(quint64 newId)
{
    id = newId;
}

const QString& SystemDefaultConfigEntity::getSKey() const
{
    return sKey;
}

void SystemDefaultConfigEntity::setSKey(const QString &newSKey)
{
    sKey = newSKey;
}

const QString& SystemDefaultConfigEntity::getSValue() const
{
    return sValue;
}

void SystemDefaultConfigEntity::setSValue(const QString &newSValue)
{
    sValue = newSValue;
}

char SystemDefaultConfigEntity::getDeleted() const
{
    return deleted;
}

void SystemDefaultConfigEntity::setDeleted(char newDeleted)
{
    deleted = newDeleted;
}

const QDateTime& SystemDefaultConfigEntity::getCreateTime() const
{
    return createTime;
}

void SystemDefaultConfigEntity::setCreateTime(const QDateTime &newCreateTime)
{
    createTime = newCreateTime;
}

const QDateTime& SystemDefaultConfigEntity::getEditTime() const
{
    return editTime;
}

void SystemDefaultConfigEntity::setEditTime(const QDateTime &newEditTime)
{
    editTime = newEditTime;
}
