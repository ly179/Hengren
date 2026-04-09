#include "idp_opened_stage_different_controller.h"
#include "ui_idp_opened_stage_different_controller.h"

struct IdpOpenedStageDifferentDataHelper {
    sem::NO_PARAM_ARG_WITH_RETURN_FUNC<bool> okCallback;
    IdpOpenedStageDiffMode mode = IOSDM_RELOAD;
};

IdpOpenedStageDifferentController::IdpOpenedStageDifferentController(QWidget* parent) : CDE90Dialog(parent) {
    create();
    createAction();
}

IdpOpenedStageDifferentController::~IdpOpenedStageDifferentController() noexcept {
    if (dataHelper) {
        delete dataHelper;
    }
    if (ui) {
        delete ui;
    }
}

void IdpOpenedStageDifferentController::create() {
    QWidget* content = new QWidget;
    setContent(content);
    ui = new cdsem::UiIdpOpenedStageDifferentController;
    ui->init(content);

    dataHelper = new IdpOpenedStageDifferentDataHelper;
}

void IdpOpenedStageDifferentController::createAction() {
    connect(ui->reloadRadio, &QRadioButton::toggled, this, [this](bool checked){
        if (checked)
            dataHelper->mode = IOSDM_RELOAD;
    });
    connect(ui->saveRadio, &QRadioButton::toggled, this, [this](bool checked){
        if (checked)
            dataHelper->mode = IOSDM_SAVE_EXIT;
    });
    connect(ui->saveAsRadio, &QRadioButton::toggled, this, [this](bool checked){
        if (checked) {
            dataHelper->mode = IOSDM_SAVE_AS_EXIT;
        }
        ui->saveAsFileNameTextField->setDisabled(!checked);
    });
    connect(ui->ignoreRadio, &QRadioButton::toggled, this, [this](bool checked){
        if (checked)
            dataHelper->mode = IOSDM_IGNORE;
    });
    connect(ui->okBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        if (!dataHelper->okCallback || dataHelper->okCallback()) {
            changeCloseStatusOnce(HW_OK);
            close();
        }
    });
}

IdpOpenedStageDiffMode IdpOpenedStageDifferentController::getMode() const {
    return dataHelper->mode;
}

bool IdpOpenedStageDifferentController::reloadIdp() const {
    return ui->loadIdpCheckBox->isChecked();
}

QString IdpOpenedStageDifferentController::saveAsFileName() const {
    return ui->saveAsFileNameTextField->text();
}

void IdpOpenedStageDifferentController::setOkCallback(const sem::NO_PARAM_ARG_WITH_RETURN_FUNC<bool>& callback) {
    dataHelper->okCallback = callback;
}
