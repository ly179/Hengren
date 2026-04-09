#ifndef LOG_VIEW_CONTROLLER_H
#define LOG_VIEW_CONTROLLER_H
#include "relation_log_viewer_controller.h"
#include "log_view_global.h"

namespace cdsem {
class UiLogViewController;
}
class HighlightLogTextarea;
class LogHistoryViewController;

struct LogViewControllerDataHelper;
class LOG_VIEW_LIB_EXPORT LogViewController : public RelationLogViewerController{
    friend struct LogViewControllerDataHelper;
private:
    QScopedPointer<LogViewControllerDataHelper> dataHelper;
    cdsem::UiLogViewController* ui = nullptr;
    LogHistoryViewController* historyController = nullptr;

    void create();
    void createAction();

public:
     explicit LogViewController(QWidget* parent = nullptr);
     ~LogViewController() noexcept;
};
#endif
