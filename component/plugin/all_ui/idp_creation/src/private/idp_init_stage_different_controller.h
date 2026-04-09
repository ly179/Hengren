#ifndef IDP_INIT_STAGE_DIFFERENT_CONTROLLER_H
#define IDP_INIT_STAGE_DIFFERENT_CONTROLLER_H
#include "cde90_dialog.h"

namespace cdsem {
class UiIdpInitStageDifferentController;
}

enum IdpInitStageDiffMode {
    IISDM_RELOAD, //reload
    IISDM_IGNORE //ignore
};

struct IdpInitStageDifferentDataHelper;
class IdpInitStageDifferentController : public CDE90Dialog{

private:
    IdpInitStageDifferentDataHelper* dataHelper = nullptr;
    cdsem::UiIdpInitStageDifferentController* ui = nullptr;

    void create();
    void createAction();
public:
     explicit IdpInitStageDifferentController(QWidget* parent = nullptr);
     ~IdpInitStageDifferentController() noexcept;

     IdpInitStageDiffMode getMode() const;
     bool reloadIdp() const;
};
#endif
