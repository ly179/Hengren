#ifndef UI_MULTI_SHOT_CHIP_SELECT_CONTROLLER
#define UI_MULTI_SHOT_CHIP_SELECT_CONTROLLER
#include <QtGlobal>
#include "cde90_layout_helper.h"
#include "cde90_button.h"
#include "cde90_label.h"
#include "cde90_list_view.h"

/**
 * 本类由编辑器自动生成。
 */
namespace cdsem {
class UiMultiShotChipSelectController;
struct UiMultiShotChipSelectControllerPath;
}

struct cdsem::UiMultiShotChipSelectControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
    UiMultiShotChipSelectControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "multi_shot_chip_selector.hui";
        path = proHuiDir + "/" + huiFileName;
    }
};
class cdsem::UiMultiShotChipSelectController {
private:
    const cdsem::UiMultiShotChipSelectControllerPath pathInfo;
public:
    CDE90Label* shotChipLabel = nullptr;
    CDE90ListView* shotChipList = nullptr;
    CDE90ButtonBox* okBtn = nullptr;
    CDE90ButtonBox* cancelBtn = nullptr;
    UiMultiShotChipSelectController() = default;
    ~UiMultiShotChipSelectController() noexcept {}

    void init(QWidget* widget) {
        CDE90LayoutHelper helper(widget);
        init(helper);
    }

    void init(CDE90LayoutHelper& helper) {
        helper.loadFromPath(pathInfo.path);
        shotChipLabel = helper.get<CDE90Label>("shotChipLabel");
        shotChipList = helper.get<CDE90ListView>("shotChipList");
        okBtn = helper.get<CDE90ButtonBox>("okBtn");
        cancelBtn = helper.get<CDE90ButtonBox>("cancelBtn");

    }
};
#endif
