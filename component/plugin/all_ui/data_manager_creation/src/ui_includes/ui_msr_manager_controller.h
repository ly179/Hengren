#ifndef UI_MSR_MANAGER_CONTROLLER
#define UI_MSR_MANAGER_CONTROLLER
#include <QtGlobal>
#include "cde90_layout_helper.h"
#include "cde90_label.h"
#include "cde90_text_field.h"
#include "process_loading_pane.h"
#include "cde90_button.h"
#include <QAction>
#include "cde90_list_view.h"

/**
 * 本类由编辑器自动生成。
 */
namespace cdsem {
class UiMsrManagerController;
struct UiMsrManagerControllerPath;
}

struct cdsem::UiMsrManagerControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
    UiMsrManagerControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/hui";
#endif
        huiFileName = "msr_manager_edit.hui";
        path = proHuiDir + "/" + huiFileName;
    }
};
class cdsem::UiMsrManagerController {
private:
    const cdsem::UiMsrManagerControllerPath pathInfo;
public:
    QAction* reloadAct = nullptr;
    QAction* exitAct = nullptr;
    CDE90ButtonBox* deviceBtn = nullptr;
    CDE90TextFieldBox* deviceTextField = nullptr;
    CDE90Label* freeLabel = nullptr;
    CDE90ListView* classList = nullptr;
    ProcessLoadingPane* processPane = nullptr;
    CDE90ListView* msrList = nullptr;
    CDE90TextFieldBox* classCountTextField = nullptr;
    CDE90TextFieldBox* msrCountTextField = nullptr;
    CDE90TextFieldBox* selectedClassTextField = nullptr;
    CDE90TextFieldBox* selectedMsrTextField = nullptr;
    UiMsrManagerController() = default;
    ~UiMsrManagerController() noexcept {}

    void init(QWidget* widget) {
        CDE90LayoutHelper helper(widget);
        init(helper);
    }

    void init(CDE90LayoutHelper& helper) {
        helper.loadFromPath(pathInfo.path);
        reloadAct = helper.get<QAction>("reloadAct");
        exitAct = helper.get<QAction>("exitAct");
        deviceBtn = helper.get<CDE90ButtonBox>("deviceBtn");
        deviceTextField = helper.get<CDE90TextFieldBox>("deviceTextField");
        freeLabel = helper.get<CDE90Label>("freeLabel");
        classList = helper.get<CDE90ListView>("classList");
        processPane = helper.get<ProcessLoadingPane>("processPane");
        msrList = helper.get<CDE90ListView>("msrList");
        classCountTextField = helper.get<CDE90TextFieldBox>("classCountTextField");
        msrCountTextField = helper.get<CDE90TextFieldBox>("msrCountTextField");
        selectedClassTextField = helper.get<CDE90TextFieldBox>("selectedClassTextField");
        selectedMsrTextField = helper.get<CDE90TextFieldBox>("selectedMsrTextField");

    }
};
#endif
