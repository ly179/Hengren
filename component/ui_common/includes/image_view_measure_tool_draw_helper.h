#ifndef IMAGE_VIEW_MEASURE_TOOL_DRAW_HELPER_H
#define IMAGE_VIEW_MEASURE_TOOL_DRAW_HELPER_H

#include "ui_common_global.h"
#include "image_measurement_tool_interface.h"

//创建静态光标
//光标不可移动

class ImageMeasurementDoubleRectTool;
class ImageMeasurementRectTool;
class ImageMeasurementAnnulusTool;
class ImageOperationWidgetCanvas;
class ImageMeasurementCrossTool;
struct ImageMeasureMarkData;
struct MeasureResultData;

UI_COMMON_EXPORT MeasurementSize getDoubleBoxSize(ImageMeasurementDoubleRectTool* measurementTool);

UI_COMMON_EXPORT MeasurementPoint calcCenterPos(const MeasurementSize& size, const QSize& sceneSize);

UI_COMMON_EXPORT MeasurementPoint calcCenterPos(const MeasurementSize& size, const MeasurementSize& sceneSize);

UI_COMMON_EXPORT MeasurementPoint calcCenterPos(const MeasurementSize& size, ImageOperationWidgetCanvas* canvas);

UI_COMMON_EXPORT void moveDoubleBoxToCenter(ImageMeasurementDoubleRectTool* measurementTool, ImageOperationWidgetCanvas* canvas, bool update);

UI_COMMON_EXPORT void moveMeasuremntToolByCenter(ImageMeasurementTool* measurementTool, const QPoint& center, bool update);

UI_COMMON_EXPORT void drawResultCross(ImageOperationCanvas* canvas, const QList<MeasureResultData>& measureList, QList<ImageMeasurementCrossTool*>* resultList = nullptr);

UI_COMMON_EXPORT void markDoubleBoxCursor(ImageOperationCanvas* canvas, const ImageMeasureMarkData&, QList<ImageMeasurementDoubleRectTool*>* resultList = nullptr);
UI_COMMON_EXPORT void markSimpleBoxCursor(ImageOperationCanvas* canvas, const ImageMeasureMarkData&, QList<ImageMeasurementRectTool*>* resultList = nullptr);
UI_COMMON_EXPORT void markSimpleBoxCursor(ImageOperationCanvas* canvas, const QRect&, ImageMeasurementRectTool** result = nullptr);
UI_COMMON_EXPORT void markAnnulayBoxCursor(ImageOperationCanvas* canvas, const ImageMeasureMarkData&, QList<ImageMeasurementAnnulusTool*>* = nullptr);

#endif // IMAGE_VIEW_MEASURE_TOOL_DRAW_HELPER_H
