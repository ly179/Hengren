#ifndef UI_PRE_DOSE_SETTING_CONTROLLER
#define UI_PRE_DOSE_SETTING_CONTROLLER
#include <QtGlobal>
#include "cde90_layout_helper.h"
#include "database_combo_box.h"
#include "cde90_check_box.h"
#include "cde90_button.h"
#include <QWidget>
#include "cde90_text_field.h"

/**
 * 本类由编辑器自动生成。
 */
namespace cdsem {
class UiPreDoseSettingController;
struct UiPreDoseSettingControllerPath;
}

struct cdsem::UiPreDoseSettingControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
    UiPreDoseSettingControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "pre_dose_setting.hui";
        path = proHuiDir + "/" + huiFileName;
    }
};
class cdsem::UiPreDoseSettingController {
private:
    const cdsem::UiPreDoseSettingControllerPath pathInfo;
public:
    CDE90CheckBox* preCbox = nullptr;
    QWidget* paramContent = nullptr;
    CDE90TextFieldBox* magTextField = nullptr;
    CDE90ButtonBox* readBtn = nullptr;
    CDE90TextFieldBox* waitSecondsTextField = nullptr;
    CDE90CheckBox* customVaccCBox = nullptr;
    CDE90TextFieldBox* vaccTextField = nullptr;
    DatabaseComboBox* vaccModeBtn = nullptr;
    CDE90CheckBox* unChangeModeCBox = nullptr;
    CDE90CheckBox* highModeCBox = nullptr;
    CDE90TextFieldBox* memTextField = nullptr;
    CDE90ButtonBox* memBtn = nullptr;
    CDE90ButtonBox* okBtn = nullptr;
    CDE90ButtonBox* backBtn = nullptr;
    CDE90ButtonBox* nextBtn = nullptr;
    CDE90ButtonBox* cancelBtn = nullptr;
    UiPreDoseSettingController() = default;
    ~UiPreDoseSettingController() noexcept {}

    void init(QWidget* widget) {
        CDE90LayoutHelper helper(widget);
        init(helper);
    }

    void init(CDE90LayoutHelper& helper) {
        helper.loadFromPath(pathInfo.path);
        preCbox = helper.get<CDE90CheckBox>("preCbox");
        paramContent = helper.get<QWidget>("paramContent");
        magTextField = helper.get<CDE90TextFieldBox>("magTextField");
        readBtn = helper.get<CDE90ButtonBox>("readBtn");
        waitSecondsTextField = helper.get<CDE90TextFieldBox>("waitSecondsTextField");
        customVaccCBox = helper.get<CDE90CheckBox>("customVaccCBox");
        vaccTextField = helper.get<CDE90TextFieldBox>("vaccTextField");
        vaccModeBtn = helper.get<DatabaseComboBox>("vaccModeBtn");
        unChangeModeCBox = helper.get<CDE90CheckBox>("unChangeModeCBox");
        highModeCBox = helper.get<CDE90CheckBox>("highModeCBox");
        memTextField = helper.get<CDE90TextFieldBox>("memTextField");
        memBtn = helper.get<CDE90ButtonBox>("memBtn");
        okBtn = helper.get<CDE90ButtonBox>("okBtn");
        backBtn = helper.get<CDE90ButtonBox>("backBtn");
        nextBtn = helper.get<CDE90ButtonBox>("nextBtn");
        cancelBtn = helper.get<CDE90ButtonBox>("cancelBtn");

    }
};
#endif
