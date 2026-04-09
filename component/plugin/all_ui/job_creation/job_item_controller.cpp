#include "job_item_controller.h"
#include "job_recipe_setting_controller.h"
#include "sem_global.h"
#include "ui_job_item_controller.h"

JobItemController::JobItemController(
    QWidget* parent)
    : QWidget(parent) {
    create();
}

JobItemController::~JobItemController() noexcept {
    if (ui) {
        delete ui;
    }
}

void JobItemController::create() {
    ui = new cdsem::UiJobItemController;
    ui->init(this);
    ui->recipeBtn->getQPushButton()->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    ui->cancelBtn->getQPushButton()->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
}

void JobItemController::createAction() {
    connect(ui->recipeBtn->getQPushButton(), &QPushButton::clicked, this, [this]() {
        createJobRecipeSettingEditor();
        recipeController->show();
        MainThreadInvoker::getInvoker().moveToScreenCenter(recipeController);
    });
}

void JobItemController::createJobRecipeSettingEditor() {
    if (recipeController) {
        return;
    }
    recipeController = new JobRecipeSettingController(this);
    recipeController->setWindowTitle("Recipe Assignment");
    recipeController->setWindowModality(Qt::WindowModal);
    recipeController->setResizable(false);
    recipeController->initAsyncCaller(caller);
    recipeController->setPort(port);
}

void JobItemController::setPortLabelText(
    const QString& text) {
    ui->portLabel->setText(text);
}
void JobItemController::setSetupLabelText(
    const QString& text) {
    ui->setupLabel->setText(text);
}

void JobItemController::initAsyncCaller(
    AsyncCaller* caller) {
    if (this->caller) {
        return;
    }
    this->caller = caller;
    createAction();
}

void JobItemController::setPort(
    char p) {
    port = p;
}
