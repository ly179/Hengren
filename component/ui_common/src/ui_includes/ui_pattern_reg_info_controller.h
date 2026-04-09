#ifndef UI_PATTERN_REG_INFO_CONTROLLER
#define UI_PATTERN_REG_INFO_CONTROLLER
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
#include "cde90_scroll_area.h"
#include "cde90_text_field.h"
#include "cde90_button.h"

/**
 * 本类由编辑器自动生成。
 * getCodeEditor函数部分可以人工修改，用于内部编辑器。
 */
namespace cdsem {
class UiPatternRegInfoController;
struct UiPatternRegInfoControllerPath;
}

struct cdsem::UiPatternRegInfoControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QString huiDir;
    QString sourceCodeDir;
    QString classFileName;
#endif
    UiPatternRegInfoControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "pattern_reg_info";
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
        classFileName = "pattern_reg_info_controller";
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
class cdsem::UiPatternRegInfoController {
private:
    const cdsem::UiPatternRegInfoControllerPath pathInfo;
public:
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QList<CDE90LayoutScriptLineObject> __allWidgets;
#endif
    CDE90TextFieldBox* nameTextField = nullptr;
    CDE90TextFieldBox* kindTextField = nullptr;
    CDE90TextFieldBox* scoreTextField = nullptr;
    CDE90TextFieldBox* algorithmTextField = nullptr;
    CDE90TextFieldBox* qualityTextField = nullptr;
    CDE90TextFieldBox* detectPatternTextField = nullptr;
    CDE90TextFieldBox* modeSizeXTextField = nullptr;
    CDE90TextFieldBox* modeSizeYTextField = nullptr;
    QWidget* lineSpaceContent = nullptr;
    CDE90TextFieldBox* lineSpaceDetectTextField = nullptr;
    CDE90TextFieldBox* processTextField = nullptr;
    CDE90ScrollArea* additionContent = nullptr;
    CDE90ButtonBox* printBtn = nullptr;
    CDE90ButtonBox* cancelBtn = nullptr;
    UiPatternRegInfoController() = default;
    ~UiPatternRegInfoController() noexcept {}

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
        nameTextField = helper.get<CDE90TextFieldBox>("nameTextField");
        kindTextField = helper.get<CDE90TextFieldBox>("kindTextField");
        scoreTextField = helper.get<CDE90TextFieldBox>("scoreTextField");
        algorithmTextField = helper.get<CDE90TextFieldBox>("algorithmTextField");
        qualityTextField = helper.get<CDE90TextFieldBox>("qualityTextField");
        detectPatternTextField = helper.get<CDE90TextFieldBox>("detectPatternTextField");
        modeSizeXTextField = helper.get<CDE90TextFieldBox>("modeSizeXTextField");
        modeSizeYTextField = helper.get<CDE90TextFieldBox>("modeSizeYTextField");
        lineSpaceContent = helper.get<QWidget>("lineSpaceContent");
        lineSpaceDetectTextField = helper.get<CDE90TextFieldBox>("lineSpaceDetectTextField");
        processTextField = helper.get<CDE90TextFieldBox>("processTextField");
        additionContent = helper.get<CDE90ScrollArea>("additionContent");
        printBtn = helper.get<CDE90ButtonBox>("printBtn");
        cancelBtn = helper.get<CDE90ButtonBox>("cancelBtn");

    }

#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    [[nodiscard]] static CodeEditorController* createCodeEditor(const cdsem::UiPatternRegInfoControllerPath& path) {
        return getCodeEditor<cdsem::UiPatternRegInfoControllerPath>(path);
    }

#endif

};
#endif
