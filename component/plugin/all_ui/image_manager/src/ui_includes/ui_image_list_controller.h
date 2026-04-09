#ifndef UI_IMAGE_LIST_CONTROLLER
#define UI_IMAGE_LIST_CONTROLLER
#include <QtGlobal>
#include "cde90_layout_helper.h"
#include "cde90_scroll_area.h"
#include "process_loading_pane.h"
#include <QWidget>
#include "cde90_text_field.h"
#include "cde90_label.h"
#include "cde90_button.h"

/**
 * 本类由编辑器自动生成。
 */
namespace cdsem {
class UiImageListController;
struct UiImageListControllerPath;
}

struct cdsem::UiImageListControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
    UiImageListControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/hui";
#endif
        huiFileName = "image_list_edit.hui";
        path = proHuiDir + "/" + huiFileName;
    }
};
class cdsem::UiImageListController {
private:
    const cdsem::UiImageListControllerPath pathInfo;
public:
    CDE90ScrollArea* imageScrollArea = nullptr;
    QWidget* imageListContent = nullptr;
    CDE90Label* detailLabel = nullptr;
    ProcessLoadingPane* processPane = nullptr;
    QWidget* pageWidget = nullptr;
    CDE90ButtonBox* prevBtn = nullptr;
    CDE90TextFieldBox* pageTextField = nullptr;
    CDE90Label* totalLabel = nullptr;
    CDE90ButtonBox* nextBtn = nullptr;
    CDE90ButtonBox* closeBtn = nullptr;
    UiImageListController() = default;
    ~UiImageListController() noexcept {}

    void init(QWidget* widget) {
        CDE90LayoutHelper helper(widget);
        init(helper);
    }

    void init(CDE90LayoutHelper& helper) {
        helper.loadFromPath(pathInfo.path);
        imageScrollArea = helper.get<CDE90ScrollArea>("imageScrollArea");
        imageListContent = helper.get<QWidget>("imageListContent");
        detailLabel = helper.get<CDE90Label>("detailLabel");
        processPane = helper.get<ProcessLoadingPane>("processPane");
        pageWidget = helper.get<QWidget>("pageWidget");
        prevBtn = helper.get<CDE90ButtonBox>("prevBtn");
        pageTextField = helper.get<CDE90TextFieldBox>("pageTextField");
        totalLabel = helper.get<CDE90Label>("totalLabel");
        nextBtn = helper.get<CDE90ButtonBox>("nextBtn");
        closeBtn = helper.get<CDE90ButtonBox>("closeBtn");

    }
};
#endif
