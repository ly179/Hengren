#include "hardware_module_task.h"
#include "cdsem_global_env_data_model.h"
#include "cdsem_global_data.h"
#include "property_util.h"
#include "system_default_config_keys.h"
#include "system_default_config_service.h"
#include "service_manager.h"
#include "cdsem_control_helper.h"
#include "zeiss_tcp_client.h"
#include "app_task_worker.h"
#include <QRegularExpression>
#include "cdsem_event_loop.h"
#include <QTimer>
#include "zeiss_client_proxy.h"

OperationResult readIpPort(const QString& config, QString& ip, int& port) {
    static QRegularExpression reg(R"(^\s*\d{1,3}(\.\d{1,3}){3}:\d{1,5}\s*$)");
    QRegularExpressionMatch match = reg.match(config);
    if (!match.hasMatch()) {
        return OperationResult{"Error to load ZEISS TCP config, the config is not currectly.", CONFIG_FORMAT_ERROR_CODE};
    }
    int index = config.indexOf(":");
    ip = config.left(index).trimmed();
    port = config.sliced(index + 1).trimmed().toInt();
    return OperationResult{};
}

OperationResult readConfigFromDatabase(const QString& key, QString& value) {
    SystemDefaultConfigService* service = ServiceManager::defaultService<SystemDefaultConfigService>();
    int errCode;
    SystemDefaultConfigEntity entity = service->getByKey(key, &errCode);
    if (errCode) {
        return OperationResult{"Error to load ZEISS TCP config", errCode};
    }
    if (entity.getId() == 0) {
        return OperationResult{"Error to load ZEISS TCP config, no config info found.", errCode};
    }
    value = entity.getSValue();
    return OperationResult{"", 0};
}


OperationResult zeissConnectToServer() {
    QString config;
    QString ip;
    int port;
    OperationResult result;
#ifdef QT_DEBUG
    config = GlobalPropertyUtil::get().getStrConfig("__zeiss_server");
    if (config.isEmpty())
#endif
        result = readConfigFromDatabase(SDC_ZEISS_TCP_ADDRESS, config);
    if (result.errCode) return result;
    result = readIpPort(config, ip, port);
    if (result.errCode) return result;
    ZeissClientProxy::instance()->setConnectInfo(ip, port);
    CDSEM_ControlHelper::instance()->connect();
    return OperationResult{"", 0};
}

struct HardwareModuleHelper {
    SignalData currentSignal;
    QTimer* timer = nullptr;
    CasLock lock;
    CDSEM_EventLoop* eventLoop = nullptr;

    int index = 0;
    bool exited = false;

    void sendIndexTest() {
        if (!timer->isActive())
            timer->start();
        CDSEM_ControlHelper::instance()->setSignalPortA(nullptr, index);
    }

    void recoverSignal() {
        if (currentSignal.channel >= 0)
            CDSEM_ControlHelper::instance()->setSignalPortA(nullptr, currentSignal.channel);
    }

    bool toNext() {
        //SEM 0 - 3
        //USB 15 - 18
        if (index >= 3) {
            //测试完毕后还原
            return false;
        }
        ++ index;
        sendIndexTest();
        return true;
    }
};

HardwareModuleTask::HardwareModuleTask(QObject* parent) : TaskModule(parent) {
    helper = new HardwareModuleHelper;
}

HardwareModuleTask::~HardwareModuleTask() noexcept {
    if (helper->timer) delete helper->timer;
    delete helper;
}

void HardwareModuleTask::notifyQuit() {
    CasLocker locker(helper->lock);
    helper->exited = true;
    if (helper->eventLoop) helper->eventLoop->quit();
}

OperationResult HardwareModuleTask::doLoad(TaskStopCmd& cmd) {
    //第一步：获取蔡司服务连接配置
    if (step == 0) {
        emit statusInfoChanged("Loading Zeiss Connect Info...");
        OperationResult result = zeissConnectToServer();
        if (result.errCode) {
            return result;
        }
        ++ step;
    }
    //第二步：连接蔡司服务
    if (step == 1) {
        if (cmd.isStop() || helper->exited) return OperationResult {};
        emit statusInfoChanged("Connect to Zeiss Server...");
        sem::AfterGc gc([this](){
            CasLocker locker(helper->lock);
            if (!helper->eventLoop) return;
            helper->eventLoop = nullptr;
        });
        CDSEM_EventLoop _eventLoop;
        CDSEM_EventLoop* eventLoop = &_eventLoop;
        {
            CasLocker locker(helper->lock);
            helper->eventLoop = eventLoop;
        }
        connect(CDSEM_ControlHelper::instance()->getControl(), &CDSEM_Control::connected, this, [this, eventLoop](){
            eventLoop->quit();
        }, Qt::ConnectionType(Qt::QueuedConnection | Qt::SingleShotConnection));
        eventLoop->exec();
        ++ step;
    }
    //第三步：读取蔡司当前的信号A的值
    if (step == 2) {
        if (cmd.isStop() || helper->exited) return OperationResult {};
        emit statusInfoChanged("Reading Current Zeiss Signal A...");
        SignalData data = SEM_PY_ENV.getSignalA();
        if (data.channel < 0) {
            sem::AfterGc gc([this](){
                CasLocker locker(helper->lock);
                if (!helper->eventLoop) return;
                helper->eventLoop = nullptr;
            });
            CDSEM_EventLoop _eventLoop;
            CDSEM_EventLoop* eventLoop = &_eventLoop;
            {
                CasLocker locker(helper->lock);
                helper->eventLoop = eventLoop;
            }
            QObject::connect(&SEM_PY_ENV, &CDSEM_GlobalSemPyParamModel::signalAChanged, this, [eventLoop](){
                SignalData data = SEM_PY_ENV.getSignalA();
                if (data.channel < 0) return;
                eventLoop->quit();
            });
            data = SEM_PY_ENV.getSignalA();
            if (data.channel < 0) {
                if (!helper->timer) helper->timer = new QTimer;
                //5秒延迟时间来读取，读取不到直接走下一步。
                helper->timer->setInterval(5000);
                helper->timer->setSingleShot(true);
                connect(helper->timer, &QTimer::timeout, this, [eventLoop](){
                    eventLoop->quit();
                });
                helper->timer->start();
            }
            eventLoop->exec();
            helper->currentSignal = SEM_PY_ENV.getSignalA();
            if (helper->timer->isActive()) helper->timer->stop();
            QObject::disconnect(&SEM_PY_ENV, &CDSEM_GlobalSemPyParamModel::signalAChanged, this, nullptr);
            QObject::disconnect(helper->timer, &QTimer::timeout, this, nullptr);
        }
        ++ step;
    }
    //第四步：读取蔡司可用的所有电镜的探头传感器编号
    if (step == 3) {
        if (cmd.isStop() || helper->exited) {
            if (helper->timer) {
                delete helper->timer;
                helper->timer = nullptr;
            }
            return OperationResult {};
        }
        emit statusInfoChanged("Checking Zeiss All Signal Ports...");
        sem::AfterGc gc([this](){
            CasLocker locker(helper->lock);
            if (!helper->eventLoop) return;
            helper->eventLoop = nullptr;
        });
        CDSEM_EventLoop _eventLoop;
        CDSEM_EventLoop* eventLoop = &_eventLoop;
        {
            CasLocker locker(helper->lock);
            helper->eventLoop = eventLoop;
        }
        if (!helper->timer) helper->timer = new QTimer;
        QObject::connect(&SEM_PY_ENV, &CDSEM_GlobalSemPyParamModel::signalAChanged, helper->timer, [this, eventLoop](){
            if (!helper->toNext()) {
                eventLoop->quit();
            }
        }, Qt::QueuedConnection);
        helper->timer->setInterval(2000);
        helper->timer->setSingleShot(false);
        connect(helper->timer, &QTimer::timeout, helper->timer, [this, eventLoop](){
            SignalData data = SEM_PY_ENV.getSignalA();
            if (data.channel != helper->index)
                helper->sendIndexTest();
            else if (!helper->toNext()) {
                eventLoop->quit();
            }

        });
        helper->sendIndexTest();
        eventLoop->exec();
        QObject::disconnect(&SEM_PY_ENV, &CDSEM_GlobalSemPyParamModel::signalAChanged, this, nullptr);
        QObject::disconnect(helper->timer, &QTimer::timeout, this, nullptr);
        helper->timer->stop();
        delete helper->timer;
        helper->timer = nullptr;
        helper->recoverSignal();
        ++ step;
    }
    return OperationResult {};
}
