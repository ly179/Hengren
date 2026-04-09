#ifndef IDP_MP_DATA_TABLE_MODEL_H
#define IDP_MP_DATA_TABLE_MODEL_H

#include "idp_data_model.h"
#include "view_data_mode.h"

//表格分裂。即把IDP MP中的子测量分裂出来到表格中展示，如果不存在子测量，则显示一个空壳（没有测量数据的记录）。
//P No. -> Chip -> Measures
//如果不存在测量数据，则每个P No.下的每个Chip仅显示一条空壳记录。
struct IdpMpTableModeData {
    QSharedPointer<IdpMpPosListData> posData;
    QSharedPointer<IdpMpParamListData> detailParamData;
    QString dNumbers;
};
class CDE90TableView;

class IdpMpDataTableModel : public SimpleRowTableModel<IdpMpTableModeData> {
private:
    QVariant getItemData(qint32 row, qint32 column) const override;

public:
    explicit IdpMpDataTableModel(CDE90TableView* parent);
    ~IdpMpDataTableModel() noexcept;
};

#endif // IDP_MP_DATA_TABLE_MODEL_H
