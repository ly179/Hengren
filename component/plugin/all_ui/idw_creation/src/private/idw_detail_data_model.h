#ifndef IDW_DETAIL_DATA_MODEL_H
#define IDW_DETAIL_DATA_MODEL_H

#include "view_data_mode.h"
#include "idw_wafer_data_model.h"

class IdwDetailDataMode : public SimpleRowTableModel<IDW_Data_Item> {
private:
    QVariant getItemData(qint32 row, qint32 column) const override;
public:
    IdwDetailDataMode(QObject* parent = nullptr);
};

#endif // IDW_DETAIL_DATA_MODEL_H
