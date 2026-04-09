#include "idp_d_no_measurement_controller.h"
#include "d_no_modify_input_validator.h"
#include "ui_idp_d_no_controller.h"
#include "view_data_mode.h"
#include "cdsem_global_data.h"
#include "idp_data_model.h"

struct MeasureDTableData {
    QString mpName;
    QString before;
    QString after;
    int object;
};

class MeasureDTableModel : public SimpleRowTableModel<MeasureDTableData> {
private:
    virtual QVariant getItemData(qint32 row, qint32 column) const override{
        if (row < 0 || column < 0) return QVariant();
        if (column == 0) return row + 1;
        const QList<MeasureDTableData>& list = getDataList();
        if (row >= list.size()) return QVariant();
        const MeasureDTableData& data = list[row];
        switch (column) {
        case 1:
            return cdsem::getMeasureObjectName(data.object);
        case 2:
            return data.mpName;
        case 3:
            return data.before;
        case 4:
            return data.after;
        default:
            return QVariant();
        }
    }
public:
    explicit MeasureDTableModel(QObject* parent = nullptr) : SimpleRowTableModel(parent){}
    ~MeasureDTableModel() noexcept {}
};

struct IdpDNoMeasurementControllerDataHelper {
    DNoModifyInputValidator* inputValidator;
    MeasureDTableModel* dTableModel;
    //修改和读取数据时使用
    QSharedPointer<IdpMpPosListData> posData;
    QSharedPointer<IdpMpParamListData> detailData;
    //检查数据是否被变更时使用
    int index = -1;
};

IdpDNoMeasurementController::IdpDNoMeasurementController(
    QWidget* parent)
    : CDE90Window(parent) {
    create();
    createAction();
}

IdpDNoMeasurementController::~IdpDNoMeasurementController() noexcept {
    if (dataHelper)
        delete dataHelper;
    if (ui) {
        delete ui;
    }
}

void IdpDNoMeasurementController::create() {
    dataHelper = new IdpDNoMeasurementControllerDataHelper;
    ui = new cdsem::UiIdpDNoController;
    setContent(new QWidget);
    ui->init(getContent());
}

void IdpDNoMeasurementController::createAction() {
    dataHelper->inputValidator = new DNoModifyInputValidator(ui->dNoTextField->getQLineEdit());
    ui->dNoTextField->getQLineEdit()->setValidator(dataHelper->inputValidator);
    dataHelper->dTableModel = new MeasureDTableModel(ui->measureTable);
    dataHelper->dTableModel->insertHead("No.");
    dataHelper->dTableModel->insertHead("Object");
    dataHelper->dTableModel->insertHead("MP Name");
    dataHelper->dTableModel->insertHead("Before");
    dataHelper->dTableModel->insertHead("After");
    ui->measureTable->replaceDataModel(dataHelper->dTableModel);
    ui->measureTable->setSelectionMode(QAbstractItemView::SingleSelection);
    bindCloseBtn(ui->cancelBtn->getQPushButton());
    connect(ui->okBtn->getQPushButton(), &QPushButton::clicked, this, [this]() {
        if (okCallback && okCallback()) {
            changeCloseStatusOnce(HW_OK);
            close();
        }
    });

    connect(ui->modifyBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        int index = ui->measureTable->getSelectedRow();
        if (index < 0) return;
        QString text = ui->dNoTextField->text();
        if (text.isEmpty()) return;
        if (text.endsWith(",")) text = text.left(text.length() - 1);
        MeasureDTableData& data = dataHelper->dTableModel->getDataList()[index];
        data.after = text;
        dataHelper->dTableModel->notifyDataChanged(index, index);
    });

    connect(ui->renumBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        QList<MeasureDTableData>& modelList = dataHelper->dTableModel->getDataList();
        if (modelList.isEmpty()) return;
        for (MeasureDTableData& d : modelList) {
            d.after = d.before;
        }
        dataHelper->dTableModel->notifyDataChanged(0, modelList.size() - 1);
    });
}

void IdpDNoMeasurementController::closeEvent(QCloseEvent* event) {
    CDE90Window::closeEvent(event);
    if (event->isAccepted()) {
        dataHelper->posData.clear();
        dataHelper->detailData.clear();
        dataHelper->index = -1;
        dataHelper->dTableModel->clear();
        ui->dNoTextField->getQLineEdit()->clear();
        ui->chipTextField->getQLineEdit()->clear();
    }
}

void IdpDNoMeasurementController::setData(const QSharedPointer<IdpMpPosListData>& posData,
             const QSharedPointer<IdpMpParamListData>& detailData,
             int index) {
    dataHelper->dTableModel->clear();
    dataHelper->posData = posData;
    dataHelper->detailData = detailData;
    dataHelper->index = index;
    ui->pTextField->setText(detailData->pNumber);
    ui->chipTextField->setText(QString("%1,%2").arg(posData->chipX.toString(), posData->chipY.toString()));
    QList<IdpMExeParamDetail>& list = dataHelper->detailData->mpTemplate.templateParam.exeParam.list;
    if (list.isEmpty()) return;
    QList<MeasureDTableData>& modelList = dataHelper->dTableModel->getDataList();
    modelList.resize(list.size());
    for (int i = 0; i < list.size(); ++ i) {
        IdpMExeParamDetail& data = list[i];
        MeasureDTableData& modelData = modelList[i];
        modelData.object = data.object;
        modelData.mpName = data.mpName;
        if (i >= dataHelper->posData->dNumbers.size()) continue;
        listJoinToStr<int>(dataHelper->posData->dNumbers[i], modelData.before);
        modelData.after = modelData.before;
    }
    dataHelper->dTableModel->resizeViewList(list.size());
}

bool IdpDNoMeasurementController::checkSameData(const QSharedPointer<IdpMpPosListData>& posData) const{
    return posData == dataHelper->posData;
}

int IdpDNoMeasurementController::getIndex() const {
    return dataHelper->index;
}

//ok callback使用
QString IdpDNoMeasurementController::modifyData() {
    QList<MeasureDTableData>& modelList = dataHelper->dTableModel->getDataList();
    QList<QList<int>>& dList = dataHelper->posData->dNumbers;
    dList.clear();
    dList.reserve(modelList.size());
    for (MeasureDTableData& d : modelList) {
        dList.append(strToNumList<int>(d.after));
    }
    return "";
}
