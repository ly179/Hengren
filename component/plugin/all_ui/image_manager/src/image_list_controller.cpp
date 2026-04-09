#include "image_list_controller.h"
#include "ui_image_list_controller.h"
#include "image_file_temp_handler.h"
#include "flow_layout.h"
#include "sem_thumbnail_widget.h"
#include <QVBoxLayout>
#include <QCloseEvent>
#include "sem_global.h"
#include "service_manager.h"
#include "image_info_service.h"
#include "cde90_widget.h"
#include <QPainter>
#include <QFileInfo>
#include <QScrollBar>
#include <QTimer>
#include "bit_set.h"
#include "cdsem_number_validator_helper.h"
#include "image_manager_relation.h"
#include  "loading_helper.h"

#define F_SIZE 100
#define LIST_GID 1
#define LOAD_GID 2


class ImageListItemCanvas : public SemThumbnailWidget {
private:
    int lineWeight = 2;
    bool drawBorderLine = false;
    QList<QPointF> points;
    void paintEvent(QPaintEvent* event) override {
        SemThumbnailWidget::paintEvent(event);
        if (!drawBorderLine) return;
        this->refreshMapFromRoot(true);
        this->painterBegin();
        int w = lineWeight * getDevicePixelRatioCache();
        this->painter->setPen(QPen(Qt::black, w));
        if (points.isEmpty()) {
            points.resize(4);
        }
        points[0] = QPointF(sem::getLeftTopViewPoint(0, w), sem::getRightDownViewPoint(canvasHeightInPainting(), w));
        points[1] = QPointF(sem::getLeftTopViewPoint(0, w), sem::getLeftTopViewPoint(0, w));
        points[2] = QPointF(sem::getRightDownViewPoint(canvasWidthInPainting(), w), sem::getLeftTopViewPoint(0, w));
        points[3] = QPointF(sem::getRightDownViewPoint(canvasWidthInPainting(), w), sem::getRightDownViewPoint(canvasHeightInPainting(), w));
        this->painter->drawPolyline(points);

        this->painter->end();
    }
public:
    explicit ImageListItemCanvas(QWidget* parent = nullptr) : SemThumbnailWidget(parent) {}

    void setDrawBorderLine(bool b) {
        if (drawBorderLine == b) return;
        drawBorderLine = b;
        update();
    }
};

struct ImageLabelControl {
    ImageListItemCanvas* canvas;
    CDE90Label* label;

    QWidget* cCotent;
};

ImageLabelControl appendImageItem(QWidget* content) {
    CDE90Widget* vContent = new CDE90Widget(content);
    QVBoxLayout* vLayout = new QVBoxLayout(vContent);
    vLayout->setSpacing(0);
    vLayout->setContentsMargins(0, 0, 0, 0);
    vContent->setLayout(vLayout);

    ImageListItemCanvas* canvas = new ImageListItemCanvas(vContent);
    canvas->setFixedSize(QSize(F_SIZE, F_SIZE));
    canvas->setDefaultBgColor(QColor(180, 180, 180), false);
    CDE90Label* fileLabel = new CDE90Label(vContent);
    fileLabel->setFixedWidth(F_SIZE);
    fileLabel->setAlignment(Qt::AlignCenter);
    fileLabel->setTextElideMode(Qt::ElideRight);
    fileLabel->setTextWithElide("Loading Image");
    fileLabel->setContentsMargins(0, 0, 0, 0);
    vLayout->addWidget(canvas);
    vLayout->addWidget(fileLabel);

    canvas->setAttribute(Qt::WA_TransparentForMouseEvents);
    fileLabel->setAttribute(Qt::WA_TransparentForMouseEvents);

    FlowLayout* layout = static_cast<FlowLayout*>(content->layout());
    layout->addWidget(vContent);

    return ImageLabelControl{canvas, fileLabel, vContent};
}

//缩小图像体积以节省内存。
void scaleImage(cdsem::TmpImageData& data, QImage& image) {
    if (data.size.width() <= F_SIZE && data.size.height() <= F_SIZE) {
        image = QImage(reinterpret_cast<const uchar*>(data.data.constData()), data.size.width(), data.size.height(), QImage::Format_Grayscale8);
        return;
    }
    QImage tmp(reinterpret_cast<const uchar*>(data.data.constData()), data.size.width(), data.size.height(), QImage::Format_Grayscale8);
    if (data.size.width() > data.size.height()) {
        image = tmp.scaled(QSize(F_SIZE, qreal(data.size.height()) * F_SIZE / data.size.width()));
    } else {
        image = tmp.scaled(QSize(qreal(data.size.width()) * F_SIZE / data.size.height(), F_SIZE));
    }
}


void clearImageList(QWidget* content) {
    QObjectList list = content->children();
    if (list.isEmpty()) return;
    FlowLayout* layout = static_cast<FlowLayout*>(content->layout());
    for (QObject* obj : list) {
        QWidget* w = qobject_cast<QWidget*>(obj);
        if (!w) continue;
        layout->removeWidget(w);
        w->deleteLater();
    }
}

class WidgetMouseEventFilter;
struct ImageListControllerDataHelper {
    QTimer delayLoadTimer;
    QList<ImageLabelControl> controls;
    QList<cdsem::ImageViewListData> imageList;
    BitSet loadedChecker;
    std::function<void(int, QWidget*)> eventFilterInstaller;
    ImageListController* d;
    CDSemIntValidator* pageValidator = nullptr;

    quint64 version;

    int selectedIndex = -1;
    int page = 1;
    int pageTotal = 1;
    const int pageSize = 24;

    bool sendedToViwer = false;

    inline void setFileNameLabelText(const QString& text) {
        d->ui->detailLabel->setTextWithElide(text);
    }

    void setCanvasText(const QString& text, int i) {
        if (!MainThreadInvoker::getInvoker().isMainThread()) {
            MainThreadInvoker::getInvoker().runLater([this, i, text](){
                setCanvasText(text, i);
            });
            return;
        }
        if (i < controls.size()) {
            controls[i].canvas->setText(text);
        }
    }

    //传入的是当前页UI元素index，而不是分页前全体数据的index
    void loadRangeImage(const QList<int>& indices) {
        d->getAsyncCaller()->push(d, LOAD_GID, [this, indices](TSC cmd){
            if (cmd.isStop()) return;
            sem::AfterGc gc([this](){
                MainThreadInvoker::getInvoker().runLater([this](){
                    d->ui->processPane->getLoadingHelper().block();
                    d->ui->processPane->hide();
                });
            });
            MainThreadInvoker::getInvoker().runLater([this](){
                d->ui->processPane->show();
                d->ui->processPane->getLoadingHelper().exec();
            });
            ImageInfoService* service = ServiceManager::defaultService<ImageInfoService>();
            int errCode;
            QString name;
            cdsem::TmpImageData imageData;
            int dIndex, i;
            for (auto it = indices.begin(); it != indices.end(); ++ it) {
                i = *it;
                dIndex = i + (pageSize * (page - 1));
                if (cmd.isStop()) break;
                MainThreadInvoker::getInvoker().runLater([this, i](){
                    loadedChecker.set(i);
                });

                if (dIndex < 0 || dIndex >= imageList.size()) continue;
                const cdsem::ImageViewListData& data = imageList[dIndex];
                if (data.path.length() > 0) {
                    if (!cdsem::readImage(imageData, data.path)) {
                        setCanvasText("Error", i);
                        continue;
                    }
                    QFileInfo info(data.path);
                    name = info.baseName();
                } else if (data.imageId > 0) {
                    ImageInfoEntity entity = service->getById(data.imageId, &errCode);
                    if (errCode) {
                        setCanvasText("Error", i);
                        continue;
                    } else if (entity.getId() == 0) {
                        setCanvasText("Lost", i);
                        continue;
                    } else {
                        imageData.data = std::move(entity.getImageData());
                        imageData.size = QSize(entity.getWidth(), entity.getHeight());
                        imageData.format = entity.getImageFormat();
                        name = entity.getImageName();
                    }
                } else {
                    setCanvasText("Empty", i);
                    continue;
                }
                // QImage image;
                // scaleImage(imageData, image);
                MainThreadInvoker::getInvoker().runLater([this, imageData, name, i](){
                    if (i < controls.size()) {
                        QImage image(reinterpret_cast<const uchar*>(imageData.data.constData()), imageData.size.width(), imageData.size.height(), QImage::Format(imageData.format));
                        controls[i].canvas->copyImage(image);
                        controls[i].label->setTextWithElide(name);
                        if (selectedIndex == i) {
                            d->ui->detailLabel->setTextWithElide(name);
                        }
                    }
                });
            }
        });
    }

    //实际上可以通过计算来减小不必要的for循环。
    //但是我感觉没必要。
    //for循环版本：
    void checkWidgetVisible() {
        if (delayLoadTimer.isActive() || controls.isEmpty() || loadedChecker.pCount() >= controls.size()) return;
        d->getAsyncCaller()->stop(LOAD_GID);
        d->getAsyncCaller()->push(d, LOAD_GID, [this](TSC cmd){
            if (cmd.isStop()) return;
            MainThreadInvoker::getInvoker().runLater([this](){
                QRect area = d->ui->imageListContent->geometry();
                QRect viewPort = QRect(QPoint(0, 0), d->ui->imageScrollArea->viewport()->size());
                QRect item;
                //保存的是当前页UI元素index，而不是分页前全体数据的index
                QList<int> list;
                list.reserve(pageSize);
                for (int i = 0; i < controls.size(); ++ i) {
                    if (loadedChecker.exists(i)) continue;
                    ImageLabelControl& c = controls[i];
                    item = c.cCotent->geometry();
                    item.moveTopLeft(item.topLeft() + area.topLeft());
                    item.setLeft(item.left() + 4);
                    item.setTop(item.top() + 4);
                    item.setRight(item.right() - 4);
                    item.setBottom(item.bottom() - 4);
                    if (item.intersects(viewPort)) {
                        list.append(i);
                    }
                }
                loadRangeImage(list);
            });
        });
    }

    void reloadBeforeClear() {
        clearImageList(d->ui->imageListContent);
        controls.clear();
        loadedChecker.clear();
        selectedIndex = -1;
        d->ui->detailLabel->clear();
    }

    void reloadCurrentPage() {
        if (delayLoadTimer.isActive()) {
            delayLoadTimer.stop();
        }
        reloadBeforeClear();
        if (imageList.isEmpty()) return;
        ImageLabelControl control;
        int end = std::min(int(imageList.size()), pageSize * page);
        int index = 0;
        for (int i = (page - 1) * pageSize ; i < end; ++ i) {
            control = appendImageItem(d->ui->imageListContent);
            eventFilterInstaller(index, control.cCotent);
            controls.append(control);
            control.canvas->setText("Loading");
            if (imageList[i].path.length() > 0) {
                QFileInfo info(imageList[i].path);
                control.label->setTextWithElide(info.baseName());
            }
            ++ index;
        }
        d->ui->imageListContent->adjustSize();
        delayLoadTimer.start();
        d->ui->processPane->show();
        d->ui->processPane->getLoadingHelper().exec();
    }
};

class WidgetResizeEventFilter : public QObject {
private:
    ImageListControllerDataHelper* dataHelper;
protected:
    bool eventFilter(QObject* watch, QEvent* event) override {
        Q_UNUSED(watch)
        if (event->type() == QEvent::Resize) {
            dataHelper->checkWidgetVisible();
        }
        return false;
    }
public:
    explicit WidgetResizeEventFilter(ImageListControllerDataHelper* dataHelper, QObject* parent = nullptr) : QObject(parent), dataHelper(dataHelper) {}
    ~WidgetResizeEventFilter() noexcept {}
};

class WidgetMouseEventFilter : public QObject {
private:
    ImageListControllerDataHelper* dataHelper;
    int index;

    void doCheck() {
        if (dataHelper->selectedIndex >= 0 && dataHelper->selectedIndex < dataHelper->controls.size()) {
            dataHelper->controls[dataHelper->selectedIndex].label->setStyleSheet("");
            dataHelper->controls[dataHelper->selectedIndex].canvas->setDrawBorderLine(false);
        }
        dataHelper->selectedIndex = index;
        dataHelper->controls[index].label->setStyleSheet("QLabel {background-color: #000; color: #FFF;}");
        dataHelper->controls[index].canvas->setDrawBorderLine(true);
        dataHelper->setFileNameLabelText(dataHelper->controls[index].label->fullText());
    }

protected:
    bool eventFilter(QObject* watch, QEvent* event) override {
        Q_UNUSED(watch)
        if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonDblClick) {
            QMouseEvent* mEvent = static_cast<QMouseEvent*>(event);
            if (mEvent->button() == Qt::LeftButton) {
                doCheck();
                if (event->type() == QEvent::MouseButtonDblClick) {
                    if (index < dataHelper->controls.size()) {
                        int no = index + dataHelper->pageSize * (dataHelper->page - 1);
                        if (dataHelper->sendedToViwer) {
                            bool succ;
                            image_manager::openImageViewController(no, 0, dataHelper->version, &succ);
                            if (succ) return true;
                        }
                        image_manager::openImageViewController(dataHelper->imageList, no, 0, &dataHelper->version);
                        if (!dataHelper->sendedToViwer)
                            dataHelper->sendedToViwer = true;
                    }
                }
                return true;
            }
        }
        return false;
    }
public:
    explicit WidgetMouseEventFilter(ImageListControllerDataHelper* dataHelper, int index, QObject* parent = nullptr) : QObject(parent), dataHelper(dataHelper), index(index) {}
    ~WidgetMouseEventFilter() noexcept {}
};

ImageListController::ImageListController(QWidget* parent) : RelationImageListController(parent) {
    //如果你没有异步任务需求，请将connect清理掉，保留create和createAction函数
    connect(this, &ImageListController::asyncCallerChanged, this, [this](){
        create();
        createAction();
    }, Qt::SingleShotConnection);
}

ImageListController::~ImageListController() noexcept {
    if (ui) {
        delete ui;
    }
    if (dataHelper) delete dataHelper;
}

void ImageListController::closeEvent(QCloseEvent* event) {
    CDE90Window::closeEvent(event);
    if (event->isAccepted()) {
        clear();
    }
}

void ImageListController::create() {
    QWidget* content = new QWidget;
    setContent(content);
    ui = new cdsem::UiImageListController;
    ui->init(content);
    ui->imageScrollArea->setMinimumSize(QSize(428, 460));

    dataHelper = new ImageListControllerDataHelper;
    dataHelper->d = this;
    dataHelper->loadedChecker.reserve(dataHelper->pageSize);
    dataHelper->eventFilterInstaller = [this](int i, QWidget* w){
        w->installEventFilter(new WidgetMouseEventFilter(dataHelper, i, w));
    };
}

void ImageListController::createAction() {
    dataHelper->pageValidator = static_cast<CDSemIntValidator*>(CDSemNumberValidatorHelper::range(1, 1).put(ui->pageTextField->getQLineEdit()).autoFixMode().getValidator());
    dataHelper->delayLoadTimer.setSingleShot(true);
    dataHelper->delayLoadTimer.setInterval(100);
    ui->imageScrollArea->installEventFilter(new WidgetResizeEventFilter(dataHelper, ui->imageScrollArea));
    bindCloseBtn(ui->closeBtn->getQPushButton());

    connect(ui->imageScrollArea->verticalScrollBar(), &QScrollBar::valueChanged, this, [this](int){
        dataHelper->checkWidgetVisible();
    });
    connect(ui->imageScrollArea->horizontalScrollBar(), &QScrollBar::valueChanged, this, [this](int){
        dataHelper->checkWidgetVisible();
    });

    connect(ui->prevBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        if (dataHelper->page < 1) return;
        -- dataHelper->page;
        if (!ui->nextBtn->getQPushButton()->isEnabled() && dataHelper->page < dataHelper->pageTotal) {
            ui->nextBtn->getQPushButton()->setDisabled(false);
        }
        if (dataHelper->page == 1) {
            ui->prevBtn->getQPushButton()->setDisabled(true);
        }
        ui->pageTextField->setText(dataHelper->page);
        dataHelper->reloadBeforeClear();
        ACaller->stop(LOAD_GID);
        ACaller->push(this, [this](TSC){
            MainThreadInvoker::getInvoker().runLater([this](){
                dataHelper->reloadCurrentPage();
            });
        });
    });
    connect(ui->nextBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        if (dataHelper->page >= dataHelper->pageTotal) return;
        ++ dataHelper->page;
        if (!ui->prevBtn->getQPushButton()->isEnabled() && dataHelper->page > 1) {
            ui->prevBtn->getQPushButton()->setDisabled(false);
        }
        if (dataHelper->page == dataHelper->pageTotal) {
            ui->nextBtn->getQPushButton()->setDisabled(true);
        }
        ui->pageTextField->setText(dataHelper->page);
        dataHelper->reloadBeforeClear();
        ACaller->stop(LOAD_GID);
        ACaller->push(this, [this](TSC){
            MainThreadInvoker::getInvoker().runLater([this](){
                dataHelper->reloadCurrentPage();
            });
        });
    });

    connect(&dataHelper->delayLoadTimer, &QTimer::timeout, this, [this](){
        dataHelper->checkWidgetVisible();
    });

    connect(ui->pageTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        bool ok;
        int p = ui->pageTextField->text().toInt(&ok);
        if (!ok || p < 1 || p > dataHelper->pageTotal) {
            ui->pageTextField->setText(dataHelper->page);
            return;
        }
        dataHelper->page = p;
        if (p == 1) {
            if (ui->prevBtn->getQPushButton()->isEnabled()) {
                ui->prevBtn->getQPushButton()->setDisabled(true);
            }
        }
        if (p < dataHelper->pageTotal && !ui->nextBtn->getQPushButton()->isEnabled()) {
            ui->nextBtn->getQPushButton()->setDisabled(false);
        }
        if (p == dataHelper->pageTotal) {
            if (ui->nextBtn->getQPushButton()->isEnabled()) {
                ui->nextBtn->getQPushButton()->setDisabled(true);
            }
        }
        if (p > 1 && !ui->prevBtn->getQPushButton()->isEnabled()) {
            ui->prevBtn->getQPushButton()->setDisabled(false);
        }
        dataHelper->reloadBeforeClear();
        ACaller->stop(LOAD_GID);
        ACaller->push(this, [this](TSC){
            MainThreadInvoker::getInvoker().runLater([this](){
                dataHelper->reloadCurrentPage();
            });
        });
    });
}

void ImageListController::setImageList(const QList<cdsem::ImageViewListData>& list) {
    dataHelper->reloadBeforeClear();
    ACaller->stop(LIST_GID);
    ACaller->stop(LOAD_GID);
    ACaller->push(this, LIST_GID, [this, list](TSC cmd){
        if (cmd.isStop()) return;
        MainThreadInvoker::getInvoker().runLater([this, list](){
            dataHelper->page = 1;
            dataHelper->imageList = list;
            dataHelper->pageTotal = (list.size() - 1) / dataHelper->pageSize + 1;
            dataHelper->sendedToViwer = false;
            ui->prevBtn->getQPushButton()->setDisabled(true);
            ui->nextBtn->getQPushButton()->setDisabled(dataHelper->pageTotal == 1);
            ui->pageTextField->setText(dataHelper->page);
            ui->totalLabel->setText(QString::number(dataHelper->pageTotal));
            dataHelper->reloadCurrentPage();
            dataHelper->pageValidator->setTop(dataHelper->pageTotal);
        });
    });
}

void ImageListController::clear() {
    ACaller->stop(LIST_GID);
    ACaller->stop(LOAD_GID);
    ACaller->push(this, [this](TSC){
        MainThreadInvoker::getInvoker().runLater([this](){
            clearImageList(ui->imageListContent);
            dataHelper->controls.clear();
            ui->processPane->getLoadingHelper().block();
            ui->processPane->hide();
        });
    });
}
