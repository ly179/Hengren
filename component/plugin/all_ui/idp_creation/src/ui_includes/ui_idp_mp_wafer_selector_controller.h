#ifndef UI_IDP_MP_WAFER_SELECTOR_CONTROLLER
#define UI_IDP_MP_WAFER_SELECTOR_CONTROLLER
#include <QtGlobal>
#include "cde90_layout_helper.h"
#include <QFrame>
#include "idw_canvas.h"
#include "cde90_combo_box.h"
#include "cde90_button.h"
#include "cde90_text_field.h"

/**
 * 本类由编辑器自动生成。
 */
namespace cdsem {
class UiIdpMpWaferSelectorController;
struct UiIdpMpWaferSelectorControllerPath;
}

struct cdsem::UiIdpMpWaferSelectorControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
    UiIdpMpWaferSelectorControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "idp_mp_wafer_selector.hui";
        path = proHuiDir + "/" + huiFileName;
    }
};
class cdsem::UiIdpMpWaferSelectorController {
private:
    const cdsem::UiIdpMpWaferSelectorControllerPath pathInfo;
public:
    IdwCanvasContent* idwCanvasContent = nullptr;
    CDE90TextFieldBox* chipNoTextField = nullptr;
    CDE90ButtonBox* allBtn = nullptr;
    CDE90TextFieldBox* countTextField = nullptr;
    QFrame* sortContent = nullptr;
    CDE90ComboBox* sortOrderBox = nullptr;
    CDE90ButtonBox* sortBtn = nullptr;
    CDE90TextFieldBox* seqNoTextField = nullptr;
    CDE90ButtonBox* seqDownBtn = nullptr;
    CDE90ButtonBox* seqUpBtn = nullptr;
    CDE90ButtonBox* leftBtn = nullptr;
    CDE90ButtonBox* upBtn = nullptr;
    CDE90ButtonBox* downBtn = nullptr;
    CDE90ButtonBox* rightBtn = nullptr;
    CDE90ComboBox* magBox = nullptr;
    CDE90ButtonBox* okBtn = nullptr;
    CDE90ButtonBox* cancelBtn = nullptr;
    UiIdpMpWaferSelectorController() = default;
    ~UiIdpMpWaferSelectorController() noexcept {}

    void init(QWidget* widget) {
        CDE90LayoutHelper helper(widget);
        init(helper);
    }

    void init(CDE90LayoutHelper& helper) {
        helper.loadFromPath(pathInfo.path);
        idwCanvasContent = helper.get<IdwCanvasContent>("idwCanvasContent");
        chipNoTextField = helper.get<CDE90TextFieldBox>("chipNoTextField");
        allBtn = helper.get<CDE90ButtonBox>("allBtn");
        countTextField = helper.get<CDE90TextFieldBox>("countTextField");
        sortContent = helper.get<QFrame>("sortContent");
        sortOrderBox = helper.get<CDE90ComboBox>("sortOrderBox");
        sortBtn = helper.get<CDE90ButtonBox>("sortBtn");
        seqNoTextField = helper.get<CDE90TextFieldBox>("seqNoTextField");
        seqDownBtn = helper.get<CDE90ButtonBox>("seqDownBtn");
        seqUpBtn = helper.get<CDE90ButtonBox>("seqUpBtn");
        leftBtn = helper.get<CDE90ButtonBox>("leftBtn");
        upBtn = helper.get<CDE90ButtonBox>("upBtn");
        downBtn = helper.get<CDE90ButtonBox>("downBtn");
        rightBtn = helper.get<CDE90ButtonBox>("rightBtn");
        magBox = helper.get<CDE90ComboBox>("magBox");
        okBtn = helper.get<CDE90ButtonBox>("okBtn");
        cancelBtn = helper.get<CDE90ButtonBox>("cancelBtn");

    }
};
#endif
