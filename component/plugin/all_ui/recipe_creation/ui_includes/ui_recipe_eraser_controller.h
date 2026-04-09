#ifndef UI_RECIPE_ERASER_CONTROLLER
#define UI_RECIPE_ERASER_CONTROLLER
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
#include "cde90_check_box.h"
#include "cde90_table_view.h"
#include "cde90_button.h"
#include "cde90_text_field.h"

/**
 * 本类由编辑器自动生成。
 * getCodeEditor函数部分可以人工修改，用于内部编辑器。
 */
namespace cdsem {
class UiRecipeEraserController;
struct UiRecipeEraserControllerPath;
}

struct cdsem::UiRecipeEraserControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QString huiDir;
    QString sourceCodeDir;
    QString classFileName;
#endif
    UiRecipeEraserControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/hui";
#endif
        huiFileName = "recipe_eraser_editor";
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
        classFileName = "recipe_eraser_controller";
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
class cdsem::UiRecipeEraserController {
private:
    const cdsem::UiRecipeEraserControllerPath pathInfo;
public:
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QList<CDE90LayoutScriptLineObject> __allWidgets;
#endif
    CDE90TextFieldBox* classTextField = nullptr;
    CDE90TableView* tableContent = nullptr;
    CDE90ButtonBox* refCheckBtn1 = nullptr;
    CDE90CheckBox* whileDelIdpCBox = nullptr;
    CDE90ButtonBox* refCheckBtn2 = nullptr;
    CDE90CheckBox* whileDelIdwCBox = nullptr;
    CDE90ButtonBox* deleteBtn = nullptr;
    CDE90ButtonBox* closeBtn = nullptr;
    UiRecipeEraserController() = default;
    ~UiRecipeEraserController() noexcept {}

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
        tableContent = helper.get<CDE90TableView>("tableContent");
        refCheckBtn1 = helper.get<CDE90ButtonBox>("refCheckBtn1");
        whileDelIdpCBox = helper.get<CDE90CheckBox>("whileDelIdpCBox");
        refCheckBtn2 = helper.get<CDE90ButtonBox>("refCheckBtn2");
        whileDelIdwCBox = helper.get<CDE90CheckBox>("whileDelIdwCBox");
        deleteBtn = helper.get<CDE90ButtonBox>("deleteBtn");
        closeBtn = helper.get<CDE90ButtonBox>("closeBtn");

    }

#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    [[nodiscard]] static CodeEditorController* createCodeEditor(const cdsem::UiRecipeEraserControllerPath& path) {
        return getCodeEditor<cdsem::UiRecipeEraserControllerPath>(path);
    }

#endif

};
#endif
