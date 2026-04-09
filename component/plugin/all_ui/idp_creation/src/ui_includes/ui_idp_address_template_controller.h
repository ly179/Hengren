#ifndef UI_IDP_ADDRESS_TEMPLATE_CONTROLLER
#define UI_IDP_ADDRESS_TEMPLATE_CONTROLLER
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
#include <QAction>
#include "sem_thumbnail_widget.h"
#include "cde90_check_box.h"
#include "cde90_text_field.h"

/**
 * 本类由编辑器自动生成。
 * getCodeEditor函数部分可以人工修改，用于内部编辑器。
 */
namespace cdsem {
class UiIdpAddressTemplateController;
struct UiIdpAddressTemplateControllerPath;
}

struct cdsem::UiIdpAddressTemplateControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QString huiDir;
    QString sourceCodeDir;
    QString classFileName;
#endif
    UiIdpAddressTemplateControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = "D:/Projects/CD-SEM/ui_module/ui_template_creation/h_ui";
#endif
        huiFileName = "idp_address_template";
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
        classFileName = "idp_address_template_controller";
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
class cdsem::UiIdpAddressTemplateController {
private:
    const cdsem::UiIdpAddressTemplateControllerPath pathInfo;
public:
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QList<CDE90LayoutScriptLineObject> __allWidgets;
#endif
    CDE90ButtonBox* addressTemplateBtn = nullptr;
    QAction* addressTempDLoadAct = nullptr;
    QAction* addressTempSLoadAct = nullptr;
    QAction* addressTempSaveAct = nullptr;
    QAction* addressTempImportAct = nullptr;
    CDE90CheckBox* address1ZSensorCheckBox = nullptr;
    CDE90CheckBox* address1PreDoesCheckBox = nullptr;
    CDE90CheckBox* address1AutoFocusCheckBox = nullptr;
    CDE90CheckBox* address1PatternRegCheckBox = nullptr;
    CDE90ButtonBox* address1ZSensorBtn = nullptr;
    CDE90ButtonBox* address1PreDoesBtn = nullptr;
    CDE90ButtonBox* address1AutoFocusBtn = nullptr;
    CDE90ButtonBox* address1PatternRegBtn = nullptr;
    SemThumbnailWidget* address1Canvas = nullptr;
    CDE90ButtonBox* address1RegImageBtn = nullptr;
    CDE90ButtonBox* address1PRTempGenBtn = nullptr;
    CDE90ButtonBox* address1ImageInfoBtn = nullptr;
    CDE90ButtonBox* address1ImageClearBtn = nullptr;
    CDE90CheckBox* address2AutoFocusCheckBox = nullptr;
    CDE90CheckBox* address2PatternRegCheckBox = nullptr;
    CDE90ButtonBox* address2AutoFocusBtn = nullptr;
    CDE90ButtonBox* address2PatternRegBtn = nullptr;
    SemThumbnailWidget* address2Canvas = nullptr;
    CDE90ButtonBox* address2RegImageBtn = nullptr;
    CDE90ButtonBox* address2PRTempGenBtn = nullptr;
    CDE90ButtonBox* address2ImageInfoBtn = nullptr;
    CDE90ButtonBox* address2ImageClearBtn = nullptr;
    CDE90CheckBox* addressRotateTemplateCheckBox = nullptr;
    CDE90TextFieldBox* addressRotateTemplateTextField = nullptr;
    UiIdpAddressTemplateController() = default;
    ~UiIdpAddressTemplateController() noexcept {}

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
        addressTemplateBtn = helper.get<CDE90ButtonBox>("addressTemplateBtn");
        addressTempDLoadAct = helper.get<QAction>("addressTempDLoadAct");
        addressTempSLoadAct = helper.get<QAction>("addressTempSLoadAct");
        addressTempSaveAct = helper.get<QAction>("addressTempSaveAct");
        addressTempImportAct = helper.get<QAction>("addressTempImportAct");
        address1ZSensorCheckBox = helper.get<CDE90CheckBox>("address1ZSensorCheckBox");
        address1PreDoesCheckBox = helper.get<CDE90CheckBox>("address1PreDoesCheckBox");
        address1AutoFocusCheckBox = helper.get<CDE90CheckBox>("address1AutoFocusCheckBox");
        address1PatternRegCheckBox = helper.get<CDE90CheckBox>("address1PatternRegCheckBox");
        address1ZSensorBtn = helper.get<CDE90ButtonBox>("address1ZSensorBtn");
        address1PreDoesBtn = helper.get<CDE90ButtonBox>("address1PreDoesBtn");
        address1AutoFocusBtn = helper.get<CDE90ButtonBox>("address1AutoFocusBtn");
        address1PatternRegBtn = helper.get<CDE90ButtonBox>("address1PatternRegBtn");
        address1Canvas = helper.get<SemThumbnailWidget>("address1Canvas");
        address1RegImageBtn = helper.get<CDE90ButtonBox>("address1RegImageBtn");
        address1PRTempGenBtn = helper.get<CDE90ButtonBox>("address1PRTempGenBtn");
        address1ImageInfoBtn = helper.get<CDE90ButtonBox>("address1ImageInfoBtn");
        address1ImageClearBtn = helper.get<CDE90ButtonBox>("address1ImageClearBtn");
        address2AutoFocusCheckBox = helper.get<CDE90CheckBox>("address2AutoFocusCheckBox");
        address2PatternRegCheckBox = helper.get<CDE90CheckBox>("address2PatternRegCheckBox");
        address2AutoFocusBtn = helper.get<CDE90ButtonBox>("address2AutoFocusBtn");
        address2PatternRegBtn = helper.get<CDE90ButtonBox>("address2PatternRegBtn");
        address2Canvas = helper.get<SemThumbnailWidget>("address2Canvas");
        address2RegImageBtn = helper.get<CDE90ButtonBox>("address2RegImageBtn");
        address2PRTempGenBtn = helper.get<CDE90ButtonBox>("address2PRTempGenBtn");
        address2ImageInfoBtn = helper.get<CDE90ButtonBox>("address2ImageInfoBtn");
        address2ImageClearBtn = helper.get<CDE90ButtonBox>("address2ImageClearBtn");
        addressRotateTemplateCheckBox = helper.get<CDE90CheckBox>("addressRotateTemplateCheckBox");
        addressRotateTemplateTextField = helper.get<CDE90TextFieldBox>("addressRotateTemplateTextField");

    }

#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    [[nodiscard]] static CodeEditorController* createCodeEditor(const cdsem::UiIdpAddressTemplateControllerPath& path) {
        return getCodeEditor<cdsem::UiIdpAddressTemplateControllerPath>(path);
    }

#endif

};
#endif
