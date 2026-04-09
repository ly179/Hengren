#include "idp_d_no_p_controller.h"
#include "d_no_modify_input_validator.h"
#include "ui_idp_p_d_no_content.h"

QVariant IdpDNoByPDataTableModel::getItemData(
    qint32 row, qint32 column) const {
    const QList<IdpDByPData>& list = getDataList();
    if (row < 0 || row >= list.size()) {
        return "";
    }
    const IdpDByPData& data = list[row];
    switch (column) {
    case 0:
        return data.pNo;
    case 1:
        return data.before;
    case 2:
        return data.after;
    case 3:
        return data.name;
    }
    return "";
}

QVariant IdpDNoByPDataTableModel::data(
    const QModelIndex& index, int role) const {
    if (role == Qt::ForegroundRole) {
        const QList<IdpDByPData>& list = getDataList();
        const IdpDByPData& data = list[index.row()];
        if (data.updated) return QColor(40, 21, 254);
    }
    return SimpleRowTableModel::data(index, role);
}

IdpDNoByPDataTableModel::IdpDNoByPDataTableModel(
    QObject* parent)
    : SimpleRowTableModel(parent) {}


struct IdpDNoByPControllerDataHelper {
    IdpDNoByPDataTableModel* tableModel;
    DNoModifyInputValidator* inputValidator;
};

IdpDNoByPController::IdpDNoByPController(
    QWidget* parent)
    : CDE90Window(parent) {
    create();
    createAction();
}

IdpDNoByPController::~IdpDNoByPController() noexcept {
    if (dataHelper) delete dataHelper;
    if (ui) {
        delete ui;
    }
}

void IdpDNoByPController::create() {
    dataHelper = new IdpDNoByPControllerDataHelper;
    ui = new cdsem::UiIdpPDNoContent;
    setContent(new QWidget);
    ui->init(getContent());
    ui->titleLabel->setText("D No.");
    
}
void IdpDNoByPController::createAction() {
    dataHelper->inputValidator = new DNoModifyInputValidator(ui->noTextField->getQLineEdit());
    ui->noTextField->getQLineEdit()->setValidator(dataHelper->inputValidator);
    dataHelper->tableModel = new IdpDNoByPDataTableModel(ui->contentTable);
    dataHelper->tableModel->insertHead("P No.");
    dataHelper->tableModel->insertHead("Before");
    dataHelper->tableModel->insertHead("After");
    dataHelper->tableModel->insertHead("Name");
    ui->contentTable->replaceDataModel(dataHelper->tableModel);
    ui->contentTable->resizeColumnOnceWhileShow();
    bindCloseBtn(ui->cancelBtn->getQPushButton());
    connect(ui->okBtn->getQPushButton(), &QPushButton::clicked, this, [this]() {
        if (okCallback && okCallback()) {
            changeCloseStatusOnce(HW_OK);
            close();
        }
    });
    connect(ui->reNumBtn->getQPushButton(), &QPushButton::clicked, this, [this]() {
        QList<IdpDByPData>& list = dataHelper->tableModel->getDataList();
        for (IdpDByPData& d : list) {
            d.after = d.before;
            d.updated = false;
        }
        dataHelper->tableModel->notifyDataChanged(0, list.size() - 1);
    });
    connect(ui->applyBtn->getQPushButton(), &QPushButton::clicked, this, [this]() {
        int index = ui->contentTable->getLimitSelectedRow();
        if (index == -1) {
            return;
        }
        QString after = ui->noTextField->text();
        if (after.isEmpty()) {
            return;
        }
        IdpDByPData& d = dataHelper->tableModel->getDataList()[index];
        if (d.after == after) {
            return;
        }
        d.after = after;
        d.updated = true;
        dataHelper->tableModel->notifyDataChanged(index, index);
    });
}

IdpDNoByPDataTableModel* IdpDNoByPController::getTableModel() const {
    return dataHelper->tableModel;
}
