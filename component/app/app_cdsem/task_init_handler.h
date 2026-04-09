#ifndef TASK_INIT_HANDLER_H
#define TASK_INIT_HANDLER_H

#include <QObject>
#include "task_module.h"

class AsyncCaller;
struct TaskInitWorkerHelper;
//用于初始化必须在子线程上才可以运行的任务。
class TaskInitWorker : public QObject {
    Q_OBJECT
private:
    TaskInitWorkerHelper* dataHelper;
public:

    explicit TaskInitWorker(QObject* parent = nullptr);
    ~TaskInitWorker() noexcept;

    void initAsyncCaller(AsyncCaller*);

    TaskPackage getTaskPackage(int);

    int appendTaskPackage(TaskModule*);

    void reloadAllFailedTasks();

    void reloadFailedTask(int id);

    void startTask();

    void notifyToStop();

    TaskPackage getLastErrorTaskPackage();

signals:
    void initStatusChanged(int id, TaskPackageStatus status);

};

#endif // TASK_INIT_HANDLER_H
