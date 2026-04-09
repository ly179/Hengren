#ifndef UI_AXIS_ALIGN_DETAIL_CONTENT
#define UI_AXIS_ALIGN_DETAIL_CONTENT
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
#include "sem_thumbnail_widget.h"
#include "cde90_label.h"
#include "cde90_text_field.h"
#include <QAction>
#include "cde90_check_box.h"
#include "cde90_button.h"
#include "database_combo_box.h"

/**
 * 本类由编辑器自动生成。
 * getCodeEditor函数部分可以人工修改，用于内部编辑器。
 */
namespace cdsem {
class UiAxisAlignDetailContent;
struct UiAxisAlignDetailContentPath;
}

struct cdsem::UiAxisAlignDetailContentPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QString huiDir;
    QString sourceCodeDir;
    QString classFileName;
#endif
    UiAxisAlignDetailContentPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "axis_align_detail_editor";
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
        classFileName = "axis_align_detail_content";
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
class cdsem::UiAxisAlignDetailContent {
private:
    const cdsem::UiAxisAlignDetailContentPath pathInfo;
public:
#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    QList<CDE90LayoutScriptLineObject> __allWidgets;
#endif
    CDE90Label* titleLabel = nullptr;
    CDE90CheckBox* moveCheckBox = nullptr;
    CDE90ButtonBox* moveBtn = nullptr;
    CDE90ButtonBox* regBtn = nullptr;
    CDE90TextFieldBox* coordinateXTextField = nullptr;
    CDE90TextFieldBox* coordinateXDeviationTextField = nullptr;
    CDE90TextFieldBox* coordinateYTextField = nullptr;
    CDE90TextFieldBox* coordinateYDeviationTextField = nullptr;
    CDE90CheckBox* zSensorCheckBox = nullptr;
    CDE90ButtonBox* zSensorMoreBtn = nullptr;
    CDE90CheckBox* autoFocusCheckBox = nullptr;
    CDE90ButtonBox* autoFocusMoreBtn = nullptr;
    CDE90CheckBox* patternCheckBox = nullptr;
    CDE90ButtonBox* patternMoreBtn = nullptr;
    SemThumbnailWidget* modelCanvas = nullptr;
    CDE90ButtonBox* actionBtn = nullptr;
    QAction* regImageAct = nullptr;
    QAction* regInfoAct = nullptr;
    QAction* regClearAct = nullptr;
    CDE90CheckBox* axisAlignCheckBox = nullptr;
    CDE90TextFieldBox* waitSecond = nullptr;
    CDE90TextFieldBox* magTextField = nullptr;
    CDE90ButtonBox* readBtn = nullptr;
    CDE90Label* axisItemLabel = nullptr;
    DatabaseComboBox* axisItemBox = nullptr;
    UiAxisAlignDetailContent() = default;
    ~UiAxisAlignDetailContent() noexcept {}

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
        titleLabel = helper.get<CDE90Label>("titleLabel");
        moveCheckBox = helper.get<CDE90CheckBox>("moveCheckBox");
        moveBtn = helper.get<CDE90ButtonBox>("moveBtn");
        regBtn = helper.get<CDE90ButtonBox>("regBtn");
        coordinateXTextField = helper.get<CDE90TextFieldBox>("coordinateXTextField");
        coordinateXDeviationTextField = helper.get<CDE90TextFieldBox>("coordinateXDeviationTextField");
        coordinateYTextField = helper.get<CDE90TextFieldBox>("coordinateYTextField");
        coordinateYDeviationTextField = helper.get<CDE90TextFieldBox>("coordinateYDeviationTextField");
        zSensorCheckBox = helper.get<CDE90CheckBox>("zSensorCheckBox");
        zSensorMoreBtn = helper.get<CDE90ButtonBox>("zSensorMoreBtn");
        autoFocusCheckBox = helper.get<CDE90CheckBox>("autoFocusCheckBox");
        autoFocusMoreBtn = helper.get<CDE90ButtonBox>("autoFocusMoreBtn");
        patternCheckBox = helper.get<CDE90CheckBox>("patternCheckBox");
        patternMoreBtn = helper.get<CDE90ButtonBox>("patternMoreBtn");
        modelCanvas = helper.get<SemThumbnailWidget>("modelCanvas");
        actionBtn = helper.get<CDE90ButtonBox>("actionBtn");
        regImageAct = helper.get<QAction>("regImageAct");
        regInfoAct = helper.get<QAction>("regInfoAct");
        regClearAct = helper.get<QAction>("regClearAct");
        axisAlignCheckBox = helper.get<CDE90CheckBox>("axisAlignCheckBox");
        waitSecond = helper.get<CDE90TextFieldBox>("waitSecond");
        magTextField = helper.get<CDE90TextFieldBox>("magTextField");
        readBtn = helper.get<CDE90ButtonBox>("readBtn");
        axisItemLabel = helper.get<CDE90Label>("axisItemLabel");
        axisItemBox = helper.get<DatabaseComboBox>("axisItemBox");

    }

#if defined(QT_DEBUG) && defined(_USE_CODE_EDITOR)
    [[nodiscard]] static CodeEditorController* createCodeEditor(const cdsem::UiAxisAlignDetailContentPath& path) {
        return getCodeEditor<cdsem::UiAxisAlignDetailContentPath>(path);
    }

#endif

};
#endif
