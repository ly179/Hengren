#ifndef IMAGE_OPERATION_WINDOW_HELPER_H
#define IMAGE_OPERATION_WINDOW_HELPER_H
#include "ui_single_main_global.h"
#include "sem_singleton_template.h"

#include "image_operation_controller.h"

class CasLock;
struct ImageOperationWindowDataHelper;

class UI_SINGLE_MAIN_EXPORT ImageOperationWindowHelper : public Singleton<ImageOperationWindowHelper>{
    friend class Singleton<ImageOperationWindowHelper>;

private:
    ImageOperationController* imageOperationController = nullptr;
    ImageOperationWindowDataHelper* dataHelper;

    ImageOperationWindowHelper();

    void checkAndStartLiveImage(const QPointer<QObject>& sender);
public:

    virtual ~ImageOperationWindowHelper() noexcept;

    void show();

    void close();

    inline ImageOperationController* operator->() const { return imageOperationController; }

    inline static ImageOperationController* get() { return instance()->imageOperationController; }
};

#endif // IMAGE_OPERATION_WINDOW_HELPER_H
