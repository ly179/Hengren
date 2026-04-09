#ifndef UI_MAINTENANCE_CONTROLLER
#define UI_MAINTENANCE_CONTROLLER
#include <QtGlobal>
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
#include "inner_code_editor.h"
#ifndef CODER_EDITOR_DEP
#define CODER_EDITOR_DEP __declspec(dllimport)
#endif
#include "code_editor_controller.h"
#include <QList>
#endif

#include "cde90_layout_helper.h"
#include "cde90_text_field.h"
#include "cde90_label.h"
#include "cde90_check_box.h"
#include "cde90_button.h"

/**
 * 本类由编辑器自动生成。
 * getCodeEditor函数部分可以人工修改，用于内部编辑器。
 */
namespace cdsem {
class UiMaintenanceController;
struct UiMaintenanceControllerPath;
}

struct cdsem::UiMaintenanceControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QString huiDir;
    QString sourceCodeDir;
    QString classFileName;
#endif
    UiMaintenanceControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/hui";
#endif
        huiFileName = "maintenance_editor";
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
        classFileName = "maintenance_controller";
#endif
        path = proHuiDir + "/" + huiFileName + ".hui";
    }
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    //该目录用于实时加载和保存hui文件
    void setHuiDir(const QString& dir) {
        huiDir = dir;
        path = huiDir + "/" + huiFileName + ".hui";
    }

    //该目录用于编辑器实时更新header
    void setCodeDir(const QString& dir) {
        sourceCodeDir = dir;
    }
#endif

};
class cdsem::UiMaintenanceController {
private:
    const cdsem::UiMaintenanceControllerPath pathInfo;
public:
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QList<CDE90LayoutScriptLineObject> __allWidgets;
#endif
    CDE90CheckBox* scopeCorrectionCBox = nullptr;
    CDE90TextFieldBox* scopeXTextField = nullptr;
    CDE90TextFieldBox* scopeXDiffTextField = nullptr;
    CDE90TextFieldBox* scopeYTextField = nullptr;
    CDE90TextFieldBox* scopeYDiffTextField = nullptr;
    CDE90CheckBox* scopeAllowAdjustmentCBox = nullptr;
    CDE90CheckBox* zSensorCBox = nullptr;
    CDE90TextFieldBox* zSensorTextField = nullptr;
    CDE90TextFieldBox* zSensorDiffTextField = nullptr;
    CDE90CheckBox* zSensorAllowAdjustmentCBox = nullptr;
    CDE90CheckBox* cdCheckCBox = nullptr;
    CDE90Label* cdCheckRefLabel = nullptr;
    CDE90TextFieldBox* cdCheckRefTextField = nullptr;
    CDE90ButtonBox* cdCheckRefMoreBtn = nullptr;
    CDE90Label* cdCheckMeasureLabel = nullptr;
    CDE90TextFieldBox* cdCheckMeasureTextField = nullptr;
    CDE90Label* cdCheckMeasureExeLabel = nullptr;
    CDE90Label* cdCheckNextMsrNoLabel = nullptr;
    CDE90TextFieldBox* cdCheckNextMsrNoTextField = nullptr;
    CDE90CheckBox* stageMonitorCBox = nullptr;
    CDE90Label* stageMonitorRangeLabel = nullptr;
    CDE90Label* stageMonitorRangeDiffLabel = nullptr;
    CDE90TextFieldBox* stageMonitorTextField = nullptr;
    CDE90Label* stageMonitorUnitLabel = nullptr;
    CDE90CheckBox* pgMonitorCBox = nullptr;
    CDE90Label* pgMonitorSetupLabel = nullptr;
    CDE90ButtonBox* pgMonitorSteupMorBtn = nullptr;
    CDE90ButtonBox* okBtn = nullptr;
    CDE90ButtonBox* cancalBtn = nullptr;
    UiMaintenanceController() = default;
    ~UiMaintenanceController() noexcept {}

    void init(QWidget* widget) {
        CDE90LayoutHelper helper(widget);
        init(helper);
    }

    void init(CDE90LayoutHelper& helper) {
        helper.loadFromPath(pathInfo.path
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
, &__allWidgets
#endif
        );
        scopeCorrectionCBox = helper.get<CDE90CheckBox>("scopeCorrectionCBox");
        scopeXTextField = helper.get<CDE90TextFieldBox>("scopeXTextField");
        scopeXDiffTextField = helper.get<CDE90TextFieldBox>("scopeXDiffTextField");
        scopeYTextField = helper.get<CDE90TextFieldBox>("scopeYTextField");
        scopeYDiffTextField = helper.get<CDE90TextFieldBox>("scopeYDiffTextField");
        scopeAllowAdjustmentCBox = helper.get<CDE90CheckBox>("scopeAllowAdjustmentCBox");
        zSensorCBox = helper.get<CDE90CheckBox>("zSensorCBox");
        zSensorTextField = helper.get<CDE90TextFieldBox>("zSensorTextField");
        zSensorDiffTextField = helper.get<CDE90TextFieldBox>("zSensorDiffTextField");
        zSensorAllowAdjustmentCBox = helper.get<CDE90CheckBox>("zSensorAllowAdjustmentCBox");
        cdCheckCBox = helper.get<CDE90CheckBox>("cdCheckCBox");
        cdCheckRefLabel = helper.get<CDE90Label>("cdCheckRefLabel");
        cdCheckRefTextField = helper.get<CDE90TextFieldBox>("cdCheckRefTextField");
        cdCheckRefMoreBtn = helper.get<CDE90ButtonBox>("cdCheckRefMoreBtn");
        cdCheckMeasureLabel = helper.get<CDE90Label>("cdCheckMeasureLabel");
        cdCheckMeasureTextField = helper.get<CDE90TextFieldBox>("cdCheckMeasureTextField");
        cdCheckMeasureExeLabel = helper.get<CDE90Label>("cdCheckMeasureExeLabel");
        cdCheckNextMsrNoLabel = helper.get<CDE90Label>("cdCheckNextMsrNoLabel");
        cdCheckNextMsrNoTextField = helper.get<CDE90TextFieldBox>("cdCheckNextMsrNoTextField");
        stageMonitorCBox = helper.get<CDE90CheckBox>("stageMonitorCBox");
        stageMonitorRangeLabel = helper.get<CDE90Label>("stageMonitorRangeLabel");
        stageMonitorRangeDiffLabel = helper.get<CDE90Label>("stageMonitorRangeDiffLabel");
        stageMonitorTextField = helper.get<CDE90TextFieldBox>("stageMonitorTextField");
        stageMonitorUnitLabel = helper.get<CDE90Label>("stageMonitorUnitLabel");
        pgMonitorCBox = helper.get<CDE90CheckBox>("pgMonitorCBox");
        pgMonitorSetupLabel = helper.get<CDE90Label>("pgMonitorSetupLabel");
        pgMonitorSteupMorBtn = helper.get<CDE90ButtonBox>("pgMonitorSteupMorBtn");
        okBtn = helper.get<CDE90ButtonBox>("okBtn");
        cancalBtn = helper.get<CDE90ButtonBox>("cancalBtn");

    }

#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    [[nodiscard]] static CodeEditorController* createCodeEditor(const cdsem::UiMaintenanceControllerPath& path) {
        return getCodeEditor<cdsem::UiMaintenanceControllerPath>(path);
    }

#endif

};
#endif
