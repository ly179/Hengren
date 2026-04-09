#include "idp_p_measure_times_controller.h"
#include "cdsem_number_validator_helper.h"
#include "idp_data_model.h"
#include "ui_idp_p_measure_times_controller.h"
#include "view_data_mode.h"

class PNoTableModel : public SimpleRowTableModel<IdpMpMeasureTimesData> {
private:
    QVariant getItemData(
        qint32 row, qint32 column) const {
        const IdpMpMeasureTimesData& data = getDataList()[row];
        QList<IdpMExeParamDetail>* list;

        switch (column) {
        case 0:
            return data.mpParam->pNumber;
        case 1:
            list = &data.mpParam->mpTemplate.templateParam.exeParam.list;
            if (list->isEmpty()) {
                return "";
            }
            return (*list)[0].mpName;
        case 2:
            return data.times;
        }
        return "";
    }

public:
    PNoTableModel(
        QObject* parent = nullptr)
        : SimpleRowTableModel(parent) {}
};

IdpPMeasureTimesController::IdpPMeasureTimesController(QWidget* parent) : CDE90Window(parent) {
    create();
    createAction();
}

IdpPMeasureTimesController::~IdpPMeasureTimesController() noexcept {
    if (ui) {
        delete ui;
    }
}

void IdpPMeasureTimesController::create() {
    ui = new cdsem::UiIdpPMeasureTimesController;
    setContent(new QWidget);
    ui->init(getContent());

    ui->pNoTable->resizeColumnOnceWhileShow();
}

void IdpPMeasureTimesController::createAction() {
    CDSemNumberValidatorHelper::range(1, 40).put(ui->timesTextField).autoFixMode();
    bindCloseBtn(ui->cancelBtn->getQPushButton());
    connect(ui->okBtn->getQPushButton(), &QPushButton::clicked, this, [this]() {
        if (okCallback && okCallback()) {
            changeCloseStatusOnce(HW_OK);
            close();
        }
    });
    connect(ui->applyBtn->getQPushButton(), &QPushButton::clicked, this, [this]() {
        int index = ui->pNoTable->getLimitSelectedRow();
        if (index < 0) {
            return;
        }
        QString text = ui->timesTextField->text();
        if (text.isEmpty()) {
            return;
        }
        tableModel->getDataList()[index].times = text.toInt();
        tableModel->notifyDataChanged(index, index);
    });
    tableModel = new PNoTableModel(ui->pNoTable);
    tableModel->insertHead("P No.");
    tableModel->insertHead("MP Name");
    tableModel->insertHead("Times");
    ui->pNoTable->replaceDataModel(tableModel);
    ui->pNoTable->setSelectionMode(QAbstractItemView::SingleSelection);
}

void IdpPMeasureTimesController::collectData() {
    QList<IdpMpMeasureTimesData>& pList = tableModel->getDataList();
    if (pList.isEmpty()) {
        return;
    }
    for (IdpMpMeasureTimesData& d : pList) {
        d.mpParam->times = d.times;
    }
}
void IdpPMeasureTimesController::resetToComponent(
    const QList<QSharedPointer<IdpMpParamListData>>& list) {
    tableModel->clear();
    if (list.isEmpty()) {
        return;
    }
    QList<IdpMpMeasureTimesData>& pList = tableModel->getDataList();
    pList.resize(list.size());
    for (int i = 0; i < list.size(); ++i) {
        IdpMpMeasureTimesData& pData = pList[i];
        const QSharedPointer<IdpMpParamListData>& data = list[i];
        pData.mpParam = data;
        pData.times = data->times;
    }
    tableModel->resizeViewList(list.size());
}
