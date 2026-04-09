#ifndef UI_LOG_HISTORY_VIEW_CONTROLLER
#define UI_LOG_HISTORY_VIEW_CONTROLLER
#include <QtGlobal>
#include "cde90_layout_helper.h"
#include "cde90_combo_box.h"
#include "cde90_datetime_box.h"
#include "cde90_text_field.h"
#include "cde90_button.h"
#include "cde90_table_view.h"

/**
 * 本类由编辑器自动生成。
 */
namespace cdsem {
class UiLogHistoryViewController;
struct UiLogHistoryViewControllerPath;
}

struct cdsem::UiLogHistoryViewControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
    UiLogHistoryViewControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "log_history_view.hui";
        path = proHuiDir + "/" + huiFileName;
    }
};
class cdsem::UiLogHistoryViewController {
private:
    const cdsem::UiLogHistoryViewControllerPath pathInfo;
public:
    CDE90ComboBox* levelItemBox = nullptr;
    CDE90TextFieldBox* catTextField = nullptr;
    CDE90TextFieldBox* contentTextField = nullptr;
    CDE90DatetimeBox* startDateTime = nullptr;
    CDE90DatetimeBox* endDateTime = nullptr;
    CDE90TextFieldBox* fileTextField = nullptr;
    CDE90ButtonBox* searchBtn = nullptr;
    CDE90TableView* contentTable = nullptr;
    CDE90ComboBox* pageSizeItemBox = nullptr;
    CDE90ButtonBox* reloadBtn = nullptr;
    CDE90ButtonBox* prevPageBtn = nullptr;
    CDE90ButtonBox* nextPageBtn = nullptr;
    UiLogHistoryViewController() = default;
    ~UiLogHistoryViewController() noexcept {}

    void init(QWidget* widget) {
        CDE90LayoutHelper helper(widget);
        init(helper);
    }

    void init(CDE90LayoutHelper& helper) {
        helper.loadFromPath(pathInfo.path);
        levelItemBox = helper.get<CDE90ComboBox>("levelItemBox");
        catTextField = helper.get<CDE90TextFieldBox>("catTextField");
        contentTextField = helper.get<CDE90TextFieldBox>("contentTextField");
        startDateTime = helper.get<CDE90DatetimeBox>("startDateTime");
        endDateTime = helper.get<CDE90DatetimeBox>("endDateTime");
        fileTextField = helper.get<CDE90TextFieldBox>("fileTextField");
        searchBtn = helper.get<CDE90ButtonBox>("searchBtn");
        contentTable = helper.get<CDE90TableView>("contentTable");
        pageSizeItemBox = helper.get<CDE90ComboBox>("pageSizeItemBox");
        reloadBtn = helper.get<CDE90ButtonBox>("reloadBtn");
        prevPageBtn = helper.get<CDE90ButtonBox>("prevPageBtn");
        nextPageBtn = helper.get<CDE90ButtonBox>("nextPageBtn");

    }
};
#endif
