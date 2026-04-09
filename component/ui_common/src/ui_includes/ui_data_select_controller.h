#ifndef UI_DATA_SELECT_CONTROLLER
#define UI_DATA_SELECT_CONTROLLER
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
#include <QWidget>
#include "cde90_text_field.h"
#include "cde90_list_view.h"
#include "cde90_button.h"

/**
 * 本类由编辑器自动生成。
 * getCodeEditor函数部分可以人工修改，用于内部编辑器。
 */
namespace cdsem {
class UiDataSelectController;
struct UiDataSelectControllerPath;
}

struct cdsem::UiDataSelectControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QString huiDir;
    QString sourceCodeDir;
    QString classFileName;
#endif
    UiDataSelectControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "data_selector";
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
        classFileName = "data_select_controller";
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
class cdsem::UiDataSelectController {
private:
    const cdsem::UiDataSelectControllerPath pathInfo;
public:
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QList<CDE90LayoutScriptLineObject> __allWidgets;
#endif
    CDE90Label* titleLabel = nullptr;
    QWidget* idContent = nullptr;
    CDE90Label* idContentLabel = nullptr;
    CDE90TextFieldBox* idContentKind = nullptr;
    CDE90ListView* listContent = nullptr;
    CDE90TextFieldBox* detailTextField = nullptr;
    CDE90ButtonBox* okBtn = nullptr;
    CDE90ButtonBox* cancelBtn = nullptr;
    UiDataSelectController() = default;
    ~UiDataSelectController() noexcept {}

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
        titleLabel = helper.get<CDE90Label>("titleLabel");
        idContent = helper.get<QWidget>("idContent");
        idContentLabel = helper.get<CDE90Label>("idContentLabel");
        idContentKind = helper.get<CDE90TextFieldBox>("idContentKind");
        listContent = helper.get<CDE90ListView>("listContent");
        detailTextField = helper.get<CDE90TextFieldBox>("detailTextField");
        okBtn = helper.get<CDE90ButtonBox>("okBtn");
        cancelBtn = helper.get<CDE90ButtonBox>("cancelBtn");

    }

#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    [[nodiscard]] static CodeEditorController* createCodeEditor(const cdsem::UiDataSelectControllerPath& path) {
        return getCodeEditor<cdsem::UiDataSelectControllerPath>(path);
    }

#endif

};
#endif
