#ifndef SCAN_MODE_CONFIG_ENTITY_H
#define SCAN_MODE_CONFIG_ENTITY_H

#include "entity_common_global.h"
#include <qglobal.h>
#include <QString>
#include <QDateTime>
class ENTITY_COMMON_EXPORT ScanModeEntity {
private:

    quint64 id;

    // 预制内容
    QString detail;

    // 用户自定义内容
    QString customDetail;

    QDateTime createTime;

    QDateTime editTime;

    char deleted = 0;

public:

    ScanModeEntity() = default;
    ScanModeEntity(const ScanModeEntity&);

    ScanModeEntity& operator=(const ScanModeEntity&);


    quint64            getId() const;
    void               setId(quint64 newId);
    const QString&     getDetail() const;
    void               setDetail(const QString& newDetail);
    QDateTime          getCreateTime() const;
    void               setCreateTime(const QDateTime& newCreateTime);
    QDateTime          getEditTime() const;
    void               setEditTime(const QDateTime& newEditTime);
    char               getDeleted() const;
    void               setDeleted(char newDeleted);
};

#endif // SCAN_MODE_CONFIG_ENTITY_H
