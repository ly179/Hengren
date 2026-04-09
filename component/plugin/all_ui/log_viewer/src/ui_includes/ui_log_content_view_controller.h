#ifndef UI_LOG_CONTENT_VIEW_CONTROLLER
#define UI_LOG_CONTENT_VIEW_CONTROLLER
#include <QtGlobal>
#include "cde90_layout_helper.h"
#include "cde90_combo_box.h"
#include "cde90_check_box.h"
#include "cde90_button.h"

/**
 * 本类由编辑器自动生成。
 */
namespace cdsem {
class UiLogContentViewController;
struct UiLogContentViewControllerPath;
}

struct cdsem::UiLogContentViewControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
    UiLogContentViewControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "log_content_view.hui";
        path = proHuiDir + "/" + huiFileName;
    }
};
class cdsem::UiLogContentViewController {
private:
    const cdsem::UiLogContentViewControllerPath pathInfo;
public:
    CDE90ComboBox* filterBox = nullptr;
    CDE90CheckBox* enableRegCBox = nullptr;
    CDE90CheckBox* enableMatchCase = nullptr;
    CDE90ButtonBox* filterBtn = nullptr;
    UiLogContentViewController() = default;
    ~UiLogContentViewController() noexcept {}

    void init(QWidget* widget) {
        CDE90LayoutHelper helper(widget);
        init(helper);
    }

    void init(CDE90LayoutHelper& helper) {
        helper.loadFromPath(pathInfo.path);
        filterBox = helper.get<CDE90ComboBox>("filterBox");
        enableRegCBox = helper.get<CDE90CheckBox>("enableRegCBox");
        enableMatchCase = helper.get<CDE90CheckBox>("enableMatchCase");
        filterBtn = helper.get<CDE90ButtonBox>("filterBtn");

    }
};
#endif
