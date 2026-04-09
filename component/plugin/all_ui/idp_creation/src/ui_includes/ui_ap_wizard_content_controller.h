#ifndef UI_AP_WIZARD_CONTENT_CONTROLLER
#define UI_AP_WIZARD_CONTENT_CONTROLLER
#include <QtGlobal>
#include "cde90_layout_helper.h"
#include "cde90_combo_box.h"
#include "cde90_button.h"
#include "cde90_widget.h"
#include "cde90_check_box.h"
#include "cde90_group_box.h"

/**
 * 本类由编辑器自动生成。
 */
namespace cdsem {
class UiApWizardContentController;
struct UiApWizardContentControllerPath;
}

struct cdsem::UiApWizardContentControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
    UiApWizardContentControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "ap_wizard_content.hui";
        path = proHuiDir + "/" + huiFileName;
    }
};
class cdsem::UiApWizardContentController {
private:
    const cdsem::UiApWizardContentControllerPath pathInfo;
public:
    CDE90Widget* autoPatternArea = nullptr;
    CDE90CheckBox* autoCBox = nullptr;
    CDE90Widget* groupBoxArea = nullptr;
    CDE90GroupBox* itemGBox = nullptr;
    CDE90Widget* btnArea = nullptr;
    CDE90Widget* scanArea = nullptr;
    CDE90ComboBox* scanRateItemBox = nullptr;
    CDE90ComboBox* frameItemBox = nullptr;
    CDE90ButtonBox* moveChipBtn = nullptr;
    UiApWizardContentController() = default;
    ~UiApWizardContentController() noexcept {}

    void init(QWidget* widget) {
        CDE90LayoutHelper helper(widget);
        init(helper);
    }

    void init(CDE90LayoutHelper& helper) {
        helper.loadFromPath(pathInfo.path);
        autoPatternArea = helper.get<CDE90Widget>("autoPatternArea");
        autoCBox = helper.get<CDE90CheckBox>("autoCBox");
        groupBoxArea = helper.get<CDE90Widget>("groupBoxArea");
        itemGBox = helper.get<CDE90GroupBox>("itemGBox");
        btnArea = helper.get<CDE90Widget>("btnArea");
        scanArea = helper.get<CDE90Widget>("scanArea");
        scanRateItemBox = helper.get<CDE90ComboBox>("scanRateItemBox");
        frameItemBox = helper.get<CDE90ComboBox>("frameItemBox");
        moveChipBtn = helper.get<CDE90ButtonBox>("moveChipBtn");

    }
};
#endif
