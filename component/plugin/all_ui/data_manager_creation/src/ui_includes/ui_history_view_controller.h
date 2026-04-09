#ifndef UI_HISTORY_VIEW_CONTROLLER
#define UI_HISTORY_VIEW_CONTROLLER
#include <QtGlobal>
#include "cde90_layout_helper.h"
#include "cde90_table_view.h"
#include "cde90_button.h"
#include "cde90_text_field.h"

/**
 * 本类由编辑器自动生成。
 */
namespace cdsem {
class UiHistoryViewController;
struct UiHistoryViewControllerPath;
}

struct cdsem::UiHistoryViewControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
    UiHistoryViewControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/hui";
#endif
        huiFileName = "history_view_edit.hui";
        path = proHuiDir + "/" + huiFileName;
    }
};
class cdsem::UiHistoryViewController {
private:
    const cdsem::UiHistoryViewControllerPath pathInfo;
public:
    CDE90TextFieldBox* fileNameTextField = nullptr;
    CDE90ButtonBox* currEditBtn = nullptr;
    CDE90TableView* currentVersionTable = nullptr;
    CDE90ButtonBox* historyEditBtn = nullptr;
    CDE90TableView* historyTable = nullptr;
    CDE90ButtonBox* closeBtn = nullptr;
    UiHistoryViewController() = default;
    ~UiHistoryViewController() noexcept {}

    void init(QWidget* widget) {
        CDE90LayoutHelper helper(widget);
        init(helper);
    }

    void init(CDE90LayoutHelper& helper) {
        helper.loadFromPath(pathInfo.path);
        fileNameTextField = helper.get<CDE90TextFieldBox>("fileNameTextField");
        currEditBtn = helper.get<CDE90ButtonBox>("currEditBtn");
        currentVersionTable = helper.get<CDE90TableView>("currentVersionTable");
        historyEditBtn = helper.get<CDE90ButtonBox>("historyEditBtn");
        historyTable = helper.get<CDE90TableView>("historyTable");
        closeBtn = helper.get<CDE90ButtonBox>("closeBtn");

    }
};
#endif
