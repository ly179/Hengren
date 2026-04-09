#ifndef MAINTENANCE_CONTROLLER_H
#define MAINTENANCE_CONTROLLER_H
#include "cde90_window.h"
#include "common_act_param_helper.h"

namespace cdsem {
class UiMaintenanceController;
}
struct MaintenanceDataModel;

class MaintenanceController : public CDE90Window{
    SEM_OK_CALL(MaintenanceDataModel)
private:
    cdsem::UiMaintenanceController* ui = nullptr;

    void create();
    void createAction();

    QString collectData(MaintenanceDataModel&);

public:
     explicit MaintenanceController(QWidget* parent = nullptr);
     ~MaintenanceController() noexcept;

     void resetToComponent(const MaintenanceDataModel&);
};
#endif
