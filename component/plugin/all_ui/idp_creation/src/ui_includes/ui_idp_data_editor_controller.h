#ifndef UI_IDP_DATA_EDITOR_CONTROLLER
#define UI_IDP_DATA_EDITOR_CONTROLLER
#include <QtGlobal>
#include "cde90_layout_helper.h"
#include "cde90_table_view.h"
#include "cde90_radio_button.h"
#include "cde90_text_field.h"
#include "cde90_button.h"
#include "cde90_menu.h"
#include <QAction>

/**
 * 本类由编辑器自动生成。
 */
namespace cdsem {
class UiIdpDataEditorController;
struct UiIdpDataEditorControllerPath;
}

struct cdsem::UiIdpDataEditorControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
    UiIdpDataEditorControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "idp_data_editor.hui";
        path = proHuiDir + "/" + huiFileName;
    }
};
class cdsem::UiIdpDataEditorController {
private:
    const cdsem::UiIdpDataEditorControllerPath pathInfo;
public:
    CDE90Menu* fileMenu = nullptr;
    QAction* saveAct = nullptr;
    QAction* saveAsAct = nullptr;
    QAction* exitAct = nullptr;
    CDE90Menu* optionMenu = nullptr;
    QAction* axisAct = nullptr;
    QAction* imgEnvAct = nullptr;
    QAction* measureTimesAct = nullptr;
    CDE90Menu* helpMenu = nullptr;
    CDE90TextFieldBox* classTextField = nullptr;
    CDE90TextFieldBox* idwTextField = nullptr;
    CDE90TextFieldBox* idpTextField = nullptr;
    CDE90ButtonBox* escBtn = nullptr;
    CDE90TextFieldBox* escTextField = nullptr;
    CDE90ButtonBox* apWizardBtn = nullptr;
    QAction* apNewAction = nullptr;
    QAction* apEditAction = nullptr;
    QAction* apModifyChipAction = nullptr;
    QAction* apCutAction = nullptr;
    QAction* apCopyAction = nullptr;
    CDE90Menu* apPasteMenu = nullptr;
    QAction* apPasteBeforeAct = nullptr;
    QAction* apPasteAfterAct = nullptr;
    CDE90TableView* apTable = nullptr;
    CDE90ButtonBox* apAlignmentBtn = nullptr;
    CDE90ButtonBox* apStopBtn = nullptr;
    CDE90ButtonBox* mpWizardBtn = nullptr;
    CDE90ButtonBox* mpChipBtn = nullptr;
    QAction* mpNewAction = nullptr;
    QAction* mpEditAction = nullptr;
    QAction* mpModifyChipAction = nullptr;
    QAction* mpModifyChipD_MAction = nullptr;
    QAction* mpModifyChipD_PAction = nullptr;
    QAction* mpModifyAxBAction = nullptr;
    QAction* mpModifyZSensorAction = nullptr;
    QAction* mpModifyPAction = nullptr;
    QAction* mpDuplicateAction = nullptr;
    QAction* mpShiftCoordinateAction = nullptr;
    QAction* mpMoveAction = nullptr;
    QAction* mpSelectAction = nullptr;
    QAction* mpSortAction = nullptr;
    QAction* mpCutAction = nullptr;
    QAction* mpCopyAction = nullptr;
    CDE90Menu* mpPasteMenu = nullptr;
    QAction* mpPasteBeforeAct = nullptr;
    QAction* mpPasteAfterAct = nullptr;
    QAction* mpApCMatchAction = nullptr;
    CDE90TableView* mpTable = nullptr;
    CDE90ButtonBox* zSensorBtn = nullptr;
    CDE90ButtonBox* measurementBtn = nullptr;
    CDE90ButtonBox* scanOverlapBtn = nullptr;
    CDE90RadioButton* allRadio = nullptr;
    CDE90RadioButton* selectedRadio = nullptr;
    CDE90ButtonBox* mpStopBtn = nullptr;
    UiIdpDataEditorController() = default;
    ~UiIdpDataEditorController() noexcept {}

    void init(QWidget* widget) {
        CDE90LayoutHelper helper(widget);
        init(helper);
    }

    void init(CDE90LayoutHelper& helper) {
        helper.loadFromPath(pathInfo.path);
        fileMenu = helper.get<CDE90Menu>("fileMenu");
        saveAct = helper.get<QAction>("saveAct");
        saveAsAct = helper.get<QAction>("saveAsAct");
        exitAct = helper.get<QAction>("exitAct");
        optionMenu = helper.get<CDE90Menu>("optionMenu");
        axisAct = helper.get<QAction>("axisAct");
        imgEnvAct = helper.get<QAction>("imgEnvAct");
        measureTimesAct = helper.get<QAction>("measureTimesAct");
        helpMenu = helper.get<CDE90Menu>("helpMenu");
        classTextField = helper.get<CDE90TextFieldBox>("classTextField");
        idwTextField = helper.get<CDE90TextFieldBox>("idwTextField");
        idpTextField = helper.get<CDE90TextFieldBox>("idpTextField");
        escBtn = helper.get<CDE90ButtonBox>("escBtn");
        escTextField = helper.get<CDE90TextFieldBox>("escTextField");
        apWizardBtn = helper.get<CDE90ButtonBox>("apWizardBtn");
        apNewAction = helper.get<QAction>("apNewAction");
        apEditAction = helper.get<QAction>("apEditAction");
        apModifyChipAction = helper.get<QAction>("apModifyChipAction");
        apCutAction = helper.get<QAction>("apCutAction");
        apCopyAction = helper.get<QAction>("apCopyAction");
        apPasteMenu = helper.get<CDE90Menu>("apPasteMenu");
        apPasteBeforeAct = helper.get<QAction>("apPasteBeforeAct");
        apPasteAfterAct = helper.get<QAction>("apPasteAfterAct");
        apTable = helper.get<CDE90TableView>("apTable");
        apAlignmentBtn = helper.get<CDE90ButtonBox>("apAlignmentBtn");
        apStopBtn = helper.get<CDE90ButtonBox>("apStopBtn");
        mpWizardBtn = helper.get<CDE90ButtonBox>("mpWizardBtn");
        mpChipBtn = helper.get<CDE90ButtonBox>("mpChipBtn");
        mpNewAction = helper.get<QAction>("mpNewAction");
        mpEditAction = helper.get<QAction>("mpEditAction");
        mpModifyChipAction = helper.get<QAction>("mpModifyChipAction");
        mpModifyChipD_MAction = helper.get<QAction>("mpModifyChipD_MAction");
        mpModifyChipD_PAction = helper.get<QAction>("mpModifyChipD_PAction");
        mpModifyAxBAction = helper.get<QAction>("mpModifyAxBAction");
        mpModifyZSensorAction = helper.get<QAction>("mpModifyZSensorAction");
        mpModifyPAction = helper.get<QAction>("mpModifyPAction");
        mpDuplicateAction = helper.get<QAction>("mpDuplicateAction");
        mpShiftCoordinateAction = helper.get<QAction>("mpShiftCoordinateAction");
        mpMoveAction = helper.get<QAction>("mpMoveAction");
        mpSelectAction = helper.get<QAction>("mpSelectAction");
        mpSortAction = helper.get<QAction>("mpSortAction");
        mpCutAction = helper.get<QAction>("mpCutAction");
        mpCopyAction = helper.get<QAction>("mpCopyAction");
        mpPasteMenu = helper.get<CDE90Menu>("mpPasteMenu");
        mpPasteBeforeAct = helper.get<QAction>("mpPasteBeforeAct");
        mpPasteAfterAct = helper.get<QAction>("mpPasteAfterAct");
        mpApCMatchAction = helper.get<QAction>("mpApCMatchAction");
        mpTable = helper.get<CDE90TableView>("mpTable");
        zSensorBtn = helper.get<CDE90ButtonBox>("zSensorBtn");
        measurementBtn = helper.get<CDE90ButtonBox>("measurementBtn");
        scanOverlapBtn = helper.get<CDE90ButtonBox>("scanOverlapBtn");
        allRadio = helper.get<CDE90RadioButton>("allRadio");
        selectedRadio = helper.get<CDE90RadioButton>("selectedRadio");
        mpStopBtn = helper.get<CDE90ButtonBox>("mpStopBtn");

    }
};
#endif
