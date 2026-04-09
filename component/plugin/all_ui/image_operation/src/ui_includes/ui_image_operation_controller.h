#ifndef UI_IMAGE_OPERATION_CONTROLLER
#define UI_IMAGE_OPERATION_CONTROLLER
#include <QtGlobal>
#include "cde90_layout_helper.h"
#include "cde90_label.h"
#include "cde90_widget.h"
#include "cde90_text_field.h"
#include "cde90_combo_box.h"
#include "cde90_button.h"

/**
 * 本类由编辑器自动生成。
 */
namespace cdsem {
class UiImageOperationController;
struct UiImageOperationControllerPath;
}

struct cdsem::UiImageOperationControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
    UiImageOperationControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "image_operation_edit.hui";
        path = proHuiDir + "/" + huiFileName;
    }
};
class cdsem::UiImageOperationController {
private:
    const cdsem::UiImageOperationControllerPath pathInfo;
public:
    CDE90Widget* imageTypeItemContent = nullptr;
    CDE90ComboBox* imageTypeItemBox = nullptr;
    CDE90ButtonBox* opticsBtn = nullptr;
    CDE90ButtonBox* omAbcBtn = nullptr;
    CDE90ButtonBox* rotBtn = nullptr;
    CDE90TextFieldBox* rotTextField = nullptr;
    CDE90ButtonBox* pmBtn = nullptr;
    CDE90TextFieldBox* magTextField = nullptr;
    CDE90Widget* magControlWidget = nullptr;
    CDE90ButtonBox* doubleUpBtn = nullptr;
    CDE90ButtonBox* doubleDownBtn = nullptr;
    CDE90ButtonBox* upBtn = nullptr;
    CDE90ButtonBox* downBtn = nullptr;
    CDE90ButtonBox* customScanBtn = nullptr;
    CDE90ButtonBox* viewSetupBtn = nullptr;
    CDE90ComboBox* viewSelectBtn = nullptr;
    CDE90Widget* beamViewContent = nullptr;
    CDE90Widget* centerWidget = nullptr;
    CDE90Widget* centerTitleWidget = nullptr;
    CDE90Label* titleLabel = nullptr;
    CDE90Label* resultLabel = nullptr;
    CDE90Widget* toolFunctionContent = nullptr;
    CDE90Widget* toolsWidget = nullptr;
    CDE90Button* vLineBtn = nullptr;
    CDE90Button* hLineBtn = nullptr;
    CDE90Button* crossBtn = nullptr;
    CDE90Button* rectBtn = nullptr;
    CDE90Button* locateCrossBtn = nullptr;
    CDE90Button* poundBtn = nullptr;
    CDE90Button* blankBtn = nullptr;
    CDE90Label* iconLabel = nullptr;
    CDE90ButtonBox* ampBtn = nullptr;
    CDE90ButtonBox* amsBtn = nullptr;
    CDE90ButtonBox* nextBtn = nullptr;
    CDE90ButtonBox* ddsBtn = nullptr;
    CDE90ButtonBox* vidBtn = nullptr;
    CDE90ButtonBox* functionBtn = nullptr;
    CDE90ButtonBox* closeBtn = nullptr;
    CDE90Widget* bottomToolContent = nullptr;
    CDE90ButtonBox* enclosureBtn = nullptr;
    CDE90ButtonBox* focusBtn = nullptr;
    CDE90ButtonBox* autoFocusBtn = nullptr;

    CDE90ButtonBox* focusDownBtn = nullptr;
    CDE90ButtonBox* focusUpBtn = nullptr;
    CDE90TextFieldBox* focusTextField = nullptr;
    UiImageOperationController() = default;
    ~UiImageOperationController() noexcept {}

    void init(QWidget* widget) {
        CDE90LayoutHelper helper(widget);
        init(helper);
    }

    void init(CDE90LayoutHelper& helper) {
        helper.loadFromPath(pathInfo.path);
        imageTypeItemContent = helper.get<CDE90Widget>("imageTypeItemContent");

        opticsBtn = helper.get<CDE90ButtonBox>("opticsBtn");
        pmBtn = helper.get<CDE90ButtonBox>("pmBtn");
        magTextField = helper.get<CDE90TextFieldBox>("magTextField");
        magControlWidget = helper.get<CDE90Widget>("magControlWidget");

        customScanBtn = helper.get<CDE90ButtonBox>("customScanBtn");
        viewSetupBtn = helper.get<CDE90ButtonBox>("viewSetupBtn");

        viewSelectBtn = helper.get<CDE90ComboBox>("viewSelectBtn");
        beamViewContent = helper.get<CDE90Widget>("beamViewContent");
        centerWidget = helper.get<CDE90Widget>("centerWidget");
        centerTitleWidget = helper.get<CDE90Widget>("centerTitleWidget");
        titleLabel = helper.get<CDE90Label>("titleLabel");
        resultLabel = helper.get<CDE90Label>("resultLabel");
        toolFunctionContent = helper.get<CDE90Widget>("toolFunctionContent");
        toolsWidget = helper.get<CDE90Widget>("toolsWidget");
        vLineBtn = helper.get<CDE90Button>("vLineBtn");
        hLineBtn = helper.get<CDE90Button>("hLineBtn");
        crossBtn = helper.get<CDE90Button>("crossBtn");
        rectBtn = helper.get<CDE90Button>("rectBtn");
        locateCrossBtn = helper.get<CDE90Button>("locateCrossBtn");
        poundBtn = helper.get<CDE90Button>("poundBtn");
        blankBtn = helper.get<CDE90Button>("blankBtn");
        iconLabel = helper.get<CDE90Label>("iconLabel");
        ampBtn = helper.get<CDE90ButtonBox>("ampBtn");
        amsBtn = helper.get<CDE90ButtonBox>("amsBtn");
        nextBtn = helper.get<CDE90ButtonBox>("nextBtn");
        ddsBtn = helper.get<CDE90ButtonBox>("ddsBtn");
        vidBtn = helper.get<CDE90ButtonBox>("vidBtn");
        functionBtn = helper.get<CDE90ButtonBox>("functionBtn");
        closeBtn = helper.get<CDE90ButtonBox>("closeBtn");
        bottomToolContent = helper.get<CDE90Widget>("bottomToolContent");
        enclosureBtn = helper.get<CDE90ButtonBox>("enclosureBtn");
        focusBtn = helper.get<CDE90ButtonBox>("focusBtn");
        autoFocusBtn = helper.get<CDE90ButtonBox>("autoFocusBtn");
        focusDownBtn = helper.get<CDE90ButtonBox>("focusDownBtn");
        focusUpBtn = helper.get<CDE90ButtonBox>("focusUpBtn");
        focusTextField = helper.get<CDE90TextFieldBox>("focusTextField");

    }
};
#endif
