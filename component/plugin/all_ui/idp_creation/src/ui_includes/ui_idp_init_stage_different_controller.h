#ifndef UI_IDP_INIT_STAGE_DIFFERENT_CONTROLLER
#define UI_IDP_INIT_STAGE_DIFFERENT_CONTROLLER
#include <QtGlobal>
#include "cde90_layout_helper.h"
#include "cde90_button.h"
#include "cde90_radio_button.h"
#include "cde90_check_box.h"

/**
 * 本类由编辑器自动生成。
 */
namespace cdsem {
class UiIdpInitStageDifferentController;
struct UiIdpInitStageDifferentControllerPath;
}

struct cdsem::UiIdpInitStageDifferentControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
    UiIdpInitStageDifferentControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "idp_init_stage_different_edit.hui";
        path = proHuiDir + "/" + huiFileName;
    }
};
class cdsem::UiIdpInitStageDifferentController {
private:
    const cdsem::UiIdpInitStageDifferentControllerPath pathInfo;
public:
    CDE90RadioButton* loadToGlobalRadio = nullptr;
    CDE90RadioButton* ignoreRadio = nullptr;
    CDE90CheckBox* loadIdpCheckBox = nullptr;
    CDE90ButtonBox* okBtn = nullptr;
    CDE90ButtonBox* cancelBtn = nullptr;
    UiIdpInitStageDifferentController() = default;
    ~UiIdpInitStageDifferentController() noexcept {}

    void init(QWidget* widget) {
        CDE90LayoutHelper helper(widget);
        init(helper);
    }

    void init(CDE90LayoutHelper& helper) {
        helper.loadFromPath(pathInfo.path);
        loadToGlobalRadio = helper.get<CDE90RadioButton>("loadToGlobalRadio");
        ignoreRadio = helper.get<CDE90RadioButton>("ignoreRadio");
        loadIdpCheckBox = helper.get<CDE90CheckBox>("loadIdpCheckBox");
        okBtn = helper.get<CDE90ButtonBox>("okBtn");
        cancelBtn = helper.get<CDE90ButtonBox>("cancelBtn");

    }
};
#endif
