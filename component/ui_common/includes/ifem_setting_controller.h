#ifndef IFEM_SETTING_CONTROLLER_H
#define IFEM_SETTING_CONTROLLER_H
#include "cde90_window.h"
#include "common_act_param_helper.h"
#include "template_data_model.h"
#include "ui_common_global.h"

namespace cdsem {
class UiIfemSettingController;
}

class UI_COMMON_EXPORT IfemSettingController : public CDE90Window {
    SEM_SET_INIT_PARAM(
        IfemParam)
    SEM_OK_CALL(
        IfemParam)
private:
    cdsem::UiIfemSettingController* ui = nullptr;

    void create();
    void createAction();

    void exchangeKind(int val);
    void setDiameterVisible(bool);
    void setPitchYVisible(bool);
    void setLenYVisible(bool);
    void setNumberYVisible(bool);
    void setTargetYVisible(bool);
    void setWidth1Visible(bool);

    void resetToComponent();
    QString collectData();

public:
     explicit IfemSettingController(QWidget* parent = nullptr);
     ~IfemSettingController() noexcept;
};
#endif
