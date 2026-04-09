#include "template_selector_controller.h"
#include "cde90_message_box.h"
#include "cde90_process_box.h"
#include "idp_ap_add_reg_helper.h"
#include "image_info_service.h"
#include "service_manager.h"
#include "template_info_model_convert.h"
#include "template_info_service.h"
#include "ui_template_selector_controller.h"
#include "view_data_mode.h"
constexpr int loadGid = 1;

TemplateSelectorController::TemplateSelectorController(QWidget* parent) : CDE90Dialog(parent) {
    //如果你没有异步任务需求，请将connect清理掉，保留create和createAction函数
    connect(this, &TemplateSelectorController::asyncCallerChanged, this, [this](){
        create();
        createAction();
    }, Qt::SingleShotConnection);
}

TemplateSelectorController::~TemplateSelectorController() noexcept {
    if (ui) {
        delete ui;
    }
}

void TemplateSelectorController::closeEvent(
    QCloseEvent* event) {
    CDE90Dialog::closeEvent(event);
    if (event->isAccepted()) {
        ui->commonTextField->getQLineEdit()->clear();
        ui->canvas->setText("", false);
        ui->canvas->clearImage();
    }
}

void TemplateSelectorController::create() {
    ui = new cdsem::UiTemplateSelectorController;
    setContent(new QWidget);
    ui->init(getContent());
}

void TemplateSelectorController::createAction() {
    bindCloseBtn(ui->cancelBtn->getQPushButton());
    listModel = new DataListViewModel;
    ui->listView->replaceDataModel(listModel);

    connect(ui->okBtn->getQPushButton(), &QPushButton::clicked, this, [this]() {
        if (!okCallback) {
            return;
        }
        QModelIndexList list = ui->listView->selectionModel()->selectedRows(0);
        if (list.isEmpty()) {
            simpleMessageBox(this, "Info", "Please select exactly one file.");
            return;
        }
        if (list.size() > 1) {
            return;
        }
        ListViewData* data = reinterpret_cast<ListViewData*>(list[0].internalPointer());
        if (!data) {
            simpleMessageBox(this, "Eroor", "No data selected.");
            return;
        }
        if (okCallback(*data)) {
            changeCloseStatusOnce(HW_OK);
            close();
        }
    });

    connect(ui->listView->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this,
            [this](const QItemSelection& selected, const QItemSelection& deselected) {
                Q_UNUSED(selected)
                Q_UNUSED(deselected)
                QModelIndexList list = ui->listView->selectionModel()->selectedRows(0);
                if (list.isEmpty()) {
                    return;
                }
                if (list.size() > 1) {
                    return;
                }
                ListViewData* data = reinterpret_cast<ListViewData*>(list[0].internalPointer());
                if (!data) {
                    return;
                }
                ui->selectedTextField->setText(data->getText());
                loadData(data->getId());
            });
}

void TemplateSelectorController::loadData(
    quint64 id) {
    ACaller->stop(loadGid);
    int type = this->type;
    bool succ = ACaller->push(this, loadGid, [this, id, type](TSC cmd) {
        if (cmd.isStop()) {
            return;
        }
        bool loadCommentSucc = false;
        bool hasImage = false;
        sem::AfterGc gc([this, &loadCommentSucc, &hasImage, id, &cmd]() {
            MainThreadInvoker::getInvoker().blockRun([this, hasImage, loadCommentSucc, id, &cmd]() {
                //cmd.isStop此处使用时要100%确定停止该任务的是自己停止自己才可以添加该判定条件，没有其他对象过来终止该任务的。
                //加上该判定条件显示更友好，可以让图片加载时不会丢失Loading字样
                if (this->dataId != id || cmd.isStop()) {
                    return;
                }
                if (!loadCommentSucc) {
                    ui->commonTextField->getQLineEdit()->clear();
                }
                if (!hasImage)
                    ui->canvas->setText("");
            });
        });
        TemplateInfoService* templateService
            = ServiceManager::instance()->getDefaultService<TemplateInfoService>();
        int errCode;
        TemplateInfoEntity entity = templateService->getById(id, &errCode);
        if (errCode) {
            simpleMessageBox(this, "Error", "Load data error.");
            return;
        }
        if (entity.getTemplateType() != type) {
            return;
        }
        MainThreadInvoker::getInvoker().blockRun([&entity, &id, this]() {
            if (this->dataId == id)
                ui->commonTextField->setText(entity.getTemplateComment());
        });
        if (cmd.isStop()) {
            return;
        }
        loadCommentSucc = true;
        //TODO 待优化，后期要加关联表专门关联各个ID之间的关系
        QRect rect;
        quint64 imageId = 0;
        if (type == TEMP_ALIGN) {
            ApTemplateInfoModel model;
            templateInfoModelFromEntity(entity, model);
            if (model.data.patternRegParam.imageId == 0) {
                return;
            }
            imageId = model.data.patternRegParam.imageId;
            rect = model.data.patternRegParam.infoParam.modelArea;
        } else if (type == TEMP_MS) {
            MultiAddressTemplateInfoModel model;
            templateInfoModelFromEntity(entity, model);
            for (MultiAddressDetailTemplateParam& address: model.data.addressList) {
                if (address.patternRegParam.imageId > 0) {
                    imageId = address.patternRegParam.imageId;
                    rect = address.patternRegParam.infoParam.modelArea;
                    break;
                }
            }
            if (imageId == 0) return;
        } else if (type == TEMP_MP) {
            MpTemplateInfoModel model;
            templateInfoModelFromEntity(entity, model);
            if (model.data.patternRegParam.imageId == 0) {
                return;
            }
            imageId = model.data.patternRegParam.imageId;
            // rect = QRect();
        } else {
            return;
        }
        ImageInfoService* imageService = ServiceManager::instance()
                                             ->getDefaultService<ImageInfoService>();
        ImageInfoEntity imageEntity = imageService->getById(imageId, &errCode);
        if (cmd.isStop()) {
            return;
        }
        if (errCode) {
            simpleMessageBox(this, "Error", "Load image data error.");
            return;
        }
        if (entity.getId() == 0) {
            simpleMessageBox(this, "Error", "Lost image data.");
            return;
        }
        QImage image(imageEntity.getWidth(),
                                                imageEntity.getHeight(),
                                                QImage::Format(imageEntity.getImageFormat()));
        if (imageEntity.getImageData().length() != image.sizeInBytes()) {
            simpleMessageBox(this, "Error", "Load image data error, data format error.");
            return;
        }
        std::memcpy(image.bits(),
                    imageEntity.getImageData().constData(),
                    imageEntity.getImageData().length());

        MainThreadInvoker::getInvoker().blockRun([this, &id, &rect, &image, &hasImage]() {
            if (this->dataId != id) {
                return;
            }
            hasImage = true;
            idpToCreateAndDrawThumbnailCanvas(this, canvasPainter, ui->canvas, image, rect);
        });
    });
    if (succ) {
        dataId = id;
        ui->commonTextField->setText("Loading");
        ui->canvas->setText("Loading", false);
        ui->canvas->clearImage();
    }
}

void TemplateSelectorController::setTitle(const QString& text) {
    ui->titleTextField->setText(text);
}

void TemplateSelectorController::load(
    const TemplateInfoSearchDTO& param) {
    type = param.getTemplateType();
    listModel->clear();
    bool succ = ACaller->push(this, [this, param](TSC) {
        TemplateInfoService* service = ServiceManager::instance()
                                           ->getDefaultService<TemplateInfoService>();
        int errCode;
        QList<TemplateInfoEntity> list = service->searchList(param, &errCode);
        if (errCode) {
            simpleMessageBox(this, "Error", "Load data list error.");
            return;
        }
        if (list.isEmpty()) {
            return;
        }
        MainThreadInvoker::getInvoker().blockRun([this, &list]() {
            QList<ListViewData>& viewList = listModel->getDataList();
            viewList.resize(list.size());
            for (int i = 0; i < list.size(); ++i) {
                ListViewData& data = viewList[i];
                const TemplateInfoEntity& e = list[i];
                data.setId(e.getId());
                data.setText(e.getTemplateName());
            }
            listModel->resizeViewList(viewList.size());
        });
    });
    if (succ) {
        loadingBox(this, "Working", "Loading...");
    }
}
