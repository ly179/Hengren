#ifndef UI_AUTO_FOCUS_OFFSET_SETTING_CONTROLLER
#define UI_AUTO_FOCUS_OFFSET_SETTING_CONTROLLER
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
#include "cde90_button.h"
#include "cde90_label.h"

/**
 * 本类由编辑器自动生成。
 * getCodeEditor函数部分可以人工修改，用于内部编辑器。
 */
namespace cdsem {
class UiAutoFocusOffsetSettingController;
struct UiAutoFocusOffsetSettingControllerPath;
}

struct cdsem::UiAutoFocusOffsetSettingControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QString huiDir;
    QString sourceCodeDir;
    QString classFileName;
#endif
    UiAutoFocusOffsetSettingControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "auto_focus_offset_setting";
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
        classFileName = "auto_focus_offset_setting_controller";
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
class cdsem::UiAutoFocusOffsetSettingController {
private:
    const cdsem::UiAutoFocusOffsetSettingControllerPath pathInfo;
public:
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QList<CDE90LayoutScriptLineObject> __allWidgets;
#endif
    CDE90TextFieldBox* offsetTextField = nullptr;
    CDE90Label* diff2Label = nullptr;
    CDE90ButtonBox* afExeBtn = nullptr;
    CDE90ButtonBox* getOffsetBtn = nullptr;
    CDE90ButtonBox* testBtn = nullptr;
    CDE90ButtonBox* setBtn = nullptr;
    CDE90ButtonBox* cancelBtn = nullptr;
    UiAutoFocusOffsetSettingController() = default;
    ~UiAutoFocusOffsetSettingController() noexcept {}

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
        offsetTextField = helper.get<CDE90TextFieldBox>("offsetTextField");
        diff2Label = helper.get<CDE90Label>("diff2Label");
        afExeBtn = helper.get<CDE90ButtonBox>("afExeBtn");
        getOffsetBtn = helper.get<CDE90ButtonBox>("getOffsetBtn");
        testBtn = helper.get<CDE90ButtonBox>("testBtn");
        setBtn = helper.get<CDE90ButtonBox>("setBtn");
        cancelBtn = helper.get<CDE90ButtonBox>("cancelBtn");

    }

#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    [[nodiscard]] static CodeEditorController* createCodeEditor(const cdsem::UiAutoFocusOffsetSettingControllerPath& path) {
        return getCodeEditor<cdsem::UiAutoFocusOffsetSettingControllerPath>(path);
    }

#endif

};
#endif
