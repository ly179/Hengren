#include "idp_init_stage_different_controller.h"
#include "ui_idp_init_stage_different_controller.h"

struct IdpInitStageDifferentDataHelper{
    IdpInitStageDiffMode mode = IISDM_RELOAD;
};

IdpInitStageDifferentController::IdpInitStageDifferentController(QWidget* parent) : CDE90Dialog(parent) {
    create();
    createAction();
}

IdpInitStageDifferentController::~IdpInitStageDifferentController() noexcept {
    if (dataHelper) {
        delete dataHelper;
    }
    if (ui) {
        delete ui;
    }
}

void IdpInitStageDifferentController::create() {
    QWidget* content = new QWidget;
    setContent(content);
    ui = new cdsem::UiIdpInitStageDifferentController;
    ui->init(content);

    dataHelper = new IdpInitStageDifferentDataHelper;
}

void IdpInitStageDifferentController::createAction() {
    bindCloseBtn(ui->cancelBtn->getQPushButton());
    connect(ui->loadToGlobalRadio, &QRadioButton::toggled, this, [this](bool checked){
        if (checked)
            dataHelper->mode = IISDM_RELOAD;
    });
    connect(ui->ignoreRadio, &QRadioButton::toggled, this, [this](bool checked){
        if (checked)
            dataHelper->mode = IISDM_IGNORE;
    });
    connect(ui->okBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        changeCloseStatusOnce(HW_OK);
        close();
    });
}

IdpInitStageDiffMode IdpInitStageDifferentController::getMode() const {
    return dataHelper->mode;
}
bool IdpInitStageDifferentController::reloadIdp() const {
    return ui->loadIdpCheckBox->isChecked();
}
