#include "procedure_manual_normal_window.h"
#include "ui_procedure_manual_normal_window.h"
#include "sem_view_common.h"

#include <QCloseEvent>

struct ProcedureManualNormalDataHelper {
    ProcedureManualNormalWindow* d;

    void doClose(ProcedureManualNormalStatus status) {
        d->ui->btnArea->setEnabled(false);
        // NOLINTNEXTLINE(clang-analyzer-optin.core.EnumCastOutOfRange)
        d->changeCloseStatusOnce(CDE90WindowCloseStatus(status));
        d->close();
    }
};

ProcedureManualNormalWindow::ProcedureManualNormalWindow(QWidget* parent) : RelatoinProcedureManualNormal(parent) {
    dataHelper = new ProcedureManualNormalDataHelper;
    dataHelper->d = this;
    create();
    createAction();
}

ProcedureManualNormalWindow::~ProcedureManualNormalWindow() noexcept {
    if (ui) {
        delete ui;
    }
    if (dataHelper) delete dataHelper;
}

void ProcedureManualNormalWindow::create() {
    QWidget* content = new QWidget;
    setContent(content);
    ui = new cdsem::UiProcedureManualNormalWindow;
    ui->init(content);

    addCssClass(ui->okBtn->getQPushButton(), "BlueText PageText");
    addCssClass(ui->processBtn->getQPushButton(), "BlueText PageText");
    addCssClass(ui->retryBtn->getQPushButton(), "BlueText PageText");
    addCssClass(ui->envBtn->getQPushButton(), "BlueText PageText");
    addCssClass(ui->rejectBtn->getQPushButton(), "BlueText PageText");
}

void ProcedureManualNormalWindow::createAction() {
    connect(ui->okBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        dataHelper->doClose(PMNS_OK);
    });
    connect(ui->processBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        dataHelper->doClose(PMNS_PROCESS);
    });
    connect(ui->retryBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        dataHelper->doClose(PMNS_RETRY);
    });
    connect(ui->envBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        dataHelper->doClose(PMNS_ENV);
    });
    connect(ui->rejectBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        dataHelper->doClose(PMNS_REJECT);
    });
}

void ProcedureManualNormalWindow::closeEvent(QCloseEvent* event) {
    int status = closeStatus();
    if (status != PMNS_OK && status != PMNS_PROCESS && status != PMNS_RETRY
        && status != PMNS_ENV && status != PMNS_REJECT && status != PMNS_FOCUS_CLOSE) {
        event->ignore();
        return;
    }
    if (status == PMNS_FOCUS_CLOSE) {
        setCloseCall(nullptr);
    }
    this->RelatoinProcedureManualNormal::closeEvent(event);
    if (!event->isAccepted()) {
        ui->btnArea->setEnabled(true);
    }
}

void ProcedureManualNormalWindow::setMsgText(const QString& text) {
    ui->msgLabel->setText(text);
}
void ProcedureManualNormalWindow::setProcessButtonText(const QString& text) {
    ui->processBtn->getQPushButton()->setText(text);
}
void ProcedureManualNormalWindow::showProcessButton(bool b) {
    ui->processBtn->setVisible(b);
}
void ProcedureManualNormalWindow::showOKButton(bool b) {
    ui->okBtn->setVisible(b);
}
void ProcedureManualNormalWindow::setRetryButtonText(const QString& text) {
    ui->retryBtn->getQPushButton()->setText(text);
}
void ProcedureManualNormalWindow::setEnvButtonText(const QString& text) {
    ui->envBtn->getQPushButton()->setText(text);
}
void ProcedureManualNormalWindow::setRejectButtonText(const QString& text) {
    ui->rejectBtn->getQPushButton()->setText(text);
}
void ProcedureManualNormalWindow::showCmdArea(bool b) {
    ui->cmdArea->setVisible(b);
}
