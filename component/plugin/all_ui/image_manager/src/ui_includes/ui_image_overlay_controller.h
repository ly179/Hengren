#ifndef UI_IMAGE_OVERLAY_CONTROLLER
#define UI_IMAGE_OVERLAY_CONTROLLER
#include <QtGlobal>
#include "cde90_layout_helper.h"
#include "cde90_button.h"
#include "cde90_table_view.h"
#include "cde90_check_box.h"

/**
 * 本类由编辑器自动生成。
 */
namespace cdsem {
class UiImageOverlayController;
struct UiImageOverlayControllerPath;
}

struct cdsem::UiImageOverlayControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
    UiImageOverlayControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/hui";
#endif
        huiFileName = "image_overlay_edit.hui";
        path = proHuiDir + "/" + huiFileName;
    }
};
class cdsem::UiImageOverlayController {
private:
    const cdsem::UiImageOverlayControllerPath pathInfo;
public:
    CDE90CheckBox* profileCBox = nullptr;
    CDE90CheckBox* edgeCBox = nullptr;
    CDE90CheckBox* designCBox = nullptr;
    CDE90CheckBox* measureCBox = nullptr;
    CDE90CheckBox* resultCBox = nullptr;
    CDE90TableView* controlList = nullptr;
    CDE90ButtonBox* closeBtn = nullptr;
    UiImageOverlayController() = default;
    ~UiImageOverlayController() noexcept {}

    void init(QWidget* widget) {
        CDE90LayoutHelper helper(widget);
        init(helper);
    }

    void init(CDE90LayoutHelper& helper) {
        helper.loadFromPath(pathInfo.path);
        profileCBox = helper.get<CDE90CheckBox>("profileCBox");
        edgeCBox = helper.get<CDE90CheckBox>("edgeCBox");
        designCBox = helper.get<CDE90CheckBox>("designCBox");
        measureCBox = helper.get<CDE90CheckBox>("measureCBox");
        resultCBox = helper.get<CDE90CheckBox>("resultCBox");
        controlList = helper.get<CDE90TableView>("controlList");
        closeBtn = helper.get<CDE90ButtonBox>("closeBtn");

    }
};
#endif
