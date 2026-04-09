#include "image_view_measure_tool_draw_helper.h"
#include "image_operation_widget_canvas.h"
#include "result_data_save_model.h"
#include "image_measurement_double_rect_tool.h"
#include "image_measurement_rect_text_tool.h"
#include "image_measurement_annulus_tool.h"
#include "image_measurement_cross_tool.h"
#include "stream_image_supplier.h"

MeasurementSize getDoubleBoxSize(ImageMeasurementDoubleRectTool* measurementTool) {
    MeasurementPoint leftTopPos = measurementTool->leftTopBoxPos();
    MeasurementPoint rightBottomPos = measurementTool->rightBottomBoxPos();
    MeasurementSize rightBottomSize = measurementTool->getRightBottomSize();
    MeasurementSize widthToolSize;
    if (measurementTool->getOrientation() == Qt::Horizontal) {
        widthToolSize = {rightBottomPos.x + rightBottomSize.width - leftTopPos.x, rightBottomSize.height};
    } else {
        widthToolSize = {rightBottomSize.width, rightBottomPos.y + rightBottomSize.height - leftTopPos.y};
    }
    return widthToolSize;
}

MeasurementPoint calcCenterPos(const MeasurementSize& size, const QSize& sceneSize) {
    return MeasurementPoint{sceneSize.width() / 2 - qint32(size.width / 2), sceneSize.height() / 2 - qint32(size.height / 2)};
}

MeasurementPoint calcCenterPos(const MeasurementSize& size, const MeasurementSize& sceneSize) {
    return MeasurementPoint{sceneSize.width / 2 - size.width / 2, sceneSize.height / 2 - size.height / 2};
}

MeasurementPoint calcCenterPos(const MeasurementSize& size, ImageOperationWidgetCanvas* canvas) {
    QSize oSize = canvas->imageStream()->currentImageSize();
    QSize sceneSize;
    if (oSize.isNull()) {
        sceneSize = QSize(canvas->canvasWidth(), canvas->canvasHeight());
    } else {
        sceneSize = oSize;
    }
    return calcCenterPos(size, sceneSize);
}


void moveDoubleBoxToCenter(ImageMeasurementDoubleRectTool* measurementTool, ImageOperationWidgetCanvas* canvas, bool update) {
    measurementTool->move(calcCenterPos(getDoubleBoxSize(measurementTool), canvas), update);
}

void moveMeasuremntToolByCenter(ImageMeasurementTool* measurementTool, const QPoint& center, bool update) {
    {
        ImageMeasurementDoubleRectTool* tool = dynamic_cast<ImageMeasurementDoubleRectTool*>(measurementTool);
        if (tool) {
            MeasurementPoint lpos = tool->leftTopBoxPos();
            MeasurementPoint rpos = tool->rightBottomBoxPos();
            MeasurementSize size = tool->leftTopBoxSize();
            MeasurementPoint diff = MeasurementPoint{MeasurementNumber(center.x()) - (lpos.x + (rpos.x + size.width - lpos.x) / 2),  MeasurementNumber(center.y()) - (lpos.y + (rpos.y + size.height - lpos.y) / 2)};
            tool->move(MeasurementPoint{lpos.x + diff.x, lpos.y + diff.y}, update);
            return;
        }
    }
    {
        ImageMeasurementAnnulusTool* tool = dynamic_cast<ImageMeasurementAnnulusTool*>(measurementTool);
        if (tool) {
            tool->moveCenter(MeasurementPoint{center.x(), center.y()}, update);
            return;
        }
    }
    {
        ImageMeasurementCircleTool* tool = dynamic_cast<ImageMeasurementCircleTool*>(measurementTool);
        if (tool) {
            tool->setCenter(MeasurementPoint{center.x(), center.y()}, update);
            return;
        }
    }
    {
        ImageMeasurementRectTool* tool = dynamic_cast<ImageMeasurementRectTool*>(measurementTool);
        if (tool) {
            MeasurementSize size = tool->getSize();
            MeasurementPoint pos = tool->getPos();
            MeasurementPoint diff = MeasurementPoint{MeasurementNumber(center.x()) - (pos.x + size.width / 2), MeasurementNumber(center.y()) - (pos.y + size.height / 2)};
            tool->move(MeasurementPoint{pos.x + diff.x, pos.y + diff.y}, update);
            return;
        }
    }
}

void drawResultCross(ImageOperationCanvas* canvas, const QList<MeasureResultData>& measureList, QList<ImageMeasurementCrossTool*>* resultList) {
    if (measureList.isEmpty()) return;
    ImageMeasurementCrossTool* cross;
    if (resultList) {
        resultList->reserve(resultList->size() + measureList.size() * 2);
    }
    for (const MeasureResultData& result : measureList) {
        for (const BaseResultData& data : result.baseResults) {
            if (data.lValid) {
                cross = new ImageMeasurementCrossTool(canvas);
                if (resultList)
                    resultList->append(cross);
                cross->setCrossSize({20, 20}, false);
                cross->setCenter({int(data.lLocation.x()), int(data.lLocation.y())}, false);
                cross->setColor(QColor(255, 0, 0, 255), false);
                cross->setWeight(2, false);
                cross->disableMouseMove();
                canvas->addMeasurementTool(cross);
            }
            if (data.rValid) {
                cross = new ImageMeasurementCrossTool(canvas);
                if (resultList)
                    resultList->append(cross);
                cross->setCrossSize({20, 20}, false);
                cross->setCenter({int(data.rLocation.x()), int(data.rLocation.y())}, false);
                cross->setColor(QColor(255, 0, 0, 255), false);
                cross->setWeight(2, false);
                cross->disableMouseMove();
                canvas->addMeasurementTool(cross);
            }
        }
    }
}

void markDoubleBoxCursor(ImageOperationCanvas* canvas, const ImageMeasureMarkData& data, QList<ImageMeasurementDoubleRectTool*>* resultList) {
    if (data.centers.isEmpty()) return;
    if (resultList)
        resultList->reserve(resultList->size() + data.centers.size());
    for (const ImageMeasureCenters& center : data.centers) {
        ImageMeasurementDoubleRectTool* measurementTool = new ImageMeasurementDoubleRectTool(canvas);
        measurementTool->setLockDistance(false);
        measurementTool->setBorderStyle(MTBS_SOLID_CORNER_DASH_LINE, false);
        measurementTool->setLockDistance(true);
        measurementTool->setBorderStyle(MTBS_DASH, false);
        measurementTool->setMouseTransparent(true);
        measurementTool->setOrientation(data.cursorSize.direction);
        measurementTool->setCrossSize(QSize(20, 20));
        measurementTool->setWeight(2, false);
        measurementTool->setColor(QColor(255, 255, 255, 125), false);
        measurementTool->setOverLine(MTBOP_INNER, false);
        measurementTool->setCrossStyle(MTCS_SOLID, false);
        for (int i = 0; i < data.cursorSize.sizes.size(); ++ i) {
            const FloatNumber2& n = data.cursorSize.sizes[i];
            switch (i) {
            case 0:
                measurementTool->setSearchArea(n, false);
                break;
            case 1:
                measurementTool->setSumLine(n, false);
                break;
            case 2:
                measurementTool->setDesignValue(n, false);
                break;
            default:
                break;
            }
        }
        moveMeasuremntToolByCenter(measurementTool, center.point, false);
        canvas->addMeasurementTool(measurementTool);
        if (resultList)
            resultList->append(measurementTool);
    }
}

void markSimpleBoxCursor(ImageOperationCanvas* canvas, const ImageMeasureMarkData& data, QList<ImageMeasurementRectTool*>* resultList) {
    if (data.centers.isEmpty()) return;
    if (resultList)
        resultList->reserve(resultList->size() + data.centers.size());
    for (const ImageMeasureCenters& center : data.centers) {
        ImageMeasurementRectTextTool* measurementTool = new ImageMeasurementRectTextTool(canvas);
        measurementTool->setBorderStyle(MTBS_DASH, false);
        measurementTool->setCrossSize(QSize(20, 20));
        measurementTool->setWeight(2, false);
        measurementTool->setColor(QColor(255, 255, 255, 125), false);
        measurementTool->setMouseTransparent(true);

        measurementTool->setOverLine(MTBOP_INNER, false);
        measurementTool->setCrossStyle(MTCS_SOLID, false);
        FloatNumber2 width = 0;
        FloatNumber2 height = 0;
        for (int i = 0; i < data.cursorSize.sizes.size(); ++ i) {
            const FloatNumber2& n = data.cursorSize.sizes[i];
            switch (i) {
            case 0:
                width = n;
                break;
            case 1:
                height = n;
                break;
            default:
                break;
            }
        }

        measurementTool->setSize({width, height});
        moveMeasuremntToolByCenter(measurementTool, center.point, false);
        canvas->addMeasurementTool(measurementTool);
        if (resultList)
            resultList->append(measurementTool);
    }
}

void markSimpleBoxCursor(ImageOperationCanvas* canvas, const QRect& rect, ImageMeasurementRectTool** result) {
    ImageMeasurementRectTextTool* measurementTool = new ImageMeasurementRectTextTool(canvas);
    measurementTool->setBorderStyle(MTBS_DASH, false);
    measurementTool->setWeight(2, false);
    measurementTool->setColor(QColor(255, 255, 255, 125), false);
    measurementTool->setMouseTransparent(true);

    measurementTool->setOverLine(MTBOP_INNER, false);
    measurementTool->setCrossStyle(MTCS_NONE, false);

    measurementTool->setSize({rect.width(), rect.height()});
    measurementTool->move({rect.x(), rect.y()});
    canvas->addMeasurementTool(measurementTool);
    if (result)
        *result = measurementTool;
}

void markAnnulayBoxCursor(ImageOperationCanvas* canvas, const ImageMeasureMarkData& data, QList<ImageMeasurementAnnulusTool*>* resultList) {
    if (data.centers.isEmpty()) return;
    if (resultList)
        resultList->reserve(resultList->size() + data.centers.size());
    for (const ImageMeasureCenters& center : data.centers) {
        ImageMeasurementAnnulusTool* measurementTool = new ImageMeasurementAnnulusTool(canvas);

        measurementTool->setCrossSize(QSize(20, 20));
        measurementTool->setWeight(2, false);
        measurementTool->setColor(QColor(255, 255, 255, 125), false);
        measurementTool->setCrossStyle(MTCS_SOLID, false);
        measurementTool->setMouseTransparent(true);

        measurementTool->ignoreInnerCircle();

        for (int i = 0; i < data.cursorSize.sizes.size(); ++ i) {
            const FloatNumber2& n = data.cursorSize.sizes[i];
            switch (i) {
            case 0:
                measurementTool->setDesignValue(n);
                break;
            case 1:
                measurementTool->setSearchArea(n);
                break;
            default:
                break;
            }
        }
        moveMeasuremntToolByCenter(measurementTool, center.point, false);
        measurementTool->toolIsCircle(true);
        canvas->addMeasurementTool(measurementTool);
        if (resultList)
            resultList->append(measurementTool);
    }
}
