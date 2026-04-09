#ifndef RELATION_IDW_WAFER_EDITOR_H
#define RELATION_IDW_WAFER_EDITOR_H
#include "relation_cde90_window_export.h"
struct IDW_BaseData {
    //wafer名称
    QString waferName = "";
    QString className = "";
    quint64 waferId = 0;
    quint64 classId = 0;
};
__R_WINDOW(RelationIDWWaferEditor)

    //虚函数方便作为插件对象让其他方调用，而不是为了继承。
    virtual void setBaseData(const IDW_BaseData&) = 0;
    virtual void setExitCallback(const sem::NO_PARAM_ARG_FUNC&) = 0;
    virtual void loadData(quint64 id)  = 0;
    virtual void disableCoverSave(bool = true)  = 0;
    virtual quint64 getIdwId() const  = 0;

__R_WINDOW_END
#endif // RELATION_IDW_WAFER_EDITOR_H
