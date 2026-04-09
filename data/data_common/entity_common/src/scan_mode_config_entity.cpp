#include "scan_mode_config_entity.h"

quint64 ScanModeEntity::getId() const
{
    return id;
}

void ScanModeEntity::setId(quint64 newId)
{
    id = newId;
}

const QString& ScanModeEntity::getDetail() const
{
    return detail;
}

void ScanModeEntity::setDetail(const QString &newDetail)
{
    detail = newDetail;
}

QDateTime ScanModeEntity::getCreateTime() const
{
    return createTime;
}

void ScanModeEntity::setCreateTime(const QDateTime &newCreateTime)
{
    createTime = newCreateTime;
}

QDateTime ScanModeEntity::getEditTime() const
{
    return editTime;
}

void ScanModeEntity::setEditTime(const QDateTime &newEditTime)
{
    editTime = newEditTime;
}

char ScanModeEntity::getDeleted() const
{
    return deleted;
}

void ScanModeEntity::setDeleted(char newDeleted)
{
    deleted = newDeleted;
}

ScanModeEntity::ScanModeEntity(const ScanModeEntity& other) {
    *this = other;
}

ScanModeEntity& ScanModeEntity::operator=(const ScanModeEntity& other) {
    this->id = other.id;
    this->detail = other.detail;
    this->customDetail = other.customDetail;
    this->createTime = other.createTime;
    this->editTime = other.editTime;
    this->deleted = other.deleted;
    return *this;
}
