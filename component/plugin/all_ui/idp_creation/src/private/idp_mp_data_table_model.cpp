#include "idp_mp_data_table_model.h"
#include <cde90_table_view.h>

IdpMpDataTableModel::IdpMpDataTableModel(CDE90TableView* parent)
    : SimpleRowTableModel(parent) {
    insertHead("No.", 0, 0);
    insertHead("Chip No.", 1, 4);
    insertHead("Coordinate X,Y", 5, 8);
    insertHead("Name", 9, 9);
    insertHead("P No.", 10, 10);
    insertHead("D No.", 11, 11);
    parent->replaceDataModel(this);

    QList<int> columnsSize;
    columnsSize.reserve(12);
    columnsSize << 40 << 30 << 5 << 30 << 5 << 80 << 5 << 80 << 5 << 100 << 50 << 50;
    parent->setMinColumnsSize(columnsSize);
    parent->recalcColumnSize();
}

IdpMpDataTableModel::~IdpMpDataTableModel() noexcept {}

QVariant IdpMpDataTableModel::getItemData(qint32 row, qint32 column) const {
    const IdpMpTableModeData& data = getDataList()[row];
    IdpMpParamListData* detailParam = nullptr;
    if (!data.detailParamData.isNull()) {
        detailParam = data.detailParamData.data();
    }
    QString str;
    switch (column) {
    case 0:
        return row + 1;
    case 1:
        return data.posData->chipX == nullptr ? "-" : data.posData->chipX.toString();
    case 2:
        return ",";
    case 3:
        return data.posData->chipY == nullptr ? "-" : data.posData->chipY.toString();
    case 4:
        if (!data.detailParamData) {
            return QVariant();
        }
        return data.detailParamData->dPointType == 1 ? "R" : "";
    case 5:
        if (detailParam && detailParam->dPoint.x != nullptr) {
            return detailParam->dPoint.x.toMinString();
        } else if (data.posData->oPoint.x != nullptr) {
            return data.posData->oPoint.x.toMinString();
        }
        // if (detailParam && detailParam->dPoint.x != nullptr && data.posData->oPoint.x != nullptr) {
        //     return (detailParam->dPoint.x + data.posData->oPoint.x).toMinString();
        // } else if (detailParam && detailParam->dPoint.x != nullptr) {
        //     return detailParam->dPoint.x.toMinString();
        // } else if (data.posData->oPoint.x != nullptr) {
        //     data.posData->oPoint.x.toMinString();
        // }
        return QVariant();
    case 6:
        return ",";
    case 7:
        // if (detailParam && detailParam->dPoint.y != nullptr && data.posData->oPoint.y != nullptr) {
        //     return (detailParam->dPoint.y + data.posData->oPoint.y).toMinString();
        // } else if (detailParam && detailParam->dPoint.y != nullptr) {
        //     return detailParam->dPoint.y.toMinString();
        // } else if (data.posData->oPoint.y != nullptr) {
        //     data.posData->oPoint.y.toMinString();
        // }
        if (detailParam && detailParam->dPoint.y != nullptr) {
            return detailParam->dPoint.y.toMinString();
        } else if (data.posData->oPoint.y != nullptr) {
            return data.posData->oPoint.y.toMinString();
        }
        return QVariant();
    case 8:
        if (!data.detailParamData) {
            return QVariant();
        }
        return data.detailParamData->usePrevPoint ? "D" : "";
    case 9:
        if (data.detailParamData) return data.detailParamData->viewMpName;
        return QVariant();
    case 10:
        if (detailParam) {
            return detailParam->pNumber;
        } else {
            return "0";
        }
    case 11:
        return data.dNumbers;
    }
    return "";
}
