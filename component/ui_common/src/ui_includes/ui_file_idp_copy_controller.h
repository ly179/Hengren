#ifndef UI_FILE_IDP_COPY_CONTROLLER
#define UI_FILE_IDP_COPY_CONTROLLER
#include <QtGlobal>
#include "cde90_layout_helper.h"
#include "cde90_button.h"
#include "cde90_check_box.h"
#include "cde90_text_field.h"

/**
 * 本类由编辑器自动生成。
 */
namespace cdsem {
class UiFileIdpCopyController;
struct UiFileIdpCopyControllerPath;
}

struct cdsem::UiFileIdpCopyControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
    UiFileIdpCopyControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "file_idp_copy_edit.hui";
        path = proHuiDir + "/" + huiFileName;
    }
};
class cdsem::UiFileIdpCopyController {
private:
    const cdsem::UiFileIdpCopyControllerPath pathInfo;
public:
    CDE90TextFieldBox* fromDeviceTextField = nullptr;
    CDE90TextFieldBox* fromIDPTextField = nullptr;
    CDE90ButtonBox* deviceBtn = nullptr;
    CDE90TextFieldBox* toDeviceTextField = nullptr;
    CDE90TextFieldBox* toClassTextField = nullptr;
    CDE90ButtonBox* toClassBtn = nullptr;
    CDE90TextFieldBox* toIDWTextField = nullptr;
    CDE90ButtonBox* toIDWBtn = nullptr;
    CDE90TextFieldBox* toIDPTextField = nullptr;
    CDE90CheckBox* templateOverwriteCBox = nullptr;
    CDE90ButtonBox* okBtn = nullptr;
    CDE90ButtonBox* cancelBtn = nullptr;
    UiFileIdpCopyController() = default;
    ~UiFileIdpCopyController() noexcept {}

    void init(QWidget* widget) {
        CDE90LayoutHelper helper(widget);
        init(helper);
    }

    void init(CDE90LayoutHelper& helper) {
        helper.loadFromPath(pathInfo.path);
        fromDeviceTextField = helper.get<CDE90TextFieldBox>("fromDeviceTextField");
        fromIDPTextField = helper.get<CDE90TextFieldBox>("fromIDPTextField");
        deviceBtn = helper.get<CDE90ButtonBox>("deviceBtn");
        toDeviceTextField = helper.get<CDE90TextFieldBox>("toDeviceTextField");
        toClassTextField = helper.get<CDE90TextFieldBox>("toClassTextField");
        toClassBtn = helper.get<CDE90ButtonBox>("toClassBtn");
        toIDWTextField = helper.get<CDE90TextFieldBox>("toIDWTextField");
        toIDWBtn = helper.get<CDE90ButtonBox>("toIDWBtn");
        toIDPTextField = helper.get<CDE90TextFieldBox>("toIDPTextField");
        templateOverwriteCBox = helper.get<CDE90CheckBox>("templateOverwriteCBox");
        okBtn = helper.get<CDE90ButtonBox>("okBtn");
        cancelBtn = helper.get<CDE90ButtonBox>("cancelBtn");

    }
};
#endif
