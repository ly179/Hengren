#ifndef UI_JOB_CLASS_SELECT_CONTROLLER
#define UI_JOB_CLASS_SELECT_CONTROLLER
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
#include "cde90_list_view.h"
#include "database_combo_box.h"
#include "cde90_button.h"

/**
 * 本类由编辑器自动生成。
 * getCodeEditor函数部分可以人工修改，用于内部编辑器。
 */
namespace cdsem {
class UiJobClassSelectController;
struct UiJobClassSelectControllerPath;
}

struct cdsem::UiJobClassSelectControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QString huiDir;
    QString sourceCodeDir;
    QString classFileName;
#endif
    UiJobClassSelectControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/hui";
#endif
        huiFileName = "job_class_select_editor";
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
        classFileName = "job_class_select_controller";
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
class cdsem::UiJobClassSelectController {
private:
    const cdsem::UiJobClassSelectControllerPath pathInfo;
public:
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QList<CDE90LayoutScriptLineObject> __allWidgets;
#endif
    DatabaseComboBox* deviceItemBox = nullptr;
    CDE90ListView* classListView = nullptr;
    CDE90TextFieldBox* classSelectedTextField = nullptr;
    CDE90ButtonBox* okBtn = nullptr;
    UiJobClassSelectController() = default;
    ~UiJobClassSelectController() noexcept {}

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
        deviceItemBox = helper.get<DatabaseComboBox>("deviceItemBox");
        classListView = helper.get<CDE90ListView>("classListView");
        classSelectedTextField = helper.get<CDE90TextFieldBox>("classSelectedTextField");
        okBtn = helper.get<CDE90ButtonBox>("okBtn");

    }

#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    [[nodiscard]] static CodeEditorController* createCodeEditor(const cdsem::UiJobClassSelectControllerPath& path) {
        return getCodeEditor<cdsem::UiJobClassSelectControllerPath>(path);
    }

#endif

};
#endif
