#ifndef UI_AMP_MPPC_EDIT_CONTROLLER
#define UI_AMP_MPPC_EDIT_CONTROLLER
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
#include "cde90_combo_box.h"
#include "database_combo_box.h"
#include "cde90_text_field.h"

/**
 * 本类由编辑器自动生成。
 * getCodeEditor函数部分可以人工修改，用于内部编辑器。
 */
namespace cdsem {
class UiAmpMppcEditController;
struct UiAmpMppcEditControllerPath;
}

struct cdsem::UiAmpMppcEditControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QString huiDir;
    QString sourceCodeDir;
    QString classFileName;
#endif
    UiAmpMppcEditControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "amp_mppc_editor";
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
        classFileName = "amp_mppc_edit_controller";
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
class cdsem::UiAmpMppcEditController {
private:
    const cdsem::UiAmpMppcEditControllerPath pathInfo;
public:
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QList<CDE90LayoutScriptLineObject> __allWidgets;
#endif
    CDE90ComboBox* objectCBox = nullptr;
    DatabaseComboBox* kindCBox = nullptr;
    DatabaseComboBox* dataCBox = nullptr;
    CDE90TextFieldBox* measurePointTextField = nullptr;
    CDE90TextFieldBox* sumLineTextField = nullptr;
    CDE90ComboBox* cursorCBox = nullptr;
    CDE90TextFieldBox* designValueTextField = nullptr;
    CDE90TextFieldBox* searchAreaTextField = nullptr;
    CDE90TextFieldBox* smoothingTextField = nullptr;
    CDE90TextFieldBox* inspectAreaTextField = nullptr;
    CDE90TextFieldBox* upperInnerTextField = nullptr;
    CDE90TextFieldBox* lowerInnerTextField = nullptr;
    CDE90TextFieldBox* upperOuterTextField = nullptr;
    CDE90TextFieldBox* lowerOuterTextField = nullptr;
    CDE90TextFieldBox* basePointTextField = nullptr;
    CDE90TextFieldBox* bbcTextField = nullptr;
    DatabaseComboBox* measurePositionCBox = nullptr;
    UiAmpMppcEditController() = default;
    ~UiAmpMppcEditController() noexcept {}

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
        objectCBox = helper.get<CDE90ComboBox>("objectCBox");
        kindCBox = helper.get<DatabaseComboBox>("kindCBox");
        dataCBox = helper.get<DatabaseComboBox>("dataCBox");
        measurePointTextField = helper.get<CDE90TextFieldBox>("measurePointTextField");
        sumLineTextField = helper.get<CDE90TextFieldBox>("sumLineTextField");
        cursorCBox = helper.get<CDE90ComboBox>("cursorCBox");
        designValueTextField = helper.get<CDE90TextFieldBox>("designValueTextField");
        searchAreaTextField = helper.get<CDE90TextFieldBox>("searchAreaTextField");
        smoothingTextField = helper.get<CDE90TextFieldBox>("smoothingTextField");
        inspectAreaTextField = helper.get<CDE90TextFieldBox>("inspectAreaTextField");
        upperInnerTextField = helper.get<CDE90TextFieldBox>("upperInnerTextField");
        lowerInnerTextField = helper.get<CDE90TextFieldBox>("lowerInnerTextField");
        upperOuterTextField = helper.get<CDE90TextFieldBox>("upperOuterTextField");
        lowerOuterTextField = helper.get<CDE90TextFieldBox>("lowerOuterTextField");
        basePointTextField = helper.get<CDE90TextFieldBox>("basePointTextField");
        bbcTextField = helper.get<CDE90TextFieldBox>("bbcTextField");
        measurePositionCBox = helper.get<DatabaseComboBox>("measurePositionCBox");

    }

#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    [[nodiscard]] static CodeEditorController* createCodeEditor(const cdsem::UiAmpMppcEditControllerPath& path) {
        return getCodeEditor<cdsem::UiAmpMppcEditControllerPath>(path);
    }

#endif

};
#endif
