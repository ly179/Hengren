#ifndef JOB_CONTROLLER_H
#define JOB_CONTROLLER_H
#include "cde90_window.h"
#include "ui_job_creation_global.h"

namespace cdsem {
class UiJobController;
}
class JobItemController;

class UI_JOB_LIB_EXPORT JobController : public CDE90Window {
private:
    cdsem::UiJobController* ui = nullptr;
    JobItemController** jobItemControllers = nullptr;

    void create();
    void createAction();

    void createJobItemController();

public:
     explicit JobController(QWidget* parent = nullptr);
     ~JobController() noexcept;
};
#endif
