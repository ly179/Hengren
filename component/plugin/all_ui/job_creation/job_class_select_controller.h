#ifndef JOB_CLASS_SELECT_CONTROLLER_H
#define JOB_CLASS_SELECT_CONTROLLER_H
#include "cde90_window.h"
#include "common_act_param_helper.h"

namespace cdsem {
class UiJobClassSelectController;
}
class ListViewMainModel;
class ListViewMainData;

class JobClassSelectController : public CDE90Window {

    SEM_OK_CALL(
        ListViewMainData)

private:
    cdsem::UiJobClassSelectController* ui = nullptr;
    ListViewMainModel* listViewModel = nullptr;

    void create();
    void createAction();
public:
     explicit JobClassSelectController(QWidget* parent = nullptr);
     ~JobClassSelectController() noexcept;

     void loadClassList();
};
#endif
