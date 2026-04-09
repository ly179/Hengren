#ifndef AUTO_FOCUS_SETTING_CONTROLLER_H
#define AUTO_FOCUS_SETTING_CONTROLLER_H
#include "cde90_window.h"
#include "common_act_param_helper.h"
#include "template_data_model.h"
#include "ui_common_global.h"

namespace cdsem {
class UiAutoFocusSettingController;
}; // namespace cdsem

class FocusAutoContent;
class FocusStigmaContent;

class UI_COMMON_EXPORT AutoFocusSettingController : public CDE90Window{
    SEM_OK_CALL(AutoFocusParam)
    SEM_SET_INIT_PARAM(AutoFocusParam)
private:
    cdsem::UiAutoFocusSettingController* ui = nullptr;
    FocusAutoContent* focusAutoContent = nullptr;
    FocusStigmaContent* focusStigmaContent = nullptr;

    bool isAddress = false;
    bool isFocusAutoOffsetSetting = false;


    void create();
    void createAction();

    void resetToComponent();
    QString collectData();
public:
    explicit AutoFocusSettingController(bool address, QWidget* parent = nullptr);
    virtual ~AutoFocusSettingController() noexcept;

    void hideNextBtn(bool b = true);
    void hideBackBtn(bool b = true);

    //禁用一切与机台互动相关的操作
    void disablePyHandlers(bool b);
    void setAPos(const IDW_PointF& aPos);
};
#endif
