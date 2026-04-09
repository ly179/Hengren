#ifndef UI_AMP_WIDTH_ROUGHNESS_EDIT_CONTROLLER
#define UI_AMP_WIDTH_ROUGHNESS_EDIT_CONTROLLER
#include <QtGlobal>
#include "cde90_layout_helper.h"
#include "cde90_check_box.h"
#include "detection_common_setting_controller.h"
#include <QWidget>
#include "database_combo_box.h"
#include "cde90_combo_box.h"
#include "cde90_text_field.h"
#include "cde90_scroll_area.h"

/**
 * 本类由编辑器自动生成。
 */
namespace cdsem {
class UiAmpWidthRoughnessEditController;
struct UiAmpWidthRoughnessEditControllerPath;
}

struct cdsem::UiAmpWidthRoughnessEditControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
    UiAmpWidthRoughnessEditControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "amp_width_roughness_editor.hui";
        path = proHuiDir + "/" + huiFileName;
    }
};
class cdsem::UiAmpWidthRoughnessEditController {
private:
    const cdsem::UiAmpWidthRoughnessEditControllerPath pathInfo;
public:
    CDE90ScrollArea* detailContent = nullptr;
    CDE90ComboBox* objectCBox = nullptr;
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
    QWidget* measurePointTextContent = nullptr;
    CDE90TextFieldBox* mMeasurePointTextField = nullptr;
    QWidget* inspectAreaLabelContent = nullptr;
    QWidget* inspectAreaTextContent = nullptr;
    CDE90TextFieldBox* mInspectTextField = nullptr;
    QWidget* sumLinesLabelContent = nullptr;
    QWidget* sumLinesTextContent = nullptr;
    CDE90TextFieldBox* mSumLinePointTextField = nullptr;
    DatabaseComboBox* measurePositionCBox = nullptr;
    CDE90ComboBox* dmMethodCBox = nullptr;
    CDE90TextFieldBox* dmSmoothingTextField = nullptr;
    CDE90ComboBox* dmPreProcessCBox = nullptr;
    CDE90TextFieldBox* dmDiffTextField = nullptr;
    DetectionCommonSettingController* commonSettingController = nullptr;
    UiAmpWidthRoughnessEditController() = default;
    ~UiAmpWidthRoughnessEditController() noexcept {}

    void init(QWidget* widget) {
        CDE90LayoutHelper helper(widget);
        init(helper);
    }

    void init(CDE90LayoutHelper& helper) {
        helper.loadFromPath(pathInfo.path);
        detailContent = helper.get<CDE90ScrollArea>("detailContent");
        objectCBox = helper.get<CDE90ComboBox>("objectCBox");
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
        measurePointTextContent = helper.get<QWidget>("measurePointTextContent");
        mMeasurePointTextField = helper.get<CDE90TextFieldBox>("mMeasurePointTextField");
        inspectAreaLabelContent = helper.get<QWidget>("inspectAreaLabelContent");
        inspectAreaTextContent = helper.get<QWidget>("inspectAreaTextContent");
        mInspectTextField = helper.get<CDE90TextFieldBox>("mInspectTextField");
        sumLinesLabelContent = helper.get<QWidget>("sumLinesLabelContent");
        sumLinesTextContent = helper.get<QWidget>("sumLinesTextContent");
        mSumLinePointTextField = helper.get<CDE90TextFieldBox>("mSumLinePointTextField");
        measurePositionCBox = helper.get<DatabaseComboBox>("measurePositionCBox");
        dmMethodCBox = helper.get<CDE90ComboBox>("dmMethodCBox");
        dmSmoothingTextField = helper.get<CDE90TextFieldBox>("dmSmoothingTextField");
        dmPreProcessCBox = helper.get<CDE90ComboBox>("dmPreProcessCBox");
        dmDiffTextField = helper.get<CDE90TextFieldBox>("dmDiffTextField");
        commonSettingController = helper.get<DetectionCommonSettingController>("commonSettingController");

    }
};
#endif
