#ifndef UI_FILE_IDW_COPY_CONTROLLER
#define UI_FILE_IDW_COPY_CONTROLLER
#include <QtGlobal>
#include "cde90_layout_helper.h"
#include "cde90_button.h"
#include "cde90_text_field.h"

/**
 * 本类由编辑器自动生成。
 */
namespace cdsem {
class UiFileIdwCopyController;
struct UiFileIdwCopyControllerPath;
}

struct cdsem::UiFileIdwCopyControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
    UiFileIdwCopyControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "file_idw_copy_edit.hui";
        path = proHuiDir + "/" + huiFileName;
    }
};
class cdsem::UiFileIdwCopyController {
private:
    const cdsem::UiFileIdwCopyControllerPath pathInfo;
public:
    CDE90TextFieldBox* fromDeviceTextField = nullptr;
    CDE90TextFieldBox* fromIDWTextField = nullptr;
    CDE90ButtonBox* deviceBtn = nullptr;
    CDE90TextFieldBox* toDeviceTextField = nullptr;
    CDE90TextFieldBox* toClassTextField = nullptr;
    CDE90ButtonBox* toClassBtn = nullptr;
    CDE90TextFieldBox* toIDWTextField = nullptr;
    CDE90ButtonBox* okBtn = nullptr;
    CDE90ButtonBox* cancelBtn = nullptr;
    UiFileIdwCopyController() = default;
    ~UiFileIdwCopyController() noexcept {}

    void init(QWidget* widget) {
        CDE90LayoutHelper helper(widget);
        init(helper);
    }

    void init(CDE90LayoutHelper& helper) {
        helper.loadFromPath(pathInfo.path);
        fromDeviceTextField = helper.get<CDE90TextFieldBox>("fromDeviceTextField");
        fromIDWTextField = helper.get<CDE90TextFieldBox>("fromIDWTextField");
        deviceBtn = helper.get<CDE90ButtonBox>("deviceBtn");
        toDeviceTextField = helper.get<CDE90TextFieldBox>("toDeviceTextField");
        toClassTextField = helper.get<CDE90TextFieldBox>("toClassTextField");
        toClassBtn = helper.get<CDE90ButtonBox>("toClassBtn");
        toIDWTextField = helper.get<CDE90TextFieldBox>("toIDWTextField");
        okBtn = helper.get<CDE90ButtonBox>("okBtn");
        cancelBtn = helper.get<CDE90ButtonBox>("cancelBtn");

    }
};
#endif
