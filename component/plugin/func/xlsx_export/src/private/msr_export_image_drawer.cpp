#include "msr_export_image_drawer.h"

#include "image_save_model.h"
#include "image_measurement_distance_tool.h"
#include "image_operation_image_canvas.h"
#include "image_measurement_result_tool.h"
#include "image_view_measure_tool_draw_helper.h"
#include "image_measurement_cross_tool.h"


void drawDesign(const ImageMeasureCenters& c, ImageOperationImageCanvas* canvas) {
    if (c.l1 == nullptr || c.l2 == nullptr || c.l3 == nullptr || c.l4 == nullptr) return;
    ImageMeasurementDistanceTool* dTool;
    for (int i = 0; i < 2; ++ i) {
        dTool = new ImageMeasurementDistanceTool(canvas);
        dTool->setLineWeight(2, false);
        dTool->setColor(QColor(255, 255, 255, 125), false);
        dTool->setLineStyle(MTBS_DASH, false);
        dTool->setOverLine(MTBOP_CENTER, false);
        dTool->setMouseTransparent(true);
        if (i == 0) {
            dTool->setOrientation(Qt::Horizontal);
            dTool->setLine1(c.l1);
            dTool->setLine2(c.l2);
        } else {
            dTool->setOrientation(Qt::Vertical);
            dTool->setLine1(c.l3);
            dTool->setLine2(c.l4);
        }
        canvas->addMeasurementTool(dTool);
    }
}

void drawMeasureCursor(const ImageMeasureMarkData& m, ImageOperationImageCanvas* canvas) {
    if (m.cursorSize.cursorType == IMMCT_DOUBLE_BOX) {
        markDoubleBoxCursor(canvas, m);
    } else if (m.cursorSize.cursorType == IMMCT_SINGLE_BOX) {
        markSimpleBoxCursor(canvas, m);
    } else if (m.cursorSize.cursorType == IMMCT_ANNULAR) {
        markAnnulayBoxCursor(canvas, m);
    }
}

void drawEdgeCrossCursors(const ImageMeasureMarkData& m, ImageOperationImageCanvas* canvas) {
    QList<ImageMeasurementCrossTool*> list;
    drawResultCross(canvas, m.resultData.measureResults, &list);
    for (ImageMeasurementCrossTool* c : list) {
        c->setColor(Qt::white, false);
    }
}

void drawDesignCursors(const ImageMeasureMarkData& m, ImageOperationImageCanvas* canvas) {
    for (const ImageMeasureCenters& c : m.centers) {
        drawDesign(c, canvas);
    }
}

inline void drawProfiles(const ImageMeasureMarkData& m, ImageOperationImageCanvas* canvas) {
    canvas->updateProfileLines(m.resultData.profile, QColor(255, 255, 255, 125));
}

void drawResult(const ImageMeasureMarkData& m, ImageOperationImageCanvas* canvas) {
    QString text;
    qreal result = m.resultData.result;
    Qt::GlobalColor color;
    if (result < 0) {
        text = "Error";
        color = Qt::red;
    } else {
        text = toMinQString(result, 3).append("nm");
        color = Qt::white;
    }

    ImageMeasurementResultTool* resultTool = new ImageMeasurementResultTool(canvas);
    resultTool->setMargin(MeasurementMargin {10, 10, 10, 10}, false);
    resultTool->setFontSize(16, false);
    resultTool->setColor(color, false);
    resultTool->setTextAlignment(MRTO_RIGHT_BOTTOM, false);
    resultTool->setText(text, false);
    canvas->addMeasurementTool(resultTool);
}

bool drawMeasureOverlayCursors(const ImageAddtionDataModel& addition, ImageOperationImageCanvas* canvas, const QImage& background, int index, QImage& result) {
    const ImageMarkData& markData = addition.markInfo;
    if (index >= 0 && index < markData.measureCursors.size()) {
        canvas->setImage(background);
        const ImageMeasureMarkData& m = markData.measureCursors[index];
        drawMeasureCursor(m, canvas);
        drawEdgeCrossCursors(m, canvas);
        drawDesignCursors(m, canvas);
        drawResult(m, canvas);
        drawProfiles(m, canvas);
        canvas->paint(result);
        return true;
    }
    return false;
}
