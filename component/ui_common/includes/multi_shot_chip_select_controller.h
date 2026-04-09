#ifndef MULTI_SHOT_CHIP_SELECT_CONTROLLER_H
#define MULTI_SHOT_CHIP_SELECT_CONTROLLER_H
#include "cde90_dialog.h"
#include "ui_common_global.h"

namespace cdsem {
class UiMultiShotChipSelectController;
struct PyShotChipCoordinate;
}

struct MultiShotChipSelectDataHelper;
class UI_COMMON_EXPORT MultiShotChipSelectController : public CDE90Dialog{

private:
    MultiShotChipSelectDataHelper* dataHelper = nullptr;
    cdsem::UiMultiShotChipSelectController* ui = nullptr;

    void create();
    void createAction();
public:
    explicit MultiShotChipSelectController(bool asChip, QWidget* parent = nullptr);
    ~MultiShotChipSelectController() noexcept;

    void setListData(const QList<cdsem::PyShotChipCoordinate>&);
    void clearListData();
    cdsem::PyShotChipCoordinate* getSelectedData() const;
};
#endif
