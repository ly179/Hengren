#ifndef UI_PROCEDURE_MANUAL_NORMAL_WINDOW
#define UI_PROCEDURE_MANUAL_NORMAL_WINDOW
#include <QtGlobal>
#include "cde90_layout_helper.h"
#include <QWidget>
#include "cde90_button.h"
#include "cde90_label.h"

/**
 * 本类由编辑器自动生成。
 */
namespace cdsem {
class UiProcedureManualNormalWindow;
struct UiProcedureManualNormalWindowPath;
}

struct cdsem::UiProcedureManualNormalWindowPath {
    QString proHuiDir;
    QString huiFileName;
    QString path;
    UiProcedureManualNormalWindowPath() {
#ifdef __PRO_HUI_DIR
        proHuiDir = __PRO_HUI_DIR;
#else
        proHuiDir = ":/hui";
#endif
        huiFileName = "procedure_manual_normal_window.hui";
        path = proHuiDir + "/" + huiFileName;
    }
};
class cdsem::UiProcedureManualNormalWindow {
private:
    const cdsem::UiProcedureManualNormalWindowPath pathInfo;
public:
    CDE90Label* msgLabel = nullptr;
    QWidget* btnArea = nullptr;
    CDE90ButtonBox* okBtn = nullptr;
    CDE90ButtonBox* processBtn = nullptr;
    QWidget* cmdArea = nullptr;
    CDE90ButtonBox* retryBtn = nullptr;
    CDE90ButtonBox* envBtn = nullptr;
    CDE90ButtonBox* rejectBtn = nullptr;
    UiProcedureManualNormalWindow() = default;
    ~UiProcedureManualNormalWindow() noexcept {}

    void init(QWidget* widget) {
        CDE90LayoutHelper helper(widget);
        init(helper);
    }

    void init(CDE90LayoutHelper& helper) {
        helper.loadFromPath(pathInfo.path);
        msgLabel = helper.get<CDE90Label>("msgLabel");
        btnArea = helper.get<QWidget>("btnArea");
        okBtn = helper.get<CDE90ButtonBox>("okBtn");
        processBtn = helper.get<CDE90ButtonBox>("processBtn");
        cmdArea = helper.get<QWidget>("cmdArea");
        retryBtn = helper.get<CDE90ButtonBox>("retryBtn");
        envBtn = helper.get<CDE90ButtonBox>("envBtn");
        rejectBtn = helper.get<CDE90ButtonBox>("rejectBtn");

    }
};
#endif
