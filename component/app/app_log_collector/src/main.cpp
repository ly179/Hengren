#include <QGuiApplication>

#include "app_process_lock.h"
#include "log_qt_bind.h"
#include "log_scan_processor.h"
#include "log_config_file.h"
#if defined(__SOURCE_CODE_DIR)
#include <QDir>
#endif

#include "cdsem_thread.h"
#include <QLocalSocket>
#include "app_process_shooter.h"
#include "app_thread_invoker.h"
#include "app_util.h"
#include "app_exit_thread_manager.h"


int main(int argc, char *argv[]) {
    QGuiApplication a(argc, argv);
    // 设置扫描监控路径
    {
        bool dirSetted = false;
        if (argc > 1) {
            for (int i = 0; i < argc - 1; ++ i) {
                if (std::strcmp(argv[i], "--path") == 0) {
                    QDir::setCurrent(argv[i + 1]);
                    qDebug() << "CURRENT SET: " << argv[i + 1];
                    dirSetted = true;
                }
            }
        }
        if (!dirSetted) {
#if defined(QT_DEBUG) && defined(__SOURCE_CODE_DIR)
            qDebug() << "source: " << __SOURCE_CODE_DIR;
            QDir::setCurrent(__SOURCE_CODE_DIR);
#endif
        }
    }
    if (!uniqueProcess(&a, 100001)) {
        return 0;
    }
    setCoreApp(&a);
    AppExitThreadManager::instance()->setApp(&a);
    // AppThreadInvoker::instance()->moveToThread(QThread::currentThread());
    bindQtLog(&a, getLogCollectorLogConfigPath());

    AppProcessShooter shooter;
    shooter.setClientName("LogCollector");
    shooter.setServerName("CD_SEM");
    shooter.setTimeout(5000);
    shooter.setThread();
    bool towerAlive = true;
    AppProcessShooter::connect(
        &shooter,
        &AppProcessShooter::serverDied,
        &shooter,
        [&shooter, &towerAlive]() {
            towerAlive = false;
            qInfo() << "检测到主应用退出";
            QThread::sleep(10);
            shooter.beginShoot();
        },
        Qt::DirectConnection);
    AppProcessShooter::connect(
        &shooter,
        &AppProcessShooter::serverConnected,
        &shooter,
        [&towerAlive]() {
            towerAlive = true;
            qInfo() << "检测到主应用上线";
        },
        Qt::DirectConnection);
    shooter.beginShoot();

    CDSEM_Thread* thread = new CDSEM_Thread;
    thread->start();
    AppThreadInvoker* invoker = new AppThreadInvoker;
    invoker->moveToThread(thread);
    invoker->runLater([invoker, &shooter, &towerAlive]() {
        LogScanProcessor processor;
        do {
            processor.start();
        } while (towerAlive);
        qInfo() << "日志收集器即将退出...";
        invoker->quit();
        shooter.close();
    });

    shooter.blockUtilClosed();
    delete invoker;
    // return a.exec();
    return 0;
}
