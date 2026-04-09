#ifndef UI_IDP_MP_COORDINATE_SETTING_CONTROLLER
#define UI_IDP_MP_COORDINATE_SETTING_CONTROLLER
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

/**
 * 本类由编辑器自动生成。
 * getCodeEditor函数部分可以人工修改，用于内部编辑器。
 */
namespace cdsem {
class UiIdpMpCoordinateSettingController;
struct UiIdpMpCoordinateSettingControllerPath;
}

struct cdsem::UiIdpMpCoordinateSettingControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QString huiDir;
    QString sourceCodeDir;
    QString classFileName;
#endif
    UiIdpMpCoordinateSettingControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "idp_mp_coordinate_setting";
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
        classFileName = "idp_mp_coordinate_setting_controller";
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
class cdsem::UiIdpMpCoordinateSettingController {
private:
    const cdsem::UiIdpMpCoordinateSettingControllerPath pathInfo;
public:
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QList<CDE90LayoutScriptLineObject> __allWidgets;
#endif
    CDE90TextFieldBox* chipNoXTextField = nullptr;
    CDE90TextFieldBox* chipNoYTextField = nullptr;
    CDE90ComboBox* posItemBox = nullptr;
    CDE90TextFieldBox* dxTextField = nullptr;
    CDE90ButtonBox* moveBtn = nullptr;
    CDE90TextFieldBox* dyTextField = nullptr;
    CDE90ButtonBox* posRegBtn = nullptr;
    CDE90ButtonBox* chipRegBtn = nullptr;
    CDE90CheckBox* diffCBox = nullptr;
    CDE90ButtonBox* okBtn = nullptr;
    CDE90ButtonBox* nextBtn = nullptr;
    CDE90ButtonBox* cancelBtn = nullptr;
    UiIdpMpCoordinateSettingController() = default;
    ~UiIdpMpCoordinateSettingController() noexcept {}

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
        chipNoXTextField = helper.get<CDE90TextFieldBox>("chipNoXTextField");
        chipNoYTextField = helper.get<CDE90TextFieldBox>("chipNoYTextField");
        posItemBox = helper.get<CDE90ComboBox>("posItemBox");
        dxTextField = helper.get<CDE90TextFieldBox>("dxTextField");
        moveBtn = helper.get<CDE90ButtonBox>("moveBtn");
        dyTextField = helper.get<CDE90TextFieldBox>("dyTextField");
        posRegBtn = helper.get<CDE90ButtonBox>("posRegBtn");
        chipRegBtn = helper.get<CDE90ButtonBox>("chipRegBtn");
        diffCBox = helper.get<CDE90CheckBox>("diffCBox");
        okBtn = helper.get<CDE90ButtonBox>("okBtn");
        nextBtn = helper.get<CDE90ButtonBox>("nextBtn");
        cancelBtn = helper.get<CDE90ButtonBox>("cancelBtn");

    }

#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    [[nodiscard]] static CodeEditorController* createCodeEditor(const cdsem::UiIdpMpCoordinateSettingControllerPath& path) {
        return getCodeEditor<cdsem::UiIdpMpCoordinateSettingControllerPath>(path);
    }

#endif

};
#endif
