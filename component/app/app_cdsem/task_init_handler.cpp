#include "task_init_handler.h"
#include "cas_lock.h"
#include "plinked_node_hash.h"
#include "plinked_node_list.h"
#include "sem_global.h"

struct TaskPackageBase {
    TaskPackage info;
    TaskModule* task;
};

struct TaskInitWorkerHelper {
    CasLock lock;
    PLinkedHashNodeHash<int, TaskPackageBase> tasks;
    PLinkedNodeList<int> errTasks;

    AsyncCaller* caller = nullptr;
    TaskInitWorker* d;
    int nextId = 1;

    bool running = false;
    bool failedRetring = false;

    void doTask(const PLinkedHashNode<int, TaskPackageBase>* node, TSC cmd) {
        node->value.info.status = TPS_LOADING;
        emit d->initStatusChanged(node->key, node->value.info.status);
        node->value.info.result = node->value.task->doLoad(cmd);
        node->value.info.status = node->value.info.result.errCode ? TPS_FAILED : TPS_FINISHED;
        emit d->initStatusChanged(node->key, node->value.info.status);
        if (node->value.info.status == TPS_FINISHED) {
            CasLocker locker(lock);
            errTasks.remove(node->key);
            QObject::disconnect(node->value.task, &TaskModule::reloadAgain, d, nullptr);
        } else {
            CasLocker locker(lock);
            errTasks.append(node->key);
        }
    }

    void doListTask(TaskPackageStatus filterStatus, bool& flag) {
        {
            CasLocker locker(lock);
            if (flag) return;
            flag = true;
        }
        caller->push(d, [this, filterStatus, &flag](TSC cmd){
            sem::AfterGc gc([this, &flag](){
                CasLocker locker(lock);
                flag = false;
            });
            const PLinkedHashNode<int, TaskPackageBase>* node;
            {
                CasLocker locker(lock);
                node = tasks.first();
            }
            do {
                if (node == nullptr) break;
                if (node->value.info.status == filterStatus) {
                    doTask(node, cmd);
                }
                CasLocker locker(lock);
                node = node->next;
                continue;
            } while (true);
        });
    }
};

TaskInitWorker::TaskInitWorker(QObject* parent) : QObject(parent) {
    dataHelper = new TaskInitWorkerHelper;
    dataHelper->d = this;
}

TaskInitWorker::~TaskInitWorker() noexcept {
    if (dataHelper->tasks.size() > 0) {
        for (auto it = dataHelper->tasks.first(); it != nullptr; it = it->next) {
            delete it->value.task;
        }
    }
    delete dataHelper;
}

void TaskInitWorker::initAsyncCaller(AsyncCaller* caller) {
    if (dataHelper->caller) return;
    dataHelper->caller = caller;
}

TaskPackage TaskInitWorker::getTaskPackage(int id) {
    CasLocker locker(dataHelper->lock);
    const PLinkedHashNode<int, TaskPackageBase>* node = dataHelper->tasks.dataNode(id);
    if (!node) return TaskPackage();
    return node->value.info;
}

int TaskInitWorker::appendTaskPackage(TaskModule* task) {
    CasLocker locker(dataHelper->lock);
    int id = dataHelper->nextId;
    connect(task, &TaskModule::reloadAgain, this, [this, id](){
        reloadFailedTask(id);
    });
    TaskPackageBase base;
    base.task = task;
    base.info.status = TPS_NOT_INIT;
    dataHelper->tasks.append(dataHelper->nextId, base);
    emit initStatusChanged(dataHelper->nextId, base.info.status);
    return dataHelper->nextId ++;
}

void TaskInitWorker::reloadAllFailedTasks() {
    dataHelper->doListTask(TPS_FAILED, dataHelper->failedRetring);
}

void TaskInitWorker::reloadFailedTask(int id) {
    const PLinkedHashNode<int, TaskPackageBase>* node;
    {
        CasLocker locker(dataHelper->lock);
        node = dataHelper->tasks.dataNode(id);
    }
    if (!node || node->value.info.status != TPS_FAILED) return;
    dataHelper->caller->push(this, [this, node](TSC cmd){
        dataHelper->doTask(node, cmd);
    });
}

void TaskInitWorker::startTask() {
    dataHelper->doListTask(TPS_NOT_INIT, dataHelper->running);
}

void TaskInitWorker::notifyToStop() {
    CasLocker locker(dataHelper->lock);
    for (auto it = dataHelper->tasks.first(); it != nullptr; it = it->next) {
        it->value.task->notifyQuit();
    }
}

TaskPackage TaskInitWorker::getLastErrorTaskPackage() {
    CasLocker locker(dataHelper->lock);
    const PLinkedNode<int>* last = dataHelper->errTasks.last();
    if (!last) {
        return TaskPackage();
    }
    const PLinkedHashNode<int, TaskPackageBase>* data = dataHelper->tasks.dataNode(last->data);
    if (!data) return TaskPackage();
    return data->value.info;
}