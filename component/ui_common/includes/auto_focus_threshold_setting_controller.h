#ifndef AUTO_FOCUS_THRESHOLD_SETTING_CONTROLLER_H
#define AUTO_FOCUS_THRESHOLD_SETTING_CONTROLLER_H
#include "cde90_window.h"
#include "common_act_param_helper.h"
#include "ui_common_global.h"
namespace cdsem {
class UiAutoFocusThresholdSettingController;
}

class UI_COMMON_EXPORT AutoFocusThresholdSettingController : public CDE90Window {
    SEM_OK_CALL(
        int)
private:
    cdsem::UiAutoFocusThresholdSettingController* ui = nullptr;

    void create();
    void createAction();

public:
    explicit AutoFocusThresholdSettingController(QWidget* parent = nullptr);
    virtual ~AutoFocusThresholdSettingController() noexcept;

    void setInitParam(int threshold);
};

#endif // AUTO_FOCUS_THRESHOLD_SETTING_CONTROLLER_H
