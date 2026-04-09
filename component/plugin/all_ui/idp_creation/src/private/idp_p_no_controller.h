#ifndef IDP_P_NO_CONTROLLER_H
#define IDP_P_NO_CONTROLLER_H
#include "cde90_window.h"
#include "common_act_param_helper.h"
#include "view_data_mode.h"

namespace cdsem {
class UiIdpPDNoContent;
}

struct IdpPatternNumberData {
    QString name;
    qint64 id;
    int before;
    int after;
};

class IdpPNoDataTableModel : public SimpleRowTableModel<IdpPatternNumberData> {
private:
    QVariant getItemData(qint32 row, qint32 column) const;

public:
    IdpPNoDataTableModel(QObject* parent);
};

class IdpPNoController : public CDE90Window {
    CDSEM_OK_CALL
private:
    cdsem::UiIdpPDNoContent* ui = nullptr;
    IdpPNoDataTableModel* tableModel = nullptr;

    void create();
    void createAction();
public:
    explicit IdpPNoController(QWidget* parent = nullptr);
    virtual ~IdpPNoController() noexcept;

    IdpPNoDataTableModel* getTableModel() const;
};
#endif
