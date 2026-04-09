#ifndef AUTO_FOCUS_OFFSET_SETTING_CONTROLLER_H
#define AUTO_FOCUS_OFFSET_SETTING_CONTROLLER_H
#include "cde90_window.h"
#include "common_act_param_helper.h"
#include "ui_common_global.h"

namespace cdsem {
class UiAutoFocusOffsetSettingController;
}

class UI_COMMON_EXPORT AutoFocusOffsetSettingController : public CDE90Window {
    SEM_OK_CALL(
        int)
private:
    cdsem::UiAutoFocusOffsetSettingController* ui = nullptr;

    int mode = 0;
    int offset = 0;
    bool isAddress = false;

    void create();
    void createAction();

public:
    explicit AutoFocusOffsetSettingController(bool address, int mode, QWidget* parent = nullptr);
    virtual ~AutoFocusOffsetSettingController() noexcept;

    void changeMode(int mode);
    void setInitParam(int offset);
};

#endif // AUTO_FOCUS_OFFSET_SETTING_CONTROLLER_H
