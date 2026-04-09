#ifndef UI_IDP_P_D_NO_CONTENT
#define UI_IDP_P_D_NO_CONTENT
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
#include "cde90_table_view.h"
#include "cde90_text_field.h"
#include "cde90_label.h"
#include "cde90_button.h"

/**
 * 本类由编辑器自动生成。
 * getCodeEditor函数部分可以人工修改，用于内部编辑器。
 */
namespace cdsem {
class UiIdpPDNoContent;
struct UiIdpPDNoContentPath;
}

struct cdsem::UiIdpPDNoContentPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QString huiDir;
    QString sourceCodeDir;
    QString classFileName;
#endif
    UiIdpPDNoContentPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "p_d_no_editor";
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
        classFileName = "idp_p_d_no_content";
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
class cdsem::UiIdpPDNoContent {
private:
    const cdsem::UiIdpPDNoContentPath pathInfo;
public:
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QList<CDE90LayoutScriptLineObject> __allWidgets;
#endif
    CDE90TableView* contentTable = nullptr;
    CDE90Label* titleLabel = nullptr;
    CDE90TextFieldBox* noTextField = nullptr;
    CDE90ButtonBox* applyBtn = nullptr;
    CDE90ButtonBox* reNumBtn = nullptr;
    CDE90ButtonBox* okBtn = nullptr;
    CDE90ButtonBox* cancelBtn = nullptr;
    UiIdpPDNoContent() = default;
    ~UiIdpPDNoContent() noexcept {}

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
        contentTable = helper.get<CDE90TableView>("contentTable");
        titleLabel = helper.get<CDE90Label>("titleLabel");
        noTextField = helper.get<CDE90TextFieldBox>("noTextField");
        applyBtn = helper.get<CDE90ButtonBox>("applyBtn");
        reNumBtn = helper.get<CDE90ButtonBox>("reNumBtn");
        okBtn = helper.get<CDE90ButtonBox>("okBtn");
        cancelBtn = helper.get<CDE90ButtonBox>("cancelBtn");

    }

#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    [[nodiscard]] static CodeEditorController* createCodeEditor(const cdsem::UiIdpPDNoContentPath& path) {
        return getCodeEditor<cdsem::UiIdpPDNoContentPath>(path);
    }

#endif

};
#endif
