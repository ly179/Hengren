#ifndef UI_TEMPLATE_SAVE_BOX_CONTROLLER
#define UI_TEMPLATE_SAVE_BOX_CONTROLLER
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
#include "cde90_check_box.h"
#include "cde90_button.h"

/**
 * 本类由编辑器自动生成。
 * getCodeEditor函数部分可以人工修改，用于内部编辑器。
 */
namespace cdsem {
class UiTemplateSaveBoxController;
struct UiTemplateSaveBoxControllerPath;
}

struct cdsem::UiTemplateSaveBoxControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QString huiDir;
    QString sourceCodeDir;
    QString classFileName;
#endif
    UiTemplateSaveBoxControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "template_save_box";
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
        classFileName = "template_save_box_controller";
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
class cdsem::UiTemplateSaveBoxController {
private:
    const cdsem::UiTemplateSaveBoxControllerPath pathInfo;
public:
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QList<CDE90LayoutScriptLineObject> __allWidgets;
#endif
    CDE90TextFieldBox* nameTextField = nullptr;
    CDE90CheckBox* overloadCheckBox = nullptr;
    CDE90ButtonBox* okBtn = nullptr;
    CDE90ButtonBox* cancel = nullptr;
    UiTemplateSaveBoxController() = default;
    ~UiTemplateSaveBoxController() noexcept {}

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
        nameTextField = helper.get<CDE90TextFieldBox>("nameTextField");
        overloadCheckBox = helper.get<CDE90CheckBox>("overloadCheckBox");
        okBtn = helper.get<CDE90ButtonBox>("okBtn");
        cancel = helper.get<CDE90ButtonBox>("cancel");
    }

#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    [[nodiscard]] static CodeEditorController* createCodeEditor(const cdsem::UiTemplateSaveBoxControllerPath& path) {
        return getCodeEditor<cdsem::UiTemplateSaveBoxControllerPath>(path);
    }

#endif

};
#endif
