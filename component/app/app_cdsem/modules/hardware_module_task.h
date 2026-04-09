#ifndef HARDWARE_MODULE_TASK_H
#define HARDWARE_MODULE_TASK_H

#include "../task_module.h"

//硬件加载过程
struct HardwareModuleHelper;
class HardwareModuleTask  : public TaskModule{
    Q_OBJECT
private:
    HardwareModuleHelper* helper;
    int step = 0;
    OperationResult doLoad(TaskStopCmd&) override;

    void notifyQuit() override;

public:
    explicit HardwareModuleTask(QObject* parent = nullptr);
    ~HardwareModuleTask() noexcept;
};

#endif // HARDWARE_MODULE_TASK_H
