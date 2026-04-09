#ifndef UI_IMAGE_VIEW_CONTROLLER
#define UI_IMAGE_VIEW_CONTROLLER
#include <QtGlobal>
#include "cde90_layout_helper.h"
#include "cde90_button.h"
#include "cde90_label.h"
#include "cde90_check_box.h"
#include "cde90_text_field.h"
#include <QWidget>

/**
 * 本类由编辑器自动生成。
 */
namespace cdsem {
class UiImageViewController;
struct UiImageViewControllerPath;
}

struct cdsem::UiImageViewControllerPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
    UiImageViewControllerPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/hui";
#endif
        huiFileName = "image_view_edit.hui";
        path = proHuiDir + "/" + huiFileName;
    }
};
class cdsem::UiImageViewController {
private:
    const cdsem::UiImageViewControllerPath pathInfo;
public:
    QWidget* canvasContent = nullptr;
    CDE90TextFieldBox* fileNameTextField = nullptr;
    CDE90ButtonBox* sendImageBtn = nullptr;
    CDE90CheckBox* eraseOvelayCBox = nullptr;
    CDE90CheckBox* patternAreaCBox = nullptr;
    CDE90ButtonBox* overlayControlBtn = nullptr;
    CDE90TextFieldBox* imageNoTextField = nullptr;
    CDE90Label* imageTotalLabel = nullptr;
    CDE90ButtonBox* leftBtn = nullptr;
    CDE90ButtonBox* rightBtn = nullptr;
    CDE90ButtonBox* infoBtn = nullptr;
    CDE90ButtonBox* closeBtn = nullptr;
    UiImageViewController() = default;
    ~UiImageViewController() noexcept {}

    void init(QWidget* widget) {
        CDE90LayoutHelper helper(widget);
        init(helper);
    }

    void init(CDE90LayoutHelper& helper) {
        helper.loadFromPath(pathInfo.path);
        canvasContent = helper.get<QWidget>("canvasContent");
        fileNameTextField = helper.get<CDE90TextFieldBox>("fileNameTextField");
        sendImageBtn = helper.get<CDE90ButtonBox>("sendImageBtn");
        eraseOvelayCBox = helper.get<CDE90CheckBox>("eraseOvelayCBox");
        patternAreaCBox = helper.get<CDE90CheckBox>("patternAreaCBox");
        overlayControlBtn = helper.get<CDE90ButtonBox>("overlayControlBtn");
        imageNoTextField = helper.get<CDE90TextFieldBox>("imageNoTextField");
        imageTotalLabel = helper.get<CDE90Label>("imageTotalLabel");
        leftBtn = helper.get<CDE90ButtonBox>("leftBtn");
        rightBtn = helper.get<CDE90ButtonBox>("rightBtn");
        infoBtn = helper.get<CDE90ButtonBox>("infoBtn");
        closeBtn = helper.get<CDE90ButtonBox>("closeBtn");

    }
};
#endif
