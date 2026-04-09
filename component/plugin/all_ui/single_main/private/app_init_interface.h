#ifndef APP_INIT_INTERFACE_H
#define APP_INIT_INTERFACE_H

#include "sem_common_data_model.h"
#include <QObject>

class AppInitLoadingData : public QObject{
    Q_OBJECT
private:
    QString title;
    bool onTaskThread = false;
public:
    explicit AppInitLoadingData(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~AppInitLoadingData() noexcept {}
    virtual void load() = 0;
    inline void setLoadOnTaskThread(bool b) {
        onTaskThread = b;
    }
    inline bool isThreadTask() const {
        return onTaskThread;
    }

    inline void setTitle(const QString& t) {
        title = t;
    }

    inline const QString& getTitle() const {
        return title;
    }

signals:
    void overLoad(const OperationResult&, bool errorCouldSkip = false);
};

#endif // APP_INIT_INTERFACE_H
