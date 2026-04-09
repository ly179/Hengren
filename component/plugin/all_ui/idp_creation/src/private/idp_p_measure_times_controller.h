#ifndef IDP_P_MEASURE_TIMES_CONTROLLER_H
#define IDP_P_MEASURE_TIMES_CONTROLLER_H
#include "cde90_window.h"
#include "common_act_param_helper.h"

namespace cdsem {
class UiIdpPMeasureTimesController;
}
struct IdpMpMeasureTimesData;
struct IdpMpParamListData;
class PNoTableModel;

class IdpPMeasureTimesController : public CDE90Window{
    CDSEM_OK_CALL
private:
    cdsem::UiIdpPMeasureTimesController* ui = nullptr;
    PNoTableModel* tableModel;

    void create();
    void createAction();
public:
     explicit IdpPMeasureTimesController(QWidget* parent = nullptr);
     ~IdpPMeasureTimesController() noexcept;

     void collectData();
     void resetToComponent(const QList<QSharedPointer<IdpMpParamListData>>&);
};
#endif
