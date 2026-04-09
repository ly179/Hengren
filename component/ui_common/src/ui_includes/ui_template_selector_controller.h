#ifndef UI_TEMPLATE_SELECTOR_CONTROLLER
#define UI_TEMPLATE_SELECTOR_CONTROLLER
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
#include "cde90_list_view.h"
#include "cde90_button.h"
#include "cde90_text_field.h"
#include "sem_thumbnail_widget.h"

/**
 * 本类由编辑器自动生成。
 * getCodeEditor函数部分可以人工修改，用于内部编辑器。
 */
namespace cdsem {
class UiTemplateSelectorController;
struct UiTemplateSelectorControllerPath;
}

struct cdsem::UiTemplateSelectorControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QString huiDir;
    QString sourceCodeDir;
    QString classFileName;
#endif
    UiTemplateSelectorControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "template_selector";
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
        classFileName = "template_selector_controller";
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
class cdsem::UiTemplateSelectorController {
private:
    const cdsem::UiTemplateSelectorControllerPath pathInfo;
public:
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QList<CDE90LayoutScriptLineObject> __allWidgets;
#endif
    CDE90TextFieldBox* titleTextField = nullptr;
    CDE90ButtonBox* actionBtn = nullptr;
    CDE90ListView* listView = nullptr;
    SemThumbnailWidget* canvas = nullptr;
    CDE90TextFieldBox* selectedTextField = nullptr;
    CDE90TextFieldBox* commonTextField = nullptr;
    CDE90ButtonBox* okBtn = nullptr;
    CDE90ButtonBox* cancelBtn = nullptr;
    UiTemplateSelectorController() = default;
    ~UiTemplateSelectorController() noexcept {}

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
        titleTextField = helper.get<CDE90TextFieldBox>("titleTextField");
        actionBtn = helper.get<CDE90ButtonBox>("actionBtn");
        listView = helper.get<CDE90ListView>("listView");
        canvas = helper.get<SemThumbnailWidget>("canvas");
        selectedTextField = helper.get<CDE90TextFieldBox>("selectedTextField");
        commonTextField = helper.get<CDE90TextFieldBox>("commonTextField");
        okBtn = helper.get<CDE90ButtonBox>("okBtn");
        cancelBtn = helper.get<CDE90ButtonBox>("cancelBtn");

    }

#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    [[nodiscard]] static CodeEditorController* createCodeEditor(const cdsem::UiTemplateSelectorControllerPath& path) {
        return getCodeEditor<cdsem::UiTemplateSelectorControllerPath>(path);
    }

#endif

};
#endif
