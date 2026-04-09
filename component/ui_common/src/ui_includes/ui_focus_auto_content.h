#ifndef UI_FOCUS_AUTO_CONTENT
#define UI_FOCUS_AUTO_CONTENT
#include <QtGlobal>
#include "cde90_layout_helper.h"
#include "cde90_check_box.h"
#include "cde90_button.h"
#include <QWidget>
#include "cde90_text_field.h"
#include "cde90_label.h"
#include "database_combo_box.h"

/**
 * 本类由编辑器自动生成。
 */
namespace cdsem {
class UiFocusAutoContent;
struct UiFocusAutoContentPath;
}

struct cdsem::UiFocusAutoContentPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
    UiFocusAutoContentPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "focus_auto.hui";
        path = proHuiDir + "/" + huiFileName;
    }
};
class cdsem::UiFocusAutoContent {
private:
    const cdsem::UiFocusAutoContentPath pathInfo;
public:
    CDE90CheckBox* autoFocusCbox = nullptr;
    CDE90TextFieldBox* dxTextField = nullptr;
    CDE90ButtonBox* regBtn = nullptr;
    CDE90TextFieldBox* dyTextField = nullptr;
    CDE90TextFieldBox* waitTextField = nullptr;
    CDE90TextFieldBox* magTextField = nullptr;
    CDE90ButtonBox* readBtn = nullptr;
    DatabaseComboBox* pixelItemBox = nullptr;
    DatabaseComboBox* frameItemBox = nullptr;
    DatabaseComboBox* scanRateItemBox = nullptr;
    DatabaseComboBox* methodComboBox = nullptr;
    CDE90ButtonBox* offsetBtn = nullptr;
    CDE90TextFieldBox* offsetTextField = nullptr;
    CDE90ButtonBox* thresholdBtn = nullptr;
    CDE90TextFieldBox* thresholdTextField = nullptr;
    CDE90Label* chargingVoltageLabel = nullptr;
    QWidget* chargingVoltageContent = nullptr;
    CDE90TextFieldBox* lowChargeVoltTextField = nullptr;
    CDE90TextFieldBox* highChargeVoltTextField = nullptr;
    UiFocusAutoContent() = default;
    ~UiFocusAutoContent() noexcept {}

    void init(QWidget* widget) {
        CDE90LayoutHelper helper(widget);
        init(helper);
    }

    void init(CDE90LayoutHelper& helper) {
        helper.loadFromPath(pathInfo.path);
        autoFocusCbox = helper.get<CDE90CheckBox>("autoFocusCbox");
        dxTextField = helper.get<CDE90TextFieldBox>("dxTextField");
        regBtn = helper.get<CDE90ButtonBox>("regBtn");
        dyTextField = helper.get<CDE90TextFieldBox>("dyTextField");
        waitTextField = helper.get<CDE90TextFieldBox>("waitTextField");
        magTextField = helper.get<CDE90TextFieldBox>("magTextField");
        readBtn = helper.get<CDE90ButtonBox>("readBtn");
        pixelItemBox = helper.get<DatabaseComboBox>("pixelItemBox");
        frameItemBox = helper.get<DatabaseComboBox>("frameItemBox");
        scanRateItemBox = helper.get<DatabaseComboBox>("scanRateItemBox");
        methodComboBox = helper.get<DatabaseComboBox>("methodComboBox");
        offsetBtn = helper.get<CDE90ButtonBox>("offsetBtn");
        offsetTextField = helper.get<CDE90TextFieldBox>("offsetTextField");
        thresholdBtn = helper.get<CDE90ButtonBox>("thresholdBtn");
        thresholdTextField = helper.get<CDE90TextFieldBox>("thresholdTextField");
        chargingVoltageLabel = helper.get<CDE90Label>("chargingVoltageLabel");
        chargingVoltageContent = helper.get<QWidget>("chargingVoltageContent");
        lowChargeVoltTextField = helper.get<CDE90TextFieldBox>("lowChargeVoltTextField");
        highChargeVoltTextField = helper.get<CDE90TextFieldBox>("highChargeVoltTextField");

    }
};
#endif
