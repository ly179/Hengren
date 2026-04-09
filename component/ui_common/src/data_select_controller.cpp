#include "data_select_controller.h"
#include <QItemSelectionModel>
#include "cde90_message_box.h"
#include "ui_data_select_controller.h"
#include "list_view_main_data_model.h"

DataSelectController::DataSelectController(QWidget* parent, bool useAsyncCaller)
    : CDE90Dialog(parent) {
    if (useAsyncCaller) {
        connect(
            this,
            &DataSelectController::asyncCallerChanged,
            this,
            [this]() {
                create();
                createAction();
            },
            Qt::SingleShotConnection);
    } else {
        create();
        createAction();
    }
}

DataSelectController::~DataSelectController() noexcept {
    if (ui) {
        delete ui;
    }
}

void DataSelectController::localSearchIdByName(
    ListViewMainData& data, int* errCode) {
    data.setId(0);
    data.setTopId(0);
    if (errCode) *errCode = 0;
}

void DataSelectController::create() {
    ui = new cdsem::UiDataSelectController;
    setContent(new QWidget);
    ui->init(getContent());
}

void DataSelectController::createAction() {
    listModel = new ListViewMainModel(ui->listContent);
    ui->listContent->setModel(listModel);
    bindCloseBtn(ui->cancelBtn->getQPushButton());

    connect(ui->okBtn->getQPushButton(), &CDE90Button::clicked, this, [this]() {
        if (okCallback && okCallback()) {
            closeAsOk();
            return;
        }
    });

    connect(ui->listContent->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this,
            [this](const QItemSelection& selected, const QItemSelection& deselected) {
                Q_UNUSED(deselected)
                if (selected.isEmpty() || selected.size() > 1) {
                    ui->detailTextField->getQLineEdit()->clear();
                    selectedData = nullptr;
                    return;
                }
                QModelIndexList list = selected[0].indexes();
                if (list.isEmpty() || list.size() > 1) {
                    ui->detailTextField->getQLineEdit()->clear();
                    selectedData = nullptr;
                    return;
                }
                int index = list[0].row();
                if (index < 0 || index >= listModel->getDataList().size()) {
                    ui->detailTextField->getQLineEdit()->clear();
                    selectedData = nullptr;
                    return;
                }
                ListViewMainData& data = listModel->getDataList()[index];
                ui->detailTextField->setText(data.getText());
                selectedData = &data;
            });
}

void DataSelectController::setViewMode(bool useKindMode) {
    ui->titleLabel->setVisible(!useKindMode);
    ui->idContent->setVisible(useKindMode);
}

void DataSelectController::setTitle(const QString& title) {
    ui->titleLabel->setText(title);
    ui->idContentLabel->setText(title);
}
void DataSelectController::setSubTitle(const QString& title) {
    ui->idContentKind->setText(title);
}

void DataSelectController::closeAsOk() {
    changeCloseStatusOnce(HW_OK);
    close();
}

QString DataSelectController::getDetail() const {
    return ui->detailTextField->text();
}

const ListViewMainData* DataSelectController::getSelectedData() const {
    return selectedData;
}

ListViewMainModel& DataSelectController::getDataModel() {
    return *listModel;
}

QItemSelectionModel& DataSelectController::getSelectModel() {
    return *ui->listContent->selectionModel();
}

QString DataSelectController::getInputText() const {
    return ui->detailTextField->text();
}

bool DataSelectController::simpleGetSelectedData(
    const std::function<bool(const ListViewMainData&)>& func) {
    const ListViewMainData* selected = getSelectedData();
    ListViewMainData result;
    QString text = getInputText();
    if (selected) {
        result.setId(selected->getId());
        result.setTopId(selected->getTopId());
        if (selected->getText() == text) {
            result.setText(selected->getText());
            if (func(result)) {
                close();
            }
            return true;
        }
        result.setText(text);
    } else {
        result.setText(text);
        if (text.isEmpty()) {
            if (func(result)) {
                close();
            }
            return true;
        }
    }
    bool succ = ACaller->push(this, [this, result, func](TSC) mutable {
        int errCode;
        localSearchIdByName(result, &errCode);
        if (errCode) {
            simpleMessageBox(this, "Error", "Load data error.");
            return;
        }
        MainThreadInvoker::getInvoker().blockRun([&result, &func, this]() {
            //loading期间关闭窗口，视为放弃
            if (!isVisible()) {
                return;
            }
            if (func(result)) {
                close();
            }
        });
    });
    if (succ && ACaller->pushFocus(this, [this](TSC) {
            MainThreadInvoker::getInvoker().blockRun([this]() {
                ui->okBtn->overLoading();
                ui->cancelBtn->setDisabled(false);
            });
        })) {
        ui->okBtn->loading();
        ui->cancelBtn->setDisabled(true);
    }
    return succ;
}
