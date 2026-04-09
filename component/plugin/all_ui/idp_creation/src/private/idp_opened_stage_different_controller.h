#ifndef IDP_OPENED_STAGE_DIFFERENT_CONTROLLER_H
#define IDP_OPENED_STAGE_DIFFERENT_CONTROLLER_H
#include "cde90_dialog.h"

namespace cdsem {
class UiIdpOpenedStageDifferentController;
}

enum IdpOpenedStageDiffMode {
    IOSDM_RELOAD, //reload
    IOSDM_SAVE_EXIT, //save and close
    IOSDM_SAVE_AS_EXIT, //save as and close
    IOSDM_IGNORE //ignore
};

struct IdpOpenedStageDifferentDataHelper;
class IdpOpenedStageDifferentController : public CDE90Dialog{

private:
    IdpOpenedStageDifferentDataHelper* dataHelper = nullptr;
    cdsem::UiIdpOpenedStageDifferentController* ui = nullptr;

    void create();
    void createAction();
public:
     explicit IdpOpenedStageDifferentController(QWidget* parent = nullptr);
     ~IdpOpenedStageDifferentController() noexcept;

     IdpOpenedStageDiffMode getMode() const;
     bool reloadIdp() const;
     QString saveAsFileName() const;

     void setOkCallback(const sem::NO_PARAM_ARG_WITH_RETURN_FUNC<bool>&);
};
#endif
