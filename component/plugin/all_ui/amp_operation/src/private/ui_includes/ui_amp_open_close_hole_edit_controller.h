#ifndef UI_AMP_OPEN_CLOSE_HOLE_EDIT_CONTROLLER
#define UI_AMP_OPEN_CLOSE_HOLE_EDIT_CONTROLLER
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
#include "cde90_button.h"
#include "cde90_text_field.h"
#include "cde90_combo_box.h"

/**
 * 本类由编辑器自动生成。
 * getCodeEditor函数部分可以人工修改，用于内部编辑器。
 */
namespace cdsem {
class UiAmpOpenCloseHoleEditController;
struct UiAmpOpenCloseHoleEditControllerPath;
}

struct cdsem::UiAmpOpenCloseHoleEditControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QString huiDir;
    QString sourceCodeDir;
    QString classFileName;
#endif
    UiAmpOpenCloseHoleEditControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "amp_open_close_hole_editor";
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
        classFileName = "amp_open_close_hole_edit_controller";
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
class cdsem::UiAmpOpenCloseHoleEditController {
private:
    const cdsem::UiAmpOpenCloseHoleEditControllerPath pathInfo;
public:
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QList<CDE90LayoutScriptLineObject> __allWidgets;
#endif
    CDE90ComboBox* dataCBox = nullptr;
    CDE90TextFieldBox* upperTextField = nullptr;
    CDE90TextFieldBox* lowerTextField = nullptr;
    CDE90TextFieldBox* cutLevelTextField = nullptr;
    CDE90ButtonBox* regBtn = nullptr;
    CDE90TextFieldBox* magTextField = nullptr;
    UiAmpOpenCloseHoleEditController() = default;
    ~UiAmpOpenCloseHoleEditController() noexcept {}

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
        dataCBox = helper.get<CDE90ComboBox>("dataCBox");
        upperTextField = helper.get<CDE90TextFieldBox>("upperTextField");
        lowerTextField = helper.get<CDE90TextFieldBox>("lowerTextField");
        cutLevelTextField = helper.get<CDE90TextFieldBox>("cutLevelTextField");
        regBtn = helper.get<CDE90ButtonBox>("regBtn");
        magTextField = helper.get<CDE90TextFieldBox>("magTextField");

    }

#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    [[nodiscard]] static CodeEditorController* createCodeEditor(const cdsem::UiAmpOpenCloseHoleEditControllerPath& path) {
        return getCodeEditor<cdsem::UiAmpOpenCloseHoleEditControllerPath>(path);
    }

#endif

};
#endif
