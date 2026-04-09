#ifndef UI_IMG_ENV_SETTING_CONTENT
#define UI_IMG_ENV_SETTING_CONTENT
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
#include "cde90_label.h"
#include "cde90_check_box.h"
#include "cde90_combo_box.h"
#include "cde90_text_field.h"

/**
 * 本类由编辑器自动生成。
 * getCodeEditor函数部分可以人工修改，用于内部编辑器。
 */
namespace cdsem {
class UiImgEnvSettingContent;
struct UiImgEnvSettingContentPath;
}

struct cdsem::UiImgEnvSettingContentPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QString huiDir;
    QString sourceCodeDir;
    QString classFileName;
#endif
    UiImgEnvSettingContentPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "img_env_setting";
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
        classFileName = "img_env_setting_content";
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
class cdsem::UiImgEnvSettingContent {
private:
    const cdsem::UiImgEnvSettingContentPath pathInfo;
public:
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QList<CDE90LayoutScriptLineObject> __allWidgets;
#endif
    CDE90ComboBox* saveSizeItemBox = nullptr;
    CDE90ComboBox* saveCompressionItemBox = nullptr;
    CDE90Label* qualityLabel = nullptr;
    CDE90TextFieldBox* qualityTextField = nullptr;
    CDE90CheckBox* overlayMeasurementCursorCBox = nullptr;
    CDE90CheckBox* overlayCrossCursorCBox = nullptr;
    CDE90CheckBox* overlayAreaCursorCBox = nullptr;
    CDE90CheckBox* overlayDDSCBox = nullptr;
    CDE90CheckBox* overlayMeasurementValueCBox = nullptr;
    CDE90CheckBox* overlayLineProfileCBox = nullptr;
    CDE90CheckBox* overlayUmMarkerCBox = nullptr;
    UiImgEnvSettingContent() = default;
    ~UiImgEnvSettingContent() noexcept {}

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
        saveSizeItemBox = helper.get<CDE90ComboBox>("saveSizeItemBox");
        saveCompressionItemBox = helper.get<CDE90ComboBox>("saveCompressionItemBox");
        qualityLabel = helper.get<CDE90Label>("qualityLabel");
        qualityTextField = helper.get<CDE90TextFieldBox>("qualityTextField");
        overlayMeasurementCursorCBox = helper.get<CDE90CheckBox>("overlayMeasurementCursorCBox");
        overlayCrossCursorCBox = helper.get<CDE90CheckBox>("overlayCrossCursorCBox");
        overlayAreaCursorCBox = helper.get<CDE90CheckBox>("overlayAreaCursorCBox");
        overlayDDSCBox = helper.get<CDE90CheckBox>("overlayDDSCBox");
        overlayMeasurementValueCBox = helper.get<CDE90CheckBox>("overlayMeasurementValueCBox");
        overlayLineProfileCBox = helper.get<CDE90CheckBox>("overlayLineProfileCBox");
        overlayUmMarkerCBox = helper.get<CDE90CheckBox>("overlayUmMarkerCBox");

    }

#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    [[nodiscard]] static CodeEditorController* createCodeEditor(const cdsem::UiImgEnvSettingContentPath& path) {
        return getCodeEditor<cdsem::UiImgEnvSettingContentPath>(path);
    }

#endif

};
#endif
