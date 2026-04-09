#ifndef MULTI_SHOT_CHIP_LIST_MODEL_H
#define MULTI_SHOT_CHIP_LIST_MODEL_H

#include "cdsem_global_data.h"
#include "view_data_mode.h"
#include "ui_common_global.h"

class UI_COMMON_EXPORT MultiShotChipListModel : public SimpleRowItemModel<cdsem::PyShotChipCoordinate> {
public:
    explicit MultiShotChipListModel(bool asChip, QObject* parent = nullptr);

    virtual ~MultiShotChipListModel() noexcept;

private:
    bool asChip = false;
    virtual QVariant getItemData(
        qint32 row) const override;
};
#endif // MULTI_SHOT_CHIP_LIST_MODEL_H
