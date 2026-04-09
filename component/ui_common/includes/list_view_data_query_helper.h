#ifndef LIST_VIEW_DATA_QUERY_HELPER_H
#define LIST_VIEW_DATA_QUERY_HELPER_H

#include "ui_common_global.h"
class ListViewMainModel;
class IdpSearchDTO;

int UI_COMMON_EXPORT localLoadClassList(ListViewMainModel& listModel);
int UI_COMMON_EXPORT localLoadIdwList(quint64 classId, ListViewMainModel& listModel);
int UI_COMMON_EXPORT localLoadIdpList(const IdpSearchDTO&, ListViewMainModel& listModel);
int UI_COMMON_EXPORT localLoadRecipeList(quint64 classId,
                                          ListViewMainModel& listModel,
                                          quint64 idwId = 0);

#endif // LIST_VIEW_DATA_QUERY_HELPER_H
