#include "recipe_ref_data_select_controller.h"
#include <QItemSelectionModel>
#include "ui_recipe_ref_data_select_controller.h"
#include "list_view_main_data_model.h"

RecipeRefDataSelectController::RecipeRefDataSelectController(QWidget* parent) : CDE90Window(parent) {
    //如果你没有异步任务需求，请将connect清理掉，保留create和createAction函数
    connect(this, &RecipeRefDataSelectController::asyncCallerChanged, this, [this](){
        create();
        createAction();
    }, Qt::SingleShotConnection);
}

RecipeRefDataSelectController::~RecipeRefDataSelectController() noexcept {
    if (ui) {
        delete ui;
    }
}

void RecipeRefDataSelectController::setOkCallback(
    const sem::NO_PARAM_ARG_FUNC& callback) {
    okCallback = callback;
}

void RecipeRefDataSelectController::create() {
    ui = new cdsem::UiRecipeRefDataSelectController;
    setContent(new QWidget);
    ui->init(getContent());
}

void RecipeRefDataSelectController::createAction() {
    listModel = new ListViewMainModel;
    ui->listContent->replaceDataModel(listModel);
    ui->listContent->setSelectionMode(QAbstractItemView::SingleSelection);
    bindCloseBtn(ui->cancelBtn->getQPushButton());
    connect(ui->okBtn->getQPushButton(), &QPushButton::clicked, this, [this]() {
        if (okCallback) {
            okCallback();
        }
    });
    connect(&getSelectModel(),
            &QItemSelectionModel::selectionChanged,
            this,
            [this](const QItemSelection& selected, const QItemSelection& deselected) {
                Q_UNUSED(selected)
                Q_UNUSED(deselected)
                int row = ui->listContent->getLimitSelectedRow();
                if (row == -1) {
                    selectedData = nullptr;
                    return;
                }
                selectedData = &listModel->getDataList()[row];
                ui->selectTextField->setText(selectedData->getText());
            });
}

const ListViewMainData* RecipeRefDataSelectController::getSelectedData() const {
    return selectedData;
}

ListViewMainModel& RecipeRefDataSelectController::getDataModel() {
    return *listModel;
}

QItemSelectionModel& RecipeRefDataSelectController::getSelectModel() {
    return *ui->listContent->selectionModel();
}

void RecipeRefDataSelectController::setSubTitle(
    const QString& str) {
    ui->typeLabel->setText(str);
}

void RecipeRefDataSelectController::setClassName(
    const QString& name) {
    ui->classTextField->setText(name);
}
