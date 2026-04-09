#ifndef IDP_SHIFT_COORDINATE_SETTING_CONTROLLER_H
#define IDP_SHIFT_COORDINATE_SETTING_CONTROLLER_H
#include "cde90_dialog.h"
#include "common_act_param_helper.h"
#include "float_number.h"

namespace cdsem {
class UiIdpShiftCoordinateSettingController;
};

class IdpShiftCoordinateSettingController : public CDE90Dialog{

    SEM_OK_CALL(IDW_PointF)
    SEM_SET_INIT_PARAM(IDW_PointF)

private:
    cdsem::UiIdpShiftCoordinateSettingController* ui = nullptr;

    void create();
    void createAction();
    QString collectData();
    void resetToComponent();
public:
     explicit IdpShiftCoordinateSettingController(QWidget* parent = nullptr);
     ~IdpShiftCoordinateSettingController() noexcept;
};
#endif
