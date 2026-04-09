#ifndef UI_PATTERN_RECOGNITION_CONTROLLER
#define UI_PATTERN_RECOGNITION_CONTROLLER
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
#include "cde90_spliter_line.h"
#include "cde90_check_box.h"
#include "cde90_combo_box.h"
#include "cde90_button.h"
#include "cde90_label.h"
#include <QWidget>

/**
 * 本类由编辑器自动生成。
 * getCodeEditor函数部分可以人工修改，用于内部编辑器。
 */
namespace cdsem {
class UiPatternRecognitionController;
struct UiPatternRecognitionControllerPath;
}

struct cdsem::UiPatternRecognitionControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QString huiDir;
    QString sourceCodeDir;
    QString classFileName;
#endif
    UiPatternRecognitionControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "pattern_recognition";
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
        classFileName = "pattern_recognition_controller";
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
class cdsem::UiPatternRecognitionController {
private:
    const cdsem::UiPatternRecognitionControllerPath pathInfo;
public:
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QList<CDE90LayoutScriptLineObject> __allWidgets;
#endif
    DatabaseComboBox* kindBtn = nullptr;
    DatabaseComboBox* processorBtn = nullptr;
    DatabaseComboBox* algorithmBtn = nullptr;
    CDE90TextFieldBox* lowLimitTextField = nullptr;
    CDE90Label* lineSpaceDetectionLabel = nullptr;
    DatabaseComboBox* lineSpaceDetectionItemBox = nullptr;
    CDE90Label* macroAlignLabel = nullptr;
    CDE90ComboBox* macroAlignItemBox = nullptr;
    CDE90ButtonBox* saveDefaultBtn = nullptr;
    CDE90SpliterHLine* line1 = nullptr;
    QWidget* execBeforeRegContent = nullptr;
    CDE90CheckBox* abcCbox = nullptr;
    CDE90CheckBox* autoFocusCBox = nullptr;
    QWidget* loadRegisteredContent = nullptr;
    CDE90CheckBox* ampCursorCbox = nullptr;
    CDE90CheckBox* axbCbox = nullptr;
    CDE90ButtonBox* okBtn = nullptr;
    CDE90ButtonBox* defaultBtn = nullptr;
    CDE90ButtonBox* cancelBtn = nullptr;
    UiPatternRecognitionController() = default;
    ~UiPatternRecognitionController() noexcept {}

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
        kindBtn = helper.get<DatabaseComboBox>("kindBtn");
        processorBtn = helper.get<DatabaseComboBox>("processorBtn");
        algorithmBtn = helper.get<DatabaseComboBox>("algorithmBtn");
        lowLimitTextField = helper.get<CDE90TextFieldBox>("lowLimitTextField");
        lineSpaceDetectionLabel = helper.get<CDE90Label>("lineSpaceDetectionLabel");
        lineSpaceDetectionItemBox = helper.get<DatabaseComboBox>("lineSpaceDetectionItemBox");
        macroAlignLabel = helper.get<CDE90Label>("macroAlignLabel");
        macroAlignItemBox = helper.get<CDE90ComboBox>("macroAlignItemBox");
        saveDefaultBtn = helper.get<CDE90ButtonBox>("saveDefaultBtn");
        line1 = helper.get<CDE90SpliterHLine>("line1");
        execBeforeRegContent = helper.get<QWidget>("execBeforeRegContent");
        abcCbox = helper.get<CDE90CheckBox>("abcCbox");
        autoFocusCBox = helper.get<CDE90CheckBox>("autoFocusCBox");
        loadRegisteredContent = helper.get<QWidget>("loadRegisteredContent");
        ampCursorCbox = helper.get<CDE90CheckBox>("ampCursorCbox");
        axbCbox = helper.get<CDE90CheckBox>("axbCbox");
        okBtn = helper.get<CDE90ButtonBox>("okBtn");
        defaultBtn = helper.get<CDE90ButtonBox>("defaultBtn");
        cancelBtn = helper.get<CDE90ButtonBox>("cancelBtn");

    }

#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    [[nodiscard]] static CodeEditorController* createCodeEditor(const cdsem::UiPatternRecognitionControllerPath& path) {
        return getCodeEditor<cdsem::UiPatternRecognitionControllerPath>(path);
    }

#endif

};
#endif
