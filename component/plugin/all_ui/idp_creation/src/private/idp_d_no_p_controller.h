#ifndef IDP_D_NO_P_CONTROLLER_H
#define IDP_D_NO_P_CONTROLLER_H

#include "cde90_window.h"
#include "common_act_param_helper.h"
#include "view_data_mode.h"

namespace cdsem {
class UiIdpPDNoContent;
}
class DNoModifyInputValidator;

struct IdpDByPData {
    QString before;
    QString after;
    QString name;
    qint64 id;
    qint32 pNo;
    bool updated = false;
};

class IdpDNoByPDataTableModel : public SimpleRowTableModel<IdpDByPData> {
private:
    QVariant getItemData(qint32 row, qint32 column) const override;
    QVariant data(
        const QModelIndex& index, int role = Qt::DisplayRole) const override;

public:
    IdpDNoByPDataTableModel(QObject* parent);
};

struct IdpDNoByPControllerDataHelper;
class IdpDNoByPController : public CDE90Window {
    CDSEM_OK_CALL
private:
    cdsem::UiIdpPDNoContent* ui = nullptr;
    IdpDNoByPControllerDataHelper* dataHelper = nullptr;

    void create();
    void createAction();

public:
    explicit IdpDNoByPController(QWidget* parent = nullptr);
    ~IdpDNoByPController() noexcept;

    IdpDNoByPDataTableModel* getTableModel() const;
};

#endif // IDP_D_NO_P_CONTROLLER_H
