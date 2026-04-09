#ifndef UI_AMP_GAP_EDIT_CONTROLLER
#define UI_AMP_GAP_EDIT_CONTROLLER
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
#include "detection_common_setting_controller.h"
#include "cde90_text_field.h"
#include "database_combo_box.h"
#include "cde90_combo_box.h"

/**
 * 本类由编辑器自动生成。
 * getCodeEditor函数部分可以人工修改，用于内部编辑器。
 */
namespace cdsem {
class UiAmpGapEditController;
struct UiAmpGapEditControllerPath;
}

struct cdsem::UiAmpGapEditControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QString huiDir;
    QString sourceCodeDir;
    QString classFileName;
#endif
    UiAmpGapEditControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "amp_gap_editor";
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
        classFileName = "amp_gap_edit_controller";
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
class cdsem::UiAmpGapEditController {
private:
    const cdsem::UiAmpGapEditControllerPath pathInfo;
public:
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QList<CDE90LayoutScriptLineObject> __allWidgets;
#endif
    CDE90ComboBox* kindCBox = nullptr;
    CDE90ComboBox* dataCBox = nullptr;
    CDE90ComboBox* directCBox = nullptr;
    CDE90TextFieldBox* measurePointTextField = nullptr;
    CDE90TextFieldBox* sumLineOrPointTextField = nullptr;
    DatabaseComboBox* acdCBox = nullptr;
    CDE90TextFieldBox* acdTargetTextField = nullptr;
    CDE90TextFieldBox* acdLimitTextField = nullptr;
    CDE90ComboBox* methodCBox = nullptr;
    CDE90TextFieldBox* designValueTextField = nullptr;
    CDE90TextFieldBox* positionOffsetTextField = nullptr;
    CDE90TextFieldBox* searchAreaTextField = nullptr;
    CDE90TextFieldBox* smoothintTextField = nullptr;
    CDE90TextFieldBox* inspectAreaTextField = nullptr;
    CDE90TextFieldBox* diffTextField = nullptr;
    DetectionCommonSettingController* commonSettingController = nullptr;
    UiAmpGapEditController() = default;
    ~UiAmpGapEditController() noexcept {}

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
        kindCBox = helper.get<CDE90ComboBox>("kindCBox");
        dataCBox = helper.get<CDE90ComboBox>("dataCBox");
        directCBox = helper.get<CDE90ComboBox>("directCBox");
        measurePointTextField = helper.get<CDE90TextFieldBox>("measurePointTextField");
        sumLineOrPointTextField = helper.get<CDE90TextFieldBox>("sumLineOrPointTextField");
        acdCBox = helper.get<DatabaseComboBox>("acdCBox");
        acdTargetTextField = helper.get<CDE90TextFieldBox>("acdTargetTextField");
        acdLimitTextField = helper.get<CDE90TextFieldBox>("acdLimitTextField");
        methodCBox = helper.get<CDE90ComboBox>("methodCBox");
        designValueTextField = helper.get<CDE90TextFieldBox>("designValueTextField");
        positionOffsetTextField = helper.get<CDE90TextFieldBox>("positionOffsetTextField");
        searchAreaTextField = helper.get<CDE90TextFieldBox>("searchAreaTextField");
        smoothintTextField = helper.get<CDE90TextFieldBox>("smoothintTextField");
        inspectAreaTextField = helper.get<CDE90TextFieldBox>("inspectAreaTextField");
        diffTextField = helper.get<CDE90TextFieldBox>("diffTextField");
        commonSettingController = helper.get<DetectionCommonSettingController>("commonSettingController");

    }

#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    [[nodiscard]] static CodeEditorController* createCodeEditor(const cdsem::UiAmpGapEditControllerPath& path) {
        return getCodeEditor<cdsem::UiAmpGapEditControllerPath>(path);
    }

#endif

};
#endif
