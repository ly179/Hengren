#ifndef CURSOR_HANDLER_H
#define CURSOR_HANDLER_H

#include "sem_common.h"
struct OperationResult;
struct ImageAddtionDataModel;
namespace cdsem {
struct TmpImageData;
}

class CursorHandler {
protected:
    virtual void doAutoMeasure(const sem::ONE_PARAM_ARG_FUNC<const OperationResult&>& callback, const cdsem::TmpImageData& imageData, const ImageAddtionDataModel& addition) = 0;
public:
    CursorHandler();
    virtual ~CursorHandler() noexcept;

    void startAutoMeasure(const sem::ONE_PARAM_ARG_FUNC<const OperationResult&>& callback);

    virtual void collectCenters() = 0;

    virtual void lockAcdCursorsMouse(bool = true) = 0;

    virtual void sortCursors() = 0;

    virtual void allAmpCursorToCenter() = 0;

    virtual void notifyAmpChanged(void* pPtr) = 0;
};

#endif // CURSOR_HANDLER_H
