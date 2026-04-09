#include "multi_shot_chip_select_controller.h"
#include "ui_multi_shot_chip_select_controller.h"
#include "multi_shot_chip_list_model.h"

struct MultiShotChipSelectDataHelper {
    MultiShotChipListModel* listModel;
    bool chipMode = false;
};

MultiShotChipSelectController::MultiShotChipSelectController(bool asChip, QWidget* parent) : CDE90Dialog(parent) {
    dataHelper = new MultiShotChipSelectDataHelper;
    dataHelper->chipMode = asChip;
    create();
    createAction();
}

MultiShotChipSelectController::~MultiShotChipSelectController() noexcept {
    if (dataHelper) delete dataHelper;
    if (ui) {
        delete ui;
    }
}

void MultiShotChipSelectController::create() {
    QWidget* content = new QWidget;
    setContent(content);
    ui = new cdsem::UiMultiShotChipSelectController;
    ui->init(content);
}

void MultiShotChipSelectController::createAction() {
    dataHelper->listModel = new MultiShotChipListModel(dataHelper->chipMode, ui->shotChipList);
    ui->shotChipList->replaceDataModel(dataHelper->listModel);
    bindCloseBtn(ui->cancelBtn->getQPushButton());
    bindSimpleOkBtn(ui->okBtn->getQPushButton());
}

void MultiShotChipSelectController::setListData(const QList<cdsem::PyShotChipCoordinate>& list) {
    dataHelper->listModel->clear();
    dataHelper->listModel->insertDatas(list);
}

cdsem::PyShotChipCoordinate* MultiShotChipSelectController::getSelectedData() const {
    int index = ui->shotChipList->getLimitSelectedRow();
    if (index < 0) return nullptr;
    return &dataHelper->listModel->getDataList()[index];
}

void MultiShotChipSelectController::clearListData() {
    dataHelper->listModel->clear();
}
