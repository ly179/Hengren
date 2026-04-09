#include "multi_chip_load_helper.h"
#include "multi_shot_chip_select_controller.h"
#include "cde90_message_box.h"
#include "cdsem_global_data.h"

MultiChipLoadHelper::MultiChipLoadHelper(QObject* parent) : QObject(parent) {
    content = qobject_cast<QWidget*>(parent);
}
MultiChipLoadHelper::~MultiChipLoadHelper() noexcept {}

void MultiChipLoadHelper::createMultiChipSelector() {
    if (multiShotChipSelector) return;
    multiShotChipSelector = new MultiShotChipSelectController(true, content);
    multiShotChipSelector->setWindowTitle("Overlapping Chips Selector");
    multiShotChipSelector->setWindowModality(Qt::WindowModal);
    multiShotChipSelector->setCloseCall([this](CDE90WindowCloseStatus status){
        if (status == HW_OK) {
            if (pyHandlerDisabled) {
                simpleMessageBox(content, "Info", "Current IDW has been unloaded.");
                return true;
            }
            cdsem::PyShotChipCoordinate* data = multiShotChipSelector->getSelectedData();
            if (!data) {
                simpleMessageBox(multiShotChipSelector, "Error", "Please select one chip.");
                return false;
            }
            callback(*data);
        }
        multiShotChipSelector->clearListData();
        return true;
    });
}

void MultiChipLoadHelper::setPyHandlerDisable(bool b) {
    pyHandlerDisabled = b;
}

OperationResult MultiChipLoadHelper::openSelector(const sem::ONE_PARAM_ARG_FUNC<const cdsem::PyShotChipCoordinate&>& callback, int currChip) {
    if (currChip < 0) {
        simpleMessageBox(content, "Error", "Empty chip data.");
        return OperationResult{"Empty chip data.", DATA_NOT_FOUND_ERROR_CODE};
    }
    cdsem::PyStageShotChipCoordinate currentChips = cdsem::WaferStageCoordinateConvert::instance()->getShotChipWaferUm(false);
    QList<cdsem::PyShotChipCoordinate> chips;
    if (currentChips.list.size() > 0) {
        chips.reserve(currentChips.list.size());
        for (cdsem::PyShotChipCoordinate& p : currentChips.list) {
            if (p.globalChip < 0) continue;
            chips.append(p);
            if (p.globalChip == currChip) {
                callback(p);
                return OperationResult{};
            }
        }
    }
    if (chips.isEmpty()) {
        return OperationResult{"No chip data detected. Defaulting to assigned registration coordinates.", DATA_NOT_FOUND_ERROR_CODE};
    } else {
        if (pyHandlerDisabled) {
            return OperationResult{ "Current IDW has been unloaded.", LOAD_DATA_ERROR_CODE};
        }
        if (chips.size() == 1) {
            callback(chips.first());
        } else {
            this->callback = callback;
            createMultiChipSelector();
            multiShotChipSelector->setListData(chips);
            multiShotChipSelector->show();
        }
    }
    return OperationResult{};
}


