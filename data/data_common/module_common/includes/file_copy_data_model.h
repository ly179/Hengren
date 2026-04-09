#ifndef FILE_COPY_DATA_MODEL_H
#define FILE_COPY_DATA_MODEL_H
#include <QString>

//简单的文件拷贝参数。
struct SimpleDataCopyParam {
    //device name, 目前没有什么用
    QString fromDeviceName;
    //拷贝目标文件名
    QString toName;
    //要被拷贝的数据ID
    quint64 fromId;
};


#endif // FILE_COPY_DATA_MODEL_H
