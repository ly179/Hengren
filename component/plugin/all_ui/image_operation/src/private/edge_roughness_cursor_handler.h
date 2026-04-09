#ifndef EDGE_ROUGHNESS_CURSOR_HANDLER_H
#define EDGE_ROUGHNESS_CURSOR_HANDLER_H

#include "cursor_handler.h"

struct AmpEdgeRoughnessParam;
struct ImageOperationControllerDataHelper;
struct ImageOperationControllerUiHelper;
class QObject;

class EdgeRoughnessCursorHandler : public CursorHandler{
private:
    ImageOperationControllerDataHelper* dataHelper;
    void doAutoMeasure(const sem::ONE_PARAM_ARG_FUNC<const OperationResult&>& callback, const cdsem::TmpImageData& imageData, const ImageAddtionDataModel& addition) override;

    void collectCenters() override;
    void lockAcdCursorsMouse(bool = true) override;
    void sortCursors() override;
    void allAmpCursorToCenter() override;
    void notifyAmpChanged(void* pPtr) override;
public:
    explicit EdgeRoughnessCursorHandler(ImageOperationControllerDataHelper* dataHelper);
    ~EdgeRoughnessCursorHandler() noexcept;

    void markEdgeCursors();
    void markEdgeDoubleBoxTools(AmpEdgeRoughnessParam& widthParam);
    void markEdgeOneBoxTools(AmpEdgeRoughnessParam& widthParam);
    void markEdgeLineResultTools();
    void markEdgeLineTools();

};

#endif // EDGE_ROUGHNESS_CURSOR_HANDLER_H
