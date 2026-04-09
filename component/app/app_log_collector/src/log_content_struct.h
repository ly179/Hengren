#ifndef LOG_CONTENT_STRUCT_H
#define LOG_CONTENT_STRUCT_H
#include <QTime>
#include <QString>
struct LogContentStruct {
    QTime time;
    QString classFile;
    QString content;
    QString cat;
    int line;
};
#endif // LOG_CONTENT_STRUCT_H
