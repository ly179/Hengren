#ifndef SYSTEM_DEFAULT_CONFIG_ENTITY_H
#define SYSTEM_DEFAULT_CONFIG_ENTITY_H

#include "entity_common_global.h"
#include <QtGlobal>
#include <QString>
#include <QDateTime>

class ENTITY_COMMON_EXPORT SystemDefaultConfigEntity {
private:
    quint64 id = 0;
    QString sKey = "";
    QString sValue = "";
    int version = 0;
    char deleted = 0;
    QDateTime createTime;
    QDateTime editTime;
public:
    SystemDefaultConfigEntity();
    SystemDefaultConfigEntity(const SystemDefaultConfigEntity&);
    SystemDefaultConfigEntity& operator=(const SystemDefaultConfigEntity&);

    quint64 getId() const;
    void setId(quint64 newId);
    const QString& getSKey() const;
    void setSKey(const QString &newSKey);
    const QString& getSValue() const;
    void setSValue(const QString &newSValue);
    char getDeleted() const;
    void setDeleted(char newDeleted);
    const QDateTime& getCreateTime() const;
    void setCreateTime(const QDateTime &newCreateTime);
    const QDateTime& getEditTime() const;
    void setEditTime(const QDateTime &newEditTime);

    int getVersion() const;
    void setVersion(int newVersion);
};

#endif // SYSTEM_DEFAULT_CONFIG_ENTITY_H
