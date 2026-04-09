#ifndef UI_AMP_LWR_ANALYSIS_EDIT_CONTROLLER
#define UI_AMP_LWR_ANALYSIS_EDIT_CONTROLLER
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
#include "detection_common_setting_controller.h"
#include "cde90_text_field.h"

/**
 * 本类由编辑器自动生成。
 * getCodeEditor函数部分可以人工修改，用于内部编辑器。
 */
namespace cdsem {
class UiAmpLwrAnalysisEditController;
struct UiAmpLwrAnalysisEditControllerPath;
}

struct cdsem::UiAmpLwrAnalysisEditControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QString huiDir;
    QString sourceCodeDir;
    QString classFileName;
#endif
    UiAmpLwrAnalysisEditControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "amp_lwr_analysis_editor";
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
        classFileName = "amp_lwr_analysis_edit_controller";
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
class cdsem::UiAmpLwrAnalysisEditController {
private:
    const cdsem::UiAmpLwrAnalysisEditControllerPath pathInfo;
public:
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QList<CDE90LayoutScriptLineObject> __allWidgets;
#endif
    CDE90ComboBox* objectCBox = nullptr;
    CDE90ComboBox* dataCBox = nullptr;
    CDE90TextFieldBox* measurePointTextField = nullptr;
    CDE90TextFieldBox* sumLineOrPointTextField = nullptr;
    CDE90TextFieldBox* transisorTextField = nullptr;
    CDE90ComboBox* methodCBox = nullptr;
    CDE90TextFieldBox* designValueTextField = nullptr;
    CDE90TextFieldBox* searchAreaTextField = nullptr;
    CDE90TextFieldBox* smoothintTextField = nullptr;
    CDE90TextFieldBox* inspectAreaTextField = nullptr;
    CDE90TextFieldBox* diffTextField = nullptr;
    DetectionCommonSettingController* commonSettingController = nullptr;
    UiAmpLwrAnalysisEditController() = default;
    ~UiAmpLwrAnalysisEditController() noexcept {}

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
        objectCBox = helper.get<CDE90ComboBox>("objectCBox");
        dataCBox = helper.get<CDE90ComboBox>("dataCBox");
        measurePointTextField = helper.get<CDE90TextFieldBox>("measurePointTextField");
        sumLineOrPointTextField = helper.get<CDE90TextFieldBox>("sumLineOrPointTextField");
        transisorTextField = helper.get<CDE90TextFieldBox>("transisorTextField");
        methodCBox = helper.get<CDE90ComboBox>("methodCBox");
        designValueTextField = helper.get<CDE90TextFieldBox>("designValueTextField");
        searchAreaTextField = helper.get<CDE90TextFieldBox>("searchAreaTextField");
        smoothintTextField = helper.get<CDE90TextFieldBox>("smoothintTextField");
        inspectAreaTextField = helper.get<CDE90TextFieldBox>("inspectAreaTextField");
        diffTextField = helper.get<CDE90TextFieldBox>("diffTextField");
        commonSettingController = helper.get<DetectionCommonSettingController>("commonSettingController");

    }

#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    [[nodiscard]] static CodeEditorController* createCodeEditor(const cdsem::UiAmpLwrAnalysisEditControllerPath& path) {
        return getCodeEditor<cdsem::UiAmpLwrAnalysisEditControllerPath>(path);
    }

#endif

};
#endif
