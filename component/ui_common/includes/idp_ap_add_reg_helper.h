#ifndef IDP_AP_ADD_REG_HELPER_H
#define IDP_AP_ADD_REG_HELPER_H
#include <QRect>
#include <QSharedPointer>
#include "ui_common_global.h"
#include "sem_common.h"
#include "image_save_model.h"

class QWidget;
class ImageOperationImageCanvas;
class SemThumbnailWidget;
class QImage;
class AsyncCaller;

enum SemRegModel {
    SRM_AP,
    SRM_ADDRESS,
    SRM_MP
};

struct NormalImageRegResult {
    ImageAddtionDataModel additionParam;
    //手动模式下的选框区域
    QRect area;

    int score = 0;
    int quality = 0;
    int error = 0;
};

using NormalImageRegCallback = std::function<void(const NormalImageRegResult &)>;

void UI_COMMON_EXPORT idpToCreateAndDrawThumbnailCanvas(QWidget *_this,
                                                         ImageOperationImageCanvas *&canvasPainter,
                                                         SemThumbnailWidget *thumbnialCanvas,
                                                         const QImage &image,
                                                         const QRect &range);

void UI_COMMON_EXPORT idpRegImage(SemRegModel mode, bool autoMode, QWidget *_this,
    ImageOperationImageCanvas *&canvasPainter,
                       SemThumbnailWidget *thumbnialCanvas, const sem::ONE_PARAM_ARG_FUNC<NormalImageRegResult&>& callback, int skipStep = 0);

//imageId传指针，传成员变量的，用于异步执行任务后回调时检查是否发生变化以及需要使用该指针的值
void UI_COMMON_EXPORT resetImageFromDatabase(QWidget *_this,
                                              AsyncCaller *caller,
                                              quint64 *imageId,
                                              ImageAddtionDataModel& imageDetail,
                                              ImageOperationImageCanvas *&canvasPainter,
                                              SemThumbnailWidget *thumbnialCanvas,
                                              const QRect &rect,
                                              const std::function<void(bool)> &callback = nullptr);
#endif // IDP_AP_ADD_REG_HELPER_H
