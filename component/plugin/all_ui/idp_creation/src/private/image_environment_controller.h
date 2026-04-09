#ifndef IMAGE_ENVIRONMENT_CONTROLLER_H
#define IMAGE_ENVIRONMENT_CONTROLLER_H
#include "cde90_window.h"
#include "common_act_param_helper.h"
#include "idp_data_model.h"

namespace cdsem {
class UiImageEnvironmentController;
}
class ImgEnvSettingContent;

class ImageEnvironmentController : public CDE90Window{
    SEM_SET_INIT_PARAM(
        IdpMpImageEnvData)
    SEM_OK_CALL(
        IdpMpImageEnvData)
private:
    cdsem::UiImageEnvironmentController* ui = nullptr;
    ImgEnvSettingContent* addressContent = nullptr;
    ImgEnvSettingContent* alignContent = nullptr;
    ImgEnvSettingContent* measureContent = nullptr;

    void create();
    void createAction();

    void resetToComponent();
    QString collectData();

public:
     explicit ImageEnvironmentController(QWidget* parent = nullptr);
     ~ImageEnvironmentController() noexcept;
};
#endif
