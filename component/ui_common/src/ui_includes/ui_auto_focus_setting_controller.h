#ifndef UI_AUTO_FOCUS_SETTING_CONTROLLER
#define UI_AUTO_FOCUS_SETTING_CONTROLLER
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
#include "cde90_check_box.h"
#include "cde90_button.h"
#include <QWidget>
#include "cde90_radio_button.h"
#include <QFrame>

/**
 * 本类由编辑器自动生成。
 * getCodeEditor函数部分可以人工修改，用于内部编辑器。
 */
namespace cdsem {
class UiAutoFocusSettingController;
struct UiAutoFocusSettingControllerPath;
}

struct cdsem::UiAutoFocusSettingControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QString huiDir;
    QString sourceCodeDir;
    QString classFileName;
#endif
    UiAutoFocusSettingControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "auto_focus_setting";
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
        classFileName = "auto_focus_setting_controller";
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
class cdsem::UiAutoFocusSettingController {
private:
    const cdsem::UiAutoFocusSettingControllerPath pathInfo;
public:
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QList<CDE90LayoutScriptLineObject> __allWidgets;
#endif
    QWidget* mpFocusContent = nullptr;
    CDE90CheckBox* focusCbox = nullptr;
    QFrame* mpFocusDetailContent = nullptr;
    CDE90RadioButton* stigmaAndFocusRadio = nullptr;
    CDE90RadioButton* offsetRadio = nullptr;
    CDE90TextFieldBox* offsetTextField = nullptr;
    QWidget* apFocusContent = nullptr;
    CDE90CheckBox* autoFocusCbox = nullptr;
    QWidget* apFocusDetailContent = nullptr;
    CDE90ButtonBox* okBtn = nullptr;
    CDE90ButtonBox* backBtn = nullptr;
    CDE90ButtonBox* nextBtn = nullptr;
    CDE90ButtonBox* cancelBtn = nullptr;
    UiAutoFocusSettingController() = default;
    ~UiAutoFocusSettingController() noexcept {}

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
        mpFocusContent = helper.get<QWidget>("mpFocusContent");
        focusCbox = helper.get<CDE90CheckBox>("focusCbox");
        mpFocusDetailContent = helper.get<QFrame>("mpFocusDetailContent");
        stigmaAndFocusRadio = helper.get<CDE90RadioButton>("stigmaAndFocusRadio");
        offsetRadio = helper.get<CDE90RadioButton>("offsetRadio");
        offsetTextField = helper.get<CDE90TextFieldBox>("offsetTextField");
        apFocusContent = helper.get<QWidget>("apFocusContent");
        autoFocusCbox = helper.get<CDE90CheckBox>("autoFocusCbox");
        apFocusDetailContent = helper.get<QWidget>("apFocusDetailContent");
        okBtn = helper.get<CDE90ButtonBox>("okBtn");
        backBtn = helper.get<CDE90ButtonBox>("backBtn");
        nextBtn = helper.get<CDE90ButtonBox>("nextBtn");
        cancelBtn = helper.get<CDE90ButtonBox>("cancelBtn");

    }

#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    [[nodiscard]] static CodeEditorController* createCodeEditor(const cdsem::UiAutoFocusSettingControllerPath& path) {
        return getCodeEditor<cdsem::UiAutoFocusSettingControllerPath>(path);
    }

#endif

};
#endif
