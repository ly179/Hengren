#include "job_class_select_controller.h"
#include "cde90_message_box.h"
#include "cde90_process_box.h"
#include "list_view_data_query_helper.h"
#include "ui_job_class_select_controller.h"
#include "list_view_main_data_model.h"

JobClassSelectController::JobClassSelectController(QWidget* parent) : CDE90Window(parent) {
    //如果你没有异步任务需求，请将connect清理掉，保留create和createAction函数
    connect(this, &JobClassSelectController::asyncCallerChanged, this, [this](){
        create();
        createAction();
    }, Qt::SingleShotConnection);
}

JobClassSelectController::~JobClassSelectController() noexcept {
    if (ui) {
        delete ui;
    }
}

void JobClassSelectController::create() {
    ui = new cdsem::UiJobClassSelectController;
    setContent(new QWidget);
    ui->init(getContent());
}

void JobClassSelectController::createAction() {
    listViewModel = new ListViewMainModel(ui->classListView);
    ui->classListView->replaceDataModel(listViewModel);
    ui->classListView->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(ui->classListView->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this,
            [this](const QItemSelection& selected, const QItemSelection& deselected) {
                Q_UNUSED(selected)
                Q_UNUSED(deselected)
                QModelIndexList index = ui->classListView->selectionModel()->selectedRows(0);
                if (index.isEmpty()) {
                    return;
                }
                ui->classSelectedTextField->setText(
                    reinterpret_cast<ListViewData*>(index[0].internalPointer())->getText());
            });
    connect(ui->okBtn->getQPushButton(), &QPushButton::clicked, this, [this]() {
        if (!okCallback) {
            changeCloseStatusOnce(HW_CLOSE);
            close();
        }
        QModelIndexList index = ui->classListView->selectionModel()->selectedRows(0);
        if (index.isEmpty()) {
            return;
        }
        if (okCallback(*reinterpret_cast<ListViewMainData*>(index[0].internalPointer()))) {
            changeCloseStatusOnce(HW_OK);
            close();
        }
    });
}

void JobClassSelectController::loadClassList() {
    listViewModel->clear();
    bool succ = ACaller->push(this, [this](TSC) {
        int errCode = localLoadClassList(*listViewModel);
        if (errCode) {
            simpleMessageBox(this, "Error", "Error to load Class Data.");
        }
    });
    if (succ) {
        loadingBox(this, "Working", "Loading...");
    }
}
