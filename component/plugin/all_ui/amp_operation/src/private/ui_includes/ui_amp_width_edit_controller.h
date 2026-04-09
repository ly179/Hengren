#ifndef UI_AMP_WIDTH_EDIT_CONTROLLER
#define UI_AMP_WIDTH_EDIT_CONTROLLER
#include <QtGlobal>
#include "cde90_layout_helper.h"
#include "cde90_text_field.h"
#include <QWidget>
#include "cde90_button.h"
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
class UiAmpWidthEditController;
struct UiAmpWidthEditControllerPath;
}

struct cdsem::UiAmpWidthEditControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
    UiAmpWidthEditControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "amp_width_editor.hui";
        path = proHuiDir + "/" + huiFileName;
    }
};
class cdsem::UiAmpWidthEditController {
private:
    const cdsem::UiAmpWidthEditControllerPath pathInfo;
public:
    CDE90ScrollArea* detailContent = nullptr;
    CDE90ComboBox* objectCBox = nullptr;
    DatabaseComboBox* kindCBox = nullptr;
    CDE90Label* dataLabel = nullptr;
    CDE90ComboBox* dataItemBox = nullptr;
    CDE90ComboBox* acdCBox = nullptr;
    DatabaseComboBox* acdWayCBox = nullptr;
    CDE90TextFieldBox* acdTargetTextField = nullptr;
    CDE90TextFieldBox* acdLimitTextField = nullptr;
    QWidget* denseObjectSettingContent = nullptr;
    CDE90ComboBox* mCursorTypeCBox = nullptr;
    DatabaseComboBox* mDirectionCBox = nullptr;
    CDE90TextFieldBox* mDesignValueTextField = nullptr;
    QWidget* searchAreaLabelContent = nullptr;
    QWidget* searchAreaContent = nullptr;
    CDE90TextFieldBox* mSearchAreaTextField = nullptr;
    QWidget* mMeasurePointLabelContent = nullptr;
    QWidget* mMeasurePointInputContent = nullptr;
    CDE90TextFieldBox* mMeasurePointTextField = nullptr;
    QWidget* sumLineOrInspectAreaLabelContent = nullptr;
    CDE90Label* mSumLinesLabel = nullptr;
    CDE90Label* mInspectAreaLabel = nullptr;
    QWidget* sumLineOrInspectAreaContent = nullptr;
    CDE90TextFieldBox* mSumLineOrInspectTextField = nullptr;
    QWidget* mSumLinePointLabelContent = nullptr;
    QWidget* mSumLinePointInputContent = nullptr;
    CDE90TextFieldBox* mSumLinePointTextField = nullptr;
    DatabaseComboBox* measurePositionCBox = nullptr;
    CDE90ComboBox* dmMethodCBox = nullptr;
    CDE90TextFieldBox* dmSmoothingTextField = nullptr;
    DatabaseComboBox* dmPreProcessCBox = nullptr;
    CDE90TextFieldBox* dmDiffTextField = nullptr;
    CDE90TextFieldBox* dmSuccRateTextField = nullptr;
    CDE90Label* rotCorrLabel = nullptr;
    CDE90ComboBox* dmRotCorrCBox = nullptr;
    DetectionCommonSettingController* commonSettingController = nullptr;
    QWidget* waveMatchContent = nullptr;
    CDE90CheckBox* waveMatchCBox = nullptr;
    CDE90TextFieldBox* wOuterThresholdTextField = nullptr;
    CDE90TextFieldBox* wInnerThresholdTextField = nullptr;
    CDE90TextFieldBox* wMagTextField = nullptr;
    CDE90TextFieldBox* wMagYTextField = nullptr;
    CDE90TextFieldBox* wVacTextField = nullptr;
    CDE90TextFieldBox* wFrameTextField = nullptr;
    CDE90TextFieldBox* wScanTextField = nullptr;
    CDE90TextFieldBox* wPixelTextField = nullptr;
    CDE90ButtonBox* wRegBtn = nullptr;
    UiAmpWidthEditController() = default;
    ~UiAmpWidthEditController() noexcept {}

    void init(QWidget* widget) {
        CDE90LayoutHelper helper(widget);
        init(helper);
    }

    void init(CDE90LayoutHelper& helper) {
        helper.loadFromPath(pathInfo.path);
        detailContent = helper.get<CDE90ScrollArea>("detailContent");
        objectCBox = helper.get<CDE90ComboBox>("objectCBox");
        kindCBox = helper.get<DatabaseComboBox>("kindCBox");
        dataLabel = helper.get<CDE90Label>("dataLabel");
        dataItemBox = helper.get<CDE90ComboBox>("dataItemBox");
        acdCBox = helper.get<CDE90ComboBox>("acdCBox");
        acdWayCBox = helper.get<DatabaseComboBox>("acdWayCBox");
        acdTargetTextField = helper.get<CDE90TextFieldBox>("acdTargetTextField");
        acdLimitTextField = helper.get<CDE90TextFieldBox>("acdLimitTextField");
        denseObjectSettingContent = helper.get<QWidget>("denseObjectSettingContent");
        mCursorTypeCBox = helper.get<CDE90ComboBox>("mCursorTypeCBox");
        mDirectionCBox = helper.get<DatabaseComboBox>("mDirectionCBox");
        mDesignValueTextField = helper.get<CDE90TextFieldBox>("mDesignValueTextField");
        searchAreaLabelContent = helper.get<QWidget>("searchAreaLabelContent");
        searchAreaContent = helper.get<QWidget>("searchAreaContent");
        mSearchAreaTextField = helper.get<CDE90TextFieldBox>("mSearchAreaTextField");
        mMeasurePointLabelContent = helper.get<QWidget>("mMeasurePointLabelContent");
        mMeasurePointInputContent = helper.get<QWidget>("mMeasurePointInputContent");
        mMeasurePointTextField = helper.get<CDE90TextFieldBox>("mMeasurePointTextField");
        sumLineOrInspectAreaLabelContent = helper.get<QWidget>("sumLineOrInspectAreaLabelContent");
        mSumLinesLabel = helper.get<CDE90Label>("mSumLinesLabel");
        mInspectAreaLabel = helper.get<CDE90Label>("mInspectAreaLabel");
        sumLineOrInspectAreaContent = helper.get<QWidget>("sumLineOrInspectAreaContent");
        mSumLineOrInspectTextField = helper.get<CDE90TextFieldBox>("mSumLineOrInspectTextField");
        mSumLinePointLabelContent = helper.get<QWidget>("mSumLinePointLabelContent");
        mSumLinePointInputContent = helper.get<QWidget>("mSumLinePointInputContent");
        mSumLinePointTextField = helper.get<CDE90TextFieldBox>("mSumLinePointTextField");
        measurePositionCBox = helper.get<DatabaseComboBox>("measurePositionCBox");
        dmMethodCBox = helper.get<CDE90ComboBox>("dmMethodCBox");
        dmSmoothingTextField = helper.get<CDE90TextFieldBox>("dmSmoothingTextField");
        dmPreProcessCBox = helper.get<DatabaseComboBox>("dmPreProcessCBox");
        dmDiffTextField = helper.get<CDE90TextFieldBox>("dmDiffTextField");
        dmSuccRateTextField = helper.get<CDE90TextFieldBox>("dmSuccRateTextField");
        rotCorrLabel = helper.get<CDE90Label>("rotCorrLabel");
        dmRotCorrCBox = helper.get<CDE90ComboBox>("dmRotCorrCBox");
        commonSettingController = helper.get<DetectionCommonSettingController>("commonSettingController");
        waveMatchContent = helper.get<QWidget>("waveMatchContent");
        waveMatchCBox = helper.get<CDE90CheckBox>("waveMatchCBox");
        wOuterThresholdTextField = helper.get<CDE90TextFieldBox>("wOuterThresholdTextField");
        wInnerThresholdTextField = helper.get<CDE90TextFieldBox>("wInnerThresholdTextField");
        wMagTextField = helper.get<CDE90TextFieldBox>("wMagTextField");
        wMagYTextField = helper.get<CDE90TextFieldBox>("wMagYTextField");
        wVacTextField = helper.get<CDE90TextFieldBox>("wVacTextField");
        wFrameTextField = helper.get<CDE90TextFieldBox>("wFrameTextField");
        wScanTextField = helper.get<CDE90TextFieldBox>("wScanTextField");
        wPixelTextField = helper.get<CDE90TextFieldBox>("wPixelTextField");
        wRegBtn = helper.get<CDE90ButtonBox>("wRegBtn");

    }
};
#endif
