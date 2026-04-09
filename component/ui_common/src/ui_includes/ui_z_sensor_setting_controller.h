#ifndef UI_Z_SENSOR_SETTING_CONTROLLER
#define UI_Z_SENSOR_SETTING_CONTROLLER
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

/**
 * 本类由编辑器自动生成。
 * getCodeEditor函数部分可以人工修改，用于内部编辑器。
 */
namespace cdsem {
class UiZSensorSettingController;
struct UiZSensorSettingControllerPath;
}

struct cdsem::UiZSensorSettingControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QString huiDir;
    QString sourceCodeDir;
    QString classFileName;
#endif
    UiZSensorSettingControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "z_sensor_setting";
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
        classFileName = "z_sensor_setting_controller";
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
class cdsem::UiZSensorSettingController {
private:
    const cdsem::UiZSensorSettingControllerPath pathInfo;
public:
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QList<CDE90LayoutScriptLineObject> __allWidgets;
#endif
    CDE90CheckBox* enableCbox = nullptr;
    QWidget* paramContent = nullptr;
    CDE90TextFieldBox* offsetTextField = nullptr;
    CDE90ButtonBox* adjustBtn = nullptr;
    CDE90ButtonBox* okBtn = nullptr;
    CDE90ButtonBox* backBtn = nullptr;
    CDE90ButtonBox* nextBtn = nullptr;
    CDE90ButtonBox* cancelBtn = nullptr;
    UiZSensorSettingController() = default;
    ~UiZSensorSettingController() noexcept {}

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
        enableCbox = helper.get<CDE90CheckBox>("enableCbox");
        paramContent = helper.get<QWidget>("paramContent");
        offsetTextField = helper.get<CDE90TextFieldBox>("offsetTextField");
        adjustBtn = helper.get<CDE90ButtonBox>("adjustBtn");
        okBtn = helper.get<CDE90ButtonBox>("okBtn");
        backBtn = helper.get<CDE90ButtonBox>("backBtn");
        nextBtn = helper.get<CDE90ButtonBox>("nextBtn");
        cancelBtn = helper.get<CDE90ButtonBox>("cancelBtn");

    }

#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    [[nodiscard]] static CodeEditorController* createCodeEditor(const cdsem::UiZSensorSettingControllerPath& path) {
        return getCodeEditor<cdsem::UiZSensorSettingControllerPath>(path);
    }

#endif

};
#endif
