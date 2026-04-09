#ifndef UI_AMP_EDGE_ROUGHNESS_EDIT_CONTROLLER
#define UI_AMP_EDGE_ROUGHNESS_EDIT_CONTROLLER
#include <QtGlobal>
#include "cde90_layout_helper.h"
#include "cde90_text_field.h"
#include <QWidget>
#include "database_combo_box.h"
#include "cde90_check_box.h"
#include "cde90_scroll_area.h"
#include "cde90_label.h"
#include "detection_common_setting_controller.h"
#include "cde90_combo_box.h"

/**
 * 本类由编辑器自动生成。
 */
namespace cdsem {
class UiAmpEdgeRoughnessEditController;
struct UiAmpEdgeRoughnessEditControllerPath;
}

struct cdsem::UiAmpEdgeRoughnessEditControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
    UiAmpEdgeRoughnessEditControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "amp_edge_roughness_editor.hui";
        path = proHuiDir + "/" + huiFileName;
    }
};
class cdsem::UiAmpEdgeRoughnessEditController {
private:
    const cdsem::UiAmpEdgeRoughnessEditControllerPath pathInfo;
public:
    CDE90ScrollArea* detailContent = nullptr;
    CDE90ComboBox* objectCBox = nullptr;
    CDE90ComboBox* edgeCBox = nullptr;
    DatabaseComboBox* dataCBox = nullptr;
    CDE90CheckBox* bfCheckBox = nullptr;
    CDE90ComboBox* acdCBox = nullptr;
    DatabaseComboBox* acdWayCBox = nullptr;
    CDE90TextFieldBox* acdTargetTextField = nullptr;
    CDE90TextFieldBox* acdLimitTextField = nullptr;
    QWidget* denseObjectSettingContent = nullptr;
    CDE90ComboBox* mCursorTypeCBox = nullptr;
    CDE90TextFieldBox* mDesignValueTextField = nullptr;
    QWidget* searchAreaLabelContent = nullptr;
    QWidget* searchAreaTextContent = nullptr;
    CDE90TextFieldBox* mSearchAreaTextField = nullptr;
    QWidget* measurePointLabelContent = nullptr;
    QWidget* measurePointInputContent = nullptr;
    CDE90TextFieldBox* mMeasurePointTextField = nullptr;
    QWidget* inspectAreaLabelContent = nullptr;
    CDE90Label* mInspectAreaLabel = nullptr;
    QWidget* inspectAreaTextContent = nullptr;
    CDE90TextFieldBox* mInspectTextField = nullptr;
    QWidget* sumLinePointLabelContent = nullptr;
    QWidget* sumLinePointInputContent = nullptr;
    CDE90TextFieldBox* mSumLinePointTextField = nullptr;
    DatabaseComboBox* measurePositionCBox = nullptr;
    CDE90ComboBox* dmMethodCBox = nullptr;
    CDE90TextFieldBox* dmSmoothingTextField = nullptr;
    CDE90ComboBox* dmPreProcessCBox = nullptr;
    CDE90TextFieldBox* dmDiffTextField = nullptr;
    CDE90ComboBox* dmRotCorrCBox = nullptr;
    DetectionCommonSettingController* commonSettingController = nullptr;
    UiAmpEdgeRoughnessEditController() = default;
    ~UiAmpEdgeRoughnessEditController() noexcept {}

    void init(QWidget* widget) {
        CDE90LayoutHelper helper(widget);
        init(helper);
    }

    void init(CDE90LayoutHelper& helper) {
        helper.loadFromPath(pathInfo.path);
        detailContent = helper.get<CDE90ScrollArea>("detailContent");
        objectCBox = helper.get<CDE90ComboBox>("objectCBox");
        edgeCBox = helper.get<CDE90ComboBox>("edgeCBox");
        dataCBox = helper.get<DatabaseComboBox>("dataCBox");
        bfCheckBox = helper.get<CDE90CheckBox>("bfCheckBox");
        acdCBox = helper.get<CDE90ComboBox>("acdCBox");
        acdWayCBox = helper.get<DatabaseComboBox>("acdWayCBox");
        acdTargetTextField = helper.get<CDE90TextFieldBox>("acdTargetTextField");
        acdLimitTextField = helper.get<CDE90TextFieldBox>("acdLimitTextField");
        denseObjectSettingContent = helper.get<QWidget>("denseObjectSettingContent");
        mCursorTypeCBox = helper.get<CDE90ComboBox>("mCursorTypeCBox");
        mDesignValueTextField = helper.get<CDE90TextFieldBox>("mDesignValueTextField");
        searchAreaLabelContent = helper.get<QWidget>("searchAreaLabelContent");
        searchAreaTextContent = helper.get<QWidget>("searchAreaTextContent");
        mSearchAreaTextField = helper.get<CDE90TextFieldBox>("mSearchAreaTextField");
        measurePointLabelContent = helper.get<QWidget>("measurePointLabelContent");
        measurePointInputContent = helper.get<QWidget>("measurePointInputContent");
        mMeasurePointTextField = helper.get<CDE90TextFieldBox>("mMeasurePointTextField");
        inspectAreaLabelContent = helper.get<QWidget>("inspectAreaLabelContent");
        mInspectAreaLabel = helper.get<CDE90Label>("mInspectAreaLabel");
        inspectAreaTextContent = helper.get<QWidget>("inspectAreaTextContent");
        mInspectTextField = helper.get<CDE90TextFieldBox>("mInspectTextField");
        sumLinePointLabelContent = helper.get<QWidget>("sumLinePointLabelContent");
        sumLinePointInputContent = helper.get<QWidget>("sumLinePointInputContent");
        mSumLinePointTextField = helper.get<CDE90TextFieldBox>("mSumLinePointTextField");
        measurePositionCBox = helper.get<DatabaseComboBox>("measurePositionCBox");
        dmMethodCBox = helper.get<CDE90ComboBox>("dmMethodCBox");
        dmSmoothingTextField = helper.get<CDE90TextFieldBox>("dmSmoothingTextField");
        dmPreProcessCBox = helper.get<CDE90ComboBox>("dmPreProcessCBox");
        dmDiffTextField = helper.get<CDE90TextFieldBox>("dmDiffTextField");
        dmRotCorrCBox = helper.get<CDE90ComboBox>("dmRotCorrCBox");
        commonSettingController = helper.get<DetectionCommonSettingController>("commonSettingController");

    }
};
#endif
