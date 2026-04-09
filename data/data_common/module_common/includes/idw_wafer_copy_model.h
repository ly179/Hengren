#ifndef IDW_WAFER_COPY_MODEL_H
#define IDW_WAFER_COPY_MODEL_H

#include "sem_common_data_model.h"
#include "file_copy_data_model.h"

//文件IDW拷贝参数
struct IDW_CopyDataParam : public SimpleDataCopyParam{
    //拷贝目标的文件夹名
    QString className;

    bool overwrite = false;
};

struct IDW_CopyDataResult: public OperationResult {
    quint64 newId = 0;
    quint64 newClassId = 0;
};

#endif //IDW_WAFER_COPY_MODEL_H
