#ifndef UI_PATTERN_RECOGNITION_PARAM_CONTROLLER
#define UI_PATTERN_RECOGNITION_PARAM_CONTROLLER
#include <QtGlobal>
#include "cde90_layout_helper.h"
#include "cde90_check_box.h"
#include "cde90_combo_box.h"
#include "cde90_label.h"
#include "database_combo_box.h"
#include "sem_thumbnail_widget.h"
#include "cde90_widget.h"
#include "cde90_spliter_line.h"
#include "cde90_tab_widget.h"
#include "cde90_radio_button.h"
#include <QAction>
#include "cde90_text_field.h"
#include "cde90_button.h"

/**
 * 本类由编辑器自动生成。
 */
namespace cdsem {
class UiPatternRecognitionParamController;
struct UiPatternRecognitionParamControllerPath;
}

struct cdsem::UiPatternRecognitionParamControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
    UiPatternRecognitionParamControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "pattern_recognition_param.hui";
        path = proHuiDir + "/" + huiFileName;
    }
};
class cdsem::UiPatternRecognitionParamController {
private:
    const cdsem::UiPatternRecognitionParamControllerPath pathInfo;
public:
    CDE90Label* omTitleLabel = nullptr;
    CDE90CheckBox* semTitleCbox = nullptr;
    SemThumbnailWidget* canvas = nullptr;
    CDE90ButtonBox* actionBtn = nullptr;
    QAction* regImageAct = nullptr;
    QAction* infoAct = nullptr;
    QAction* clearImageAct = nullptr;
    CDE90RadioButton* autoRadio = nullptr;
    CDE90RadioButton* manualRadio = nullptr;
    CDE90TabWidget* detailWidget = nullptr;
    DatabaseComboBox* kindItemBox = nullptr;
    DatabaseComboBox* algItemBox = nullptr;
    CDE90Widget* energyFilterLabelContent = nullptr;
    CDE90CheckBox* energyFilterCBox = nullptr;
    CDE90Widget* energyFilterParamContent = nullptr;
    CDE90TextFieldBox* energyFilterTextField = nullptr;
    CDE90TextFieldBox* energyFilterBseTextField = nullptr;
    CDE90SpliterHLine* energySpliter = nullptr;
    CDE90TextFieldBox* autoBTextField = nullptr;
    CDE90TextFieldBox* autoCTextField = nullptr;
    CDE90TextFieldBox* autoAcceptanceTextField = nullptr;
    CDE90TextFieldBox* autoWaitTextField = nullptr;
    CDE90Widget* minNumberLabelContent = nullptr;
    CDE90Widget* minNumberContent = nullptr;
    CDE90TextFieldBox* autoMinNumber1TextField = nullptr;
    CDE90TextFieldBox* autoMinNumber2TextField = nullptr;
    CDE90Widget* centerLabelContent = nullptr;
    CDE90Widget* centerMinNoLabelContent = nullptr;
    DatabaseComboBox* autoCenterItemBox = nullptr;
    CDE90TextFieldBox* autoCMinNumber1TextField = nullptr;
    CDE90TextFieldBox* autoCMinNumber2TextField = nullptr;
    CDE90Widget* autoOffsetLabelContent = nullptr;
    CDE90Widget* autoOffsetContent = nullptr;
    CDE90TextFieldBox* autoOffsetXTextField = nullptr;
    CDE90TextFieldBox* autoOffsetYTextField = nullptr;
    CDE90ButtonBox* autoOffsetRegBtn = nullptr;
    CDE90TextFieldBox* autoRotationTextField = nullptr;
    CDE90Widget* autoRotRageNoteContent = nullptr;
    DatabaseComboBox* autoScanRateItemBox = nullptr;
    CDE90CheckBox* autoFlatModeItemBox = nullptr;
    CDE90Widget* autoFrameLabelContent = nullptr;
    CDE90Widget* autoFrameFilterContent = nullptr;
    DatabaseComboBox* autoFrameItemBox = nullptr;
    DatabaseComboBox* autoFilterItemBox = nullptr;
    CDE90Widget* autoFrameDetectionLabelContent = nullptr;
    CDE90Widget* autoFrameDMContent = nullptr;
    DatabaseComboBox* autoFrameDetectionItemBox = nullptr;
    DatabaseComboBox* autoFrameMeasurementItemBox = nullptr;
    CDE90Label* autoFilterLabel = nullptr;
    CDE90CheckBox* searchAroundCBox = nullptr;
    CDE90Widget* autoFilterEContent = nullptr;
    DatabaseComboBox* autoEFilterItemBox = nullptr;
    CDE90CheckBox* autoEvaluationCBox = nullptr;
    CDE90TextFieldBox* autoEvaluationTextField = nullptr;
    CDE90Widget* autoEntireModeLabelContent = nullptr;
    CDE90Widget* autoEntireContent = nullptr;
    DatabaseComboBox* autoEntireModeItemBox = nullptr;
    CDE90CheckBox* autoContrastModeCBox = nullptr;
    CDE90Widget* autoDcModeLabelContent = nullptr;
    CDE90Widget* autoDcModeContent = nullptr;
    DatabaseComboBox* autoDcModeItemBox = nullptr;
    CDE90ComboBox* lsDetectionItemBox = nullptr;
    CDE90CheckBox* lsContrastModeCBox = nullptr;
    CDE90ComboBox* autoMethodItemBox = nullptr;
    CDE90ComboBox* autoEdgeDetectionItemBox = nullptr;
    CDE90ComboBox* imageQualityItemBox = nullptr;
    CDE90Label* dLabel = nullptr;
    CDE90SpliterHLine* dSplitLine = nullptr;
    CDE90Label* magLabel = nullptr;
    CDE90Label* rotLabel = nullptr;
    CDE90Label* frameLabel = nullptr;
    CDE90Label* filterLabel = nullptr;
    CDE90Label* scanLabel = nullptr;
    CDE90Widget* bLabelContent = nullptr;
    CDE90Label* dLeftLabel = nullptr;
    CDE90TextFieldBox* bTextField = nullptr;
    CDE90TextFieldBox* dPosXTextField = nullptr;
    CDE90TextFieldBox* manualAcceptanceTextField = nullptr;
    CDE90TextFieldBox* magTextField = nullptr;
    CDE90TextFieldBox* rotTextField = nullptr;
    DatabaseComboBox* manualFrameItemBox = nullptr;
    DatabaseComboBox* manualFilterItemBox = nullptr;
    CDE90RadioButton* viewRadio = nullptr;
    CDE90RadioButton* integRadio = nullptr;
    CDE90Widget* cLabelContent = nullptr;
    CDE90Label* dCenterLabel = nullptr;
    CDE90Label* rotUnitLabel = nullptr;
    CDE90Widget* cItemContent = nullptr;
    CDE90TextFieldBox* cTextField = nullptr;
    CDE90TextFieldBox* dPosYTextField = nullptr;
    CDE90Label* dRightLabel = nullptr;
    CDE90ButtonBox* readBcBtn = nullptr;
    CDE90ButtonBox* dRegPosBtn = nullptr;
    CDE90Label* viewLabel = nullptr;
    CDE90ComboBox* viewItemsBtn = nullptr;
    CDE90ButtonBox* okBtn = nullptr;
    CDE90ButtonBox* backBtn = nullptr;
    CDE90ButtonBox* nextBtn = nullptr;
    CDE90ButtonBox* cancelBtn = nullptr;
    UiPatternRecognitionParamController() = default;
    ~UiPatternRecognitionParamController() noexcept {}

    void init(QWidget* widget) {
        CDE90LayoutHelper helper(widget);
        init(helper);
    }

    void init(CDE90LayoutHelper& helper) {
        helper.loadFromPath(pathInfo.path);
        omTitleLabel = helper.get<CDE90Label>("omTitleLabel");
        semTitleCbox = helper.get<CDE90CheckBox>("semTitleCbox");
        canvas = helper.get<SemThumbnailWidget>("canvas");
        actionBtn = helper.get<CDE90ButtonBox>("actionBtn");
        regImageAct = helper.get<QAction>("regImageAct");
        infoAct = helper.get<QAction>("infoAct");
        clearImageAct = helper.get<QAction>("clearImageAct");
        autoRadio = helper.get<CDE90RadioButton>("autoRadio");
        manualRadio = helper.get<CDE90RadioButton>("manualRadio");
        detailWidget = helper.get<CDE90TabWidget>("detailWidget");
        kindItemBox = helper.get<DatabaseComboBox>("kindItemBox");
        algItemBox = helper.get<DatabaseComboBox>("algItemBox");
        energyFilterLabelContent = helper.get<CDE90Widget>("energyFilterLabelContent");
        energyFilterCBox = helper.get<CDE90CheckBox>("energyFilterCBox");
        energyFilterParamContent = helper.get<CDE90Widget>("energyFilterParamContent");
        energyFilterTextField = helper.get<CDE90TextFieldBox>("energyFilterTextField");
        energyFilterBseTextField = helper.get<CDE90TextFieldBox>("energyFilterBseTextField");
        energySpliter = helper.get<CDE90SpliterHLine>("energySpliter");
        autoBTextField = helper.get<CDE90TextFieldBox>("autoBTextField");
        autoCTextField = helper.get<CDE90TextFieldBox>("autoCTextField");
        autoAcceptanceTextField = helper.get<CDE90TextFieldBox>("autoAcceptanceTextField");
        autoWaitTextField = helper.get<CDE90TextFieldBox>("autoWaitTextField");
        minNumberLabelContent = helper.get<CDE90Widget>("minNumberLabelContent");
        minNumberContent = helper.get<CDE90Widget>("minNumberContent");
        autoMinNumber1TextField = helper.get<CDE90TextFieldBox>("autoMinNumber1TextField");
        autoMinNumber2TextField = helper.get<CDE90TextFieldBox>("autoMinNumber2TextField");
        centerLabelContent = helper.get<CDE90Widget>("centerLabelContent");
        centerMinNoLabelContent = helper.get<CDE90Widget>("centerMinNoLabelContent");
        autoCenterItemBox = helper.get<DatabaseComboBox>("autoCenterItemBox");
        autoCMinNumber1TextField = helper.get<CDE90TextFieldBox>("autoCMinNumber1TextField");
        autoCMinNumber2TextField = helper.get<CDE90TextFieldBox>("autoCMinNumber2TextField");
        autoOffsetLabelContent = helper.get<CDE90Widget>("autoOffsetLabelContent");
        autoOffsetContent = helper.get<CDE90Widget>("autoOffsetContent");
        autoOffsetXTextField = helper.get<CDE90TextFieldBox>("autoOffsetXTextField");
        autoOffsetYTextField = helper.get<CDE90TextFieldBox>("autoOffsetYTextField");
        autoOffsetRegBtn = helper.get<CDE90ButtonBox>("autoOffsetRegBtn");
        autoRotationTextField = helper.get<CDE90TextFieldBox>("autoRotationTextField");
        autoRotRageNoteContent = helper.get<CDE90Widget>("autoRotRageNoteContent");
        autoScanRateItemBox = helper.get<DatabaseComboBox>("autoScanRateItemBox");
        autoFlatModeItemBox = helper.get<CDE90CheckBox>("autoFlatModeItemBox");
        autoFrameLabelContent = helper.get<CDE90Widget>("autoFrameLabelContent");
        autoFrameFilterContent = helper.get<CDE90Widget>("autoFrameFilterContent");
        autoFrameItemBox = helper.get<DatabaseComboBox>("autoFrameItemBox");
        autoFilterItemBox = helper.get<DatabaseComboBox>("autoFilterItemBox");
        autoFrameDetectionLabelContent = helper.get<CDE90Widget>("autoFrameDetectionLabelContent");
        autoFrameDMContent = helper.get<CDE90Widget>("autoFrameDMContent");
        autoFrameDetectionItemBox = helper.get<DatabaseComboBox>("autoFrameDetectionItemBox");
        autoFrameMeasurementItemBox = helper.get<DatabaseComboBox>("autoFrameMeasurementItemBox");
        autoFilterLabel = helper.get<CDE90Label>("autoFilterLabel");
        searchAroundCBox = helper.get<CDE90CheckBox>("searchAroundCBox");
        autoFilterEContent = helper.get<CDE90Widget>("autoFilterEContent");
        autoEFilterItemBox = helper.get<DatabaseComboBox>("autoEFilterItemBox");
        autoEvaluationCBox = helper.get<CDE90CheckBox>("autoEvaluationCBox");
        autoEvaluationTextField = helper.get<CDE90TextFieldBox>("autoEvaluationTextField");
        autoEntireModeLabelContent = helper.get<CDE90Widget>("autoEntireModeLabelContent");
        autoEntireContent = helper.get<CDE90Widget>("autoEntireContent");
        autoEntireModeItemBox = helper.get<DatabaseComboBox>("autoEntireModeItemBox");
        autoContrastModeCBox = helper.get<CDE90CheckBox>("autoContrastModeCBox");
        autoDcModeLabelContent = helper.get<CDE90Widget>("autoDcModeLabelContent");
        autoDcModeContent = helper.get<CDE90Widget>("autoDcModeContent");
        autoDcModeItemBox = helper.get<DatabaseComboBox>("autoDcModeItemBox");
        lsDetectionItemBox = helper.get<CDE90ComboBox>("lsDetectionItemBox");
        lsContrastModeCBox = helper.get<CDE90CheckBox>("lsContrastModeCBox");
        autoMethodItemBox = helper.get<CDE90ComboBox>("autoMethodItemBox");
        autoEdgeDetectionItemBox = helper.get<CDE90ComboBox>("autoEdgeDetectionItemBox");
        imageQualityItemBox = helper.get<CDE90ComboBox>("imageQualityItemBox");
        dLabel = helper.get<CDE90Label>("dLabel");
        dSplitLine = helper.get<CDE90SpliterHLine>("dSplitLine");
        magLabel = helper.get<CDE90Label>("magLabel");
        rotLabel = helper.get<CDE90Label>("rotLabel");
        frameLabel = helper.get<CDE90Label>("frameLabel");
        filterLabel = helper.get<CDE90Label>("filterLabel");
        scanLabel = helper.get<CDE90Label>("scanLabel");
        bLabelContent = helper.get<CDE90Widget>("bLabelContent");
        dLeftLabel = helper.get<CDE90Label>("dLeftLabel");
        bTextField = helper.get<CDE90TextFieldBox>("bTextField");
        dPosXTextField = helper.get<CDE90TextFieldBox>("dPosXTextField");
        manualAcceptanceTextField = helper.get<CDE90TextFieldBox>("manualAcceptanceTextField");
        magTextField = helper.get<CDE90TextFieldBox>("magTextField");
        rotTextField = helper.get<CDE90TextFieldBox>("rotTextField");
        manualFrameItemBox = helper.get<DatabaseComboBox>("manualFrameItemBox");
        manualFilterItemBox = helper.get<DatabaseComboBox>("manualFilterItemBox");
        viewRadio = helper.get<CDE90RadioButton>("viewRadio");
        integRadio = helper.get<CDE90RadioButton>("integRadio");
        cLabelContent = helper.get<CDE90Widget>("cLabelContent");
        dCenterLabel = helper.get<CDE90Label>("dCenterLabel");
        rotUnitLabel = helper.get<CDE90Label>("rotUnitLabel");
        cItemContent = helper.get<CDE90Widget>("cItemContent");
        cTextField = helper.get<CDE90TextFieldBox>("cTextField");
        dPosYTextField = helper.get<CDE90TextFieldBox>("dPosYTextField");
        dRightLabel = helper.get<CDE90Label>("dRightLabel");
        readBcBtn = helper.get<CDE90ButtonBox>("readBcBtn");
        dRegPosBtn = helper.get<CDE90ButtonBox>("dRegPosBtn");
        viewLabel = helper.get<CDE90Label>("viewLabel");
        viewItemsBtn = helper.get<CDE90ComboBox>("viewItemsBtn");
        okBtn = helper.get<CDE90ButtonBox>("okBtn");
        backBtn = helper.get<CDE90ButtonBox>("backBtn");
        nextBtn = helper.get<CDE90ButtonBox>("nextBtn");
        cancelBtn = helper.get<CDE90ButtonBox>("cancelBtn");

    }
};
#endif
