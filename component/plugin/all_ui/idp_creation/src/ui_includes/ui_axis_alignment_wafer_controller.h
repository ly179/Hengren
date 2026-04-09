#ifndef UI_AXIS_ALIGNMENT_WAFER_CONTROLLER
#define UI_AXIS_ALIGNMENT_WAFER_CONTROLLER
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
#include "cde90_combo_box.h"
#include <QWidget>
#include "cde90_text_field.h"
#include "cde90_check_box.h"
#include "cde90_button.h"

/**
 * 本类由编辑器自动生成。
 * getCodeEditor函数部分可以人工修改，用于内部编辑器。
 */
namespace cdsem {
class UiAxisAlignmentWaferController;
struct UiAxisAlignmentWaferControllerPath;
}

struct cdsem::UiAxisAlignmentWaferControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QString huiDir;
    QString sourceCodeDir;
    QString classFileName;
#endif
    UiAxisAlignmentWaferControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "axis_alignment_wafer_editor";
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
        classFileName = "axis_alignment_wafer_controller";
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
class cdsem::UiAxisAlignmentWaferController {
private:
    const cdsem::UiAxisAlignmentWaferControllerPath pathInfo;
public:
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QList<CDE90LayoutScriptLineObject> __allWidgets;
#endif
    CDE90CheckBox* axisAlignCheckBox = nullptr;
    CDE90ComboBox* manualAssistCBox = nullptr;
    CDE90TextFieldBox* chipXTextField = nullptr;
    CDE90TextFieldBox* chipXDeviationTextField = nullptr;
    CDE90ButtonBox* chipMoreBtn = nullptr;
    CDE90TextFieldBox* chipYTextField = nullptr;
    CDE90TextFieldBox* chipYDeviationTextField = nullptr;
    QWidget* detailContent = nullptr;
    CDE90CheckBox* confirmCheckBox = nullptr;
    CDE90CheckBox* updateSemConditionCheckBox = nullptr;
    CDE90TextFieldBox* memNo1 = nullptr;
    CDE90TextFieldBox* memNo2 = nullptr;
    CDE90TextFieldBox* memNo3 = nullptr;
    CDE90ButtonBox* testBtn = nullptr;
    CDE90ButtonBox* stopBtn = nullptr;
    CDE90ButtonBox* okBtn = nullptr;
    CDE90ButtonBox* cancelBtn = nullptr;
    UiAxisAlignmentWaferController() = default;
    ~UiAxisAlignmentWaferController() noexcept {}

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
        axisAlignCheckBox = helper.get<CDE90CheckBox>("axisAlignCheckBox");
        manualAssistCBox = helper.get<CDE90ComboBox>("manualAssistCBox");
        chipXTextField = helper.get<CDE90TextFieldBox>("chipXTextField");
        chipXDeviationTextField = helper.get<CDE90TextFieldBox>("chipXDeviationTextField");
        chipMoreBtn = helper.get<CDE90ButtonBox>("chipMoreBtn");
        chipYTextField = helper.get<CDE90TextFieldBox>("chipYTextField");
        chipYDeviationTextField = helper.get<CDE90TextFieldBox>("chipYDeviationTextField");
        detailContent = helper.get<QWidget>("detailContent");
        confirmCheckBox = helper.get<CDE90CheckBox>("confirmCheckBox");
        updateSemConditionCheckBox = helper.get<CDE90CheckBox>("updateSemConditionCheckBox");
        memNo1 = helper.get<CDE90TextFieldBox>("memNo1");
        memNo2 = helper.get<CDE90TextFieldBox>("memNo2");
        memNo3 = helper.get<CDE90TextFieldBox>("memNo3");
        testBtn = helper.get<CDE90ButtonBox>("testBtn");
        stopBtn = helper.get<CDE90ButtonBox>("stopBtn");
        okBtn = helper.get<CDE90ButtonBox>("okBtn");
        cancelBtn = helper.get<CDE90ButtonBox>("cancelBtn");

    }

#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    [[nodiscard]] static CodeEditorController* createCodeEditor(const cdsem::UiAxisAlignmentWaferControllerPath& path) {
        return getCodeEditor<cdsem::UiAxisAlignmentWaferControllerPath>(path);
    }

#endif

};
#endif
