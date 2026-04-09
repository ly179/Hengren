#ifndef UI_LOG_VIEW_CONTROLLER
#define UI_LOG_VIEW_CONTROLLER
#include <QtGlobal>
#include "cde90_layout_helper.h"
#include "cde90_tab_widget.h"
#include <QAction>

/**
 * 本类由编辑器自动生成。
 */
namespace cdsem {
class UiLogViewController;
struct UiLogViewControllerPath;
}

struct cdsem::UiLogViewControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
    UiLogViewControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "log_view.hui";
        path = proHuiDir + "/" + huiFileName;
    }
};
class cdsem::UiLogViewController {
private:
    const cdsem::UiLogViewControllerPath pathInfo;
public:
    QAction* exitAct = nullptr;
    CDE90TabWidget* logContent = nullptr;
    UiLogViewController() = default;
    ~UiLogViewController() noexcept {}

    void init(QWidget* widget) {
        CDE90LayoutHelper helper(widget);
        init(helper);
    }

    void init(CDE90LayoutHelper& helper) {
        helper.loadFromPath(pathInfo.path);
        exitAct = helper.get<QAction>("exitAct");
        logContent = helper.get<CDE90TabWidget>("logContent");

    }
};
#endif
