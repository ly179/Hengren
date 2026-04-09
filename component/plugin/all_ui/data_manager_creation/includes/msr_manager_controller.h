#ifndef MSR_MANAGER_CONTROLLER_H
#define MSR_MANAGER_CONTROLLER_H
#include "ui_data_manager_creation_global.h"
#include "relation_msr_manager.h"

namespace cdsem {
class UiMsrManagerController;
}

struct MsrManagerDataHelper;

class UI_DATA_MANAGER_EXPORT MsrManagerController : public RelationMSRManager {
    friend struct MsrManagerDataHelper;
    Q_OBJECT
private:
    MsrManagerDataHelper* dataHelper = nullptr;
    cdsem::UiMsrManagerController* ui = nullptr;

    void create();
    void createAction();
public:
     explicit MsrManagerController(QWidget* parent = nullptr);
     ~MsrManagerController() noexcept;

 signals:
     void openWorkSheet(quint64 msrTopId);
};
#endif
