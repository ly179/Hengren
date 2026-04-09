#ifndef APP_INIT_VIEW_CONTROLLER_H
#define APP_INIT_VIEW_CONTROLLER_H
#include "cde90_window.h"
#include "ui_single_main_global.h"

namespace cdsem {
class UiAppInitViewController;
}

class MainController;
class SerialLoadingManager;
class UI_SINGLE_MAIN_EXPORT AppInitViewController : public CDE90Window{

private:
    cdsem::UiAppInitViewController* ui = nullptr;
    SerialLoadingManager* manager = nullptr;

    void create();
    void createAction();

    void startup();
public:
     explicit AppInitViewController(QWidget* parent = nullptr);
     ~AppInitViewController() noexcept;
};
#endif
