#ifndef AXIS_ALIGNMENT_WAFER_CONTROLLER_H
#define AXIS_ALIGNMENT_WAFER_CONTROLLER_H
#include "axis_align_model.h"
#include "cde90_window.h"
#include "common_act_param_helper.h"

namespace cdsem {
class UiAxisAlignmentWaferController;
}
class AxisAlignDetailContent;
class AxisAlignWaferSelectorController;

class AxisAlignmentWaferController : public CDE90Window {
    SEM_SET_INIT_PARAM(
        AxisAlignModel)
    SEM_OK_CALL(
        AxisAlignModel)
private:
    cdsem::UiAxisAlignmentWaferController* ui = nullptr;
    AxisAlignDetailContent* axisAlignmentContent = nullptr;
    AxisAlignDetailContent* stigmaContent = nullptr;
    AxisAlignWaferSelectorController* waferSelector = nullptr;

    void create();
    void createAction();

    void createWaferSelector();
    void resetToComponent();
    QString collectData();

public:
     explicit AxisAlignmentWaferController(QWidget* parent = nullptr);
     ~AxisAlignmentWaferController() noexcept;

     void setGroup(const QString&);
};
#endif
