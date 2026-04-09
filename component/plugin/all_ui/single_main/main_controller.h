#ifndef MAIN_CONTROLLER_H
#define MAIN_CONTROLLER_H
#include "ui_single_main_global.h"
#include <QWidget>
#include "cde90_window.h"

namespace Ui {
class MainController;
}

class MainControllerHelper;
class QTimer;
class UI_SINGLE_MAIN_EXPORT MainController : public CDE90Window
{
    Q_OBJECT

public:
    explicit MainController(QWidget *parent = nullptr);
    ~MainController();

private:

    MainControllerHelper* helper;

    QTimer *timer;

    Ui::MainController *ui;

    void startClock();
};

#endif // MAIN_CONTROLLER_H
