#include "msr_manager_controller.h"
#include "ui_msr_manager_controller.h"
#include "list_view_main_data_model.h"
#include "private/common_data_loader.h"
#include "sem_global.h"
#include "class_entity.h"
#include "msr_entity.h"
#include "cde90_message_box.h"
#include  "loading_helper.h"

#define LOAD_DATA_GID 1

struct MsrManagerDataHelper {

    MsrManagerController* d;
    ListViewMainModel* classListModel;
    ListViewMainModel* msrListModel;

    int classTotalLimitCount = 5000;
    int msrTotalLimitCount = 10000;

    void refreshClassData() {
        int index = d->ui->classList->getLimitSelectedRow();
        d->getAsyncCaller()->stop(LOAD_DATA_GID);
        d->getAsyncCaller()->push(d, LOAD_DATA_GID, [this, index](TSC){
            bool needHideLoading = false;
            MainThreadInvoker::getInvoker().runLater([this](){
                d->ui->processPane->show();
                d->ui->processPane->getLoadingHelper().exec();
            });
            sem::AfterGc gc([this, &needHideLoading](){
                if (!needHideLoading) return;
                MainThreadInvoker::getInvoker().runLater([this](){
                    d->ui->processPane->hide();
                    d->ui->processPane->getLoadingHelper().block();
                });
            });
            quint64 selectedId = -1;
            QList<ListViewMainData> & list = classListModel->getDataList();
            if (index >= 0) {
                if (index < list.size()) {
                    selectedId = list[index].getId();
                }
            }
            ClassSearchDTO param;
            FileCommonResult result = refreshClass(param, selectedId);
            if (result.errCode) {
                needHideLoading = true;
                simpleMessageBox(d, "Error", result.errMsg);
                MainThreadInvoker::getInvoker().runLater([this](){
                    d->ui->classCountTextField->getQLineEdit()->clear();
                });
                return;
            }
            if (result.list.isEmpty()) {
                needHideLoading = true;
                classListModel->clear();
                msrListModel->clear();
                MainThreadInvoker::getInvoker().runLater([this](){
                    d->ui->classCountTextField->setText(QString("0/%1").arg(classTotalLimitCount));
                    d->ui->msrCountTextField->setText(QString("0/%1").arg(msrTotalLimitCount));
                });
                return;
            }
            classListModel->resizeDataList(result.list.size());
            list = result.list;
            classListModel->notifyDataChanged(0, result.list.size() - 1);
            int size = list.size();
            if (result.selectedIndex >= 0) {
                int index = result.selectedIndex;
                MainThreadInvoker::getInvoker().runLater([this, index, size](){
                    d->ui->classList->selectRow(index);
                    d->ui->classCountTextField->setText(QString("%1/%2").arg(size).arg(classTotalLimitCount));
                    refreshMsrData();
                });
            } else {
                MainThreadInvoker::getInvoker().runLater([this, size](){
                    d->ui->classCountTextField->setText(QString("%1/%2").arg(size).arg(classTotalLimitCount));
                    refreshMsrData();
                });
            }
        });
    }

    void refreshMsrData(quint64 selectedId = -1) {
        int classIndex = d->ui->classList->getLimitSelectedRow();
        if (classIndex < 0) {
            d->getAsyncCaller()->push(d, [this](TSC) {
                msrListModel->clear();
            });
            d->ui->msrCountTextField->getQLineEdit()->clear();
            d->ui->processPane->hide();
            d->ui->processPane->getLoadingHelper().block();
            return;
        }
        int msrIndex = d->ui->msrList->getLimitSelectedRow();
        d->getAsyncCaller()->push(d, LOAD_DATA_GID, [this, classIndex, msrIndex, selectedId](TSC) mutable {
            QList<ListViewMainData>& classList = classListModel->getDataList();
            if (classIndex >= classList.size()) {
                msrListModel->clear();
                MainThreadInvoker::getInvoker().runLater([this](){
                    d->ui->msrCountTextField->getQLineEdit()->clear();
                    d->ui->processPane->hide();
                    d->ui->processPane->getLoadingHelper().block();
                });
                return;
            }
            MsrSearchDTO param;
            param.setClassId(classList[classIndex].getId());
            param.setInvalid(0);
            MainThreadInvoker::getInvoker().runLater([this](){
                d->ui->processPane->show();
                d->ui->processPane->getLoadingHelper().exec();
            });
            sem::AfterGc gc([this](){
                MainThreadInvoker::getInvoker().runLater([this](){
                    d->ui->processPane->hide();
                    d->ui->processPane->getLoadingHelper().block();
                });
            });
            QList<ListViewMainData> & list = msrListModel->getDataList();
            if (selectedId < 0 && msrIndex >= 0) {
                if (msrIndex < list.size()) {
                    selectedId = list[msrIndex].getId();
                }
            }
            FileCommonResult result = refreshMSR(param, selectedId);
            if (result.errCode) {
                simpleMessageBox(d, "Error", result.errMsg);
                return;
            }
            if (result.list.isEmpty()) {
                msrListModel->clear();
                MainThreadInvoker::getInvoker().runLater([this](){
                    d->ui->msrCountTextField->setText(QString("0/%1").arg(msrTotalLimitCount));
                });
                return;
            }
            msrListModel->resizeDataList(result.list.size());
            list = result.list;
            msrListModel->notifyDataChanged(0, result.list.size() - 1);
            int size = result.list.size();
            if (result.selectedIndex >= 0) {
                int index = result.selectedIndex;
                MainThreadInvoker::getInvoker().runLater([this, size, index](){
                    d->ui->msrList->selectRow(index);
                    d->ui->msrCountTextField->setText(QString("%1/%2").arg(size).arg(msrTotalLimitCount));
                });
            } else {
                MainThreadInvoker::getInvoker().runLater([this, size](){
                    d->ui->msrCountTextField->setText(QString("%1/%2").arg(size).arg(msrTotalLimitCount));
                });
            }
        });
    }
};

MsrManagerController::MsrManagerController(QWidget* parent) : RelationMSRManager(parent) {
    //如果你没有异步任务需求，请将connect清理掉，保留create和createAction函数
    connect(this, &MsrManagerController::asyncCallerChanged, this, [this](){
        create();
        createAction();
    }, Qt::SingleShotConnection);
}

MsrManagerController::~MsrManagerController() noexcept {
    if (dataHelper) delete dataHelper;
    if (ui) {
        delete ui;
    }
}

void MsrManagerController::create() {

    dataHelper = new MsrManagerDataHelper;
    dataHelper->d = this;

    QWidget* content = new QWidget;
    setContent(content);
    ui = new cdsem::UiMsrManagerController;
    ui->init(content);
}

void MsrManagerController::createAction() {
    dataHelper->classListModel = new ListViewMainModel(ui->classList);
    ui->classList->replaceDataModel(dataHelper->classListModel);
    dataHelper->msrListModel = new ListViewMainModel(ui->msrList);
    ui->msrList->replaceDataModel(dataHelper->msrListModel);

    dataHelper->refreshClassData();

    connect(ui->classList->selectionModel(), &QItemSelectionModel::selectionChanged, this, [this](const QItemSelection &selected, const QItemSelection &deselected){
        dataHelper->refreshMsrData();
        int row = ui->classList->getLimitSelectedRow();
        if (row < 0) {
            ui->selectedClassTextField->getQLineEdit()->clear();
            return;
        }
        ACaller->push(this, [this, row](TSC){
            QList<ListViewMainData> & list = dataHelper->classListModel->getDataList();
            if (row >= list.size()) return;
            const QString& text = list[row].getText();
            MainThreadInvoker::getInvoker().runLater([this, text](){
                ui->selectedClassTextField->setText(text);
            });
        });
    });

    connect(ui->msrList->selectionModel(), &QItemSelectionModel::selectionChanged, this, [this](const QItemSelection &selected, const QItemSelection &deselected){
        int row = ui->msrList->getLimitSelectedRow();
        if (row < 0) {
            ui->selectedMsrTextField->getQLineEdit()->clear();
            return;
        }
        ACaller->push(this, [this, row](TSC){
            QList<ListViewMainData> & list = dataHelper->msrListModel->getDataList();
            if (row >= list.size()) return;
            const QString& text = list[row].getText();
            MainThreadInvoker::getInvoker().runLater([this, text](){
                ui->selectedMsrTextField->setText(text);
            });
        });
    });

    connect(ui->msrList, &QListView::doubleClicked, this, [this](const QModelIndex &index){
        int i = index.row();
        if (i < 0) return;
        ACaller->push(this, [this, i](TSC){
            QList<ListViewMainData>& list = dataHelper->msrListModel->getDataList();
            if (i >= list.size()) return;
            emit openWorkSheet(list[i].getTopId());
        });
    });

    connect(ui->exitAct, &QAction::triggered, this, [this](){
        close();
    });
    connect(ui->reloadAct, &QAction::triggered, this, [this](){
        dataHelper->refreshClassData();
    });
}
