#ifndef COMMON_DATA_LOADER_H
#define COMMON_DATA_LOADER_H

#include "sem_common_data_model.h"
#include <QList>

class ListViewMainData;
class ClassSearchDTO;
class MsrSearchDTO;

struct FileCommonResult : public OperationResult {
    QList<ListViewMainData> list;
    int selectedIndex = -1;
};

FileCommonResult refreshClass(const ClassSearchDTO& searchParam, quint64 selectedId = -1);
FileCommonResult refreshMSR(const MsrSearchDTO& searchParam, quint64 selectedTopId = -1);

#endif // COMMON_DATA_LOADER_H
