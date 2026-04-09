#ifndef APP_INIT_LOADING_HELPER_H
#define APP_INIT_LOADING_HELPER_H

#include "sem_common_data_model.h"
#include "app_init_interface.h"
#include <QObject>

class AsyncCaller;

class AppInitLoadingHelper : public QObject{
    Q_OBJECT
public:
    enum LoadingStatus {
        StandBy,
        Processing,
        Finished,
        Failed
    };
private:
    AppInitLoadingData* loadingData;
    LoadingStatus l = StandBy;

public:

    explicit AppInitLoadingHelper(AppInitLoadingData* data, QObject* parent = nullptr);
    ~AppInitLoadingHelper() noexcept;

    inline void doLoad() {
        loadingData->load();
    }

    inline bool isThreadTask() const {
        return loadingData->isThreadTask();
    }

signals:
    void statusChanged(LoadingStatus, bool failedCouldSkip = false);
    void errorOccern(const QString&);
    void retry();
};

class SerialLoadingManager : public QObject{
    Q_OBJECT
private:
    QWidget* content;
    AsyncCaller* caller;
    QList<AppInitLoadingHelper*> loadingSteps;
    int processIndex = 0;
    bool isExit = false;
public:
    explicit SerialLoadingManager(QObject* parent);
    ~SerialLoadingManager() noexcept {}
    void setASyncCaller(AsyncCaller*);
    void setContent(QWidget*);
    void append(AppInitLoadingData*);
    void exit();
    void start();

signals:
    void finished();
};
#endif // APP_INIT_LOADING_HELPER_H
