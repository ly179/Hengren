#ifndef HISTORY_VIEW_CONTROLLER_H
#define HISTORY_VIEW_CONTROLLER_H
#include "cde90_window.h"

namespace cdsem {
class UiHistoryViewController;
}

enum HistoryVersionDataType {
    HVDT_IDW,
    HVDT_IDP,
    HVDT_RECIPE
};

struct HistoryViewDataHelper;
class HistoryViewController : public CDE90Window{
    Q_OBJECT
private:
    cdsem::UiHistoryViewController* ui = nullptr;
    HistoryViewDataHelper* dataHelper = nullptr;

    void create();
    void createAction();

    void localLoadIDW(quint64);
    void localLoadIDP(quint64);
    void localLoadRecipe(quint64);
public:
     explicit HistoryViewController(QWidget* parent = nullptr);
     ~HistoryViewController() noexcept;

     void loadData(quint64 id, HistoryVersionDataType);
     HistoryVersionDataType getLastDataType() const;

     void enableEditBtn(bool b = true);

 signals:
     void openEditor(HistoryVersionDataType type, quint64, bool allowCoverSave);
};
#endif
