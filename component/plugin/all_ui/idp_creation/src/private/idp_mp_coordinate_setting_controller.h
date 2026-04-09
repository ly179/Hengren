#ifndef IDP_MP_COORDINATE_SETTING_CONTROLLER_H
#define IDP_MP_COORDINATE_SETTING_CONTROLLER_H
#include "cde90_dialog.h"
#include "common_act_param_helper.h"
#include "float_number.h"

struct IdpMpCoordinateSettingParam {
    IDW_PointF dPoint;
    IDW_PointF prevCoordinate;
    Integer32 chipX;
    Integer32 chipY;
    int chipNumber = -1;
    // 0 - coordinate 1 - relative
    int dPointType = 0;
    bool usePrevPoint = false;
};

namespace cdsem {
class UiIdpMpCoordinateSettingController;
};

struct IdpMpCoordinateSettingDataHelper;

class IdpMpCoordinateSettingController : public CDE90Dialog{
    friend struct IdpMpCoordinateSettingDataHelper;
    SEM_SET_INIT_PARAM(IdpMpCoordinateSettingParam)
    SEM_OK_CALL(IdpMpCoordinateSettingParam)

private:
    IdpMpCoordinateSettingDataHelper* dataHelper = nullptr;
    cdsem::UiIdpMpCoordinateSettingController* ui = nullptr;

    void create();
    void createAction();

    void resetToComponent();
    QString collectData();
public:
    explicit IdpMpCoordinateSettingController(QWidget* parent = nullptr);
    ~IdpMpCoordinateSettingController() noexcept;

    //禁用一切与机台互动相关的操作
    void disablePyHandlers(bool b);
};
#endif
