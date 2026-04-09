#ifndef UI_RECIPE_EDITOR_CONTROLLER
#define UI_RECIPE_EDITOR_CONTROLLER
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
#include "wafer_cassette_widget.h"
#include "cde90_radio_button.h"
#include "cde90_check_box.h"
#include "cde90_button.h"

/**
 * 本类由编辑器自动生成。
 * getCodeEditor函数部分可以人工修改，用于内部编辑器。
 */
namespace cdsem {
class UiRecipeEditorController;
struct UiRecipeEditorControllerPath;
}

struct cdsem::UiRecipeEditorControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QString huiDir;
    QString sourceCodeDir;
    QString classFileName;
#endif
    UiRecipeEditorControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/hui";
#endif
        huiFileName = "recipe_editor";
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
        classFileName = "recipe_editor_controller";
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
class cdsem::UiRecipeEditorController {
private:
    const cdsem::UiRecipeEditorControllerPath pathInfo;
public:
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QList<CDE90LayoutScriptLineObject> __allWidgets;
#endif
    QAction* saveAct = nullptr;
    QAction* saveAsAct = nullptr;
    QAction* exitAct = nullptr;
    QAction* envAct = nullptr;
    CDE90TextFieldBox* classTextField = nullptr;
    CDE90TextFieldBox* recipeTextField = nullptr;
    CDE90ButtonBox* idwBtn = nullptr;
    CDE90TextFieldBox* idwTextField = nullptr;
    CDE90ButtonBox* idpBtn = nullptr;
    CDE90TextFieldBox* idpTextField = nullptr;
    CDE90TextFieldBox* semConditionNoTextField = nullptr;
    CDE90RadioButton* highOxideRadio = nullptr;
    CDE90RadioButton* normalRadio = nullptr;
    CDE90RadioButton* arfRadio = nullptr;
    CDE90CheckBox* autoCalibrationCBox = nullptr;
    CDE90RadioButton* naRadio = nullptr;
    CDE90RadioButton* femMeasureRadio = nullptr;
    CDE90ButtonBox* femMeasureBtn = nullptr;
    CDE90RadioButton* mainRadio = nullptr;
    CDE90ButtonBox* mainBtn = nullptr;
    CDE90RadioButton* microscaleRadio = nullptr;
    CDE90ButtonBox* microscaleBtn = nullptr;
    CDE90RadioButton* nanoscaleRadio = nullptr;
    CDE90ButtonBox* nanoscaleBtn = nullptr;
    WaferCassetteWidget* cassetteWidget = nullptr;
    UiRecipeEditorController() = default;
    ~UiRecipeEditorController() noexcept {}

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
        saveAct = helper.get<QAction>("saveAct");
        saveAsAct = helper.get<QAction>("saveAsAct");
        exitAct = helper.get<QAction>("exitAct");
        envAct = helper.get<QAction>("envAct");
        classTextField = helper.get<CDE90TextFieldBox>("classTextField");
        recipeTextField = helper.get<CDE90TextFieldBox>("recipeTextField");
        idwBtn = helper.get<CDE90ButtonBox>("idwBtn");
        idwTextField = helper.get<CDE90TextFieldBox>("idwTextField");
        idpBtn = helper.get<CDE90ButtonBox>("idpBtn");
        idpTextField = helper.get<CDE90TextFieldBox>("idpTextField");
        semConditionNoTextField = helper.get<CDE90TextFieldBox>("semConditionNoTextField");
        highOxideRadio = helper.get<CDE90RadioButton>("highOxideRadio");
        normalRadio = helper.get<CDE90RadioButton>("normalRadio");
        arfRadio = helper.get<CDE90RadioButton>("arfRadio");
        autoCalibrationCBox = helper.get<CDE90CheckBox>("autoCalibrationCBox");
        naRadio = helper.get<CDE90RadioButton>("naRadio");
        femMeasureRadio = helper.get<CDE90RadioButton>("femMeasureRadio");
        femMeasureBtn = helper.get<CDE90ButtonBox>("femMeasureBtn");
        mainRadio = helper.get<CDE90RadioButton>("mainRadio");
        mainBtn = helper.get<CDE90ButtonBox>("mainBtn");
        microscaleRadio = helper.get<CDE90RadioButton>("microscaleRadio");
        microscaleBtn = helper.get<CDE90ButtonBox>("microscaleBtn");
        nanoscaleRadio = helper.get<CDE90RadioButton>("nanoscaleRadio");
        nanoscaleBtn = helper.get<CDE90ButtonBox>("nanoscaleBtn");
        cassetteWidget = helper.get<WaferCassetteWidget>("cassetteWidget");

    }

#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    [[nodiscard]] static CodeEditorController* createCodeEditor(const cdsem::UiRecipeEditorControllerPath& path) {
        return getCodeEditor<cdsem::UiRecipeEditorControllerPath>(path);
    }

#endif

};
#endif
