#ifndef IDP_D_NO_MEASUREMENT_CONTROLLER_H
#define IDP_D_NO_MEASUREMENT_CONTROLLER_H
#include "cde90_window.h"
#include "common_act_param_helper.h"
#include "idp_data_model.h"

namespace cdsem {
class UiIdpDNoController;
}
class DNoModifyInputValidator;

struct IdpDNoMeasurementControllerDataHelper;
class IdpDNoMeasurementController : public CDE90Window {
    CDSEM_OK_CALL
private:
    cdsem::UiIdpDNoController* ui = nullptr;
    IdpDNoMeasurementControllerDataHelper* dataHelper = nullptr;

    void create();
    void createAction();

    void closeEvent(QCloseEvent* event) override;
public:
    explicit IdpDNoMeasurementController(QWidget* parent = nullptr);
    ~IdpDNoMeasurementController() noexcept;

    void setData(const QSharedPointer<IdpMpPosListData>& posData,
                 const QSharedPointer<IdpMpParamListData>& detailData,
                 int index);

    bool checkSameData(const QSharedPointer<IdpMpPosListData>& posData) const;
    int getIndex() const;

    //ok callback使用
    QString modifyData();
};
#endif
