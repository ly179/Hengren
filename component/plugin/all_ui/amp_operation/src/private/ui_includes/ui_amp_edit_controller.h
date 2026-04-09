#ifndef UI_AMP_EDIT_CONTROLLER
#define UI_AMP_EDIT_CONTROLLER
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
#include <QWidget>
#include "cde90_combo_box.h"
#include "cde90_button.h"
#include <QAction>

/**
 * 本类由编辑器自动生成。
 * getCodeEditor函数部分可以人工修改，用于内部编辑器。
 */
namespace cdsem {
class UiAmpEditController;
struct UiAmpEditControllerPath;
}

struct cdsem::UiAmpEditControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QString huiDir;
    QString sourceCodeDir;
    QString classFileName;
#endif
    UiAmpEditControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "amp_editor";
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
        classFileName = "amp_edit_controller";
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
class cdsem::UiAmpEditController {
private:
    const cdsem::UiAmpEditControllerPath pathInfo;
public:
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QList<CDE90LayoutScriptLineObject> __allWidgets;
#endif
    QWidget* addSubContent = nullptr;
    CDE90ButtonBox* addBtn = nullptr;
    CDE90ButtonBox* subBtn = nullptr;
    CDE90ComboBox* measurementOptionCBox = nullptr;
    QAction* saveAct = nullptr;
    QAction* loadAct = nullptr;
    QWidget* toolContent = nullptr;
    CDE90ButtonBox* okBtn = nullptr;
    CDE90ButtonBox* applyBtn = nullptr;
    CDE90ButtonBox* cancelBtn = nullptr;
    UiAmpEditController() = default;
    ~UiAmpEditController() noexcept {}

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
        addSubContent = helper.get<QWidget>("addSubContent");
        addBtn = helper.get<CDE90ButtonBox>("addBtn");
        subBtn = helper.get<CDE90ButtonBox>("subBtn");
        measurementOptionCBox = helper.get<CDE90ComboBox>("measurementOptionCBox");
        saveAct = helper.get<QAction>("saveAct");
        loadAct = helper.get<QAction>("loadAct");
        toolContent = helper.get<QWidget>("toolContent");
        okBtn = helper.get<CDE90ButtonBox>("okBtn");
        applyBtn = helper.get<CDE90ButtonBox>("applyBtn");
        cancelBtn = helper.get<CDE90ButtonBox>("cancelBtn");

    }

#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    [[nodiscard]] static CodeEditorController* createCodeEditor(const cdsem::UiAmpEditControllerPath& path) {
        return getCodeEditor<cdsem::UiAmpEditControllerPath>(path);
    }

#endif

};
#endif
