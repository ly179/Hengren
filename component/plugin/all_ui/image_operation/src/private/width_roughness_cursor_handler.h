#ifndef WIDTH_ROUGHNESS_CURSOR_HANDLER_H
#define WIDTH_ROUGHNESS_CURSOR_HANDLER_H

#include "cursor_handler.h"

struct AmpWidthRoughnessParam;
struct ImageOperationControllerDataHelper;
class QObject;

class WidthRoughnessCursorHandler : public CursorHandler{
private:
    struct ImageOperationControllerDataHelper* dataHelper;
    void doAutoMeasure(const sem::ONE_PARAM_ARG_FUNC<const OperationResult&>& callback, const cdsem::TmpImageData& imageData, const ImageAddtionDataModel& addition) override;
    void collectCenters() override;
    void lockAcdCursorsMouse(bool = true) override;
    void sortCursors() override;
    void allAmpCursorToCenter() override;
    void notifyAmpChanged(void* pPtr) override;
public:
    explicit WidthRoughnessCursorHandler(ImageOperationControllerDataHelper* dataHelper);
    ~WidthRoughnessCursorHandler() noexcept;

    void markWidthCursors();
    void markWidthDoubleBoxTools(AmpWidthRoughnessParam& widthParam);
    void markWidthOneBoxTools(AmpWidthRoughnessParam& widthParam);
    void markWidthLineResultTools();
    void markWidthLineTools();
};

#endif // WIDTH_ROUGHNESS_CURSOR_HANDLER_H
