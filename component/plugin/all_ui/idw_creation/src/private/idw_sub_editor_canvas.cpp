#include "idw_sub_editor_canvas.h"
#include "screen_range_unit.h"
#include "idw_wafer_data_model.h"
#include "idw_view_data_model.h"
#include "sem_global.h"
#include "loading_helper.h"

struct IdwSubEditorCanvasDataHelper {
    RectUnitIndex* unitIndex;
    IDW_ShotChipData editData;
    IDW_WaferData waferData;
    QSet<qint32> selectedNumbers;

    //绘制线程画布大小
    QRect screenRange;
    //主线程画布大小，当主线程发现screen变化时，通知绘制线程来更新screen，线程安全的一种策略
    QRect ui_screenRange;

    SimplerRender render;
    AsyncCaller* caller;
    LoadingHelper* loadingHelper = nullptr;
    //汇总pixmap，多线程渲染结束后再次绘制到此map上。
    //此map必须加锁，在多线程绘制结束后，锁定与画布的update要互斥。
    SimplerRender mergeRender;
    QImage fastCacheImage;

    QPen crossDashLineStrongPen;
    QPen crossDashLineThinPen;
    QColor centerPointColor;
    QPen selectedPen;
    QBrush selectedBrush;

    QFont numberFont;

    //totalyPixmap在更新时，canvas的update不能访问，需要加锁来确认是否安全，但是不能加
    //系统级的重量级锁，这里使用轻量级的自旋锁
    std::atomic_bool mergePainterLock = false;
    std::atomic_bool fastCacheCopyLock = false;

    qreal lastDevicePixelRatio = 0;

    qint32 originX;
    qint32 originY;
    qint32 originX_Offset = 0;
    qint32 originY_Offset = 0;

    qint32 padding = 5;
    int fullyUpdateGid= - 1;
    int selectedUpdateGid= - 1;

    int lastNumber = -1;

    idw_edit_canvas::EditDataType dataType = idw_edit_canvas::SHOT;

    bool mousePressed = false;
};


IdwSubEditorCanvas::IdwSubEditorCanvas(AsyncCaller* caller, QWidget* parent) : PixelCanvas(parent) {
    dataHelper = new IdwSubEditorCanvasDataHelper;
    dataHelper->caller = caller;
    dataHelper->unitIndex = new RectUnitIndex;
    dataHelper->centerPointColor = QColor(255,0,0);
    dataHelper->crossDashLineStrongPen = QPen(QBrush(QColor(255,0,0,50)), 2, Qt::DashLine);
    dataHelper->crossDashLineThinPen = QPen(QBrush(QColor(255,0,0,50)), 1, Qt::DashLine);
    dataHelper->selectedBrush = QBrush(Qt::yellow);
    dataHelper->selectedPen = QPen(dataHelper->selectedBrush, 1);
    dataHelper->fastCacheCopyLock.store(false, std::memory_order_release);
    dataHelper->loadingHelper = new LoadingHelper(this);
    dataHelper->loadingHelper->setPBorder(2, Qt::white);
    dataHelper->loadingHelper->setCanvasSizeFunc([this](){
        return QSize(canvasWidth(), canvasHeight());
    });
    connect(dataHelper->loadingHelper, &LoadingHelper::timerUpdate, this, [this](const QRect& rect){
        qreal radio = this->getDevicePixelRatioCache();
        if (radio == 0) radio = devicePixelRatio();
        this->update(QRect(rect.x() / radio,
                           rect.y() / radio,
                           rect.width() / radio,
                           rect.height() / radio
                           ));
    });
    dataHelper->numberFont = font();
}
IdwSubEditorCanvas::~IdwSubEditorCanvas() noexcept{
    delete dataHelper->unitIndex;
    DEFAULT_DELETE_POINTER_IN_CLASS(dataHelper->loadingHelper)
    delete dataHelper;
}

void IdwSubEditorCanvas::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)
    refreshMapFromRoot(true);
    painterBegin();
    if (dataHelper->lastDevicePixelRatio != getDevicePixelRatioCache()) {
        dataHelper->lastDevicePixelRatio = getDevicePixelRatioCache();
        dataHelper->numberFont.setPixelSize(10 * dataHelper->lastDevicePixelRatio);
        dataHelper->loadingHelper->setRadius(30 * dataHelper->lastDevicePixelRatio);
        dataHelper->loadingHelper->setPRadius(3 * dataHelper->lastDevicePixelRatio);
        dataHelper->loadingHelper->setPBorder(2 * dataHelper->lastDevicePixelRatio, Qt::white);
    }
    if (dataHelper->ui_screenRange.width() != canvasWidthInPainting() || dataHelper->ui_screenRange.height() != canvasHeightInPainting()) {
        dataHelper->ui_screenRange.setWidth(canvasWidthInPainting());
        dataHelper->ui_screenRange.setHeight(canvasHeightInPainting());
        QRect tmp = dataHelper->ui_screenRange;
        if (dataHelper->fullyUpdateGid >= 0) {
            dataHelper->caller->stop(dataHelper->fullyUpdateGid);
            if (dataHelper->selectedUpdateGid > 0) {
                dataHelper->caller->stop(dataHelper->selectedUpdateGid);
            }
            dataHelper->caller->push(this, [this, tmp](TSC){
                dataHelper->screenRange.setWidth(tmp.width());
                dataHelper->screenRange.setHeight(tmp.height());
            });
            dataHelper->caller->push(this, dataHelper->fullyUpdateGid, [this](TSC c){
                localUpdate(c);
            });
        } else {
            dataHelper->caller->push(this, [this, tmp](TSC c){
                dataHelper->screenRange.setWidth(tmp.width());
                dataHelper->screenRange.setHeight(tmp.height());
                localUpdate(c);
            });
        }
    }
    painter->fillRect(QRect(0, 0, canvasWidthInPainting(), canvasHeightInPainting()), Qt::white);

    bool d = true;
    bool o = false;
    QImage* image = nullptr;
    bool isCache = false;
    if (dataHelper->mergePainterLock.compare_exchange_strong(o, d, std::memory_order_acq_rel)) {
        image = &dataHelper->mergeRender.getCanvas();
    } else {
        o = false;
        if (!dataHelper->fastCacheCopyLock.compare_exchange_strong(o, d, std::memory_order_acq_rel)) {
            return;
        }
        qDebug() << "获取到fast";
        image = &dataHelper->fastCacheImage;
        isCache = true;
    }
    if (image) {
        QRect range = QRect(0,0, image->width(), image->height());
        painter->drawImage(range, *image, range);
    }
    if (isCache) {
        dataHelper->fastCacheCopyLock.store(false, std::memory_order_release);
    } else {
        dataHelper->mergePainterLock.store(false, std::memory_order_release);
    }
    if (dataHelper->loadingHelper->isActive()) {
        dataHelper->loadingHelper->paintLoadIcon(*painter);
    }
    painter->end();
}
void IdwSubEditorCanvas::mousePressEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton) {
        event->ignore();
        PixelCanvas::mousePressEvent(event);
        return;
    }
    dataHelper->mousePressed = true;
    dataHelper->lastNumber = -1;
    event->accept();
    updateMouseInfo(event);
    qint32 mx = mouseX();
    qint32 my = mouseY();
    dataHelper->caller->push(this, [this, mx, my](TSC){
        IDW_EditUnitPointInfo r = unitAt(QPoint(mx, my));
        MainThreadInvoker::getInvoker().blockRun([this, &r](){
            emit itemChecked(r);
        });
    });
}
void IdwSubEditorCanvas::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton || !dataHelper->mousePressed) {
        event->ignore();
        PixelCanvas::mouseReleaseEvent(event);
        return;
    }
    dataHelper->mousePressed = false;
}
void IdwSubEditorCanvas::mouseMoveEvent(QMouseEvent* event) {
    if (!dataHelper->mousePressed) {
        event->ignore();
        PixelCanvas::mouseMoveEvent(event);
        return;
    }
    updateMouseInfo(event);
    qint32 mx = mouseX();
    qint32 my = mouseY();
    dataHelper->caller->push(this, [this, mx, my](TSC){
        IDW_EditUnitPointInfo r = unitAt(QPoint(mx, my));
        MainThreadInvoker::getInvoker().blockRun([this, &r](){
            emit itemChecked(r);
        });
    });
}

void IdwSubEditorCanvas::updateShotOrigin() {
    MainThreadInvoker::getInvoker().blockRun([this](){
        dataHelper->originX = canvasWidth() / 2;
        dataHelper->originY = canvasHeight() / 2;
    });
}

bool IdwSubEditorCanvas::prepareDrawParam(PrepareDrawParam &param) {
    qint32 originX = dataHelper->originX + 1;
    qint32 originY = dataHelper->originY + 1;

    qint32 cWidth = (originX - dataHelper->padding) * 2;
    qint32 cHeight = (originY - dataHelper->padding) * 2;
    if (cWidth <= 0 || cHeight <= 0) {
        return false;
    }

    FloatNumber1 P_X = std::max(dataHelper->editData.getPitchX(), dataHelper->editData.getSizeX());
    FloatNumber1 P_Y = std::max(dataHelper->editData.getPitchY(), dataHelper->editData.getSizeY());
    qreal sizeX = dataHelper->editData.getSizeX().toDouble();
    qreal sizeY = dataHelper->editData.getSizeY().toDouble();
    qreal pitchX = P_X.toDouble();
    qreal pitchY = P_Y.toDouble();
    qreal realWidth = (P_X * dataHelper->editData.getArrayX() - (P_X - dataHelper->editData.getSizeX())).toDouble();
    qreal realHeight = (P_Y * dataHelper->editData.getArrayY() - (P_Y - dataHelper->editData.getSizeY())).toDouble();

    qint32 pixHeight = realHeight * cWidth / realWidth;
    if (pixHeight <= cHeight) {
        cHeight = pixHeight;
    } else {
        cWidth = realWidth * cHeight / realHeight;
    }

    param.ratioX = realWidth / cWidth;
    param.ratioY = realHeight / cHeight;
    param.pixelSizeX = sizeX * cWidth / realWidth;
    param.pixelSizeY = sizeY * cHeight / realHeight;

    param.pixelPitchX = pitchX * cWidth / realWidth;
    param.pixelPitchY = pitchY * cHeight / realHeight;

    param.pixelArrayWidth = (dataHelper->editData.getArrayX() - 1) * param.pixelPitchX + param.pixelSizeX;
    param.pixelArrayHeight = (dataHelper->editData.getArrayY() - 1) * param.pixelPitchY + param.pixelSizeY;

    param.pixelStartX = originX - param.pixelArrayWidth / 2;
    param.pixelStartY = originY - param.pixelArrayHeight / 2;

    if (dataHelper->dataType == idw_edit_canvas::CHIP) {
        if (param.pixelSizeX > 0) {
            dataHelper->originX = originX - param.pixelArrayWidth / 2;
            param.originX = dataHelper->originX;
        } else {
            dataHelper->originX = originX - cWidth / 2;
            param.originX = dataHelper->originX;
        }
        if (param.pixelSizeY > 0) {
            dataHelper->originY = originY + param.pixelArrayHeight / 2;
            //这里的逻辑是硬调出来的，至于为什么暂时还不清楚
            if (param.pixelSizeY == 1) {
                param.originY = dataHelper->originY - 1;
            } else {
                param.originY = dataHelper->originY;
            }
        } else {
            dataHelper->originY = originY + cHeight / 2;
            //这里的逻辑是硬调出来的，至于为什么暂时还不清楚
            param.originY = dataHelper->originY - 2;
        }
    } else {
        param.originX = originX;
        param.originY = originY;

        if (param.pixelSizeX > 0) {
            dataHelper->originX_Offset = dataHelper->waferData.getMapOffset().x.toDouble() / sizeX * param.pixelSizeX;
        } else {
            FloatNumber1 ratio = dataHelper->waferData.getWaferDiameter() / cWidth;
            dataHelper->originX_Offset = (dataHelper->waferData.getMapOffset().x / ratio).justRemoveDecimalPointTo32();
        }

        if (param.pixelSizeY > 0) {
            dataHelper->originY_Offset = dataHelper->waferData.getMapOffset().y.toDouble() / sizeY * param.pixelSizeY;
        } else {
            if (dataHelper->waferData.getWaferDiameter() == nullptr) return false;
            FloatNumber1 ratio = dataHelper->waferData.getWaferDiameter() / cHeight;
            dataHelper->originY_Offset = (dataHelper->waferData.getMapOffset().y / ratio).justRemoveDecimalPointTo32();
        }
    }
    return true;
}

bool IdwSubEditorCanvas::prepareRectIndex(const PrepareDrawParam& param, qint32 itemCount) {
    return dataHelper->unitIndex->prepareRectMem({std::max(param.pixelSizeX, 1),
                                      std::max(param.pixelSizeY, 1),
                                      dataHelper->screenRange.x(),
                                      dataHelper->screenRange.y(),
                                      dataHelper->screenRange.width(),
                                      dataHelper->screenRange.height(),
                                      itemCount});
}

void IdwSubEditorCanvas::putDataToIndex(const PrepareDrawParam& param, TSC cmd) {
    QList<IDW_Data_Item> &items = *dataHelper->editData.getDataList();
    qint32 listSize = items.size();
    qint64 currSizeX, currSizeY, currPixelX, currPixelY;
    int L(IDW_PIXEL_LEN);
    qreal itemSizeX = (dataHelper->editData.getSizeX() * L).justRemoveDecimalPointTo64();
    qreal itemSizeY = (dataHelper->editData.getSizeY() * L).justRemoveDecimalPointTo64();

    qint64 sizeX, sizeY, offsetX, offsetY;
    for (qint32 i = 0; i < listSize; ++ i) {
        if (cmd.isStop()) {
            return;
        }
        IDW_Data_Item &item = items[i];
        sizeX = (item.getSizeX() * L).justRemoveDecimalPointTo64();
        sizeY = (item.getSizeY() * L).justRemoveDecimalPointTo64();
        offsetX = (item.getOffsetX() * L).justRemoveDecimalPointTo64();
        offsetY = (item.getOffsetY() * L).justRemoveDecimalPointTo64();
        if (param.pixelSizeX > 0) {
            currSizeX = sizeX * param.pixelSizeX / itemSizeX;
            currPixelX = (offsetX < 0 ? int(offsetX * param.pixelSizeX / itemSizeX) : qRound(offsetX * param.pixelSizeX / itemSizeX)) + param.originX;
        } else {
            currSizeX = 1;
            currPixelX = offsetX / L / param.ratioX + param.originX;
        }
        if (param.pixelSizeY > 0) {
            currSizeY = sizeY * param.pixelSizeY / itemSizeY;
            currPixelY = int(-offsetY * param.pixelSizeY / itemSizeY) + param.originY - currSizeY;
        } else {
            currSizeY = 1;
            currPixelY = -offsetY / L / param.ratioY + param.originY;
        }
        QRect rect = QRect(currPixelX, currPixelY,currSizeX, currSizeY);
        dataHelper->unitIndex->putRectInRectMem(rect, item.getNumber());
    }
}

void IdwSubEditorCanvas::rend(TSC cmd) {
    dataHelper->render.resize(QSize(dataHelper->screenRange.width(), dataHelper->screenRange.height()));
    dataHelper->render.fill();
    dataHelper->render.begin();
    dataHelper->render.getPainter().setFont(dataHelper->numberFont);
    QFontMetrics fontMetrics = dataHelper->render.getPainter().fontMetrics();
    qreal ratio = devicePixelRatio();
    qint32 padding = ratio * 2;

    dataHelper->unitIndex->eachUnitForRange(
        QRect(dataHelper->screenRange.x(), dataHelper->screenRange.y(), dataHelper->screenRange.width(), dataHelper->screenRange.height()),
        [this, &fontMetrics, padding](const QRect& rect, qint32 number) {
            dataHelper->render.getPainter().drawRect(
                sem::getDrawRect(QRect(rect.x(), rect.y(), rect.width(), rect.height())));
            QString str = QString::number(number);
            QRect bounding = fontMetrics.tightBoundingRect(str);
            if (bounding.width() + padding * 2 <= rect.width()
                && bounding.height() + padding * 2 <= rect.height()) {
                dataHelper->render.getPainter()
                .drawText(QPoint(rect.x() + rect.width() / 2 - bounding.width() / 2,
                                 rect.y() + rect.height() / 2
                                     - (fontMetrics.ascent() + fontMetrics.descent()) / 2
                                     + fontMetrics.ascent()),
                          str);
            }
        },
        cmd);
    dataHelper->render.end();
}

void IdwSubEditorCanvas::mergePixmap() {
    if (dataHelper->screenRange.width() <= 0 || dataHelper->screenRange.height() <= 0) {
        return;
    }
    bool d = true;
    bool o = false;
    while (!dataHelper->mergePainterLock.compare_exchange_weak(o, d, std::memory_order_acq_rel)) {
        o = false;
        QThread::yieldCurrentThread();
    }
    dataHelper->mergeRender.resize(QSize(dataHelper->screenRange.width(), dataHelper->screenRange.height()));
    dataHelper->mergeRender.fill();
    dataHelper->mergeRender.begin();
    if (dataHelper->dataType == idw_edit_canvas::SHOT) {
        dataHelper->mergeRender.getPainter().fillRect(dataHelper->originX - 2 + dataHelper->originX_Offset, dataHelper->originY - 2 + dataHelper->originY_Offset, 6, 6, dataHelper->centerPointColor);
        dataHelper->mergeRender.getPainter().save();
        dataHelper->mergeRender.getPainter().setPen(dataHelper->crossDashLineStrongPen);
        dataHelper->mergeRender.getPainter().drawLine(sem::getDrawLinePoint(dataHelper->originX - 4 + dataHelper->originX_Offset, dataHelper->originY + dataHelper->originY_Offset, 2), sem::getDrawLinePoint(0, dataHelper->originY + dataHelper->originY_Offset, 2));
        dataHelper->mergeRender.getPainter().drawLine(sem::getDrawLinePoint(dataHelper->originX + 4 + dataHelper->originX_Offset, dataHelper->originY + dataHelper->originY_Offset, 2), sem::getDrawLinePoint(dataHelper->screenRange.width(), dataHelper->originY + dataHelper->originY_Offset, 2));
        dataHelper->mergeRender.getPainter().drawLine(sem::getDrawLinePoint(dataHelper->originX + dataHelper->originX_Offset, dataHelper->originY - 4 + dataHelper->originY_Offset, 2), sem::getDrawLinePoint(dataHelper->originX + dataHelper->originX_Offset, 0, 2));
        dataHelper->mergeRender.getPainter().drawLine(sem::getDrawLinePoint(dataHelper->originX + dataHelper->originX_Offset, dataHelper->originY + 4 + dataHelper->originY_Offset, 2), sem::getDrawLinePoint(dataHelper->originX + dataHelper->originX_Offset, dataHelper->screenRange.height(), 2));
        dataHelper->mergeRender.getPainter().restore();
    } else {
        dataHelper->mergeRender.getPainter().fillRect(dataHelper->originX - 2 + dataHelper->originX_Offset, dataHelper->originY - 3 + dataHelper->originY_Offset, 5, 5, dataHelper->centerPointColor);
        dataHelper->mergeRender.getPainter().save();
        dataHelper->mergeRender.getPainter().setPen(dataHelper->crossDashLineThinPen);
        dataHelper->mergeRender.getPainter().drawLine(sem::getDrawLinePoint(dataHelper->originX - 3 + dataHelper->originX_Offset, dataHelper->originY - 1 + dataHelper->originY_Offset), sem::getDrawLinePoint(dataHelper->originX_Offset, dataHelper->originY - 1 + dataHelper->originY_Offset));
        dataHelper->mergeRender.getPainter().drawLine(sem::getDrawLinePoint(dataHelper->originX + 3 + dataHelper->originX_Offset, dataHelper->originY - 1 + dataHelper->originY_Offset), sem::getDrawLinePoint(dataHelper->screenRange.width(), dataHelper->originY - 1 + dataHelper->originY_Offset));
        dataHelper->mergeRender.getPainter().drawLine(sem::getDrawLinePoint(dataHelper->originX + dataHelper->originX_Offset, dataHelper->originY - 4 + dataHelper->originY_Offset), sem::getDrawLinePoint(dataHelper->originX + dataHelper->originX_Offset, dataHelper->originY_Offset));
        dataHelper->mergeRender.getPainter().drawLine(sem::getDrawLinePoint(dataHelper->originX + dataHelper->originX_Offset, dataHelper->originY + 2 + dataHelper->originY_Offset), sem::getDrawLinePoint(dataHelper->originX +dataHelper-> originX_Offset, dataHelper->screenRange.height()));
        dataHelper->mergeRender.getPainter().restore();
    }


    if (dataHelper->render.isReady()) {
        //TODO 绘制选中的内容
        if (dataHelper->selectedNumbers.size() > 0) {
            dataHelper->mergeRender.getPainter().setBrush(dataHelper->selectedBrush);
            dataHelper->mergeRender.getPainter().setPen(dataHelper->selectedPen);
            for (auto it = dataHelper->selectedNumbers.begin(); it != dataHelper->selectedNumbers.end(); ++ it) {
                UnitRect* rect = dataHelper->unitIndex->findRectByNumber(*it);
                if (rect) {
                    QRect unitRect = QRect(rect->x1, rect->y1, rect->x2 - rect->x1 + 1, rect->y2 - rect->y1 + 1);
                    dataHelper->mergeRender.getPainter().drawRect(getDrawRect(unitRect, 1));
                }
            }
        }
        dataHelper->mergeRender.getPainter().drawImage(0, 0, dataHelper->render.getCanvas());
    }

    dataHelper->mergeRender.end();
    dataHelper->mergePainterLock.store(false, std::memory_order_release);

    d = true;
    o = false;
    while (!dataHelper->fastCacheCopyLock.compare_exchange_weak(o, d, std::memory_order_acq_rel)) {
        o = false;
        QThread::yieldCurrentThread();
    }
    dataHelper->fastCacheImage = dataHelper->mergeRender.getCanvas();
    dataHelper->fastCacheCopyLock.store(false, std::memory_order_release);
}

IDW_EditUnitPointInfo IdwSubEditorCanvas::unitAt(const QPoint& pos) {
    if (pos.x() >= 0 && pos.y() >= 0) {
        UnitRect* unit = nullptr;
        bool repeat = false;
        unit = dataHelper->unitIndex->findRectByPoint(pos.x(), pos.y());
        int number = -1;
        if (unit) {
            number = unit->number;
            repeat = dataHelper->lastNumber == number;
            dataHelper->lastNumber = number;
        }
        return {number, repeat};
    }
    return {-1, false};
}

AsyncCaller& IdwSubEditorCanvas::getAsyncCaller() {
    return *dataHelper->caller;
}

IDW_ShotChipData& IdwSubEditorCanvas::localEditData() {
    return dataHelper->editData;
}

IDW_WaferData& IdwSubEditorCanvas::localWaferData() {
    return dataHelper->waferData;
}

bool IdwSubEditorCanvas::isLocalThread() {
    return dataHelper->caller->isCurrThread();
}

void IdwSubEditorCanvas::checkItem(qint32 number, char checked) {
    if (!isLocalThread()) {
        dataHelper->caller->push(this, [this, number, checked](TSC){
            localCheckItem(number, checked);
        });
        return;
    }
    localCheckItem(number, checked);
}

void IdwSubEditorCanvas::localCheckItem(qint32 number, char checked) {
    if (checked == 1) {
        dataHelper->selectedNumbers.insert(number);
    } else if (checked == 0){
        dataHelper->selectedNumbers.remove(number);
    } else {
        if (dataHelper->selectedNumbers.contains(number)) {
            dataHelper->selectedNumbers.remove(number);
        } else {
            dataHelper->selectedNumbers.insert(number);
        }
    }
}

void IdwSubEditorCanvas::localClearCheckedItem() {
    dataHelper->selectedNumbers.clear();
}

//注册全量更新任务组ID
void IdwSubEditorCanvas::registFullyUpdateGid(int id) {
    dataHelper->fullyUpdateGid = id;
}

void IdwSubEditorCanvas::registSelectedUpdateGid(int id) {
    dataHelper->selectedUpdateGid = id;
}

void IdwSubEditorCanvas::localUpdate(TSC cmd) {
    dataHelper->render.setReady(false);
    //TODO 是否是shot画布
    updateShotOrigin();
    if (dataHelper->editData.getDataList() && dataHelper->editData.getDataList()->size() > 0) {
        MainThreadInvoker::getInvoker().runLater([this](){
            dataHelper->loadingHelper->exec();
        });
        sem::AfterGc gc([this](){
            MainThreadInvoker::getInvoker().runLater([this](){
                dataHelper->loadingHelper->block();
            });
        });
        dataHelper->unitIndex->invalidIndex();
        PrepareDrawParam drawParam;
        if (!prepareDrawParam(drawParam)) {
            return;
        }
        prepareRectIndex(drawParam, dataHelper->editData.getDataList()->size());
        putDataToIndex(drawParam, cmd);
        if (cmd.isStop()) {
            return;
        }
        rend(cmd);
        if (cmd.isStop()) {
            return;
        }
        dataHelper->render.setReady(true);
    } else if (dataHelper->dataType == idw_edit_canvas::CHIP) {
        MainThreadInvoker::getInvoker().blockRun([this](){
            dataHelper->originX = dataHelper->padding;
            dataHelper->originY = dataHelper->originY * 2 - dataHelper->padding;
        });
    } else {
        FloatNumber1 ratio = dataHelper->waferData.getWaferDiameter() / canvasWidth();
        dataHelper->originX_Offset = (dataHelper->waferData.getMapOffset().x / ratio).justRemoveDecimalPointTo32();
        ratio = dataHelper->waferData.getWaferDiameter() / canvasHeight();
        dataHelper->originY_Offset = (dataHelper->waferData.getMapOffset().y / ratio).justRemoveDecimalPointTo32();
    }
    mergePixmap();
    MainThreadInvoker::getInvoker().runLater([this](){
        this->update();
    });
}

void IdwSubEditorCanvas::localUpdateCheckOnly() {
    mergePixmap();
    MainThreadInvoker::getInvoker().runLater([this](){
        this->update();
    });
}

void IdwSubEditorCanvas::setDataType(idw_edit_canvas::EditDataType dataType) {
    dataHelper->dataType = dataType;
}
