#ifndef CONFIG_MODULE_TASK_H
#define CONFIG_MODULE_TASK_H

#include "../task_module.h"

//读取数据库中的数据
class TaskInitConfigModule : public TaskModule {
    Q_OBJECT
private:
    int step = 0;
    OperationResult doLoad(TaskStopCmd&) override;

public:
    explicit TaskInitConfigModule(QObject* parent = nullptr);
    ~TaskInitConfigModule() noexcept;

};

#endif // CONFIG_MODULE_TASK_H
