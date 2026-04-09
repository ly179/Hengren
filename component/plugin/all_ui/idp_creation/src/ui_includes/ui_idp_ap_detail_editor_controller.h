#ifndef UI_IDP_AP_DETAIL_EDITOR_CONTROLLER
#define UI_IDP_AP_DETAIL_EDITOR_CONTROLLER
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
#include <QFrame>
#include "cde90_text_field.h"
#include "cde90_button.h"

/**
 * 本类由编辑器自动生成。
 * getCodeEditor函数部分可以人工修改，用于内部编辑器。
 */
namespace cdsem {
class UiIdpApDetailEditorController;
struct UiIdpApDetailEditorControllerPath;
}

struct cdsem::UiIdpApDetailEditorControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QString huiDir;
    QString sourceCodeDir;
    QString classFileName;
#endif
    UiIdpApDetailEditorControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "idp_ap_detail_setting";
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
        classFileName = "idp_ap_detail_editor_controller";
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
class cdsem::UiIdpApDetailEditorController {
private:
    const cdsem::UiIdpApDetailEditorControllerPath pathInfo;
public:
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QList<CDE90LayoutScriptLineObject> __allWidgets;
#endif
    CDE90TextFieldBox* titleNo = nullptr;
    CDE90TextFieldBox* originXTextField = nullptr;
    CDE90ButtonBox* originMoveBtn = nullptr;
    CDE90TextFieldBox* originYTextField = nullptr;
    CDE90ButtonBox* originRegBtn = nullptr;
    CDE90ButtonBox* originGetBtn = nullptr;
    CDE90TextFieldBox* dxTextField = nullptr;
    CDE90ButtonBox* dRegBtn = nullptr;
    CDE90TextFieldBox* dyTextField = nullptr;
    CDE90ButtonBox* dMoveBtn = nullptr;
    QFrame* alignmentTemplateContent = nullptr;
    CDE90ButtonBox* okBtn = nullptr;
    CDE90ButtonBox* testBtn = nullptr;
    CDE90ButtonBox* cancelBtn = nullptr;
    UiIdpApDetailEditorController() = default;
    ~UiIdpApDetailEditorController() noexcept {}

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
        titleNo = helper.get<CDE90TextFieldBox>("titleNo");
        originXTextField = helper.get<CDE90TextFieldBox>("originXTextField");
        originMoveBtn = helper.get<CDE90ButtonBox>("originMoveBtn");
        originYTextField = helper.get<CDE90TextFieldBox>("originYTextField");
        originRegBtn = helper.get<CDE90ButtonBox>("originRegBtn");
        originGetBtn = helper.get<CDE90ButtonBox>("originGetBtn");
        dxTextField = helper.get<CDE90TextFieldBox>("dxTextField");
        dRegBtn = helper.get<CDE90ButtonBox>("dRegBtn");
        dyTextField = helper.get<CDE90TextFieldBox>("dyTextField");
        dMoveBtn = helper.get<CDE90ButtonBox>("dMoveBtn");
        alignmentTemplateContent = helper.get<QFrame>("alignmentTemplateContent");
        okBtn = helper.get<CDE90ButtonBox>("okBtn");
        testBtn = helper.get<CDE90ButtonBox>("testBtn");
        cancelBtn = helper.get<CDE90ButtonBox>("cancelBtn");

    }

#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    [[nodiscard]] static CodeEditorController* createCodeEditor(const cdsem::UiIdpApDetailEditorControllerPath& path) {
        return getCodeEditor<cdsem::UiIdpApDetailEditorControllerPath>(path);
    }

#endif

};
#endif
