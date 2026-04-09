#ifndef Z_SENSOR_SETTING_CONTROLLER_H
#define Z_SENSOR_SETTING_CONTROLLER_H
#include "cde90_dialog.h"
#include "common_act_param_helper.h"
#include "template_data_model.h"
#include "ui_common_global.h"

namespace cdsem {
class UiZSensorSettingController;
};

class UI_COMMON_EXPORT ZSensorSettingController : public CDE90Dialog{
    SEM_OK_CALL(ZSensorSettingParam)
    SEM_SET_INIT_PARAM(ZSensorSettingParam)
private:
    cdsem::UiZSensorSettingController* ui = nullptr;

    void create();
    void createAction();
    void resetToComponent();
    QString collectData();
public:
    explicit ZSensorSettingController(QWidget* parent = nullptr);
    ~ZSensorSettingController() noexcept;

    void hideNextBtn(bool b = true);
    void hideBackBtn(bool b = true);
};
#endif
