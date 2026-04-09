#ifndef UI_IDP_MEASUREMENT_ARRANGE_SETTING_CONTROLLER
#define UI_IDP_MEASUREMENT_ARRANGE_SETTING_CONTROLLER
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
#include "cde90_combo_box.h"
#include <QWidget>
#include "cde90_text_field.h"
#include "cde90_label.h"

/**
 * 本类由编辑器自动生成。
 * getCodeEditor函数部分可以人工修改，用于内部编辑器。
 */
namespace cdsem {
class UiIdpMeasurementArrangeSettingController;
struct UiIdpMeasurementArrangeSettingControllerPath;
}

struct cdsem::UiIdpMeasurementArrangeSettingControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QString huiDir;
    QString sourceCodeDir;
    QString classFileName;
#endif
    UiIdpMeasurementArrangeSettingControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "idp_measurement_arrange_setting";
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
        classFileName = "idp_measurement_arrange_setting_controller";
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
class cdsem::UiIdpMeasurementArrangeSettingController {
private:
    const cdsem::UiIdpMeasurementArrangeSettingControllerPath pathInfo;
public:
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QList<CDE90LayoutScriptLineObject> __allWidgets;
#endif
    CDE90ComboBox* typeItemBox = nullptr;
    QWidget* matrixContent = nullptr;
    CDE90Label* upLabel = nullptr;
    CDE90TextFieldBox* upTextField = nullptr;
    CDE90Label* leftLabel = nullptr;
    CDE90TextFieldBox* leftTextField = nullptr;
    CDE90Label* centerLabel = nullptr;
    CDE90TextFieldBox* rightTextField = nullptr;
    CDE90Label* rightLabel = nullptr;
    CDE90TextFieldBox* downTextField = nullptr;
    CDE90Label* downLabel = nullptr;
    QWidget* diagonalContent = nullptr;
    CDE90Label* leftUpLabel = nullptr;
    CDE90Label* rightUpLabel = nullptr;
    CDE90TextFieldBox* leftUpTextField = nullptr;
    CDE90TextFieldBox* rightUpTextField = nullptr;
    CDE90Label* xCenterLabel = nullptr;
    CDE90TextFieldBox* leftDownTextField = nullptr;
    CDE90TextFieldBox* rightDownTextField = nullptr;
    CDE90Label* leftDownLabel = nullptr;
    CDE90Label* rightDownLabel = nullptr;
    CDE90TextFieldBox* pitchXTextField = nullptr;
    CDE90TextFieldBox* pitchYTextField = nullptr;
    QWidget* matrixInfoContent = nullptr;
    CDE90TextFieldBox* mxTextField = nullptr;
    CDE90TextFieldBox* myTextField = nullptr;
    CDE90ButtonBox* applyBtn = nullptr;
    QWidget* orderContent = nullptr;
    CDE90ButtonBox* okBtn = nullptr;
    CDE90ButtonBox* cancelBtn = nullptr;
    UiIdpMeasurementArrangeSettingController() = default;
    ~UiIdpMeasurementArrangeSettingController() noexcept {}

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
        typeItemBox = helper.get<CDE90ComboBox>("typeItemBox");
        matrixContent = helper.get<QWidget>("matrixContent");
        upLabel = helper.get<CDE90Label>("upLabel");
        upTextField = helper.get<CDE90TextFieldBox>("upTextField");
        leftLabel = helper.get<CDE90Label>("leftLabel");
        leftTextField = helper.get<CDE90TextFieldBox>("leftTextField");
        centerLabel = helper.get<CDE90Label>("centerLabel");
        rightTextField = helper.get<CDE90TextFieldBox>("rightTextField");
        rightLabel = helper.get<CDE90Label>("rightLabel");
        downTextField = helper.get<CDE90TextFieldBox>("downTextField");
        downLabel = helper.get<CDE90Label>("downLabel");
        diagonalContent = helper.get<QWidget>("diagonalContent");
        leftUpLabel = helper.get<CDE90Label>("leftUpLabel");
        rightUpLabel = helper.get<CDE90Label>("rightUpLabel");
        leftUpTextField = helper.get<CDE90TextFieldBox>("leftUpTextField");
        rightUpTextField = helper.get<CDE90TextFieldBox>("rightUpTextField");
        xCenterLabel = helper.get<CDE90Label>("xCenterLabel");
        leftDownTextField = helper.get<CDE90TextFieldBox>("leftDownTextField");
        rightDownTextField = helper.get<CDE90TextFieldBox>("rightDownTextField");
        leftDownLabel = helper.get<CDE90Label>("leftDownLabel");
        rightDownLabel = helper.get<CDE90Label>("rightDownLabel");
        pitchXTextField = helper.get<CDE90TextFieldBox>("pitchXTextField");
        pitchYTextField = helper.get<CDE90TextFieldBox>("pitchYTextField");
        matrixInfoContent = helper.get<QWidget>("matrixInfoContent");
        mxTextField = helper.get<CDE90TextFieldBox>("mxTextField");
        myTextField = helper.get<CDE90TextFieldBox>("myTextField");
        applyBtn = helper.get<CDE90ButtonBox>("applyBtn");
        orderContent = helper.get<QWidget>("orderContent");
        okBtn = helper.get<CDE90ButtonBox>("okBtn");
        cancelBtn = helper.get<CDE90ButtonBox>("cancelBtn");

    }

#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    [[nodiscard]] static CodeEditorController* createCodeEditor(const cdsem::UiIdpMeasurementArrangeSettingControllerPath& path) {
        return getCodeEditor<cdsem::UiIdpMeasurementArrangeSettingControllerPath>(path);
    }

#endif

};
#endif
