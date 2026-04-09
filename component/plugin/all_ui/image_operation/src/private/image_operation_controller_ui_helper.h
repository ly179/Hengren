#ifndef IMAGE_OPERATION_CONTROLLER_UI_HELPER_H
#define IMAGE_OPERATION_CONTROLLER_UI_HELPER_H

#include "pixel_canvas.h"

class ImageCanvasBorderWidget : public PixelCanvas {
private:
    QPen pen;
    QBrush brush;
    int borderWidth = 10;
    void paintEvent(QPaintEvent* event) override;

public:
    ImageCanvasBorderWidget(QWidget* parent = nullptr);
    ~ImageCanvasBorderWidget() noexcept;

    void setColor(const QColor&);
};

class ImageOperationController;
class CDE90Menu;
class ImageCanvasBorderWidget;
class ImageOperationWidgetCanvas;
class CDE90Button;
class CDE90Label;
class QWidget;
class QSpacerItem;
class ParamHorizontalSlider;
class QValidator;
class ImageOperationCustomSetterController;
class ImageOperationViewSetterController;
class ImageMeasurementTool;
class BeamViewCanvas;
struct ImageOperationControllerUiHelper {
    QColor normalColor;
    QColor lower10k;
    QColor higherEq10k;
    ImageOperationController *d;

    CDE90Menu *semPmMenu;
    CDE90Menu *omPmMenu;
    ImageCanvasBorderWidget *canvasContent;
    ImageOperationWidgetCanvas *canvas;
    CDE90Button *scanParamViewBtn;
    CDE90Label *scanParamLabel;
    QWidget *scanParamWidget;

    QWidget* canvasToolWidget;
    CDE90Button *alignBtn;
    CDE90Button *detectBtn;
    CDE90Button *sortBtn;
    CDE90Button *lockCursorMoveBtn;
    CDE90Button *gatherToCenterBtn;
    CDE90Button *cursorToolViewBtn;
    QSpacerItem *cursorToolLeftMargin;

    ParamHorizontalSlider *focusSlider;
    QValidator *focusTextValidator;

    BeamViewCanvas* beamViewCanvas;

    ImageOperationViewSetterController *viewSetter = nullptr;
    ImageOperationCustomSetterController *customSetter = nullptr;

    ImageMeasurementTool *currentTool = nullptr;

    QWidget *initImageCanvas(QWidget *centerContent);
};

#endif // IMAGE_OPERATION_CONTROLLER_UI_HELPER_H
