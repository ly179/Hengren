#ifndef ELLIPSE_CURSOR_HANDLER_H
#define ELLIPSE_CURSOR_HANDLER_H

#include "cursor_handler.h"

struct AmpEllipseParam;
struct ImageOperationControllerDataHelper;
struct ImageOperationControllerUiHelper;
class QObject;
class EllipseCursorHandler : public CursorHandler{
private:
    ImageOperationControllerDataHelper* dataHelper;
    void doAutoMeasure(const sem::ONE_PARAM_ARG_FUNC<const OperationResult&>& callback, const cdsem::TmpImageData& imageData, const ImageAddtionDataModel& addition) override;

    void collectCenters() override;
    void lockAcdCursorsMouse(bool = true) override;
    void sortCursors() override;
    void allAmpCursorToCenter() override;
    void notifyAmpChanged(void* pPtr) override;
public:
    explicit EllipseCursorHandler(ImageOperationControllerDataHelper* dataHelper);
    ~EllipseCursorHandler() noexcept;

    void markEdgeCursors();
    void markEdgeOneBoxTools(AmpEllipseParam& param);
};

#endif // ELLIPSE_CURSOR_HANDLER_H
