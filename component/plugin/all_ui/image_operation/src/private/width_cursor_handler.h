#ifndef WIDTH_CURSOR_HANDLER_H
#define WIDTH_CURSOR_HANDLER_H

#include "cursor_handler.h"

struct AmpWidthParam;
struct ImageOperationControllerDataHelper;
class QObject;

class WidthCursorHandler  : public CursorHandler{
private:
    struct ImageOperationControllerDataHelper* dataHelper;
    void doAutoMeasure(const sem::ONE_PARAM_ARG_FUNC<const OperationResult&>& callback, const cdsem::TmpImageData& imageData, const ImageAddtionDataModel& addition) override;
    void collectCenters() override;
    void lockAcdCursorsMouse(bool = true) override;
    void sortCursors() override;
    void allAmpCursorToCenter() override;
    void notifyAmpChanged(void* pPtr) override;
public:
    explicit WidthCursorHandler(ImageOperationControllerDataHelper* dataHelper);
    ~WidthCursorHandler() noexcept;

    void markWidthCursors();
    void markWidthDoubleBoxTools(AmpWidthParam& widthParam);
    void markWidthOneBoxTools(AmpWidthParam& widthParam);
    void markWidthLineResultTools();
    void markWidthLineTools();
};

#endif // WIDTH_CURSOR_HANDLER_H
