#ifndef UI_IMAGE_SCAN_SETTING_CONTENT
#define UI_IMAGE_SCAN_SETTING_CONTENT
#include <QtGlobal>
#include "cde90_layout_helper.h"
#include "cde90_check_box.h"
#include <QWidget>
#include "cde90_text_field.h"
#include "database_combo_box.h"
#include "cde90_button.h"

/**
 * 本类由编辑器自动生成。
 */
namespace cdsem {
class UiImageScanSettingContent;
struct UiImageScanSettingContentPath;
}

struct cdsem::UiImageScanSettingContentPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
    UiImageScanSettingContentPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "image_scan_setting.hui";
        path = proHuiDir + "/" + huiFileName;
    }
};
class cdsem::UiImageScanSettingContent {
private:
    const cdsem::UiImageScanSettingContentPath pathInfo;
public:
    QWidget* titleContent = nullptr;
    CDE90ButtonBox* readBtn = nullptr;
    QWidget* detailContent = nullptr;
    CDE90CheckBox* energyFilterCheckBox = nullptr;
    CDE90TextFieldBox* energyFilterTextField = nullptr;
    CDE90TextFieldBox* bseTextField = nullptr;
    CDE90TextFieldBox* bTextField = nullptr;
    CDE90TextFieldBox* cTextField = nullptr;
    CDE90TextFieldBox* magTextField = nullptr;
    CDE90TextFieldBox* rotTextField = nullptr;
    DatabaseComboBox* scanRateItemBox = nullptr;
    CDE90CheckBox* flatModeCheckBox = nullptr;
    DatabaseComboBox* frameItemBox = nullptr;
    DatabaseComboBox* filterItemBox = nullptr;
    DatabaseComboBox* pixelXItemBox = nullptr;
    DatabaseComboBox* pixelYItemBox = nullptr;
    UiImageScanSettingContent() = default;
    ~UiImageScanSettingContent() noexcept {}

    void init(QWidget* widget) {
        CDE90LayoutHelper helper(widget);
        init(helper);
    }

    void init(CDE90LayoutHelper& helper) {
        helper.loadFromPath(pathInfo.path);
        titleContent = helper.get<QWidget>("titleContent");
        readBtn = helper.get<CDE90ButtonBox>("readBtn");
        detailContent = helper.get<QWidget>("detailContent");
        energyFilterCheckBox = helper.get<CDE90CheckBox>("energyFilterCheckBox");
        energyFilterTextField = helper.get<CDE90TextFieldBox>("energyFilterTextField");
        bseTextField = helper.get<CDE90TextFieldBox>("bseTextField");
        bTextField = helper.get<CDE90TextFieldBox>("bTextField");
        cTextField = helper.get<CDE90TextFieldBox>("cTextField");
        magTextField = helper.get<CDE90TextFieldBox>("magTextField");
        rotTextField = helper.get<CDE90TextFieldBox>("rotTextField");
        scanRateItemBox = helper.get<DatabaseComboBox>("scanRateItemBox");
        flatModeCheckBox = helper.get<CDE90CheckBox>("flatModeCheckBox");
        frameItemBox = helper.get<DatabaseComboBox>("frameItemBox");
        filterItemBox = helper.get<DatabaseComboBox>("filterItemBox");
        pixelXItemBox = helper.get<DatabaseComboBox>("pixelXItemBox");
        pixelYItemBox = helper.get<DatabaseComboBox>("pixelYItemBox");

    }
};
#endif
