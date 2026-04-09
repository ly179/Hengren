#ifndef RELATION_IMAGE_OPERATION_CONTROLLER_H
#define RELATION_IMAGE_OPERATION_CONTROLLER_H
#include "relation_cde90_window_export.h"
#include "measurement_tool_defition.h"
#include "float_number.h"

namespace image_operation {
    enum RectCursorType {
        RECT_CURSOR_BEGIN = 0,
        RECT_CURSOR_32_192 = 0,
        RECT_CURSOR_64 = 1,
        RECT_CURSOR_192x80 = 2,
        RECT_CURSOR_80x192 = 3,
        RECT_CURSOR_40x96 = 4,
        RECT_CURSOR_96x40 = 5,
        RECT_CURSOR_END = 6,
        __RECT_CURSOR = 0x7FFFFF
    };
}

struct ImageResultData;
__R_WINDOW(RelationImageOperationController)

virtual void localCrossCursorMode() = 0;
virtual void rectCursorMode(const MeasurementRect& pos, image_operation::RectCursorType = image_operation::RECT_CURSOR_32_192) = 0;
virtual void rectCursorMode(image_operation::RectCursorType = image_operation::RECT_CURSOR_32_192) = 0;
virtual void blankCursorMode() = 0;

virtual QPoint getCrossCursorCenter() = 0;
virtual QRect getRectCursorRange() = 0;

virtual void amsToFirstStep() = 0;

virtual void setMeasureResult(const ImageResultData&) = 0;
virtual void stopAutoMeasure() = 0;
virtual bool collectCursorCenterToTmp() = 0;

__R_WINDOW_END

#endif //RELATION_IMAGE_OPERATION_CONTROLLER_H
