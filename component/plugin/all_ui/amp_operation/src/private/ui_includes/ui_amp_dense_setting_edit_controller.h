#ifndef UI_AMP_DENSE_SETTING_EDIT_CONTROLLER
#define UI_AMP_DENSE_SETTING_EDIT_CONTROLLER
#include <QtGlobal>
#include "cde90_layout_helper.h"
#include "cde90_combo_box.h"
#include "cde90_label.h"
#include "cde90_text_field.h"
#include "cde90_check_box.h"

/**
 * 本类由编辑器自动生成。
 */
namespace cdsem {
class UiAmpDenseSettingEditController;
struct UiAmpDenseSettingEditControllerPath;
}

struct cdsem::UiAmpDenseSettingEditControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
    UiAmpDenseSettingEditControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "amp_dense_setting_editor.hui";
        path = proHuiDir + "/" + huiFileName;
    }
};
class cdsem::UiAmpDenseSettingEditController {
private:
    const cdsem::UiAmpDenseSettingEditControllerPath pathInfo;
public:
    CDE90Label* measTypeLabel = nullptr;
    CDE90ComboBox* dMeasLineCBox = nullptr;
    CDE90Label* startNumLabel = nullptr;
    CDE90TextFieldBox* dStartLineTextField = nullptr;
    CDE90ComboBox* dMeasDirectionCBox = nullptr;
    CDE90TextFieldBox* dMeasNumTextField = nullptr;
    CDE90TextFieldBox* dCursorSizeXTextField = nullptr;
    CDE90TextFieldBox* dCursorSizeYTextField = nullptr;
    CDE90CheckBox* dEdgeDetectCheckBox = nullptr;
    UiAmpDenseSettingEditController() = default;
    ~UiAmpDenseSettingEditController() noexcept {}

    void init(QWidget* widget) {
        CDE90LayoutHelper helper(widget);
        init(helper);
    }

    void init(CDE90LayoutHelper& helper) {
        helper.loadFromPath(pathInfo.path);
        measTypeLabel = helper.get<CDE90Label>("measTypeLabel");
        dMeasLineCBox = helper.get<CDE90ComboBox>("dMeasLineCBox");
        startNumLabel = helper.get<CDE90Label>("startNumLabel");
        dStartLineTextField = helper.get<CDE90TextFieldBox>("dStartLineTextField");
        dMeasDirectionCBox = helper.get<CDE90ComboBox>("dMeasDirectionCBox");
        dMeasNumTextField = helper.get<CDE90TextFieldBox>("dMeasNumTextField");
        dCursorSizeXTextField = helper.get<CDE90TextFieldBox>("dCursorSizeXTextField");
        dCursorSizeYTextField = helper.get<CDE90TextFieldBox>("dCursorSizeYTextField");
        dEdgeDetectCheckBox = helper.get<CDE90CheckBox>("dEdgeDetectCheckBox");

    }
};
#endif
