#ifndef UI_AMP_CORNER_EDIT_CONTROLLER
#define UI_AMP_CORNER_EDIT_CONTROLLER
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
#include "cde90_text_field.h"

/**
 * 本类由编辑器自动生成。
 * getCodeEditor函数部分可以人工修改，用于内部编辑器。
 */
namespace cdsem {
class UiAmpCornerEditController;
struct UiAmpCornerEditControllerPath;
}

struct cdsem::UiAmpCornerEditControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QString huiDir;
    QString sourceCodeDir;
    QString classFileName;
#endif
    UiAmpCornerEditControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "amp_corner_editor";
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
        classFileName = "amp_corner_edit_controller";
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
class cdsem::UiAmpCornerEditController {
private:
    const cdsem::UiAmpCornerEditControllerPath pathInfo;
public:
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QList<CDE90LayoutScriptLineObject> __allWidgets;
#endif
    CDE90ComboBox* objectCBox = nullptr;
    CDE90ComboBox* edgeCBox = nullptr;
    CDE90ComboBox* dataCBox = nullptr;
    CDE90TextFieldBox* measurePointTextField = nullptr;
    CDE90ComboBox* methodCBox = nullptr;
    CDE90TextFieldBox* designValueTextField = nullptr;
    CDE90TextFieldBox* smoothingTextField = nullptr;
    CDE90TextFieldBox* diffTextField = nullptr;
    CDE90TextFieldBox* thresholdTextField = nullptr;
    UiAmpCornerEditController() = default;
    ~UiAmpCornerEditController() noexcept {}

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
        edgeCBox = helper.get<CDE90ComboBox>("edgeCBox");
        dataCBox = helper.get<CDE90ComboBox>("dataCBox");
        measurePointTextField = helper.get<CDE90TextFieldBox>("measurePointTextField");
        methodCBox = helper.get<CDE90ComboBox>("methodCBox");
        designValueTextField = helper.get<CDE90TextFieldBox>("designValueTextField");
        smoothingTextField = helper.get<CDE90TextFieldBox>("smoothingTextField");
        diffTextField = helper.get<CDE90TextFieldBox>("diffTextField");
        thresholdTextField = helper.get<CDE90TextFieldBox>("thresholdTextField");

    }

#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    [[nodiscard]] static CodeEditorController* createCodeEditor(const cdsem::UiAmpCornerEditControllerPath& path) {
        return getCodeEditor<cdsem::UiAmpCornerEditControllerPath>(path);
    }

#endif

};
#endif
