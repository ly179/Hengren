#ifndef IMAGE_OVERLAY_CONTROLLER_H
#define IMAGE_OVERLAY_CONTROLLER_H
#include "cde90_window.h"
#include "overlay_control_model.h"

namespace cdsem {
class UiImageOverlayController;
}

enum OverlaySelectedType {
    OST_UNKNOW,
    OST_PROFILE_LINE,
    OST_EDGE_CROSS_MARK,
    OST_DESIGN_MARK,
    OST_CURSOR_MARK,
    OST_VIEW_RESULT,
    OST_PATTERN_AREA
};

struct ImageMeasureMarkData;
struct ImageOverlayDataModel : public OverlayControlModel{
    const ImageMeasureMarkData* data = nullptr;

    ImageOverlayDataModel() = default;
    ImageOverlayDataModel(const ImageOverlayDataModel&) = default;
    ImageOverlayDataModel(ImageOverlayDataModel&&) = default;
    ImageOverlayDataModel& operator=(const ImageOverlayDataModel&) = default;
    ImageOverlayDataModel& operator=(ImageOverlayDataModel&&) = default;
    ImageOverlayDataModel& operator=(const OverlayControlModel& other) {
        if (this != &other) {
            profileLine = other.profileLine;
            edgeCrossMark = other.edgeCrossMark;
            designMark = other.designMark;
            cursorMark = other.cursorMark;
            viewResult = other.viewResult;
        }
        return *this;
    }
};

struct ImageOverlayControllerDataHelper;
class ImageOverlayController : public CDE90Window{
    friend struct ImageOverlayControllerDataHelper;
    Q_OBJECT
private:
    cdsem::UiImageOverlayController* ui = nullptr;
    ImageOverlayControllerDataHelper* dataHelper = nullptr;
    void create();
    void createAction();
public:
     explicit ImageOverlayController(QWidget* parent = nullptr);
     ~ImageOverlayController() noexcept;

     void setDataPtr(const QList<ImageOverlayDataModel>& list, int selectedRow = 0, bool blockSignal = true);
     void selectRow(int);
     void clear();

     void setDefaultSetting(OverlaySelectedType, bool);

 signals:
     void overlaySelectedType(int i, OverlaySelectedType type, bool checked);
     void overlaySelectedRow(int row);

     void overlaySettingChanged(OverlaySelectedType, bool);
};
#endif
