#include "cursor_handler.h"
#include "cdsem_global_data.h"
#include "image_operation_data_model.h"

CursorHandler::CursorHandler() {}

CursorHandler::~CursorHandler() noexcept {}

void CursorHandler::startAutoMeasure(const sem::ONE_PARAM_ARG_FUNC<const OperationResult&>& callback) {
    cdsem::CDSEM_GlobalDataHelper::get()->getImage(IOPT->getImageOperationType(), [this, callback](const cdsem::TmpImageData& imageData, const ImageAddtionDataModel& addition){
        doAutoMeasure(callback, imageData, addition);
    });
}
