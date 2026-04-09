#ifndef UI_IDP_D_NO_CONTROLLER
#define UI_IDP_D_NO_CONTROLLER
#include <QtGlobal>
#include "cde90_layout_helper.h"
#include "cde90_text_field.h"
#include "cde90_table_view.h"
#include "cde90_button.h"

/**
 * 本类由编辑器自动生成。
 */
namespace cdsem {
class UiIdpDNoController;
struct UiIdpDNoControllerPath;
}

struct cdsem::UiIdpDNoControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
    UiIdpDNoControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "d_no_editor.hui";
        path = proHuiDir + "/" + huiFileName;
    }
};
class cdsem::UiIdpDNoController {
private:
    const cdsem::UiIdpDNoControllerPath pathInfo;
public:
    CDE90TextFieldBox* pTextField = nullptr;
    CDE90TextFieldBox* chipTextField = nullptr;
    CDE90ButtonBox* renumBtn = nullptr;
    CDE90TableView* measureTable = nullptr;
    CDE90TextFieldBox* dNoTextField = nullptr;
    CDE90ButtonBox* modifyBtn = nullptr;
    CDE90ButtonBox* okBtn = nullptr;
    CDE90ButtonBox* cancelBtn = nullptr;
    UiIdpDNoController() = default;
    ~UiIdpDNoController() noexcept {}

    void init(QWidget* widget) {
        CDE90LayoutHelper helper(widget);
        init(helper);
    }

    void init(CDE90LayoutHelper& helper) {
        helper.loadFromPath(pathInfo.path);
        pTextField = helper.get<CDE90TextFieldBox>("pTextField");
        chipTextField = helper.get<CDE90TextFieldBox>("chipTextField");
        renumBtn = helper.get<CDE90ButtonBox>("renumBtn");
        measureTable = helper.get<CDE90TableView>("measureTable");
        dNoTextField = helper.get<CDE90TextFieldBox>("dNoTextField");
        modifyBtn = helper.get<CDE90ButtonBox>("modifyBtn");
        okBtn = helper.get<CDE90ButtonBox>("okBtn");
        cancelBtn = helper.get<CDE90ButtonBox>("cancelBtn");

    }
};
#endif
