#ifndef UI_DETECTION_COMMON_SETTING_CONTROLLER
#define UI_DETECTION_COMMON_SETTING_CONTROLLER
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
#include "cde90_label.h"
#include <QWidget>
#include "cde90_check_box.h"
#include "database_combo_box.h"

/**
 * 本类由编辑器自动生成。
 * getCodeEditor函数部分可以人工修改，用于内部编辑器。
 */
namespace cdsem {
class UiDetectionCommonSettingController;
struct UiDetectionCommonSettingControllerPath;
}

struct cdsem::UiDetectionCommonSettingControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QString huiDir;
    QString sourceCodeDir;
    QString classFileName;
#endif
    UiDetectionCommonSettingControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "detection_common_setting";
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
        classFileName = "detection_common_setting_controller";
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
class cdsem::UiDetectionCommonSettingController {
private:
    const cdsem::UiDetectionCommonSettingControllerPath pathInfo;
public:
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QList<CDE90LayoutScriptLineObject> __allWidgets;
#endif
    CDE90Label* title1Label = nullptr;
    CDE90Label* title2Label = nullptr;
    CDE90Label* shapeLabel = nullptr;
    QWidget* shape1Content = nullptr;
    DatabaseComboBox* shape1CBox = nullptr;
    QWidget* shape2Content = nullptr;
    DatabaseComboBox* shape2CBox = nullptr;
    CDE90TextFieldBox* threshold1TextField = nullptr;
    CDE90TextFieldBox* threshold2TextField = nullptr;
    CDE90TextFieldBox* edgeNum1TextField = nullptr;
    CDE90TextFieldBox* edgeNum2TextField = nullptr;
    CDE90TextFieldBox* bLineStartPoint1TextField = nullptr;
    CDE90TextFieldBox* bLineStartPoint2TextField = nullptr;
    CDE90TextFieldBox* bLineArea1TextField = nullptr;
    CDE90TextFieldBox* bLineArea2TextField = nullptr;
    DatabaseComboBox* edgeSearchDirect1CBox = nullptr;
    DatabaseComboBox* edgeSearchDirect2CBox = nullptr;
    QWidget* flyerLabelContent = nullptr;
    CDE90CheckBox* flyerCheckBox = nullptr;
    QWidget* flyer1InputContent = nullptr;
    CDE90TextFieldBox* flyer1TextField = nullptr;
    QWidget* flyer2InputContent = nullptr;
    CDE90TextFieldBox* flyer2TextField = nullptr;
    UiDetectionCommonSettingController() = default;
    ~UiDetectionCommonSettingController() noexcept {}

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
        title1Label = helper.get<CDE90Label>("title1Label");
        title2Label = helper.get<CDE90Label>("title2Label");
        shapeLabel = helper.get<CDE90Label>("shapeLabel");
        shape1Content = helper.get<QWidget>("shape1Content");
        shape1CBox = helper.get<DatabaseComboBox>("shape1CBox");
        shape2Content = helper.get<QWidget>("shape2Content");
        shape2CBox = helper.get<DatabaseComboBox>("shape2CBox");
        threshold1TextField = helper.get<CDE90TextFieldBox>("threshold1TextField");
        threshold2TextField = helper.get<CDE90TextFieldBox>("threshold2TextField");
        edgeNum1TextField = helper.get<CDE90TextFieldBox>("edgeNum1TextField");
        edgeNum2TextField = helper.get<CDE90TextFieldBox>("edgeNum2TextField");
        bLineStartPoint1TextField = helper.get<CDE90TextFieldBox>("bLineStartPoint1TextField");
        bLineStartPoint2TextField = helper.get<CDE90TextFieldBox>("bLineStartPoint2TextField");
        bLineArea1TextField = helper.get<CDE90TextFieldBox>("bLineArea1TextField");
        bLineArea2TextField = helper.get<CDE90TextFieldBox>("bLineArea2TextField");
        edgeSearchDirect1CBox = helper.get<DatabaseComboBox>("edgeSearchDirect1CBox");
        edgeSearchDirect2CBox = helper.get<DatabaseComboBox>("edgeSearchDirect2CBox");
        flyerLabelContent = helper.get<QWidget>("flyerLabelContent");
        flyerCheckBox = helper.get<CDE90CheckBox>("flyerCheckBox");
        flyer1InputContent = helper.get<QWidget>("flyer1InputContent");
        flyer1TextField = helper.get<CDE90TextFieldBox>("flyer1TextField");
        flyer2InputContent = helper.get<QWidget>("flyer2InputContent");
        flyer2TextField = helper.get<CDE90TextFieldBox>("flyer2TextField");

    }

#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    [[nodiscard]] static CodeEditorController* createCodeEditor(const cdsem::UiDetectionCommonSettingControllerPath& path) {
        return getCodeEditor<cdsem::UiDetectionCommonSettingControllerPath>(path);
    }

#endif

};
#endif
