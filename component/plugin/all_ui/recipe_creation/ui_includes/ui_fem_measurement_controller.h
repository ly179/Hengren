#ifndef UI_FEM_MEASUREMENT_CONTROLLER
#define UI_FEM_MEASUREMENT_CONTROLLER
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
#include "cde90_button.h"
#include "cde90_text_field.h"
#include "cde90_combo_box.h"
#include "cde90_radio_button.h"

/**
 * 本类由编辑器自动生成。
 * getCodeEditor函数部分可以人工修改，用于内部编辑器。
 */
namespace cdsem {
class UiFemMeasurementController;
struct UiFemMeasurementControllerPath;
}

struct cdsem::UiFemMeasurementControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QString huiDir;
    QString sourceCodeDir;
    QString classFileName;
#endif
    UiFemMeasurementControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/hui";
#endif
        huiFileName = "fem_measurement_editor";
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
        classFileName = "fem_measurement_controller";
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
class cdsem::UiFemMeasurementController {
private:
    const cdsem::UiFemMeasurementControllerPath pathInfo;
public:
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QList<CDE90LayoutScriptLineObject> __allWidgets;
#endif
    CDE90ComboBox* femTypeItemBox = nullptr;
    CDE90TextFieldBox* focusStartTextField = nullptr;
    CDE90TextFieldBox* focusStepTextField = nullptr;
    CDE90TextFieldBox* exposureStartTextField = nullptr;
    CDE90TextFieldBox* exposureStepTextField = nullptr;
    CDE90RadioButton* lowLeftChipRadio = nullptr;
    CDE90RadioButton* centerChipRadio = nullptr;
    CDE90ButtonBox* okBtn = nullptr;
    CDE90ButtonBox* cancelBtn = nullptr;
    UiFemMeasurementController() = default;
    ~UiFemMeasurementController() noexcept {}

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
        femTypeItemBox = helper.get<CDE90ComboBox>("femTypeItemBox");
        focusStartTextField = helper.get<CDE90TextFieldBox>("focusStartTextField");
        focusStepTextField = helper.get<CDE90TextFieldBox>("focusStepTextField");
        exposureStartTextField = helper.get<CDE90TextFieldBox>("exposureStartTextField");
        exposureStepTextField = helper.get<CDE90TextFieldBox>("exposureStepTextField");
        lowLeftChipRadio = helper.get<CDE90RadioButton>("lowLeftChipRadio");
        centerChipRadio = helper.get<CDE90RadioButton>("centerChipRadio");
        okBtn = helper.get<CDE90ButtonBox>("okBtn");
        cancelBtn = helper.get<CDE90ButtonBox>("cancelBtn");

    }

#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    [[nodiscard]] static CodeEditorController* createCodeEditor(const cdsem::UiFemMeasurementControllerPath& path) {
        return getCodeEditor<cdsem::UiFemMeasurementControllerPath>(path);
    }

#endif

};
#endif
