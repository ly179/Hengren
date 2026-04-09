#ifndef UI_JOB_RECIPE_SETTING_CONTROLLER
#define UI_JOB_RECIPE_SETTING_CONTROLLER
#include <QtGlobal>
#include "cde90_layout_helper.h"
#include "cde90_combo_box.h"
#include "cde90_list_view.h"
#include "cde90_radio_button.h"
#include "wafer_cassette_widget.h"
#include <QAction>
#include "cde90_text_field.h"
#include "cde90_button.h"

/**
 * 本类由编辑器自动生成。
 */
namespace cdsem {
class UiJobRecipeSettingController;
struct UiJobRecipeSettingControllerPath;
}

struct cdsem::UiJobRecipeSettingControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
    UiJobRecipeSettingControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/hui";
#endif
        huiFileName = "job_recipe_setting_editor.hui";
        path = proHuiDir + "/" + huiFileName;
    }
};
class cdsem::UiJobRecipeSettingController {
private:
    const cdsem::UiJobRecipeSettingControllerPath pathInfo;
public:
    CDE90TextFieldBox* portTextField = nullptr;
    CDE90ButtonBox* classSelectBtn = nullptr;
    CDE90TextFieldBox* classTextField = nullptr;
    CDE90ButtonBox* actionBtn = nullptr;
    QAction* filterByIdwAct = nullptr;
    QAction* viewAllAct = nullptr;
    CDE90ListView* recipeListView = nullptr;
    CDE90TextFieldBox* recipeTextField = nullptr;
    CDE90TextFieldBox* idwTextField = nullptr;
    CDE90TextFieldBox* idpTextField = nullptr;
    CDE90ButtonBox* setBtn = nullptr;
    CDE90TextFieldBox* userDataSlotTextField = nullptr;
    CDE90TextFieldBox* operatorTextField = nullptr;
    CDE90TextFieldBox* lotIdTextField = nullptr;
    CDE90TextFieldBox* processTextField = nullptr;
    CDE90TextFieldBox* equipmentTextField = nullptr;
    CDE90TextFieldBox* waferIdTextField = nullptr;
    CDE90TextFieldBox* commentTextField = nullptr;
    CDE90ComboBox* recipeUintItemBox = nullptr;
    WaferCassetteWidget* cassetteWidget = nullptr;
    CDE90RadioButton* slotAppointRadio = nullptr;
    CDE90RadioButton* changeEnvRadio = nullptr;
    CDE90RadioButton* editUserDataRadio = nullptr;
    CDE90RadioButton* slotInfoRadio = nullptr;
    CDE90ButtonBox* closeBtn = nullptr;
    CDE90ButtonBox* exeBtn = nullptr;
    CDE90ButtonBox* restartBtn = nullptr;
    CDE90ButtonBox* clearBtn = nullptr;
    UiJobRecipeSettingController() = default;
    ~UiJobRecipeSettingController() noexcept {}

    void init(QWidget* widget) {
        CDE90LayoutHelper helper(widget);
        init(helper);
    }

    void init(CDE90LayoutHelper& helper) {
        helper.loadFromPath(pathInfo.path);
        portTextField = helper.get<CDE90TextFieldBox>("portTextField");
        classSelectBtn = helper.get<CDE90ButtonBox>("classSelectBtn");
        classTextField = helper.get<CDE90TextFieldBox>("classTextField");
        actionBtn = helper.get<CDE90ButtonBox>("actionBtn");
        filterByIdwAct = helper.get<QAction>("filterByIdwAct");
        viewAllAct = helper.get<QAction>("viewAllAct");
        recipeListView = helper.get<CDE90ListView>("recipeListView");
        recipeTextField = helper.get<CDE90TextFieldBox>("recipeTextField");
        idwTextField = helper.get<CDE90TextFieldBox>("idwTextField");
        idpTextField = helper.get<CDE90TextFieldBox>("idpTextField");
        setBtn = helper.get<CDE90ButtonBox>("setBtn");
        userDataSlotTextField = helper.get<CDE90TextFieldBox>("userDataSlotTextField");
        operatorTextField = helper.get<CDE90TextFieldBox>("operatorTextField");
        lotIdTextField = helper.get<CDE90TextFieldBox>("lotIdTextField");
        processTextField = helper.get<CDE90TextFieldBox>("processTextField");
        equipmentTextField = helper.get<CDE90TextFieldBox>("equipmentTextField");
        waferIdTextField = helper.get<CDE90TextFieldBox>("waferIdTextField");
        commentTextField = helper.get<CDE90TextFieldBox>("commentTextField");
        recipeUintItemBox = helper.get<CDE90ComboBox>("recipeUintItemBox");
        cassetteWidget = helper.get<WaferCassetteWidget>("cassetteWidget");
        slotAppointRadio = helper.get<CDE90RadioButton>("slotAppointRadio");
        changeEnvRadio = helper.get<CDE90RadioButton>("changeEnvRadio");
        editUserDataRadio = helper.get<CDE90RadioButton>("editUserDataRadio");
        slotInfoRadio = helper.get<CDE90RadioButton>("slotInfoRadio");
        closeBtn = helper.get<CDE90ButtonBox>("closeBtn");
        exeBtn = helper.get<CDE90ButtonBox>("exeBtn");
        restartBtn = helper.get<CDE90ButtonBox>("restartBtn");
        clearBtn = helper.get<CDE90ButtonBox>("clearBtn");

    }
};
#endif
