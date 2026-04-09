#ifndef UI_IDP_MP_DETAIL_EDITOR_CONTROLLER
#define UI_IDP_MP_DETAIL_EDITOR_CONTROLLER
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
#include <QAction>
#include "cde90_text_field.h"
#include "cde90_combo_box.h"
#include "cde90_button.h"

/**
 * 本类由编辑器自动生成。
 * getCodeEditor函数部分可以人工修改，用于内部编辑器。
 */
namespace cdsem {
class UiIdpMpDetailEditorController;
struct UiIdpMpDetailEditorControllerPath;
}

struct cdsem::UiIdpMpDetailEditorControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QString huiDir;
    QString sourceCodeDir;
    QString classFileName;
#endif
    UiIdpMpDetailEditorControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "idp_mp_detail_setting";
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
        classFileName = "idp_mp_detail_editor_controller";
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
class cdsem::UiIdpMpDetailEditorController {
private:
    const cdsem::UiIdpMpDetailEditorControllerPath pathInfo;
public:
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QList<CDE90LayoutScriptLineObject> __allWidgets;
#endif
    CDE90ComboBox* handleTypeItembox = nullptr;
    CDE90TextFieldBox* seqTitleTextield = nullptr;
    CDE90TextFieldBox* templateTitleTextField = nullptr;
    CDE90ButtonBox* templateBtn = nullptr;
    QAction* titleTempDLoadAct = nullptr;
    QAction* titleTempSLoadAct = nullptr;
    QAction* titleSaveAct = nullptr;
    CDE90ButtonBox* optionBtn = nullptr;
    CDE90ComboBox* posItembox = nullptr;
    CDE90TextFieldBox* dxTextField = nullptr;
    CDE90ButtonBox* regPosBtn = nullptr;
    CDE90ButtonBox* regPosMoreBtn = nullptr;
    CDE90TextFieldBox* dyTextField = nullptr;
    CDE90ButtonBox* movePosBtn = nullptr;
    CDE90ButtonBox* okBtn = nullptr;
    CDE90ButtonBox* wizardBtn = nullptr;
    CDE90ButtonBox* testBtn = nullptr;
    CDE90ButtonBox* cancelBtn = nullptr;
    UiIdpMpDetailEditorController() = default;
    ~UiIdpMpDetailEditorController() noexcept {}

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
        handleTypeItembox = helper.get<CDE90ComboBox>("handleTypeItembox");
        seqTitleTextield = helper.get<CDE90TextFieldBox>("seqTitleTextield");
        templateTitleTextField = helper.get<CDE90TextFieldBox>("templateTitleTextField");
        templateBtn = helper.get<CDE90ButtonBox>("templateBtn");
        titleTempDLoadAct = helper.get<QAction>("titleTempDLoadAct");
        titleTempSLoadAct = helper.get<QAction>("titleTempSLoadAct");
        titleSaveAct = helper.get<QAction>("titleSaveAct");
        optionBtn = helper.get<CDE90ButtonBox>("optionBtn");
        posItembox = helper.get<CDE90ComboBox>("posItembox");
        dxTextField = helper.get<CDE90TextFieldBox>("dxTextField");
        regPosBtn = helper.get<CDE90ButtonBox>("regPosBtn");
        regPosMoreBtn = helper.get<CDE90ButtonBox>("regPosMoreBtn");
        dyTextField = helper.get<CDE90TextFieldBox>("dyTextField");
        movePosBtn = helper.get<CDE90ButtonBox>("movePosBtn");
        okBtn = helper.get<CDE90ButtonBox>("okBtn");
        wizardBtn = helper.get<CDE90ButtonBox>("wizardBtn");
        testBtn = helper.get<CDE90ButtonBox>("testBtn");
        cancelBtn = helper.get<CDE90ButtonBox>("cancelBtn");

    }

#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    [[nodiscard]] static CodeEditorController* createCodeEditor(const cdsem::UiIdpMpDetailEditorControllerPath& path) {
        return getCodeEditor<cdsem::UiIdpMpDetailEditorControllerPath>(path);
    }

#endif

};
#endif
