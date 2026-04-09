#ifndef UI_IMAGE_OPERATION_ROTATE_CONTROLLER
#define UI_IMAGE_OPERATION_ROTATE_CONTROLLER
#include <QtGlobal>
#include "cde90_layout_helper.h"
#include "cde90_text_field.h"
#include "cde90_button.h"

/**
 * 本类由编辑器自动生成。
 */
namespace cdsem {
class UiImageOperationRotateController;
struct UiImageOperationRotateControllerPath;
}

struct cdsem::UiImageOperationRotateControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
    UiImageOperationRotateControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "image_operation_rotate_editor.hui";
        path = proHuiDir + "/" + huiFileName;
    }
};
class cdsem::UiImageOperationRotateController {
private:
    const cdsem::UiImageOperationRotateControllerPath pathInfo;
public:
    CDE90TextFieldBox* rotateTextField = nullptr;
    CDE90ButtonBox* n45Btn = nullptr;
    CDE90ButtonBox* p45Btn = nullptr;
    CDE90ButtonBox* n1Btn = nullptr;
    CDE90ButtonBox* p1Btn = nullptr;
    CDE90ButtonBox* n01Btn = nullptr;
    CDE90ButtonBox* p01Btn = nullptr;
    CDE90ButtonBox* closeBtn = nullptr;
    UiImageOperationRotateController() = default;
    ~UiImageOperationRotateController() noexcept {}

    void init(QWidget* widget) {
        CDE90LayoutHelper helper(widget);
        init(helper);
    }

    void init(CDE90LayoutHelper& helper) {
        helper.loadFromPath(pathInfo.path);
        rotateTextField = helper.get<CDE90TextFieldBox>("rotateTextField");
        n45Btn = helper.get<CDE90ButtonBox>("n45Btn");
        p45Btn = helper.get<CDE90ButtonBox>("p45Btn");
        n1Btn = helper.get<CDE90ButtonBox>("n1Btn");
        p1Btn = helper.get<CDE90ButtonBox>("p1Btn");
        n01Btn = helper.get<CDE90ButtonBox>("n01Btn");
        p01Btn = helper.get<CDE90ButtonBox>("p01Btn");
        closeBtn = helper.get<CDE90ButtonBox>("closeBtn");

    }
};
#endif
