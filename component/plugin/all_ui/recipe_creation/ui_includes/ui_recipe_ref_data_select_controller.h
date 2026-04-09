#ifndef UI_RECIPE_REF_DATA_SELECT_CONTROLLER
#define UI_RECIPE_REF_DATA_SELECT_CONTROLLER
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
#include "cde90_label.h"
#include "cde90_list_view.h"
#include "cde90_button.h"
#include "cde90_text_field.h"

/**
 * 本类由编辑器自动生成。
 * getCodeEditor函数部分可以人工修改，用于内部编辑器。
 */
namespace cdsem {
class UiRecipeRefDataSelectController;
struct UiRecipeRefDataSelectControllerPath;
}

struct cdsem::UiRecipeRefDataSelectControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QString huiDir;
    QString sourceCodeDir;
    QString classFileName;
#endif
    UiRecipeRefDataSelectControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/hui";
#endif
        huiFileName = "recipe_ref_data_selector";
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
        classFileName = "recipe_ref_data_select_controller";
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
class cdsem::UiRecipeRefDataSelectController {
private:
    const cdsem::UiRecipeRefDataSelectControllerPath pathInfo;
public:
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QList<CDE90LayoutScriptLineObject> __allWidgets;
#endif
    CDE90TextFieldBox* classTextField = nullptr;
    CDE90Label* typeLabel = nullptr;
    CDE90ListView* listContent = nullptr;
    CDE90TextFieldBox* selectTextField = nullptr;
    CDE90ButtonBox* okBtn = nullptr;
    CDE90ButtonBox* cancelBtn = nullptr;
    UiRecipeRefDataSelectController() = default;
    ~UiRecipeRefDataSelectController() noexcept {}

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
        classTextField = helper.get<CDE90TextFieldBox>("classTextField");
        typeLabel = helper.get<CDE90Label>("typeLabel");
        listContent = helper.get<CDE90ListView>("listContent");
        selectTextField = helper.get<CDE90TextFieldBox>("selectTextField");
        okBtn = helper.get<CDE90ButtonBox>("okBtn");
        cancelBtn = helper.get<CDE90ButtonBox>("cancelBtn");

    }

#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    [[nodiscard]] static CodeEditorController* createCodeEditor(const cdsem::UiRecipeRefDataSelectControllerPath& path) {
        return getCodeEditor<cdsem::UiRecipeRefDataSelectControllerPath>(path);
    }

#endif

};
#endif
