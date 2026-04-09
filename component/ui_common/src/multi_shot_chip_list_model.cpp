#include "multi_shot_chip_list_model.h"

MultiShotChipListModel::MultiShotChipListModel(bool asChip, QObject* parent)
        : SimpleRowItemModel(parent), asChip(asChip) {}

MultiShotChipListModel::~MultiShotChipListModel() noexcept {}

QVariant MultiShotChipListModel::getItemData(qint32 row) const{
    if (row < 0) return QVariant();
    const QList<cdsem::PyShotChipCoordinate>& list = getDataList();
    if (row >= list.size()) return QVariant();
    const cdsem::PyShotChipCoordinate& data = list[row];
    if (data.shot == -1 || (asChip && data.chip == -1)) {
        return "Invalid Data";
    }
    if (asChip) {
        return QString("Shot: (%1,%2) Chip: (%3,%4) Number: %5,%6 Offset: %7μm,%8μm Size: %9μm,%10μm")
            .arg(data.shotX).arg(data.shotY)
            .arg(data.chipX).arg(data.chipY)
            .arg(QString::number(data.shot), QString::number(data.chip),
                 data.cx.toQStringAndKeepFullDecimal(), data.cy.toQStringAndKeepFullDecimal(),
                 data.cw.toQStringAndKeepFullDecimal(), data.ch.toQStringAndKeepFullDecimal());
    } else {
        return QString("(%1,%2) Number: %3: Offset: %4μm,%5μm Size: %6μm,%7μm")
            .arg(data.shotX).arg(data.shotY).arg(QString::number(data.shot),
                 data.sx.toQStringAndKeepFullDecimal(), data.sy.toQStringAndKeepFullDecimal(),
                 data.sw.toQStringAndKeepFullDecimal(), data.sh.toQStringAndKeepFullDecimal());
    }
}

