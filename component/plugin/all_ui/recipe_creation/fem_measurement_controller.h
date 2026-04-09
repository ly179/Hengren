#ifndef FEM_MEASUREMENT_CONTROLLER_H
#define FEM_MEASUREMENT_CONTROLLER_H
#include "cde90_window.h"
#include "common_act_param_helper.h"

namespace cdsem {
class UiFemMeasurementController;
}
class FemMeasurementCanvas;
struct FemMeasureModel;

class FemMeasurementController : public CDE90Window{
    SEM_OK_CALL(
        FemMeasureModel)
private:
    cdsem::UiFemMeasurementController* ui = nullptr;
    FemMeasurementCanvas* canvas;

    void create();
    void createAction();

    QString collectData(FemMeasureModel&);

public:
     explicit FemMeasurementController(QWidget* parent = nullptr);
     ~FemMeasurementController() noexcept;

     void resetToComponent(const FemMeasureModel&);
};
#endif
