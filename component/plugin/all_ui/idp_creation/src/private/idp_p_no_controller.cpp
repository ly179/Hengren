#include "idp_p_no_controller.h"
#include "cdsem_number_validator_helper.h"
#include "ui_idp_p_d_no_content.h"

QVariant IdpPNoDataTableModel::getItemData(
    qint32 row, qint32 column) const {
    const QList<IdpPatternNumberData>& list = getDataList();
    if (row < 0 || row >= list.size()) {
        return "";
    }
    const IdpPatternNumberData& data = list[row];
    switch (column) {
    case 0:
        return data.before;
    case 1:
        return data.after;
    case 2:
        return data.name;
    }
    return "";
}
IdpPNoDataTableModel::IdpPNoDataTableModel(
    QObject* parent)
    : SimpleRowTableModel(parent) {}

IdpPNoController::IdpPNoController(
    QWidget* parent)
    : CDE90Window(parent) {
    create();
    createAction();
}

IdpPNoController::~IdpPNoController() noexcept {
    if (ui) {
        delete ui;
    }
}

void IdpPNoController::create() {
    ui = new cdsem::UiIdpPDNoContent;
    setContent(new QWidget);
    ui->init(getContent());
}

void IdpPNoController::createAction() {
    CDSemNumberValidatorHelper::intFree().put(ui->noTextField).autoFixMode();
    ui->contentTable->setSelectionMode(QTableView::SingleSelection);
    tableModel = new IdpPNoDataTableModel(ui->contentTable);
    tableModel->insertHead("Before");
    tableModel->insertHead("After");
    tableModel->insertHead("Name");
    ui->contentTable->replaceDataModel(tableModel);
    ui->contentTable->resizeColumnOnceWhileShow();
    bindCloseBtn(ui->cancelBtn->getQPushButton());
    connect(ui->okBtn->getQPushButton(), &QPushButton::clicked, this, [this]() {
        if (okCallback && okCallback()) {
            changeCloseStatusOnce(HW_OK);
            close();
        }
    });
    connect(ui->reNumBtn->getQPushButton(), &QPushButton::clicked, this, [this]() {
        QList<IdpPatternNumberData>& list = tableModel->getDataList();
        for (IdpPatternNumberData& d : list) {
            d.after = d.before;
        }
        tableModel->notifyDataChanged(0, list.size() - 1);
    });
    connect(ui->applyBtn->getQPushButton(), &QPushButton::clicked, this, [this]() {
        int index = ui->contentTable->getLimitSelectedRow();
        if (index == -1) {
            return;
        }
        QString text = ui->noTextField->text();
        if (text.isEmpty()) {
            return;
        }
        int after = text.toInt();
        IdpPatternNumberData& d = tableModel->getDataList()[index];
        if (d.after == after) {
            return;
        }
        d.after = after;
        tableModel->notifyDataChanged(index, index);
    });
}

IdpPNoDataTableModel* IdpPNoController::getTableModel() const {
    return tableModel;
}
