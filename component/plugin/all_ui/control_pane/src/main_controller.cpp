#include "main_controller.h"
#include <QDateTime>
#include <QStyle>
#include <QTimer>
#include <QWidgetAction>
#include "cde90_label.h"
#include "cde90_menu.h"
#include "private/main_controller_helper.h"
#include "sem_common.h"
#include "cdsem_global_data.h"
#include "service_manager.h"
#include "system_default_config_service.h"
#include "cdsem_global_data.h"
#include "cde90_message_box.h"
#include "ui_control_pane_controller.h"
#include "../../image_operation/includes/image_operation_controller.h"
#include "../../image_manager/includes/image_view_controller.h"
#include "signal_hub_plugin_ids.h"
#include "signal_hub_single_object_manager.h"
#include <QVBoxLayout>

#define BTN_MIN_WIDTH 120

void toLightStyle(CDE90ButtonBox* box) {
    addCssClass(*box->getQPushButton(), "LightBg");
    box->getQPushButton()->style()->unpolish(box->getQPushButton());
    box->getQPushButton()->style()->polish(box->getQPushButton());
}

MainController::MainController(
    QWidget *parent)
    : RelationMainController(parent) {
    connect(this, &CDE90Window::asyncCallerChanged, this, [this](){
        create();
        createAction();
    }, Qt::SingleShotConnection);
}
MainController::~MainController()
{
    delete helper;
    delete ui;
}

void MainController::create() {
    ui = new cdsem::UiControlPaneController;
    setContent(new QWidget);
    ui->init(getContent());
    helper = new MainControllerHelper;
    helper->setMainWidget(this);

    ui->btn_fileManager->getQPushButton()->setMinimumWidth(BTN_MIN_WIDTH);
    ui->btn_templateManager->getQPushButton()->setMinimumWidth(BTN_MIN_WIDTH);
    ui->btn_worksheet->getQPushButton()->setMinimumWidth(BTN_MIN_WIDTH);
    ui->btn_amp->getQPushButton()->setMinimumWidth(BTN_MIN_WIDTH);
    ui->btn_msr->getQPushButton()->setMinimumWidth(BTN_MIN_WIDTH);
    ui->btn_image->getQPushButton()->setMinimumWidth(BTN_MIN_WIDTH);

    if (ui->canvasContent) {
        imageOpWidget = new ImageOperationController(ui->canvasContent);

        imageOpWidget->setWindowFlags(Qt::Widget);
        imageOpWidget->setParent(ui->canvasContent);
        imageOpWidget->setWindowFlag(Qt::Window, false);

        imageOpWidget->initAsyncCaller([](){}, false);

        SignalHubSingleObjectManager::instance()->setObject(OPEN_SINGLE_IMAGE_OPERATION_WINDOW, imageOpWidget);

        QVBoxLayout* layout = new QVBoxLayout(ui->canvasContent);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(imageOpWidget);

        emit imageOpWidget->asyncCallerChanged();

        imageOpWidget->show();
    }
}

void MainController::createAction() {
    connect(ui->btn_fileManager->getQPushButton(), &CDE90Button::clicked, this, [this]() {
        helper->openFileController();
    });
    connect(ui->btn_templateManager->getQPushButton(), &CDE90Button::clicked, this, [this]() {
        helper->openTemplateController();
    });
    connect(ui->btn_worksheet->getQPushButton(), &QPushButton::clicked, this, [this]() {
        helper->openWorkSheetController();
    });
    connect(ui->btn_amp->getQPushButton(), &QPushButton::clicked, this, [this]() {
        helper->openAmpController();
    });
    connect(ui->btn_msr->getQPushButton(), &QPushButton::clicked, this, [this]() {
        helper->openMSRController();
    });
    connect(ui->btn_image->getQPushButton(), &QPushButton::clicked, this, [this]() {
        helper->openImageController();
    });

    startClock();
    
}

void MainController::startClock() {
    ui->label_time->setText(QDateTime::currentDateTime().toString("yyyy/MM/dd\nHH:mm"));
    timer = new QTimer(this);
    timer->setInterval(-1);
    QTime t = QTime::currentTime();
    int s = t.second();
    int m = t.msec();
    timer->singleShot(std::max(0, 60000 - (s * 1000 + m) + 10), this, [this]() {
        ui->label_time->setText(QDateTime::currentDateTime().toString("yyyy/MM/dd\nHH:mm"));
        connect(timer, &QTimer::timeout, this, [this]() {
            ui->label_time->setText(QDateTime::currentDateTime().toString("yyyy/MM/dd\nHH:mm"));
        });
        timer->start(60000);
    });
}
