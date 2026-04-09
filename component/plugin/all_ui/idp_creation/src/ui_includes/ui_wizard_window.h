#ifndef UI_WIZARD_WINDOW
#define UI_WIZARD_WINDOW
#include <QtGlobal>
#include "cde90_layout_helper.h"
#include "cde90_label.h"
#include "cde90_table_view.h"
#include "cde90_widget.h"
#include "cde90_button.h"

/**
 * 本类由编辑器自动生成。
 */
namespace cdsem {
class UiWizardWindow;
struct UiWizardWindowPath;
}

struct cdsem::UiWizardWindowPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
    UiWizardWindowPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "wizard_window.hui";
        path = proHuiDir + "/" + huiFileName;
    }
};
class cdsem::UiWizardWindow {
private:
    const cdsem::UiWizardWindowPath pathInfo;
public:
    CDE90TableView* stepTable = nullptr;
    CDE90Widget* contentContext = nullptr;
    CDE90Label* noteLabel = nullptr;
    CDE90ButtonBox* cancelBtn = nullptr;
    CDE90ButtonBox* backBtn = nullptr;
    CDE90ButtonBox* nextBtn = nullptr;
    CDE90ButtonBox* skipBtn = nullptr;
    UiWizardWindow() = default;
    ~UiWizardWindow() noexcept {}

    void init(QWidget* widget) {
        CDE90LayoutHelper helper(widget);
        init(helper);
    }

    void init(CDE90LayoutHelper& helper) {
        helper.loadFromPath(pathInfo.path);
        stepTable = helper.get<CDE90TableView>("stepTable");
        contentContext = helper.get<CDE90Widget>("contentContext");
        noteLabel = helper.get<CDE90Label>("noteLabel");
        cancelBtn = helper.get<CDE90ButtonBox>("cancelBtn");
        backBtn = helper.get<CDE90ButtonBox>("backBtn");
        nextBtn = helper.get<CDE90ButtonBox>("nextBtn");
        skipBtn = helper.get<CDE90ButtonBox>("skipBtn");

    }
};
#endif
