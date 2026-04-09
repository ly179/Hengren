#ifndef RELATION_IDP_DATA_EDIT_CONTROLLER_H
#define RELATION_IDP_DATA_EDIT_CONTROLLER_H
#include "relation_cde90_window_export.h"
struct IDP_BaseData{
    QString className = "";
    QString waferName = "";
    quint64 idpId = 0;
    quint64 classId = 0;
    quint64 idwId = 0;
    quint64 idwTopId = 0;
};

__R_WINDOW(RelationIdpEditController)

virtual void loadExistsData(const IDP_BaseData&) = 0;
virtual void newData(quint64 classId, quint64 idwId, const QString& idpName) = 0;
virtual void disableCoverSave(bool = true) = 0;
virtual quint64 getIdpId() const = 0;
virtual quint64 getIdwId() const = 0;
virtual quint64 getIdwTopId() const = 0;

__R_WINDOW_END

#endif //RELATION_IDP_DATA_EDIT_CONTROLLER_H
