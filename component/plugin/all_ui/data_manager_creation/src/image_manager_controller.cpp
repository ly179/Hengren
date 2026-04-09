#include "image_manager_controller.h"
#include "ui_image_manager_controller.h"
#include "list_view_main_data_model.h"
#include "private/common_data_loader.h"
#include "sem_global.h"
#include "class_entity.h"
#include "msr_entity.h"
#include "image_info_entity.h"
#include "service_manager.h"
#include "msr_service.h"
#include "image_info_service.h"
#include "msr_data_convert.h"
#include "msr_data_model.h"
#include "cde90_message_box.h"
#include "cde90_process_box.h"
#include  "loading_helper.h"

#define LOAD_CLASS_GID 1
#define LOAD_MSR_GID 2
#define LOAD_IMAGE_GID 3

struct ImageManagerDataHelper {
    ImageManagerController* d;

    ListViewMainModel* classListModel;
    ListViewMainModel* msrListModel;
    ListViewMainModel* imageListModel;

    int classTotalLimitCount = 5000;
    int fileTotalLimitCount = 10000;

    void refreshClassData() {
        int index = d->ui->classList->getLimitSelectedRow();
        d->getAsyncCaller()->stop(LOAD_CLASS_GID);
        d->getAsyncCaller()->push(d, LOAD_CLASS_GID, [this, index](TSC){
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
                    d->ui->msrCountTextField->setText(QString("0/%1").arg(fileTotalLimitCount));
                    d->ui->imageCountTextField->setText(QString("0/%1").arg(fileTotalLimitCount));
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
        d->getAsyncCaller()->stop(LOAD_MSR_GID);
        d->getAsyncCaller()->push(d, LOAD_MSR_GID, [this, classIndex, msrIndex, selectedId](TSC) mutable {
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
            bool needHideLoading = false;
            sem::AfterGc gc([this, &needHideLoading](){
                if (!needHideLoading) return;
                MainThreadInvoker::getInvoker().runLater([this](){
                    d->ui->processPane->hide();
                    d->ui->processPane->getLoadingHelper().block();
                });
            });
            MsrSearchDTO param;
            param.setClassId(classList[classIndex].getId());
            param.setInvalid(0);
            MainThreadInvoker::getInvoker().runLater([this](){
                d->ui->processPane->show();
                d->ui->processPane->getLoadingHelper().exec();
            });
            QList<ListViewMainData> & list = msrListModel->getDataList();
            if (selectedId < 0 && msrIndex >= 0) {
                if (msrIndex < list.size()) {
                    selectedId = list[msrIndex].getId();
                }
            }
            FileCommonResult result = refreshMSR(param, selectedId);
            if (result.errCode) {
                needHideLoading = true;
                simpleMessageBox(d, "Error", result.errMsg);
                return;
            }
            if (result.list.isEmpty()) {
                needHideLoading = true;
                msrListModel->clear();
                MainThreadInvoker::getInvoker().runLater([this](){
                    d->ui->msrCountTextField->setText(QString("0/%1").arg(fileTotalLimitCount));
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
                    d->ui->msrCountTextField->setText(QString("%1/%2").arg(size).arg(fileTotalLimitCount));
                    refreshImageData();
                });
            } else {
                MainThreadInvoker::getInvoker().runLater([this, size](){
                    d->ui->msrCountTextField->setText(QString("%1/%2").arg(size).arg(fileTotalLimitCount));
                    refreshImageData();
                });
            }
        });
    }

    void refreshImageData(quint64 selectedId = -1) {
        int msrIndex = d->ui->msrList->getLimitSelectedRow();
        if (msrIndex < 0) {
            d->getAsyncCaller()->push(d, [this](TSC) {
                imageListModel->clear();
            });
            d->ui->imageCountTextField->getQLineEdit()->clear();
            d->ui->processPane->hide();
            d->ui->processPane->getLoadingHelper().block();
            return;
        }
        int imageIndex = d->ui->imageList->getLimitSelectedRow();
        d->getAsyncCaller()->stop(LOAD_IMAGE_GID);
        d->getAsyncCaller()->push(d, LOAD_IMAGE_GID, [this, msrIndex, imageIndex, selectedId](TSC cmd) mutable {
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
            quint64 msrId = 0;
            {
                QList<ListViewMainData> & list = msrListModel->getDataList();
                if (msrIndex < list.size()) {
                    msrId = list[msrIndex].getId();
                }
            }
            if (msrId <= 0) return;
            QList<ListViewMainData>& list = imageListModel->getDataList();
            if (selectedId <= 0 && imageIndex >= 0 && imageIndex < list.size()) {
                selectedId = list[imageIndex].getId();
            }
            MsrService* msrService = ServiceManager::defaultService<MsrService>();
            int errCode;
            MsrEntity entity = msrService->getById(msrId, &errCode);
            if (errCode) {
                simpleMessageBox(d, "Error", "Error to load MSR data.");
                return;
            }
            if (cmd.isStop()) return;
            MSR_Chip msrData;
            errCode = msrModeDataFromEntity(entity.getMsrDetail(), msrData);
            if (errCode) {
                simpleMessageBox(d, "Error", "Error to load MSR data, MSR data format error.");
                return;
            }
            QList<quint64> imageIds;
            for (MSR_AP_Chip& ap : msrData.apList) {
                if (ap.apPatternResult.imageId > 0) imageIds.append(ap.apPatternResult.imageId);
            }
            for (MSR_MP_Chip& mp : msrData.mpList) {
                for (auto it = mp.validIndices.begin(); it != mp.validIndices.end(); ++ it) {
                    if (it.value() < 0 || it.value() >= mp.list.size()) {
                        continue;
                    }
                    MSR_MP_Data& d = mp.list[it.value()];
                    for (MSR_PatternResult& addressResult : d.addressPatternResults) {
                        if (addressResult.imageId > 0) imageIds.append(addressResult.imageId);
                    }
                    if (d.mpPatternResult.imageId > 0) imageIds.append(d.mpPatternResult.imageId);
                    if (d.measureImageId > 0) imageIds.append(d.measureImageId);
                }
            }
            if (imageIds.isEmpty()) {
                imageListModel->clear();
                MainThreadInvoker::getInvoker().runLater([this](){
                    MainThreadInvoker::getInvoker().runLater([this](){
                        d->ui->imageCountTextField->setText(QString("0/%1").arg(fileTotalLimitCount));
                    });
                });
                return;
            }

            ImageInfoService* imageService = ServiceManager::defaultService<ImageInfoService>();
            QList<ImageInfoEntity> imageList = imageService->getSimpleByIds(imageIds, &errCode);
            if (errCode) {
                simpleMessageBox(d, "Error", "Error to load Image List.");
                return;
            }
            if (imageList.isEmpty()) {
                imageListModel->clear();
                MainThreadInvoker::getInvoker().runLater([this](){
                    MainThreadInvoker::getInvoker().runLater([this](){
                        d->ui->imageCountTextField->setText(QString("0/%1").arg(fileTotalLimitCount));
                    });
                });
                return;
            }
            imageListModel->resizeDataList(imageList.size());
            int selectedIndex = -1;
            for (int i = 0; i < imageList.size(); ++ i) {
                ImageInfoEntity& entity = imageList[i];
                ListViewMainData& iData = list[i];
                iData.setId(entity.getId());
                iData.setText(entity.getImageName());
                if (selectedIndex == -1 && selectedId > 0 && entity.getId() == selectedId) {
                    selectedIndex = i;
                }
            }
            imageListModel->notifyDataChanged(0, list.size() - 1);
            int size = imageList.size();
            if (selectedIndex >= 0) {
                MainThreadInvoker::getInvoker().runLater([this, size, selectedIndex](){
                    d->ui->imageList->selectRow(selectedIndex);
                    d->ui->imageCountTextField->setText(QString("%1/%2").arg(size).arg(fileTotalLimitCount));
                });
            } else {
                MainThreadInvoker::getInvoker().runLater([this, size](){
                    d->ui->imageCountTextField->setText(QString("%1/%2").arg(size).arg(fileTotalLimitCount));
                });
            }
        });
    }
};

ImageManagerController::ImageManagerController(QWidget* parent) : RelationImageManager(parent) {
    //如果你没有异步任务需求，请将connect清理掉，保留create和createAction函数
    connect(this, &ImageManagerController::asyncCallerChanged, this, [this](){
        create();
        createAction();
    }, Qt::SingleShotConnection);
}

ImageManagerController::~ImageManagerController() noexcept {
    if (dataHelper) delete dataHelper;
    if (ui) {
        delete ui;
    }
}

void ImageManagerController::create() {
    dataHelper = new ImageManagerDataHelper;
    dataHelper->d = this;
    QWidget* content = new QWidget;
    setContent(content);
    ui = new cdsem::UiImageManagerController;
    ui->init(content);
}

void ImageManagerController::createAction() {
    dataHelper->classListModel = new ListViewMainModel(ui->classList);
    ui->classList->replaceDataModel(dataHelper->classListModel);
    dataHelper->msrListModel = new ListViewMainModel(ui->msrList);
    ui->msrList->replaceDataModel(dataHelper->msrListModel);
    dataHelper->imageListModel = new ListViewMainModel(ui->imageList);
    ui->imageList->replaceDataModel(dataHelper->imageListModel);

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
        dataHelper->refreshImageData();
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

    connect(ui->imageList->selectionModel(), &QItemSelectionModel::selectionChanged, this, [this](const QItemSelection &selected, const QItemSelection &deselected){
        int row = ui->imageList->getLimitSelectedRow();
        if (row < 0) {
            ui->selectedImageTextField->getQLineEdit()->clear();
            return;
        }
        ACaller->push(this, [this, row](TSC){
            QList<ListViewMainData> & list = dataHelper->imageListModel->getDataList();
            if (row >= list.size()) return;
            const QString& text = list[row].getText();
            MainThreadInvoker::getInvoker().runLater([this, text](){
                ui->selectedImageTextField->setText(text);
            });
        });
    });

    connect(ui->imageList, &QListView::doubleClicked, this, [this](const QModelIndex &index){
        int i = index.row();
        if (i < 0) return;
        ACaller->push(this, [this, i](TSC){
            QList<ListViewMainData>& list = dataHelper->imageListModel->getDataList();
            if (i >= list.size()) return;
            QList<cdsem::ImageViewListData> imageList;
            imageList.reserve(list.size());
            cdsem::ImageViewListData p;
            for (ListViewMainData& d : list) {
                p.imageId = d.getId();
                imageList.append(p);
            }
            emit openImageView(imageList, i);
        });
    });

    connect(ui->exitAct, &QAction::triggered, this, [this](){
        close();
    });
    connect(ui->reloadAct, &QAction::triggered, this, [this](){
        dataHelper->refreshClassData();
    });
}
