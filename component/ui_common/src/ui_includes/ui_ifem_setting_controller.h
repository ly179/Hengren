#ifndef UI_IFEM_SETTING_CONTROLLER
#define UI_IFEM_SETTING_CONTROLLER
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
#include "cde90_combo_box.h"
#include "cde90_label.h"
#include "sem_thumbnail_widget.h"
#include <QWidget>
#include "cde90_radio_button.h"
#include "cde90_button.h"

/**
 * 本类由编辑器自动生成。
 * getCodeEditor函数部分可以人工修改，用于内部编辑器。
 */
namespace cdsem {
class UiIfemSettingController;
struct UiIfemSettingControllerPath;
}

struct cdsem::UiIfemSettingControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QString huiDir;
    QString sourceCodeDir;
    QString classFileName;
#endif
    UiIfemSettingControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "ifem_setting";
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
        classFileName = "ifem_setting_controller";
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
class cdsem::UiIfemSettingController {
private:
    const cdsem::UiIfemSettingControllerPath pathInfo;
public:
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QList<CDE90LayoutScriptLineObject> __allWidgets;
#endif
    CDE90ComboBox* kindCBox = nullptr;
    CDE90Label* diameterLabel = nullptr;
    CDE90TextFieldBox* diameterTextField = nullptr;
    CDE90Label* diameterUnitLabel = nullptr;
    CDE90Label* diameterRangeLabel = nullptr;
    CDE90Label* widthLabel = nullptr;
    CDE90TextFieldBox* widthTextField = nullptr;
    CDE90Label* widthUnitLabel = nullptr;
    CDE90Label* widthRangeLabel = nullptr;
    CDE90TextFieldBox* pitchXTextField = nullptr;
    CDE90Label* pitchYLabel = nullptr;
    CDE90TextFieldBox* pitchYTextField = nullptr;
    CDE90Label* pitchYUnitLabel = nullptr;
    CDE90Label* pitchYRangeLabel = nullptr;
    CDE90Label* lenYLabel = nullptr;
    CDE90TextFieldBox* lengthYTextField = nullptr;
    CDE90Label* lenYUnitLabel = nullptr;
    CDE90Label* lenYRangeLabel = nullptr;
    CDE90TextFieldBox* numberXTextField = nullptr;
    CDE90Label* numberXRangeLabel = nullptr;
    CDE90Label* numberYLabel = nullptr;
    CDE90TextFieldBox* numberYTextField = nullptr;
    CDE90Label* numberYRangeLabel = nullptr;
    CDE90RadioButton* measureCursorPosRadio = nullptr;
    CDE90RadioButton* selectMeasurePosRadio = nullptr;
    CDE90TextFieldBox* targetXTextField = nullptr;
    QWidget* targetYLabelContent = nullptr;
    CDE90TextFieldBox* targetYTextField = nullptr;
    CDE90Label* targetYRangeLabel = nullptr;
    SemThumbnailWidget* exampleCanvas = nullptr;
    CDE90ButtonBox* okBtn = nullptr;
    CDE90ButtonBox* cancelBtn = nullptr;
    UiIfemSettingController() = default;
    ~UiIfemSettingController() noexcept {}

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
        diameterLabel = helper.get<CDE90Label>("diameterLabel");
        diameterTextField = helper.get<CDE90TextFieldBox>("diameterTextField");
        diameterUnitLabel = helper.get<CDE90Label>("diameterUnitLabel");
        diameterRangeLabel = helper.get<CDE90Label>("diameterRangeLabel");
        widthLabel = helper.get<CDE90Label>("widthLabel");
        widthTextField = helper.get<CDE90TextFieldBox>("widthTextField");
        widthUnitLabel = helper.get<CDE90Label>("widthUnitLabel");
        widthRangeLabel = helper.get<CDE90Label>("widthRangeLabel");
        pitchXTextField = helper.get<CDE90TextFieldBox>("pitchXTextField");
        pitchYLabel = helper.get<CDE90Label>("pitchYLabel");
        pitchYTextField = helper.get<CDE90TextFieldBox>("pitchYTextField");
        pitchYUnitLabel = helper.get<CDE90Label>("pitchYUnitLabel");
        pitchYRangeLabel = helper.get<CDE90Label>("pitchYRangeLabel");
        lenYLabel = helper.get<CDE90Label>("lenYLabel");
        lengthYTextField = helper.get<CDE90TextFieldBox>("lengthYTextField");
        lenYUnitLabel = helper.get<CDE90Label>("lenYUnitLabel");
        lenYRangeLabel = helper.get<CDE90Label>("lenYRangeLabel");
        numberXTextField = helper.get<CDE90TextFieldBox>("numberXTextField");
        numberXRangeLabel = helper.get<CDE90Label>("numberXRangeLabel");
        numberYLabel = helper.get<CDE90Label>("numberYLabel");
        numberYTextField = helper.get<CDE90TextFieldBox>("numberYTextField");
        numberYRangeLabel = helper.get<CDE90Label>("numberYRangeLabel");
        measureCursorPosRadio = helper.get<CDE90RadioButton>("measureCursorPosRadio");
        selectMeasurePosRadio = helper.get<CDE90RadioButton>("selectMeasurePosRadio");
        targetXTextField = helper.get<CDE90TextFieldBox>("targetXTextField");
        targetYLabelContent = helper.get<QWidget>("targetYLabelContent");
        targetYTextField = helper.get<CDE90TextFieldBox>("targetYTextField");
        targetYRangeLabel = helper.get<CDE90Label>("targetYRangeLabel");
        exampleCanvas = helper.get<SemThumbnailWidget>("exampleCanvas");
        okBtn = helper.get<CDE90ButtonBox>("okBtn");
        cancelBtn = helper.get<CDE90ButtonBox>("cancelBtn");

    }

#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    [[nodiscard]] static CodeEditorController* createCodeEditor(const cdsem::UiIfemSettingControllerPath& path) {
        return getCodeEditor<cdsem::UiIfemSettingControllerPath>(path);
    }

#endif

};
#endif
