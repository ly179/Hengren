#ifndef RELATION_MPIP_TEMPLATE_EDIT_CONTROLLER_H
#define RELATION_MPIP_TEMPLATE_EDIT_CONTROLLER_H
#include "relation_cde90_window_export.h"
__R_WINDOW(RelationMpIpTemplateEditController)

virtual void loadData(quint64 id) = 0;
virtual void initData(const QString& name) = 0;
virtual quint64 getId() const = 0;

__R_WINDOW_END

#endif //RELATION_MPIP_TEMPLATE_EDIT_CONTROLLER_H
