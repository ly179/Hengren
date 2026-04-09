#ifndef UI_AMP_ELLIPSE_EDIT_CONTROLLER
#define UI_AMP_ELLIPSE_EDIT_CONTROLLER
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
#include "database_combo_box.h"
#include "cde90_check_box.h"
#include "cde90_combo_box.h"
#include "cde90_text_field.h"
#include <QWidget>

/**
 * 本类由编辑器自动生成。
 * getCodeEditor函数部分可以人工修改，用于内部编辑器。
 */
namespace cdsem {
class UiAmpEllipseEditController;
struct UiAmpEllipseEditControllerPath;
}

struct cdsem::UiAmpEllipseEditControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QString huiDir;
    QString sourceCodeDir;
    QString classFileName;
#endif
    UiAmpEllipseEditControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "amp_ellipse_editor";
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
        classFileName = "amp_ellipse_edit_controller";
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
class cdsem::UiAmpEllipseEditController {
private:
    const cdsem::UiAmpEllipseEditControllerPath pathInfo;
public:
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QList<CDE90LayoutScriptLineObject> __allWidgets;
#endif
    CDE90ComboBox* objectCBox = nullptr;
    CDE90ComboBox* dataCBox = nullptr;
    CDE90ComboBox* preProcessCBox = nullptr;
    CDE90ComboBox* acdCBox = nullptr;
    DatabaseComboBox* acdWayCBox = nullptr;
    CDE90TextFieldBox* acdTargetTextField = nullptr;
    CDE90TextFieldBox* acdLimitTextField = nullptr;
    QWidget* cursorPosContent = nullptr;
    CDE90ComboBox* cursorPosCBox = nullptr;
    QWidget* patternSearchContent = nullptr;
    CDE90ComboBox* patternSearchCBox = nullptr;
    CDE90ComboBox* methodCBox = nullptr;
    CDE90TextFieldBox* designValueTextField = nullptr;
    CDE90TextFieldBox* cursorSizeXTextField = nullptr;
    CDE90TextFieldBox* smoothingTextField = nullptr;
    CDE90TextFieldBox* cursorSizeYTextField = nullptr;
    CDE90TextFieldBox* diffTextField = nullptr;
    CDE90TextFieldBox* thresholdTextField = nullptr;
    CDE90TextFieldBox* sumLineOrPointTextField = nullptr;
    QWidget* optionEdgeContent = nullptr;
    CDE90CheckBox* convenProfileCheckBox = nullptr;
    CDE90CheckBox* highSenCheckBox = nullptr;
    CDE90CheckBox* acceptPriCheckBox = nullptr;
    CDE90CheckBox* bottomSufCheckBox = nullptr;
    CDE90CheckBox* waveMatchCheckBox = nullptr;
    CDE90ButtonBox* waveMatchBtn = nullptr;
    CDE90CheckBox* edgeDetectByPRCheckBox = nullptr;
    CDE90TextFieldBox* edgeNumberTextField = nullptr;
    CDE90TextFieldBox* edgeTotalTextField = nullptr;
    CDE90TextFieldBox* baseLineStartPointTextField = nullptr;
    CDE90TextFieldBox* baseLineAreaTextField = nullptr;
    CDE90ComboBox* edgeSearchDirectCBox = nullptr;
    CDE90CheckBox* imgSmoothCheckBox = nullptr;
    CDE90TextFieldBox* imgSmoothTextField = nullptr;
    UiAmpEllipseEditController() = default;
    ~UiAmpEllipseEditController() noexcept {}

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
        objectCBox = helper.get<CDE90ComboBox>("objectCBox");
        dataCBox = helper.get<CDE90ComboBox>("dataCBox");
        preProcessCBox = helper.get<CDE90ComboBox>("preProcessCBox");
        acdCBox = helper.get<CDE90ComboBox>("acdCBox");
        acdWayCBox = helper.get<DatabaseComboBox>("acdWayCBox");
        acdTargetTextField = helper.get<CDE90TextFieldBox>("acdTargetTextField");
        acdLimitTextField = helper.get<CDE90TextFieldBox>("acdLimitTextField");
        cursorPosContent = helper.get<QWidget>("cursorPosContent");
        cursorPosCBox = helper.get<CDE90ComboBox>("cursorPosCBox");
        patternSearchContent = helper.get<QWidget>("patternSearchContent");
        patternSearchCBox = helper.get<CDE90ComboBox>("patternSearchCBox");
        methodCBox = helper.get<CDE90ComboBox>("methodCBox");
        designValueTextField = helper.get<CDE90TextFieldBox>("designValueTextField");
        cursorSizeXTextField = helper.get<CDE90TextFieldBox>("cursorSizeXTextField");
        smoothingTextField = helper.get<CDE90TextFieldBox>("smoothingTextField");
        cursorSizeYTextField = helper.get<CDE90TextFieldBox>("cursorSizeYTextField");
        diffTextField = helper.get<CDE90TextFieldBox>("diffTextField");
        thresholdTextField = helper.get<CDE90TextFieldBox>("thresholdTextField");
        sumLineOrPointTextField = helper.get<CDE90TextFieldBox>("sumLineOrPointTextField");
        optionEdgeContent = helper.get<QWidget>("optionEdgeContent");
        convenProfileCheckBox = helper.get<CDE90CheckBox>("convenProfileCheckBox");
        highSenCheckBox = helper.get<CDE90CheckBox>("highSenCheckBox");
        acceptPriCheckBox = helper.get<CDE90CheckBox>("acceptPriCheckBox");
        bottomSufCheckBox = helper.get<CDE90CheckBox>("bottomSufCheckBox");
        waveMatchCheckBox = helper.get<CDE90CheckBox>("waveMatchCheckBox");
        waveMatchBtn = helper.get<CDE90ButtonBox>("waveMatchBtn");
        edgeDetectByPRCheckBox = helper.get<CDE90CheckBox>("edgeDetectByPRCheckBox");
        edgeNumberTextField = helper.get<CDE90TextFieldBox>("edgeNumberTextField");
        edgeTotalTextField = helper.get<CDE90TextFieldBox>("edgeTotalTextField");
        baseLineStartPointTextField = helper.get<CDE90TextFieldBox>("baseLineStartPointTextField");
        baseLineAreaTextField = helper.get<CDE90TextFieldBox>("baseLineAreaTextField");
        edgeSearchDirectCBox = helper.get<CDE90ComboBox>("edgeSearchDirectCBox");
        imgSmoothCheckBox = helper.get<CDE90CheckBox>("imgSmoothCheckBox");
        imgSmoothTextField = helper.get<CDE90TextFieldBox>("imgSmoothTextField");

    }

#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    [[nodiscard]] static CodeEditorController* createCodeEditor(const cdsem::UiAmpEllipseEditControllerPath& path) {
        return getCodeEditor<cdsem::UiAmpEllipseEditControllerPath>(path);
    }

#endif

};
#endif
