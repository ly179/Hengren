#ifndef UI_IDP_CHIP_SELECTOR_CONTROLLER
#define UI_IDP_CHIP_SELECTOR_CONTROLLER
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
#include "cde90_table_view.h"
#include "idw_canvas.h"

/**
 * 本类由编辑器自动生成。
 * getCodeEditor函数部分可以人工修改，用于内部编辑器。
 */
namespace cdsem {
class UiIdpChipSelectorController;
struct UiIdpChipSelectorControllerPath;
}

struct cdsem::UiIdpChipSelectorControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QString huiDir;
    QString sourceCodeDir;
    QString classFileName;
#endif
    UiIdpChipSelectorControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "idp_chip_selector";
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
        classFileName = "idp_chip_selector_controller";
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
class cdsem::UiIdpChipSelectorController {
private:
    const cdsem::UiIdpChipSelectorControllerPath pathInfo;
public:
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QList<CDE90LayoutScriptLineObject> __allWidgets;
#endif
    CDE90TableView* seqTable = nullptr;
    CDE90ButtonBox* moveBackBtn = nullptr;
    CDE90ButtonBox* moveForwardBtn = nullptr;
    CDE90TableView* pNoTable = nullptr;
    CDE90ButtonBox* addBtn = nullptr;
    CDE90ButtonBox* deleteBtn = nullptr;
    CDE90TableView* templateTable = nullptr;
    IdwCanvasContent* idwCanvasContent = nullptr;
    CDE90TextFieldBox* countTextField = nullptr;
    CDE90ComboBox* magBox = nullptr;
    CDE90CheckBox* chipNoCbox = nullptr;
    CDE90ButtonBox* okBtn = nullptr;
    CDE90ButtonBox* cancelBtn = nullptr;
    UiIdpChipSelectorController() = default;
    ~UiIdpChipSelectorController() noexcept {}

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
        seqTable = helper.get<CDE90TableView>("seqTable");
        moveBackBtn = helper.get<CDE90ButtonBox>("moveBackBtn");
        moveForwardBtn = helper.get<CDE90ButtonBox>("moveForwardBtn");
        pNoTable = helper.get<CDE90TableView>("pNoTable");
        addBtn = helper.get<CDE90ButtonBox>("addBtn");
        deleteBtn = helper.get<CDE90ButtonBox>("deleteBtn");
        templateTable = helper.get<CDE90TableView>("templateTable");
        idwCanvasContent = helper.get<IdwCanvasContent>("idwCanvasContent");
        countTextField = helper.get<CDE90TextFieldBox>("countTextField");
        magBox = helper.get<CDE90ComboBox>("magBox");
        chipNoCbox = helper.get<CDE90CheckBox>("chipNoCbox");
        okBtn = helper.get<CDE90ButtonBox>("okBtn");
        cancelBtn = helper.get<CDE90ButtonBox>("cancelBtn");

    }

#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    [[nodiscard]] static CodeEditorController* createCodeEditor(const cdsem::UiIdpChipSelectorControllerPath& path) {
        return getCodeEditor<cdsem::UiIdpChipSelectorControllerPath>(path);
    }

#endif

};
#endif
