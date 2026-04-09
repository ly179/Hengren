#ifndef LOG_HISTORY_VIEW_CONTROLLER_H
#define LOG_HISTORY_VIEW_CONTROLLER_H
#include <QWidget>

namespace cdsem {
class UiLogHistoryViewController;
}
class AsyncCaller;
class LogRecordSearchDTO;
struct LogHistoryViewDataHelper;

class LogHistoryViewController : public QWidget{

private:
    cdsem::UiLogHistoryViewController* ui = nullptr;
    AsyncCaller* caller = nullptr;
    LogHistoryViewDataHelper* dataHelper;

    void create();
    void createAction();

    void doLoad();
    void localSearch(const LogRecordSearchDTO&);
public:
     explicit LogHistoryViewController(QWidget* parent = nullptr);
     ~LogHistoryViewController() noexcept;

     void initAsyncCaller(AsyncCaller*);
     AsyncCaller* getAsyncCaller();
};
#endif
