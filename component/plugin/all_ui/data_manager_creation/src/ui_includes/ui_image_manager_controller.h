#ifndef UI_IMAGE_MANAGER_CONTROLLER
#define UI_IMAGE_MANAGER_CONTROLLER
#include <QtGlobal>
#include "cde90_layout_helper.h"
#include "cde90_list_view.h"
#include "cde90_text_field.h"
#include <QAction>
#include "process_loading_pane.h"
#include "cde90_button.h"
#include "cde90_label.h"

/**
 * 本类由编辑器自动生成。
 */
namespace cdsem {
class UiImageManagerController;
struct UiImageManagerControllerPath;
}

struct cdsem::UiImageManagerControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
    UiImageManagerControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/hui";
#endif
        huiFileName = "image_manager_edit.hui";
        path = proHuiDir + "/" + huiFileName;
    }
};
class cdsem::UiImageManagerController {
private:
    const cdsem::UiImageManagerControllerPath pathInfo;
public:
    QAction* reloadAct = nullptr;
    QAction* displayAct = nullptr;
    QAction* copyAct = nullptr;
    QAction* moveAct = nullptr;
    QAction* deleteAct = nullptr;
    QAction* renameAct = nullptr;
    QAction* displayAll = nullptr;
    QAction* compressAct = nullptr;
    QAction* copyToPublicAct = nullptr;
    QAction* uploadAct = nullptr;
    QAction* printAct = nullptr;
    QAction* exitAct = nullptr;
    CDE90ButtonBox* deviceBtn = nullptr;
    CDE90TextFieldBox* deviceTextField = nullptr;
    CDE90Label* freeLabel = nullptr;
    CDE90ListView* classList = nullptr;
    CDE90ListView* msrList = nullptr;
    ProcessLoadingPane* processPane = nullptr;
    CDE90ListView* imageList = nullptr;
    CDE90TextFieldBox* classCountTextField = nullptr;
    CDE90TextFieldBox* msrCountTextField = nullptr;
    CDE90TextFieldBox* imageCountTextField = nullptr;
    CDE90TextFieldBox* selectedClassTextField = nullptr;
    CDE90TextFieldBox* selectedMsrTextField = nullptr;
    CDE90TextFieldBox* selectedImageTextField = nullptr;
    UiImageManagerController() = default;
    ~UiImageManagerController() noexcept {}

    void init(QWidget* widget) {
        CDE90LayoutHelper helper(widget);
        init(helper);
    }

    void init(CDE90LayoutHelper& helper) {
        helper.loadFromPath(pathInfo.path);
        reloadAct = helper.get<QAction>("reloadAct");
        displayAct = helper.get<QAction>("displayAct");
        copyAct = helper.get<QAction>("copyAct");
        moveAct = helper.get<QAction>("moveAct");
        deleteAct = helper.get<QAction>("deleteAct");
        renameAct = helper.get<QAction>("renameAct");
        displayAll = helper.get<QAction>("displayAll");
        compressAct = helper.get<QAction>("compressAct");
        copyToPublicAct = helper.get<QAction>("copyToPublicAct");
        uploadAct = helper.get<QAction>("uploadAct");
        printAct = helper.get<QAction>("printAct");
        exitAct = helper.get<QAction>("exitAct");
        deviceBtn = helper.get<CDE90ButtonBox>("deviceBtn");
        deviceTextField = helper.get<CDE90TextFieldBox>("deviceTextField");
        freeLabel = helper.get<CDE90Label>("freeLabel");
        classList = helper.get<CDE90ListView>("classList");
        msrList = helper.get<CDE90ListView>("msrList");
        processPane = helper.get<ProcessLoadingPane>("processPane");
        imageList = helper.get<CDE90ListView>("imageList");
        classCountTextField = helper.get<CDE90TextFieldBox>("classCountTextField");
        msrCountTextField = helper.get<CDE90TextFieldBox>("msrCountTextField");
        imageCountTextField = helper.get<CDE90TextFieldBox>("imageCountTextField");
        selectedClassTextField = helper.get<CDE90TextFieldBox>("selectedClassTextField");
        selectedMsrTextField = helper.get<CDE90TextFieldBox>("selectedMsrTextField");
        selectedImageTextField = helper.get<CDE90TextFieldBox>("selectedImageTextField");

    }
};
#endif
