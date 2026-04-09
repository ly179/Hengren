#ifndef UI_FOCUS_STIGMA_CONTENT
#define UI_FOCUS_STIGMA_CONTENT
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
#include "database_combo_box.h"
#include "cde90_text_field.h"
#include "cde90_check_box.h"
#include "cde90_button.h"

/**
 * 本类由编辑器自动生成。
 * getCodeEditor函数部分可以人工修改，用于内部编辑器。
 */
namespace cdsem {
class UiFocusStigmaContent;
struct UiFocusStigmaContentPath;
}

struct cdsem::UiFocusStigmaContentPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QString huiDir;
    QString sourceCodeDir;
    QString classFileName;
#endif
    UiFocusStigmaContentPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "focus_stigma";
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
        classFileName = "focus_stigma_content";
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
class cdsem::UiFocusStigmaContent {
private:
    const cdsem::UiFocusStigmaContentPath pathInfo;
public:
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QList<CDE90LayoutScriptLineObject> __allWidgets;
#endif
    CDE90CheckBox* autoStigmaCbox = nullptr;
    CDE90TextFieldBox* dxTextField = nullptr;
    CDE90ButtonBox* regBtn = nullptr;
    CDE90TextFieldBox* dyTextField = nullptr;
    CDE90TextFieldBox* waitTextField = nullptr;
    CDE90TextFieldBox* magTextField = nullptr;
    CDE90ButtonBox* readBtn = nullptr;
    DatabaseComboBox* modeComboBox = nullptr;
    CDE90ButtonBox* offsetBtn = nullptr;
    CDE90TextFieldBox* offsetTextField = nullptr;
    UiFocusStigmaContent() = default;
    ~UiFocusStigmaContent() noexcept {}

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
        autoStigmaCbox = helper.get<CDE90CheckBox>("autoStigmaCbox");
        dxTextField = helper.get<CDE90TextFieldBox>("dxTextField");
        regBtn = helper.get<CDE90ButtonBox>("regBtn");
        dyTextField = helper.get<CDE90TextFieldBox>("dyTextField");
        waitTextField = helper.get<CDE90TextFieldBox>("waitTextField");
        magTextField = helper.get<CDE90TextFieldBox>("magTextField");
        readBtn = helper.get<CDE90ButtonBox>("readBtn");
        modeComboBox = helper.get<DatabaseComboBox>("modeComboBox");
        offsetBtn = helper.get<CDE90ButtonBox>("offsetBtn");
        offsetTextField = helper.get<CDE90TextFieldBox>("offsetTextField");

    }

#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    [[nodiscard]] static CodeEditorController* createCodeEditor(const cdsem::UiFocusStigmaContentPath& path) {
        return getCodeEditor<cdsem::UiFocusStigmaContentPath>(path);
    }

#endif

};
#endif
