#ifndef UI_IMAGE_INFO_CONTROLLER
#define UI_IMAGE_INFO_CONTROLLER
#include <QtGlobal>
#include "cde90_layout_helper.h"
#include "cde90_text_edit.h"
#include "cde90_text_field.h"
#include "cde90_button.h"

/**
 * 本类由编辑器自动生成。
 */
namespace cdsem {
class UiImageInfoController;
struct UiImageInfoControllerPath;
}

struct cdsem::UiImageInfoControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
    UiImageInfoControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/hui";
#endif
        huiFileName = "image_info_edit.hui";
        path = proHuiDir + "/" + huiFileName;
    }
};
class cdsem::UiImageInfoController {
private:
    const cdsem::UiImageInfoControllerPath pathInfo;
public:
    CDE90TextFieldBox* imageNameTextField = nullptr;
    CDE90TextEdit* imageInfoTextArea = nullptr;
    CDE90ButtonBox* closeBtn = nullptr;
    UiImageInfoController() = default;
    ~UiImageInfoController() noexcept {}

    void init(QWidget* widget) {
        CDE90LayoutHelper helper(widget);
        init(helper);
    }

    void init(CDE90LayoutHelper& helper) {
        helper.loadFromPath(pathInfo.path);
        imageNameTextField = helper.get<CDE90TextFieldBox>("imageNameTextField");
        imageInfoTextArea = helper.get<CDE90TextEdit>("imageInfoTextArea");
        closeBtn = helper.get<CDE90ButtonBox>("closeBtn");

    }
};
#endif
