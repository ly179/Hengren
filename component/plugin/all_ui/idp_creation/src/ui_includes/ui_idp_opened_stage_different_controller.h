#ifndef UI_IDP_OPENED_STAGE_DIFFERENT_CONTROLLER
#define UI_IDP_OPENED_STAGE_DIFFERENT_CONTROLLER
#include <QtGlobal>
#include "cde90_layout_helper.h"
#include "cde90_text_field.h"
#include "cde90_button.h"
#include "cde90_radio_button.h"
#include "cde90_check_box.h"

/**
 * 本类由编辑器自动生成。
 */
namespace cdsem {
class UiIdpOpenedStageDifferentController;
struct UiIdpOpenedStageDifferentControllerPath;
}

struct cdsem::UiIdpOpenedStageDifferentControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
    UiIdpOpenedStageDifferentControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "idp_opened_stage_different_edit.hui";
        path = proHuiDir + "/" + huiFileName;
    }
};
class cdsem::UiIdpOpenedStageDifferentController {
private:
    const cdsem::UiIdpOpenedStageDifferentControllerPath pathInfo;
public:
    CDE90RadioButton* reloadRadio = nullptr;
    CDE90RadioButton* saveRadio = nullptr;
    CDE90RadioButton* saveAsRadio = nullptr;
    CDE90TextFieldBox* saveAsFileNameTextField = nullptr;
    CDE90RadioButton* ignoreRadio = nullptr;
    CDE90CheckBox* loadIdpCheckBox = nullptr;
    CDE90ButtonBox* okBtn = nullptr;
    UiIdpOpenedStageDifferentController() = default;
    ~UiIdpOpenedStageDifferentController() noexcept {}

    void init(QWidget* widget) {
        CDE90LayoutHelper helper(widget);
        init(helper);
    }

    void init(CDE90LayoutHelper& helper) {
        helper.loadFromPath(pathInfo.path);
        reloadRadio = helper.get<CDE90RadioButton>("reloadRadio");
        saveRadio = helper.get<CDE90RadioButton>("saveRadio");
        saveAsRadio = helper.get<CDE90RadioButton>("saveAsRadio");
        saveAsFileNameTextField = helper.get<CDE90TextFieldBox>("saveAsFileNameTextField");
        ignoreRadio = helper.get<CDE90RadioButton>("ignoreRadio");
        loadIdpCheckBox = helper.get<CDE90CheckBox>("loadIdpCheckBox");
        okBtn = helper.get<CDE90ButtonBox>("okBtn");

    }
};
#endif
