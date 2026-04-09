#ifndef IDP_DATA_COPY_MODULE_H
#define IDP_DATA_COPY_MODULE_H
#include "file_copy_data_model.h"
#include "sem_common_data_model.h"

struct IDP_CopyDataParam: public SimpleDataCopyParam {
    QString toClassName;
    QString toIdwName;

    bool overwrite = false;
};

struct IDP_CopyDataResult: public OperationResult {
    quint64 newId = 0;
    quint64 newClassId = 0;
};


#endif // IDP_DATA_COPY_MODULE_H
