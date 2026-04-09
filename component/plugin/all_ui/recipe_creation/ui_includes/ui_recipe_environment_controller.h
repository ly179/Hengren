#ifndef UI_RECIPE_ENVIRONMENT_CONTROLLER
#define UI_RECIPE_ENVIRONMENT_CONTROLLER
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
#include "cde90_combo_box.h"
#include "cde90_button.h"
#include <QWidget>

/**
 * 本类由编辑器自动生成。
 * getCodeEditor函数部分可以人工修改，用于内部编辑器。
 */
namespace cdsem {
class UiRecipeEnvironmentController;
struct UiRecipeEnvironmentControllerPath;
}

struct cdsem::UiRecipeEnvironmentControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QString huiDir;
    QString sourceCodeDir;
    QString classFileName;
#endif
    UiRecipeEnvironmentControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/hui";
#endif
        huiFileName = "recipe_environment_editor";
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
        classFileName = "recipe_environment_controller";
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
class cdsem::UiRecipeEnvironmentController {
private:
    const cdsem::UiRecipeEnvironmentControllerPath pathInfo;
public:
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QList<CDE90LayoutScriptLineObject> __allWidgets;
#endif
    CDE90ComboBox* waferExeItemBox = nullptr;
    CDE90ComboBox* waferExeManualAssistItemBox = nullptr;
    CDE90ComboBox* waferExeImageSaveItemBox = nullptr;
    CDE90ComboBox* measureAddressItemBox = nullptr;
    CDE90ComboBox* measureAddressManualAssistItemBox = nullptr;
    CDE90ComboBox* measureAddressImageSaveItemBox = nullptr;
    CDE90ComboBox* measureExeItemBox = nullptr;
    CDE90ComboBox* measureExeManualAssistItemBox = nullptr;
    CDE90ComboBox* measureExeImageSaveItemBox = nullptr;
    CDE90CheckBox* diskSaveCBox = nullptr;
    CDE90CheckBox* sendToHostCBox = nullptr;
    CDE90CheckBox* printOutCBox = nullptr;
    CDE90CheckBox* networkTransferCBox = nullptr;
    CDE90ComboBox* confirmItemBox = nullptr;
    QWidget* succRateContent = nullptr;
    CDE90TextFieldBox* confirmSuccRateTextField = nullptr;
    CDE90ComboBox* methodItemBox = nullptr;
    CDE90ComboBox* limitCheckItemBox = nullptr;
    CDE90CheckBox* editIdpCBox = nullptr;
    CDE90CheckBox* presetBcCBox = nullptr;
    CDE90CheckBox* retryAlignCBox = nullptr;
    CDE90CheckBox* retryAddressCBox = nullptr;
    CDE90CheckBox* retryPatternRegCBox = nullptr;
    CDE90CheckBox* reMeasureErrCBox = nullptr;
    CDE90CheckBox* periodicWaferAlignCBox = nullptr;
    CDE90TextFieldBox* ionizerTextField = nullptr;
    CDE90ButtonBox* okBtn = nullptr;
    CDE90ButtonBox* saveDefaultBtn = nullptr;
    CDE90ButtonBox* cancelBtn = nullptr;
    UiRecipeEnvironmentController() = default;
    ~UiRecipeEnvironmentController() noexcept {}

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
        waferExeItemBox = helper.get<CDE90ComboBox>("waferExeItemBox");
        waferExeManualAssistItemBox = helper.get<CDE90ComboBox>("waferExeManualAssistItemBox");
        waferExeImageSaveItemBox = helper.get<CDE90ComboBox>("waferExeImageSaveItemBox");
        measureAddressItemBox = helper.get<CDE90ComboBox>("measureAddressItemBox");
        measureAddressManualAssistItemBox = helper.get<CDE90ComboBox>("measureAddressManualAssistItemBox");
        measureAddressImageSaveItemBox = helper.get<CDE90ComboBox>("measureAddressImageSaveItemBox");
        measureExeItemBox = helper.get<CDE90ComboBox>("measureExeItemBox");
        measureExeManualAssistItemBox = helper.get<CDE90ComboBox>("measureExeManualAssistItemBox");
        measureExeImageSaveItemBox = helper.get<CDE90ComboBox>("measureExeImageSaveItemBox");
        diskSaveCBox = helper.get<CDE90CheckBox>("diskSaveCBox");
        sendToHostCBox = helper.get<CDE90CheckBox>("sendToHostCBox");
        printOutCBox = helper.get<CDE90CheckBox>("printOutCBox");
        networkTransferCBox = helper.get<CDE90CheckBox>("networkTransferCBox");
        confirmItemBox = helper.get<CDE90ComboBox>("confirmItemBox");
        succRateContent = helper.get<QWidget>("succRateContent");
        confirmSuccRateTextField = helper.get<CDE90TextFieldBox>("confirmSuccRateTextField");
        methodItemBox = helper.get<CDE90ComboBox>("methodItemBox");
        limitCheckItemBox = helper.get<CDE90ComboBox>("limitCheckItemBox");
        editIdpCBox = helper.get<CDE90CheckBox>("editIdpCBox");
        presetBcCBox = helper.get<CDE90CheckBox>("presetBcCBox");
        retryAlignCBox = helper.get<CDE90CheckBox>("retryAlignCBox");
        retryAddressCBox = helper.get<CDE90CheckBox>("retryAddressCBox");
        retryPatternRegCBox = helper.get<CDE90CheckBox>("retryPatternRegCBox");
        reMeasureErrCBox = helper.get<CDE90CheckBox>("reMeasureErrCBox");
        periodicWaferAlignCBox = helper.get<CDE90CheckBox>("periodicWaferAlignCBox");
        ionizerTextField = helper.get<CDE90TextFieldBox>("ionizerTextField");
        okBtn = helper.get<CDE90ButtonBox>("okBtn");
        saveDefaultBtn = helper.get<CDE90ButtonBox>("saveDefaultBtn");
        cancelBtn = helper.get<CDE90ButtonBox>("cancelBtn");

    }

#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    [[nodiscard]] static CodeEditorController* createCodeEditor(const cdsem::UiRecipeEnvironmentControllerPath& path) {
        return getCodeEditor<cdsem::UiRecipeEnvironmentControllerPath>(path);
    }

#endif

};
#endif
