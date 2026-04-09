#ifndef AXIS_ALIGN_DETAIL_CONTENT_H
#define AXIS_ALIGN_DETAIL_CONTENT_H
#include "axis_align_model.h"
#include "cde90_frame.h"

namespace cdsem {
class UiAxisAlignDetailContent;
}
class AsyncCaller;
class ZSensorSettingController;
class AbstractAxisFocusSettingController;
class PatternRecognitionParamController;
class PatternRecognitionController;
class IfemSettingController;
class ImageOperationImageCanvas;
class PatternRegInfoController;
class SemThumbnailWidget;

class AxisAlignDetailContent : public CDE90Frame {
    template<class T>
    friend void _resetToComponent(AxisAlignDetailContent*, AxisAlignContentModel<T>& param);
    template<class T>
    friend QString _collectData(AxisAlignDetailContent*, AxisAlignContentModel<T>& param);
    Q_OBJECT
private:
    QString group;
    cdsem::UiAxisAlignDetailContent* ui = nullptr;
    ZSensorSettingController* zSensorController = nullptr;
    AsyncCaller* caller = nullptr;
    AbstractAxisFocusSettingController* focusSettingController = nullptr;
    PatternRecognitionParamController* patternRegParamController = nullptr;
    PatternRecognitionController* patternAlgController = nullptr;
    IfemSettingController* ifemController = nullptr;
    ImageOperationImageCanvas* canvasPainter = nullptr;
    PatternRegInfoController* patternRegInfoController = nullptr;
    void* param = nullptr;
    //0 - axis alignment
    //1 - stigma
    //default: 0
    char mode = 0;

    void create();
    void createAction();

    void createZSensorController();
    void showFocusSettingController();
    void createPatternRecognitionParamController();
    void createPatternAlgContrller();
    void createPatternRegInfoController();
    void createIfemController();
    void beginRegImage();
    void resetAddressImage(QString& imagePath,
                           quint64* imageId,
                           ImageAddtionDataModel& imageDetail,
                           SemThumbnailWidget* canvas,
                           ImageOperationImageCanvas*& canvasPainter,
                           const QRect& range);

public:
    explicit AxisAlignDetailContent(QWidget* parent = nullptr);
    explicit AxisAlignDetailContent(int mode, QWidget* parent = nullptr);
    ~AxisAlignDetailContent() noexcept;

    void initAsyncCaller(AsyncCaller* caller);

    void initParam(AxisAlignmentContentModel* param);
    void initParam(AxisStigmaContentModel* param);

    void resetToComponent();

    QString collectData();

    void setGroup(const QString&);

signals:
    void shouldHideParent(bool);
};
#endif
