#ifndef UI_WORKSHEET_CONTROLLER
#define UI_WORKSHEET_CONTROLLER
#include <QtGlobal>
#include "cde90_layout_helper.h"
#include "process_loading_pane.h"
#include "cde90_label.h"
#include "cde90_button.h"
#include "cde90_text_field.h"
#include "cde90_menu.h"
#include <QAction>
#include <QWidget>

/**
 * 本类由编辑器自动生成。
 */
namespace cdsem {
class UiWorksheetController;
struct UiWorksheetControllerPath;
}

struct cdsem::UiWorksheetControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
    UiWorksheetControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/h_ui";
#endif
        huiFileName = "worksheet_editor.hui";
        path = proHuiDir + "/" + huiFileName;
    }
};
class cdsem::UiWorksheetController {
private:
    const cdsem::UiWorksheetControllerPath pathInfo;
public:
    CDE90Menu* fileMenu = nullptr;
    QAction* saveAct = nullptr;
    QAction* saveAsAct = nullptr;
    QAction* exportToXlsx = nullptr;
    QAction* exitAct = nullptr;
    CDE90Menu* editMenu = nullptr;
    CDE90Menu* actionMenu = nullptr;
    QAction* measureRetryAct = nullptr;
    QAction* retryRejectedAct = nullptr;
    QAction* moveAct = nullptr;
    QAction* loadAmpAct = nullptr;
    QAction* getMeasureValue = nullptr;
    QAction* getValueAct = nullptr;
    QAction* showFemResultAct = nullptr;
    QAction* maintenanceAct = nullptr;
    CDE90Menu* imageMenu = nullptr;
    QAction* showMpImageAct = nullptr;
    QAction* showApImageAct = nullptr;
    QAction* displayAllMpImageAct = nullptr;
    QAction* sendImageAct = nullptr;
    CDE90Menu* modeMenu = nullptr;
    CDE90Menu* toolMenu = nullptr;
    CDE90Menu* helpMenu = nullptr;
    QWidget* detailContent = nullptr;
    CDE90ButtonBox* moveBtn = nullptr;
    CDE90ButtonBox* getValueBtn = nullptr;
    CDE90ButtonBox* retryBtn = nullptr;
    CDE90ButtonBox* cancelBtn = nullptr;
    ProcessLoadingPane* loadingPane = nullptr;
    CDE90TextFieldBox* modifyTextField = nullptr;
    CDE90ButtonBox* modifyBtn = nullptr;
    QWidget* worksheetContent = nullptr;
    CDE90Label* infoLabel = nullptr;
    UiWorksheetController() = default;
    ~UiWorksheetController() noexcept {}

    void init(QWidget* widget) {
        CDE90LayoutHelper helper(widget);
        init(helper);
    }

    void init(CDE90LayoutHelper& helper) {
        helper.loadFromPath(pathInfo.path);
        fileMenu = helper.get<CDE90Menu>("fileMenu");
        saveAct = helper.get<QAction>("saveAct");
        saveAsAct = helper.get<QAction>("saveAsAct");
        exportToXlsx = helper.get<QAction>("exportToXlsx");
        exitAct = helper.get<QAction>("exitAct");
        editMenu = helper.get<CDE90Menu>("editMenu");
        actionMenu = helper.get<CDE90Menu>("actionMenu");
        measureRetryAct = helper.get<QAction>("measureRetryAct");
        retryRejectedAct = helper.get<QAction>("retryRejectedAct");
        moveAct = helper.get<QAction>("moveAct");
        loadAmpAct = helper.get<QAction>("loadAmpAct");
        getMeasureValue = helper.get<QAction>("getMeasureValue");
        getValueAct = helper.get<QAction>("getValueAct");
        showFemResultAct = helper.get<QAction>("showFemResultAct");
        maintenanceAct = helper.get<QAction>("maintenanceAct");
        imageMenu = helper.get<CDE90Menu>("imageMenu");
        showMpImageAct = helper.get<QAction>("showMpImageAct");
        showApImageAct = helper.get<QAction>("showApImageAct");
        displayAllMpImageAct = helper.get<QAction>("displayAllMpImageAct");
        sendImageAct = helper.get<QAction>("sendImageAct");
        modeMenu = helper.get<CDE90Menu>("modeMenu");
        toolMenu = helper.get<CDE90Menu>("toolMenu");
        helpMenu = helper.get<CDE90Menu>("helpMenu");
        detailContent = helper.get<QWidget>("detailContent");
        moveBtn = helper.get<CDE90ButtonBox>("moveBtn");
        getValueBtn = helper.get<CDE90ButtonBox>("getValueBtn");
        retryBtn = helper.get<CDE90ButtonBox>("retryBtn");
        cancelBtn = helper.get<CDE90ButtonBox>("cancelBtn");
        loadingPane = helper.get<ProcessLoadingPane>("loadingPane");
        modifyTextField = helper.get<CDE90TextFieldBox>("modifyTextField");
        modifyBtn = helper.get<CDE90ButtonBox>("modifyBtn");
        worksheetContent = helper.get<QWidget>("worksheetContent");
        infoLabel = helper.get<CDE90Label>("infoLabel");

    }
};
#endif
