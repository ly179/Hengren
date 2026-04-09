#ifndef MSR_EXPORT_IMAGE_DRAWER_H
#define MSR_EXPORT_IMAGE_DRAWER_H

struct ImageAddtionDataModel;
class ImageOperationImageCanvas;
class QImage;
bool drawMeasureOverlayCursors(const ImageAddtionDataModel& addition, ImageOperationImageCanvas* canvas, const QImage& background, int index, QImage& result);

#endif // MSR_EXPORT_IMAGE_DRAWER_H
