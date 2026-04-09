#ifndef MAIN_CONTROLLER_HELPER_H
#define MAIN_CONTROLLER_HELPER_H

#include <QObject>

class MainController;
class MainControllerHelper : public QObject{
    Q_OBJECT
private:

    MainController* parent;

public:
    explicit MainControllerHelper(QObject* = nullptr);

    virtual ~MainControllerHelper() noexcept;

    void setMainWidget(MainController* parent);

    void openFileController();

    void openTemplateController();

    void openImageOperationController();

    void openWorkSheetController();

    void openAmpController();

    void openMSRController();

    void openImageController();
};

#endif // MAIN_CONTROLLER_HELPER_H
