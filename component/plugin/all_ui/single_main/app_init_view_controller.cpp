#include "app_init_view_controller.h"
#include "ui_app_init_view_controller.h"
#include <QGridLayout>
#include <QRegularExpression>
#include "main_controller.h"
#include "private/app_init_interface.h"
#include "private/app_init_all.h"
#include "private/app_init_loading_helper.h"

class AppInitLastStep : public AppInitLoadingData {
public:
    explicit AppInitLastStep(QObject* parent = nullptr) : AppInitLoadingData(parent) {
        setLoadOnTaskThread(false);
        setTitle("Finished");
    }
    void load() override{
        MainController* mainController = new MainController;
        mainController->setWindowTitle("Main Controller");
        mainController->setAttribute(Qt::WA_DeleteOnClose);
        // mainController->hideTitle();
        // mainController->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        // mainController->setResizable(false);
        mainController->resize(QSize(mainController->width(), 60));
        mainController->show();
        emit overLoad(OperationResult {});
    }
};

AppInitViewController::AppInitViewController(QWidget* parent) : CDE90Window(parent) {
    //如果你没有异步任务需求，请将connect清理掉，保留create和createAction函数
    connect(this, &AppInitViewController::asyncCallerChanged, this, [this](){
        create();
        createAction();
    }, Qt::SingleShotConnection);
}

AppInitViewController::~AppInitViewController() noexcept {
    if (manager) delete manager;
    if (ui) {
        delete ui;
    }
}

void AppInitViewController::create() {
    QWidget* content = new QWidget;
    setContent(content);
    ui = new cdsem::UiAppInitViewController;
    ui->init(content);

    manager = new SerialLoadingManager(content);
    manager->setContent(ui->content);
    itsChildAndBeManaged(manager, this);
    manager->setASyncCaller(ACaller);
}

void AppInitViewController::createAction() {
    bindCloseBtn(ui->closeBtn->getQPushButton());
    connect(manager, &SerialLoadingManager::finished, this, [this](){
        this->close();
    });
    appendAll(manager);
    manager->append(new AppInitLastStep);

    startup();
}

void AppInitViewController::startup() {
    manager->start();
}
