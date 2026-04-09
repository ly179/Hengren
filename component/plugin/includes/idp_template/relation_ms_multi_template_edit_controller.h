//
// Created by Shisan on 2026/1/16.
//

#ifndef HENGREN_CD_SEM_RELATION_MS_MULTI_TEMPLATE_EDIT_CONTROLLER_H
#define HENGREN_CD_SEM_RELATION_MS_MULTI_TEMPLATE_EDIT_CONTROLLER_H

#include "relation_cde90_window_export.h"
__R_WINDOW(RelationMSMultiTemplateEditController)

virtual void loadData(quint64 id) = 0;
virtual void initData(const QString& name) = 0;
virtual quint64 getId() const = 0;

__R_WINDOW_END

#endif //HENGREN_CD_SEM_RELATION_MS_MULTI_TEMPLATE_EDIT_CONTROLLER_H