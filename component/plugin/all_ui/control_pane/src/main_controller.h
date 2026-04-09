#ifndef MAIN_CONTROLLER_H
#define MAIN_CONTROLLER_H
#include "control_pane_global.h"
#include "relation_main_controller.h"

class ImageOperationController;

namespace cdsem {
class UiControlPaneController;
}

class MainControllerHelper;
class QTimer;
class UI_CONTROL_PANE_EXPORT MainController : public RelationMainController {
    friend class MainControllerHelper;
    Q_OBJECT

public:
    explicit MainController(QWidget *parent = nullptr);
    ~MainController();

    ImageOperationController* getImageOpWidget() const { return imageOpWidget; }
private:
    MainControllerHelper* helper;
    QTimer *timer;
    cdsem::UiControlPaneController* ui;
    ImageOperationController* imageOpWidget = nullptr;

    void create();
    void createAction();
    void startClock();
};

#endif // MAIN_CONTROLLER_H
