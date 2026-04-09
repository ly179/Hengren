#include "idw_wafer_entity.h"

IDW_WaferInfoEntity::IDW_WaferInfoEntity() {}

IDW_WaferInfoEntity::IDW_WaferInfoEntity(const IDW_WaferInfoEntity& other) {
    *this = other;
}
IDW_WaferInfoEntity::~IDW_WaferInfoEntity() noexcept{

}

quint64 IDW_WaferInfoEntity::getId() const
{
    return id;
}

void IDW_WaferInfoEntity::setId(quint64 newId)
{
    id = newId;
}

quint64 IDW_WaferInfoEntity::getTopId() const
{
    return topId;
}

void IDW_WaferInfoEntity::setTopId(quint64 newTopId)
{
    topId = newTopId;
}

quint64 IDW_WaferInfoEntity::getClassId() const
{
    return classId;
}

void IDW_WaferInfoEntity::setClassId(quint64 newClassId)
{
    classId = newClassId;
}

const QString& IDW_WaferInfoEntity::getWaferName() const
{
    return waferName;
}

void IDW_WaferInfoEntity::setWaferName(const QString &newWaferName)
{
    waferName = newWaferName;
}

const QString& IDW_WaferInfoEntity::getItems() const
{
    return items;
}

void IDW_WaferInfoEntity::setItems(const QString &newItems)
{
    items = newItems;
}

const QString& IDW_WaferInfoEntity::getDetail() const
{
    return detail;
}

void IDW_WaferInfoEntity::setDetail(const QString &newDetail)
{
    detail = newDetail;
}

const FloatNumber1& IDW_WaferInfoEntity::getWaferDiameter() const
{
    return waferDiameter;
}

void IDW_WaferInfoEntity::setWaferDiameter(const FloatNumber1 &newWaferDiameter)
{
    waferDiameter = newWaferDiameter;
}

qint32 IDW_WaferInfoEntity::getVersion() const
{
    return version;
}

void IDW_WaferInfoEntity::setVersion(qint32 newVersion)
{
    version = newVersion;
}

bool IDW_WaferInfoEntity::getDeleted() const
{
    return deleted;
}

void IDW_WaferInfoEntity::setDeleted(bool newDeleted)
{
    deleted = newDeleted;
}

QDateTime IDW_WaferInfoEntity::getCreateTime() const
{
    return createTime;
}

void IDW_WaferInfoEntity::setCreateTime(const QDateTime &newCreateTime)
{
    createTime = newCreateTime;
}

QDateTime IDW_WaferInfoEntity::getEditTime() const
{
    return editTime;
}

void IDW_WaferInfoEntity::setEditTime(const QDateTime &newEditTime)
{
    editTime = newEditTime;
}

qint32 IDW_WaferInfoEntity::getInvalid() const
{
    return invalid;
}

void IDW_WaferInfoEntity::setInvalid(qint32 newInvalid)
{
    invalid = newInvalid;
}

void IDW_WaferInfoEntity::copy(const IDW_WaferInfoEntity& cp) {
    this->id = cp.id;
    this->topId = cp.topId;
    this->waferDiameter = cp.waferDiameter;
    this->version = cp.version;
    this->waferName = cp.waferName;
    this->createTime = cp.createTime;
    this->editTime = cp.editTime;
    this->deleted = cp.deleted;
    this->items = cp.items;
    this->classId = cp.classId;
    this->detail = cp.detail;
    this->invalid = cp.invalid;
}

IDW_WaferInfoEntity& IDW_WaferInfoEntity::operator=(const IDW_WaferInfoEntity& other) {
    if (this == &other) {
        return *this;
    }
    this->copy(other);
    return *this;
}


