#ifndef UI_APP_INIT_VIEW_CONTROLLER
#define UI_APP_INIT_VIEW_CONTROLLER
#include <QtGlobal>
#include "cde90_layout_helper.h"
#include "cde90_button.h"
#include <QWidget>

/**
 * 本类由编辑器自动生成。
 */
namespace cdsem {
class UiAppInitViewController;
struct UiAppInitViewControllerPath;
}

struct cdsem::UiAppInitViewControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
    UiAppInitViewControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/hui";
#endif
        huiFileName = "app_init_view.hui";
        path = proHuiDir + "/" + huiFileName;
    }
};
class cdsem::UiAppInitViewController {
private:
    const cdsem::UiAppInitViewControllerPath pathInfo;
public:
    QWidget* content = nullptr;
    CDE90ButtonBox* closeBtn = nullptr;
    UiAppInitViewController() = default;
    ~UiAppInitViewController() noexcept {}

    void init(QWidget* widget) {
        CDE90LayoutHelper helper(widget);
        init(helper);
    }

    void init(CDE90LayoutHelper& helper) {
        helper.loadFromPath(pathInfo.path);
        content = helper.get<QWidget>("content");
        closeBtn = helper.get<CDE90ButtonBox>("closeBtn");

    }
};
#endif
