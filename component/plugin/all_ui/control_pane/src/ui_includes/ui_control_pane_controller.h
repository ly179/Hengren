#ifndef UI_CONTROL_PANE_CONTROLLER
#define UI_CONTROL_PANE_CONTROLLER
#include <QtGlobal>
#include "cde90_layout_helper.h"
#include <QWidget>
#include "cde90_tree_view.h"
#include "cde90_widget.h"
#include "cde90_button.h"
#include "cde90_label.h"

/**
 * 本类由编辑器自动生成。
 */
namespace cdsem {
class UiControlPaneController;
struct UiControlPaneControllerPath;
}

struct cdsem::UiControlPaneControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
    UiControlPaneControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/hui";
#endif
        huiFileName = "control_pane.hui";
        path = proHuiDir + "/" + huiFileName;
    }
};
class cdsem::UiControlPaneController {
private:
    const cdsem::UiControlPaneControllerPath pathInfo;
public:
    CDE90ButtonBox* btn_worksheet = nullptr;
    CDE90ButtonBox* btn_fileManager = nullptr;
    CDE90ButtonBox* btn_templateManager = nullptr;
    CDE90ButtonBox* btn_msr = nullptr;
    CDE90ButtonBox* btn_amp = nullptr;
    CDE90ButtonBox* btn_image = nullptr;
    CDE90Widget* _rgLayout = nullptr;
    CDE90Widget* _rvLayout = nullptr;
    CDE90Label* label_time = nullptr;
    CDE90TreeView* imageTreeview = nullptr;
    CDE90Widget* centerWidget = nullptr;
    QWidget* canvasContent = nullptr;
    UiControlPaneController() = default;
    ~UiControlPaneController() noexcept {}

    void init(QWidget* widget) {
        CDE90LayoutHelper helper(widget);
        init(helper);
    }

    void init(CDE90LayoutHelper& helper) {
        helper.loadFromPath(pathInfo.path);
        btn_worksheet = helper.get<CDE90ButtonBox>("btn_worksheet");
        btn_fileManager = helper.get<CDE90ButtonBox>("btn_fileManager");
        btn_templateManager = helper.get<CDE90ButtonBox>("btn_templateManager");
        btn_msr = helper.get<CDE90ButtonBox>("btn_msr");
        btn_amp = helper.get<CDE90ButtonBox>("btn_amp");
        btn_image = helper.get<CDE90ButtonBox>("btn_image");
        _rgLayout = helper.get<CDE90Widget>("_rgLayout");
        _rvLayout = helper.get<CDE90Widget>("_rvLayout");
        label_time = helper.get<CDE90Label>("label_time");
        imageTreeview = helper.get<CDE90TreeView>("imageTreeview");
        centerWidget = helper.get<CDE90Widget>("centerWidget");
        canvasContent = helper.get<QWidget>("canvasContent");

    }
};
#endif
