#ifndef DIAMETER_HOLE_CURSOR_HANDLER_H
#define DIAMETER_HOLE_CURSOR_HANDLER_H

#include "cursor_handler.h"

struct AmpDiameterHoleParam;
struct ImageOperationControllerDataHelper;
struct ImageOperationControllerUiHelper;
class QObject;

class DiameterHoleCursorHandler : public CursorHandler{
private:
    int lastKind = -1;
    ImageOperationControllerDataHelper* dataHelper;
    void doAutoMeasure(const sem::ONE_PARAM_ARG_FUNC<const OperationResult&>& callback, const cdsem::TmpImageData& imageData, const ImageAddtionDataModel& addition) override;

    void collectCenters() override;
    void lockAcdCursorsMouse(bool = true) override;
    void sortCursors() override;
    void allAmpCursorToCenter() override;
    void notifyAmpChanged(void* pPtr) override;
public:
    explicit DiameterHoleCursorHandler(ImageOperationControllerDataHelper*);
    ~DiameterHoleCursorHandler() noexcept;

    void markDiameterHoleCursors();
    void markDoubleBoxTools(AmpDiameterHoleParam& diameterHoleParam);
    void markAnnulusBoxTools(AmpDiameterHoleParam& diameterHoleParam);
    void markDiameterHoleLineResultTools();
    void markDiameterHoleLineTools();
};

#endif // DIAMETER_HOLE_CURSOR_HANDLER_H
