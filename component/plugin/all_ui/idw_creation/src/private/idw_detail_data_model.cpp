#include "idw_detail_data_model.h"
#include "view_data_mode.h"

IdwDetailDataMode::IdwDetailDataMode(QObject* parent) : SimpleRowTableModel(parent) {}

QVariant IdwDetailDataMode::getItemData(qint32 row, qint32 column) const {
    const IDW_Data_Item& item = getDataList()[row];
    switch(column) {
    case 0:
        return item.getNumber();
    case 1:
        return item.getOffsetX().toQStringAndKeepFullDecimal();
    case 2:
        return item.getOffsetY().toQStringAndKeepFullDecimal();
    case 3:
        return item.getSizeX().toQStringAndKeepFullDecimal();
    case 4:
        return item.getSizeY().toQStringAndKeepFullDecimal();
    }
    return "";
}
