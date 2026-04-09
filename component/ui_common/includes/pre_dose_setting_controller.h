#ifndef PRE_DOSE_SETTING_CONTROLLER_H
#define PRE_DOSE_SETTING_CONTROLLER_H
#include "cde90_dialog.h"
#include "common_act_param_helper.h"
#include "template_data_model.h"
#include "ui_common_global.h"

namespace cdsem {
class UiPreDoseSettingController;
};

class UI_COMMON_EXPORT PreDoseSettingController : public CDE90Dialog{
    SEM_SET_INIT_PARAM(PreDoseSettingParam)
    SEM_OK_CALL(PreDoseSettingParam)
private:
    cdsem::UiPreDoseSettingController* ui = nullptr;

    void create();
    void createAction();
    void resetToComponent();
    QString collectData();
public:
    explicit PreDoseSettingController(QWidget* parent = nullptr);
    ~PreDoseSettingController() noexcept;

    void hideNextBtn(bool b = true);
    void hideBackBtn(bool b = true);
};
#endif
