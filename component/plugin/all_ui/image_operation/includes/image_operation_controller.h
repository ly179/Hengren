#ifndef IMAGE_OPERATION_CONTROLLER_H
#define IMAGE_OPERATION_CONTROLLER_H

#include "relation_image_operation_controller.h"
#include "ui_image_operation_global.h"
#include "measurement_tool_defition.h"
namespace cdsem {
class UiImageOperationController;
}
class ImageMeasurementTool;
class ImageMeasurementRectTool;
class ImageMeasurementRectTextTool;
class ImageMeasurementAnnulusTool;
class ImageMeasurementCircleTool;
class ImageMeasurementCrossTool;
class ImageMeasurementDoubleRectTool;
class ImageMeasurementSectorTool;
class ImageMeasurementLinesTool;
class QGridLayout;
class QVBoxLayout;
class QHBoxLayout;
class CDE90Label;
class CDE90ComboBox;
class CDE90ButtonBox;
class CDE90Button;
class CDE90TextFieldBox;
class ImageOperationWidgetCanvas;
class ParamHorizontalSlider;
class ImageOperationCustomSetterController;
class ImageOperationViewSetterController;
class ImageCanvasBorderWidget;
class ImageMeasurementDistanceTool;
struct AmpMeasureCursors;
class ScanModeDataModel;
class QSpacerItem;
class OpticsEditController;
class StreamImageSupplier;
class QValidator;
class CDE90Menu;
struct ImageResultData;

struct ImageOperationControllerDataHelper;
struct ImageOperationControllerUiHelper;

class UI_IMAGE_OPERATION_EXPORT ImageOperationController : public RelationImageOperationController{
    friend struct ImageOperationControllerDataHelper;
    friend struct ImageOperationControllerUiHelper;
    friend class WidthCursorHandler;
    friend class WidthRoughnessCursorHandler;
    friend class EdgeRoughnessCursorHandler;
    friend class DiameterHoleCursorHandler;
    friend class EllipseCursorHandler;
private:
    cdsem::UiImageOperationController* ui = nullptr;
    ImageOperationControllerUiHelper* uiHelper = nullptr;

    ImageOperationControllerDataHelper* dataHelper = nullptr;

    image_operation::RectCursorType rectCursorStatus = image_operation::RECT_CURSOR_32_192;

    //1 beamShift for localCrossBtn
    //2 stageMove for localCrossBtn
    int localShiftMode = 0;

    bool resetRectCursorMode = true;

    bool isTakingPhoto = false;

    void create();
    void createAction();
    void triggerAction();

    void createDistenceTool(Qt::Orientation);
    void createRectAreaTool();
    void updateRectToolParam(ImageMeasurementRectTool* rectTool);
    void createCrossTool();
    void clearTool();
    //0 - 5，分别代表view1-view2，integ1-integ4

    //传入0 - 5；
    //0 - view1
    //1 - view2
    //2 - integ1
    //...

    void setMag(int);

    void renderMag(int);

    void changeIconLabel();

    void refreshCanvasCursorToolContent();

protected:
    void closeEvent(QCloseEvent*) override;

public:
    ImageOperationController(QWidget* parent = nullptr);
    ~ImageOperationController() noexcept;

    void openViewSetter();
    void openCustomSetter();

    [[nodiscard]] ImageMeasurementRectTool* createMeasurementRectTool();
    [[nodiscard]] ImageMeasurementRectTextTool* createMeasurementRectTextTool();
    [[nodiscard]] ImageMeasurementAnnulusTool* createMeasurementAnnulusTool();
    [[nodiscard]] ImageMeasurementCircleTool* createMeasurementCircleTool();
    [[nodiscard]] ImageMeasurementCrossTool* createMeasurementCrossTool();
    [[nodiscard]] ImageMeasurementDoubleRectTool* createMeasurementDoubleRectTool();
    [[nodiscard]] ImageMeasurementSectorTool* createMeasurementSectorTool();
    [[nodiscard]] ImageMeasurementLinesTool* createMeasurementLinesTool();

    void addMeasurementTool(ImageMeasurementTool* t);
    void removeMeasurementTool(ImageMeasurementTool* t);
    void measurementToolToPrevFloor(ImageMeasurementTool* source, ImageMeasurementTool* target = nullptr);
    void measurementToolToBehindFloor(ImageMeasurementTool* source, ImageMeasurementTool* target = nullptr);
    void measurementToolToTopFloor(ImageMeasurementTool* t);
    void measurementToolToBottomFloor(ImageMeasurementTool* t);

    void createOpticsController();

    void localCrossCursorMode() override;
    void rectCursorMode(const MeasurementRect& pos, image_operation::RectCursorType) override;
    void rectCursorMode(image_operation::RectCursorType) override;
    void blankCursorMode() override;

    QPoint getCrossCursorCenter() override;
    QRect getRectCursorRange() override;

    StreamImageSupplier* imageStream() const;

    void refreshScanParamLabel();

    void reloadData();

    void amsToFirstStep() override;

    void setMeasureResult(const ImageResultData&) override;
    void stopAutoMeasure() override;
    bool collectCursorCenterToTmp() override;

    void reloadImageFromGlobal();
    void showEvent(QShowEvent* event) override;


};

#endif // IMAGE_OPERATION_CONTROLLER_H
