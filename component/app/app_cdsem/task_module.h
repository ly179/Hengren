#ifndef TASK_MODULE_H
#define TASK_MODULE_H

#include "sem_common_data_model.h"
#include <QObject>

enum TaskPackageStatus {
    TPS_NOT_EXISTS,
    TPS_FINISHED,
    TPS_LOADING,
    TPS_NOT_INIT,
    TPS_FAILED
};
struct TaskPackage {
    OperationResult result;
    TaskPackageStatus status = TPS_NOT_EXISTS;
};

class TaskStopCmd;
class TaskModule : public QObject{
    Q_OBJECT
public:
    explicit TaskModule(QObject* parent = nullptr) : QObject(parent) {}
    virtual OperationResult doLoad(TaskStopCmd&) = 0;
    virtual ~TaskModule() noexcept {}
    //通知该退出了，如果有异步等待的任务等，就别等了，退了吧。
    virtual void notifyQuit() {}

signals:
    void statusInfoChanged(const QString&);
    //该信号用于可通过自身进行再次重试，而不需要retire按钮。
    void reloadAgain();
};

class SimpleTaskModule : public TaskModule {
    Q_OBJECT
private:
    std::function<OperationResult(TaskStopCmd&)> func;
    QString name;
public:
    SimpleTaskModule(const std::function<OperationResult(TaskStopCmd&)>& func, const QString& name, QObject* parent = nullptr) : TaskModule(parent), func(func), name(name) {}

    OperationResult doLoad(TaskStopCmd& cmd) override {
        emit statusInfoChanged(name);
        return func(cmd);
    }

    SimpleTaskModule(const SimpleTaskModule& other) {
        *this = other;
    }
    SimpleTaskModule(SimpleTaskModule&& other) {
        *this = std::move(other);
    }

    SimpleTaskModule& operator=(const SimpleTaskModule& other) {
        if (this == &other) return *this;
        func = other.func;
        return *this;
    }

    SimpleTaskModule& operator=(SimpleTaskModule&& other) {
        if (this == &other) return *this;
        func = std::move(other.func);
        return *this;
    }
};

#endif // TASK_MODULE_H
