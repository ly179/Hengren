#include "main_controller.h"
#include <QDateTime>
#include <QStyle>
#include <QTimer>
#include <QWidgetAction>
#include "cde90_label.h"
#include "cde90_menu.h"
#include "main_controller_helper.h"
#include "sem_common.h"
#include "ui_main_controller.h"
#include "image_operation_window_helper.h"
#include "cdsem_global_data.h"
#include "service_manager.h"
#include "system_default_config_service.h"
#include "cdsem_global_data.h"
#include "cde90_message_box.h"
#include <QPointer>

#define BTN_MIN_WIDTH 120

void toLightStyle(CDE90ButtonBox* box) {
    addCssClass(*box->getQPushButton(), "LightBg");
    box->getQPushButton()->style()->unpolish(box->getQPushButton());
    box->getQPushButton()->style()->polish(box->getQPushButton());
}

MainController::MainController(
    QWidget *parent)
    : CDE90Window(parent)
    , ui(new Ui::MainController) {
    setContent(new QWidget);
    ui->setupUi(getContent());
    helper = new MainControllerHelper;
    helper->setMainWidget(this);

    ui->btn_manualController->getQPushButton()->setText("Manual Controller");
    ui->btn_fileManager->getQPushButton()->setText("File Manager");
    ui->btn_templateManager->getQPushButton()->setText("Template");
    ui->btn_imageOperation->getQPushButton()->setText("Image Operation");
    ui->btn_recipeController->getQPushButton()->setText("Recipe Controller");
    ui->btn_utility->getQPushButton()->setText("Utility");
    ui->btn_worksheet->getQPushButton()->setText("Work Sheet");
    ui->btn_stageController->getQPushButton()->setText("Stage Controller");
    ui->btn_loaderController->getQPushButton()->setText("Loader Controller");
    ui->btn_amp->getQPushButton()->setText("AMP");
    ui->btn_changeUser->getQPushButton()->setText("Change User");
    ui->btn_hardCopy->getQPushButton()->setText("Hard Copy");
    ui->btn_monitor->getQPushButton()->setText("Monitor");
    ui->btn_option->getQPushButton()->setText("Option");
    ui->btn_logout->getQPushButton()->setText("Logout");
    ui->btn_msr->getQPushButton()->setText("MSR");
    ui->btn_image->getQPushButton()->setText("Image");
    ui->btn_maintenance->getQPushButton()->setText("Maintenance");



    ui->btn_manualController->getQPushButton()->setMinimumWidth(BTN_MIN_WIDTH);
    ui->btn_fileManager->getQPushButton()->setMinimumWidth(BTN_MIN_WIDTH);
    ui->btn_templateManager->getQPushButton()->setMinimumWidth(BTN_MIN_WIDTH);
    ui->btn_imageOperation->getQPushButton()->setMinimumWidth(BTN_MIN_WIDTH);
    ui->btn_recipeController->getQPushButton()->setMinimumWidth(BTN_MIN_WIDTH);
    ui->btn_utility->getQPushButton()->setMinimumWidth(BTN_MIN_WIDTH);
    ui->btn_worksheet->getQPushButton()->setMinimumWidth(BTN_MIN_WIDTH);
    ui->btn_stageController->getQPushButton()->setMinimumWidth(BTN_MIN_WIDTH);
    ui->btn_loaderController->getQPushButton()->setMinimumWidth(BTN_MIN_WIDTH);
    ui->btn_amp->getQPushButton()->setMinimumWidth(BTN_MIN_WIDTH);
    ui->btn_changeUser->getQPushButton()->setMinimumWidth(BTN_MIN_WIDTH);
    ui->btn_hardCopy->getQPushButton()->setMinimumWidth(BTN_MIN_WIDTH);
    ui->btn_monitor->getQPushButton()->setMinimumWidth(BTN_MIN_WIDTH);
    ui->btn_option->getQPushButton()->setMinimumWidth(BTN_MIN_WIDTH);
    ui->btn_logout->getQPushButton()->setMinimumWidth(BTN_MIN_WIDTH);
    ui->btn_msr->getQPushButton()->setMinimumWidth(BTN_MIN_WIDTH);
    ui->btn_image->getQPushButton()->setMinimumWidth(BTN_MIN_WIDTH);
    ui->btn_maintenance->getQPushButton()->setMinimumWidth(BTN_MIN_WIDTH);

    QPointer pointer(this);

    connect(helper, &MainControllerHelper::manualControllerVisible, this, [pointer](bool v){
        if (!pointer) return;
        if (v) addCssClass(*pointer->ui->btn_manualController->getQPushButton(), "GreenBg");
        else removeCssClass(*pointer->ui->btn_manualController->getQPushButton(), "GreenBg");
        pointer->ui->btn_manualController->getQPushButton()->style()->unpolish(pointer->ui->btn_manualController->getQPushButton());
        pointer->ui->btn_manualController->getQPushButton()->style()->polish(pointer->ui->btn_manualController->getQPushButton());
    });

    connect(helper, &MainControllerHelper::fileManagerControllerVisible, this, [pointer](bool v){
        if (!pointer) return;
        if (v) addCssClass(*pointer->ui->btn_fileManager->getQPushButton(), "GreenBg");
        else removeCssClass(*pointer->ui->btn_fileManager->getQPushButton(), "GreenBg");
        pointer->ui->btn_fileManager->getQPushButton()->style()->unpolish(pointer->ui->btn_fileManager->getQPushButton());
        pointer->ui->btn_fileManager->getQPushButton()->style()->polish(pointer->ui->btn_fileManager->getQPushButton());
    });
    connect(helper, &MainControllerHelper::templateManagerControllerVisible, this, [pointer](bool v){
        if (!pointer) return;
        if (v) addCssClass(*pointer->ui->btn_templateManager->getQPushButton(), "GreenBg");
        else removeCssClass(*pointer->ui->btn_templateManager->getQPushButton(), "GreenBg");
        pointer->ui->btn_templateManager->getQPushButton()->style()->unpolish(pointer->ui->btn_templateManager->getQPushButton());
        pointer->ui->btn_templateManager->getQPushButton()->style()->polish(pointer->ui->btn_templateManager->getQPushButton());
    });
    connect(helper, &MainControllerHelper::recipeControllerVisible, this, [pointer](bool v){
        if (!pointer) return;
        if (v) addCssClass(*pointer->ui->btn_recipeController->getQPushButton(), "GreenBg");
        else removeCssClass(*pointer->ui->btn_recipeController->getQPushButton(), "GreenBg");
        pointer->ui->btn_recipeController->getQPushButton()->style()->unpolish(pointer->ui->btn_recipeController->getQPushButton());
        pointer->ui->btn_recipeController->getQPushButton()->style()->polish(pointer->ui->btn_recipeController->getQPushButton());
    });
    connect(helper, &MainControllerHelper::worksheetControllerVisible, this, [pointer](bool v){
        if (!pointer) return;
        if (v) addCssClass(*pointer->ui->btn_worksheet->getQPushButton(), "GreenBg");
        else removeCssClass(*pointer->ui->btn_worksheet->getQPushButton(), "GreenBg");
        pointer->ui->btn_worksheet->getQPushButton()->style()->unpolish(pointer->ui->btn_worksheet->getQPushButton());
        pointer->ui->btn_worksheet->getQPushButton()->style()->polish(pointer->ui->btn_worksheet->getQPushButton());
    });
    connect(helper, &MainControllerHelper::stageControllerVisible, this, [pointer](bool v){
        if (!pointer) return;
        if (v) addCssClass(*pointer->ui->btn_stageController->getQPushButton(), "GreenBg");
        else removeCssClass(*pointer->ui->btn_stageController->getQPushButton(), "GreenBg");
        pointer->ui->btn_stageController->getQPushButton()->style()->unpolish(pointer->ui->btn_stageController->getQPushButton());
        pointer->ui->btn_stageController->getQPushButton()->style()->polish(pointer->ui->btn_stageController->getQPushButton());
    });
    connect(helper, &MainControllerHelper::loaderControllerVisible, this, [pointer](bool v){
        if (!pointer) return;
        if (v) addCssClass(*pointer->ui->btn_loaderController->getQPushButton(), "GreenBg");
        else removeCssClass(*pointer->ui->btn_loaderController->getQPushButton(), "GreenBg");
        pointer->ui->btn_loaderController->getQPushButton()->style()->unpolish(pointer->ui->btn_loaderController->getQPushButton());
        pointer->ui->btn_loaderController->getQPushButton()->style()->polish(pointer->ui->btn_loaderController->getQPushButton());
    });
    connect(ImageOperationWindowHelper::get(), &ImageOperationController::visibleSignal, this, [pointer](bool v){
        if (!pointer) return;
        if (v) addCssClass(*pointer->ui->btn_imageOperation->getQPushButton(), "GreenBg");
        else removeCssClass(*pointer->ui->btn_imageOperation->getQPushButton(), "GreenBg");
        pointer->ui->btn_imageOperation->getQPushButton()->style()->unpolish(pointer->ui->btn_imageOperation->getQPushButton());
        pointer->ui->btn_imageOperation->getQPushButton()->style()->polish(pointer->ui->btn_imageOperation->getQPushButton());
    });
    connect(cdsem::CDSEM_GlobalDataHelper::get(), &cdsem::CDSEM_GlobalData::ampWindowStatusChanged, this, [pointer](){
        if (!pointer) return;
        if (cdsem::CDSEM_GlobalDataHelper::get()->getAmpWindowStatus()) {
            addCssClass(*pointer->ui->btn_amp->getQPushButton(), "GreenBg");
        } else {
            removeCssClass(*pointer->ui->btn_amp->getQPushButton(), "GreenBg");
        }
        pointer->ui->btn_amp->getQPushButton()->style()->unpolish(pointer->ui->btn_amp->getQPushButton());
        pointer->ui->btn_amp->getQPushButton()->style()->polish(pointer->ui->btn_amp->getQPushButton());
    });
    connect(helper, &MainControllerHelper::msrControllerVisible, this, [pointer](bool v){
        if (!pointer) return;
        if (v) addCssClass(*pointer->ui->btn_msr->getQPushButton(), "GreenBg");
        else removeCssClass(*pointer->ui->btn_msr->getQPushButton(), "GreenBg");
        pointer->ui->btn_msr->getQPushButton()->style()->unpolish(pointer->ui->btn_msr->getQPushButton());
        pointer->ui->btn_msr->getQPushButton()->style()->polish(pointer->ui->btn_msr->getQPushButton());
    });
    connect(helper, &MainControllerHelper::imageControllerVisible, this, [pointer](bool v){
        if (!pointer) return;
        if (v) addCssClass(*pointer->ui->btn_image->getQPushButton(), "GreenBg");
        else removeCssClass(*pointer->ui->btn_image->getQPushButton(), "GreenBg");
        pointer->ui->btn_image->getQPushButton()->style()->unpolish(pointer->ui->btn_image->getQPushButton());
        pointer->ui->btn_image->getQPushButton()->style()->polish(pointer->ui->btn_image->getQPushButton());
    });


    connect(ui->btn_manualController->getQPushButton(), &CDE90Button::clicked, this, [this](){
        helper->openManualController();
    });
    connect(ui->btn_fileManager->getQPushButton(), &CDE90Button::clicked, this, [this]() {
        helper->openFileController();
    });
    connect(ui->btn_imageOperation->getQPushButton(), &CDE90Button::clicked, this, [this](){
        helper->openImageOperationController();
    });
    connect(ui->btn_templateManager->getQPushButton(), &CDE90Button::clicked, this, [this]() {
        helper->openTemplateController();
    });
    connect(ui->btn_recipeController->getQPushButton(), &CDE90Button::clicked, this, [this]() {
        helper->openRecipeController();
    });
    connect(ui->btn_worksheet->getQPushButton(), &QPushButton::clicked, this, [this]() {
        helper->openWorkSheetController();
    });

    connect(ui->btn_stageController->getQPushButton(), &QPushButton::clicked, this, [this]() {
        helper->openStageController();
    });

    connect(ui->btn_loaderController->getQPushButton(), &QPushButton::clicked, this, [this]() {
        helper->openLoaderController();
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

    toLightStyle(ui->btn_utility);
    toLightStyle(ui->btn_hardCopy);
    toLightStyle(ui->btn_monitor);
    toLightStyle(ui->btn_option);
    toLightStyle(ui->btn_maintenance);

    CDE90Menu *menu = new CDE90Menu(ui->btn_utility->getQPushButton());
    QWidgetAction *titleAction = new QWidgetAction(menu);
    CDE90Label *label = new CDE90Label("Select Function", menu);
    label->setStyleSheet("QLabel {background-color: rgb(215, 215, 215);}");
    label->setAlignment(Qt::AlignCenter);
    titleAction->setDefaultWidget(label);

    menu->addAction(titleAction);
    menu->addSeparator();
    QAction *action;
    menu->addAction("Device Controller");
    menu->addAction("Memo Print");
    menu->addAction("SECS Parameters");
    menu->addAction("System Information");
    menu->addAction("Security Level");
    menu->addAction("System Parameters...");
    action = menu->addAction("Save Calibration");
    connect(action, &QAction::triggered, this, [this](){
        //这是个临时功能，因此不做子任务线程处理。
        SystemDefaultConfigService* service = ServiceManager::defaultService<SystemDefaultConfigService>();
        int errCode = service->saveGlobalCalibration(CALIBRATION_ENV);
        if (errCode) simpleMessageBox(this, "Error", "Error to save Calibration data.");
    });
    ui->btn_utility->getQPushButton()->setMenu(menu);
}

MainController::~MainController()
{
    delete helper;
    delete ui;
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
