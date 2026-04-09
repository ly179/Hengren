#ifndef IDW_WAFER_ENTITY_H
#define IDW_WAFER_ENTITY_H

#include <QDateTime>
#include "float_number.h"
#include "entity_common_global.h"
#include "model_setget.h"
class IDW_WaferSearchDTO {
    CDSEMPC(QList<quint64>, Ids, ids)
    //wafer name
    CDSEMPSTRING(WaferName, waferName)
    // 分类ID
    CDSEMPULONG64(ClassId, classId)
    // 查找历史版本，即首个版本ID
    CDSEMPULONG64(TopId, topId)
    // 已作废的是否要查询出来，即历史版本 0 - 当前版本 1 - 历史版本
    CDSEMPINTEGER32(Invalid, invalid)
};

class ENTITY_COMMON_EXPORT IDW_WaferInfoEntity {
private:
    quint64 id = 0;
    quint64 topId = 0;
    quint64 classId = 0;
    QString waferName = "";
    QString items = "";
    QString detail = "";
    FloatNumber1 waferDiameter = 0;
    qint32 version = 0;
    qint32 invalid = 0;
    bool deleted = 0;
    QDateTime createTime;
    QDateTime editTime;

    void copy(const IDW_WaferInfoEntity&);

public:

    IDW_WaferInfoEntity();
    IDW_WaferInfoEntity(const IDW_WaferInfoEntity&);
    ~IDW_WaferInfoEntity() noexcept;

    IDW_WaferInfoEntity&   operator=(const IDW_WaferInfoEntity&);


    quint64                getId() const;
    void                   setId(quint64 newId);
    quint64                getTopId() const;
    void                   setTopId(quint64 newTopId);
    quint64                getClassId() const;
    void                   setClassId(quint64 newClassId);
    const QString&         getWaferName() const;
    void                   setWaferName(const QString& newWaferName);
    const QString&         getItems() const;
    void                   setItems(const QString& newItems);
    const QString&         getDetail() const;
    void                   setDetail(const QString& newDetail);
    const FloatNumber1&    getWaferDiameter() const;
    void                   setWaferDiameter(const FloatNumber1& newWaferDiameter);
    qint32                 getVersion() const;
    void                   setVersion(qint32 newVersion);

    bool                   getDeleted() const;
    void                   setDeleted(bool newDeleted);
    QDateTime              getCreateTime() const;
    void                   setCreateTime(const QDateTime& newCreateTime);
    QDateTime              getEditTime() const;
    void                   setEditTime(const QDateTime& newEditTime);
    qint32                 getInvalid() const;
    void                   setInvalid(qint32 newInvalid);
};
#endif // IDW_WAFER_ENTITY_H
