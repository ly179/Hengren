#ifndef UI_AMP_DIAMETER_HOLE_EDIT_CONTROLLER
#define UI_AMP_DIAMETER_HOLE_EDIT_CONTROLLER
#include <QtGlobal>
#include "cde90_layout_helper.h"
#include "cde90_label.h"
#include "cde90_combo_box.h"
#include <QWidget>
#include "detection_common_setting_controller.h"
#include "cde90_text_field.h"
#include "database_combo_box.h"

/**
 * 本类由编辑器自动生成。
 */
namespace cdsem {
class UiAmpDiameterHoleEditController;
struct UiAmpDiameterHoleEditControllerPath;
}

struct cdsem::UiAmpDiameterHoleEditControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
    UiAmpDiameterHoleEditControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "amp_diameter_hole_editor.hui";
        path = proHuiDir + "/" + huiFileName;
    }
};
class cdsem::UiAmpDiameterHoleEditController {
private:
    const cdsem::UiAmpDiameterHoleEditControllerPath pathInfo;
public:
    CDE90ComboBox* objectCBox = nullptr;
    CDE90ComboBox* kindCBox = nullptr;
    CDE90Label* dataLabelContent = nullptr;
    CDE90ComboBox* dataCBox = nullptr;
    CDE90Label* directionLabel = nullptr;
    CDE90ComboBox* directionCBox = nullptr;
    QWidget* measurePointContent = nullptr;
    CDE90TextFieldBox* measurePointTextField = nullptr;
    QWidget* diametersLabelContent = nullptr;
    CDE90TextFieldBox* diametersTextField = nullptr;
    CDE90ComboBox* centerCBox = nullptr;
    QWidget* sumLineOrPointContent = nullptr;
    CDE90TextFieldBox* sumLineOrPointTextField = nullptr;
    QWidget* preProcessContent = nullptr;
    CDE90ComboBox* preProcessCBox = nullptr;
    CDE90ComboBox* acdCBox = nullptr;
    DatabaseComboBox* acdTypeCBox = nullptr;
    CDE90TextFieldBox* acdTargetTextField = nullptr;
    CDE90TextFieldBox* acdLimitTextField = nullptr;
    CDE90ComboBox* methodCBox = nullptr;
    CDE90TextFieldBox* designValueTextFiled = nullptr;
    CDE90TextFieldBox* searchAreaTextField = nullptr;
    CDE90TextFieldBox* smoothingTextField = nullptr;
    CDE90Label* sumLineLabel = nullptr;
    CDE90Label* inspectAreaLabel = nullptr;
    CDE90Label* lineInspectAreaLabel = nullptr;
    CDE90Label* radialInspectAreaLabel = nullptr;
    CDE90TextFieldBox* radialInspectAreaTextField = nullptr;
    CDE90TextFieldBox* lineInspectAreaTextField = nullptr;
    CDE90TextFieldBox* diffTextField = nullptr;
    DetectionCommonSettingController* commonSettingController = nullptr;
    DatabaseComboBox* measurePositionCBox = nullptr;
    UiAmpDiameterHoleEditController() = default;
    ~UiAmpDiameterHoleEditController() noexcept {}

    void init(QWidget* widget) {
        CDE90LayoutHelper helper(widget);
        init(helper);
    }

    void init(CDE90LayoutHelper& helper) {
        helper.loadFromPath(pathInfo.path);
        objectCBox = helper.get<CDE90ComboBox>("objectCBox");
        kindCBox = helper.get<CDE90ComboBox>("kindCBox");
        dataLabelContent = helper.get<CDE90Label>("dataLabelContent");
        dataCBox = helper.get<CDE90ComboBox>("dataCBox");
        directionLabel = helper.get<CDE90Label>("directionLabel");
        directionCBox = helper.get<CDE90ComboBox>("directionCBox");
        measurePointContent = helper.get<QWidget>("measurePointContent");
        measurePointTextField = helper.get<CDE90TextFieldBox>("measurePointTextField");
        diametersLabelContent = helper.get<QWidget>("diametersLabelContent");
        diametersTextField = helper.get<CDE90TextFieldBox>("diametersTextField");
        centerCBox = helper.get<CDE90ComboBox>("centerCBox");
        sumLineOrPointContent = helper.get<QWidget>("sumLineOrPointContent");
        sumLineOrPointTextField = helper.get<CDE90TextFieldBox>("sumLineOrPointTextField");
        preProcessContent = helper.get<QWidget>("preProcessContent");
        preProcessCBox = helper.get<CDE90ComboBox>("preProcessCBox");
        acdCBox = helper.get<CDE90ComboBox>("acdCBox");
        acdTypeCBox = helper.get<DatabaseComboBox>("acdTypeCBox");
        acdTargetTextField = helper.get<CDE90TextFieldBox>("acdTargetTextField");
        acdLimitTextField = helper.get<CDE90TextFieldBox>("acdLimitTextField");
        methodCBox = helper.get<CDE90ComboBox>("methodCBox");
        designValueTextFiled = helper.get<CDE90TextFieldBox>("designValueTextFiled");
        searchAreaTextField = helper.get<CDE90TextFieldBox>("searchAreaTextField");
        smoothingTextField = helper.get<CDE90TextFieldBox>("smoothingTextField");
        sumLineLabel = helper.get<CDE90Label>("sumLineLabel");
        inspectAreaLabel = helper.get<CDE90Label>("inspectAreaLabel");
        lineInspectAreaLabel = helper.get<CDE90Label>("lineInspectAreaLabel");
        radialInspectAreaLabel = helper.get<CDE90Label>("radialInspectAreaLabel");
        radialInspectAreaTextField = helper.get<CDE90TextFieldBox>("radialInspectAreaTextField");
        lineInspectAreaTextField = helper.get<CDE90TextFieldBox>("lineInspectAreaTextField");
        diffTextField = helper.get<CDE90TextFieldBox>("diffTextField");
        commonSettingController = helper.get<DetectionCommonSettingController>("commonSettingController");
        measurePositionCBox = helper.get<DatabaseComboBox>("measurePositionCBox");

    }
};
#endif
