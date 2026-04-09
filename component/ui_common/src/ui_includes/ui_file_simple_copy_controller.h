#ifndef UI_FILE_SIMPLE_COPY_CONTROLLER
#define UI_FILE_SIMPLE_COPY_CONTROLLER
#include <QtGlobal>
#include "cde90_layout_helper.h"
#include "cde90_button.h"
#include "cde90_label.h"
#include "cde90_text_field.h"

/**
 * 本类由编辑器自动生成。
 */
namespace cdsem {
class UiFileSimpleCopyController;
struct UiFileSimpleCopyControllerPath;
}

struct cdsem::UiFileSimpleCopyControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
    UiFileSimpleCopyControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "file_simple_copy_edit.hui";
        path = proHuiDir + "/" + huiFileName;
    }
};
class cdsem::UiFileSimpleCopyController {
private:
    const cdsem::UiFileSimpleCopyControllerPath pathInfo;
public:
    CDE90Label* titleLabel = nullptr;
    CDE90TextFieldBox* fromDeviceTextField = nullptr;
    CDE90Label* fromLabel = nullptr;
    CDE90TextFieldBox* fromTextField = nullptr;
    CDE90ButtonBox* deviceBtn = nullptr;
    CDE90TextFieldBox* toDeviceTextField = nullptr;
    CDE90Label* toLabel = nullptr;
    CDE90TextFieldBox* toTextField = nullptr;
    CDE90ButtonBox* okBtn = nullptr;
    CDE90ButtonBox* cancelBtn = nullptr;
    UiFileSimpleCopyController() = default;
    ~UiFileSimpleCopyController() noexcept {}

    void init(QWidget* widget) {
        CDE90LayoutHelper helper(widget);
        init(helper);
    }

    void init(CDE90LayoutHelper& helper) {
        helper.loadFromPath(pathInfo.path);
        titleLabel = helper.get<CDE90Label>("titleLabel");
        fromDeviceTextField = helper.get<CDE90TextFieldBox>("fromDeviceTextField");
        fromLabel = helper.get<CDE90Label>("fromLabel");
        fromTextField = helper.get<CDE90TextFieldBox>("fromTextField");
        deviceBtn = helper.get<CDE90ButtonBox>("deviceBtn");
        toDeviceTextField = helper.get<CDE90TextFieldBox>("toDeviceTextField");
        toLabel = helper.get<CDE90Label>("toLabel");
        toTextField = helper.get<CDE90TextFieldBox>("toTextField");
        okBtn = helper.get<CDE90ButtonBox>("okBtn");
        cancelBtn = helper.get<CDE90ButtonBox>("cancelBtn");

    }
};
#endif
