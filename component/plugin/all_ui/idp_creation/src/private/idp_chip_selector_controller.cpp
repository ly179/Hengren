#include "idp_chip_selector_controller.h"
#include <QEvent>
#include <QKeyEvent>
#include "cde90_message_box.h"
#include "cde90_process_box.h"
#include "cdsem_global_resource.h"
#include "chip_map_helper.h"
#include "idw_canvas_item_check_style.h"
#include "idw_model_data_convert.h"
#include "idw_wafer_service.h"
#include "service_manager.h"
#include "template_info_model_convert.h"
#include "template_info_service.h"
#include "ui_idp_chip_selector_controller.h"
#include "idp_common_util.h"
#include "idw_wafer_data_model.h"

#define CHIP_TEG_CHECKED 2
#define SHOT_TEG_CHECKED 1

#define CHIP_SELECTED 4
#define CHIP_TMP_SELECTED 3
#define CHIP_CROSS_SELECTED 5

#define CANVAS ui->idwCanvasContent->getCanvas()
#define CCANVAS ui->idwCanvasContent->getCanvas()->
#define waferData CCANVAS localWafer()
#define shotData CCANVAS localShot()
#define chipData CCANVAS localChip()

void toViewDNumber(const QList<int>& dList, QString& view) {
    listJoinToStr<int>(dList, view, ",", 2);
    if (dList.size() > 2) {
        view += "...";
    }
}

QString reCalcPNoDataDNumber(int pNo, const QList<IdpMpTableModeData>& cList, const QHash<QPoint, QList<QList<int>>>& oldPosDNumbers) {
    QList<QList<int>> list;
    list.reserve(3);
    bool hasDNumber = false;
    for (const IdpMpTableModeData& data : cList) {
        if (pNo != data.detailParamData->pNumber) continue;
        auto oit = oldPosDNumbers.find(QPoint(data.posData->chipNumber, data.detailParamData->pNumber));
        if (oit == oldPosDNumbers.end()) {
            if (!hasDNumber) {
                hasDNumber = data.detailParamData->mpTemplate.templateParam.exeParam.list.size() > 0;
            }
            continue;
        }
        if (!hasDNumber) hasDNumber = true;
        list.append(*oit);
    }
    QString str;
    if (list.size() > 0)
        collectViewDName(list, str);
    else if (hasDNumber)
        str = QString::number(pNo);
    return str;
}

class IdpMpPosTableModel : public SimpleRowTableModel<IdpMpTableModeData> {
private:
    QVariant getItemData(qint32 row, qint32 column) const override {
        const IdpMpTableModeData& data = getDataList()[row];
        IdpMpParamListData* detailParam = nullptr;
        if (!data.detailParamData.isNull()) {
            detailParam = data.detailParamData.data();
        }
        QString str;
        switch (column) {
        case 0:
            return row + 1;
        case 1:
            return data.posData->chipX == nullptr ? "-" : data.posData->chipX.toString();
        case 2:
            return ",";
        case 3:
            return data.posData->chipY == nullptr ? "-" : data.posData->chipY.toString();
        case 4:
            if (!data.detailParamData) {
                return QVariant();
            }
            return data.detailParamData->dPointType == 1 ? "R" : "";
        case 5:
            if (detailParam && detailParam->dPoint.x != nullptr) {
                return detailParam->dPoint.x.toMinString();
            } else if (data.posData->oPoint.x != nullptr) {
                return data.posData->oPoint.x.toMinString();
            }
            // if (detailParam && detailParam->dPoint.x != nullptr && data.posData->oPoint.x != nullptr) {
            //     return (detailParam->dPoint.x + data.posData->oPoint.x).toMinString();
            // } else if (detailParam && detailParam->dPoint.x != nullptr) {
            //     return detailParam->dPoint.x.toMinString();
            // } else if (data.posData->oPoint.x != nullptr) {
            //     data.posData->oPoint.x.toMinString();
            // }
            return QVariant();
        case 6:
            return ",";
        case 7:
            // if (detailParam && detailParam->dPoint.y != nullptr && data.posData->oPoint.y != nullptr) {
            //     return (detailParam->dPoint.y + data.posData->oPoint.y).toMinString();
            // } else if (detailParam && detailParam->dPoint.y != nullptr) {
            //     return detailParam->dPoint.y.toMinString();
            // } else if (data.posData->oPoint.y != nullptr) {
            //     data.posData->oPoint.y.toMinString();
            // }
            if (detailParam && detailParam->dPoint.y != nullptr) {
                return detailParam->dPoint.y.toMinString();
            } else if (data.posData->oPoint.y != nullptr) {
                return data.posData->oPoint.y.toMinString();
            }
            return QVariant();
        case 8:
            if (!data.detailParamData) {
                return QVariant();
            }
            return data.detailParamData->usePrevPoint ? "D" : "";
        case 9:
            if (!data.detailParamData) {
                return QVariant();
            }
            return data.detailParamData->viewMpName;
        case 10:
            if (detailParam) {
                return detailParam->pNumber;
            } else {
                return "0";
            }
        case 11:
            return data.dNumbers;
        }
        return "";
    }

public:
    explicit IdpMpPosTableModel(CDE90TableView* parent): SimpleRowTableModel(parent) {
        insertHead("No.", 0, 0);
        insertHead("Chip No.", 1, 4);
        insertHead("Coordinate X,Y", 5, 8);
        insertHead("Name", 9, 9);
        insertHead("P No.", 10, 10);
        insertHead("D No.", 11, 11);
        parent->replaceDataModel(this);

        QList<int> columnsSize;
        columnsSize.reserve(12);
        columnsSize << 40 << 30 << 5 << 30 << 5 << 80 << 5 << 80 << 5 << 100 << 50 << 50;
        parent->setMinColumnsSize(columnsSize);
        parent->recalcColumnSize();
    }
    ~IdpMpPosTableModel() noexcept {}
};

class ChipSelectorIdpPNoTableModel : public SimpleRowTableModel<ChipSelectorIdpPNoData> {
private:
    QVariant getItemData(
        qint32 row, qint32 column) const {
        const ChipSelectorIdpPNoData& data = getDataList()[row];

        switch (column) {
        case 0:
            return QString("( %1, %2 )")
                .arg(data.data->dPoint.x.toQStringAndKeepFullDecimal())
                .arg(data.data->dPoint.y.toQStringAndKeepFullDecimal());
        case 1:
            if (data.data->seqType == 0) {
                return QString("MP%1").arg(data.data->pNumber);
            } else if (data.data->seqType == 1) {
                return QString("IP%1").arg(data.data->pNumber);
            }
            return QVariant();
        case 2:
            return data.dNumber;
        }
        return "";
    }

public:
    ChipSelectorIdpPNoTableModel(
        CDE90TableView* parent)
        : SimpleRowTableModel(parent) {
        insertHead("Coordinate(dx,dy)");
        insertHead("MP/IP");
        insertHead("D No.");
        parent->replaceDataModel(this);

        QList<int> columnsSize;
        columnsSize.reserve(3);
        columnsSize << 160 << 80 << 80;
        parent->setMinColumnsSize(columnsSize);
        parent->recalcColumnSize();
    }
};

class ChipSelectorIdpTemplateTableModel : public SimpleRowTableModel<ChipSelectorIdpTemplateData> {
private:
    QVariant getItemData(
        qint32 row, qint32 column) const {
        const ChipSelectorIdpTemplateData& data = getDataList()[row];

        switch (column) {
        case 0:
            return QString("( %1, %2 )")
                .arg(data.dPoint.x.toQStringAndKeepFullDecimal())
                .arg(data.dPoint.y.toQStringAndKeepFullDecimal());
        case 1:
            return data.templateName;
        }
        return "";
    }

public:
    ChipSelectorIdpTemplateTableModel(
        CDE90TableView* parent)
        : SimpleRowTableModel(parent) {
        insertHead("Coordinate(dx,dy)");
        insertHead("Template");
        parent->replaceDataModel(this);

        QList<int> columnsSize;
        columnsSize.reserve(2);
        columnsSize << 160 << 120;
        parent->setMinColumnsSize(columnsSize);
        parent->recalcColumnSize();
    }
};

class AlreadyCrossDrawer : public SelectNumberDrawerHandler {
private:
    QPen pen;
    int halfSize = 2;

public:
    AlreadyCrossDrawer() { pen = QPen(QBrush(QColor(Qt::black)), 2); }
    ~AlreadyCrossDrawer() {};
    virtual void begin(
        QPainter& painter) override {
        pen.setWidth(deviceRatio * 2);
        halfSize = pen.width();
        painter.setPen(pen);
    }
    // virtual void end(QPainter&) {}
    void paint(
        const QRect& rect, QPainter& painter, qint32) override {
        qint32 halfWidth = rect.width() / 2;
        qint32 halfHeight = rect.height() / 2;
        //横线
        painter.drawLine(QPoint(rect.x() + halfWidth - halfSize, rect.y() + halfHeight),
                         QPoint(rect.x() + halfWidth + halfSize, rect.y() + halfHeight));
        //竖线
        painter.drawLine(QPoint(rect.x() + halfWidth, rect.y() + halfHeight - halfSize),
                         QPoint(rect.x() + halfWidth, rect.y() + halfHeight + halfSize));
    }
};

IdpChipSelectorController::IdpChipSelectorController(QWidget* parent) : CDE90Dialog(parent) {
    //如果你没有异步任务需求，请将connect清理掉，保留create和createAction函数
    connect(this, &IdpChipSelectorController::asyncCallerChanged, this, [this](){
        create();
        createAction();
    }, Qt::SingleShotConnection);
}

IdpChipSelectorController::~IdpChipSelectorController() noexcept {
    if (ui) {
        delete ui;
    }
    if (helper) {
        delete helper;
    }
}

void IdpChipSelectorController::create() {
    ui = new cdsem::UiIdpChipSelectorController;
    setContent(new QWidget);
    ui->init(getContent());
    ui->moveBackBtn->getQPushButton()->setIcon(
        ResourceManager::getResourceManager().getIdwShiftUpIcon());
    ui->moveForwardBtn->getQPushButton()->setIcon(
        ResourceManager::getResourceManager().getIdwShiftDownIcon());
    ui->addBtn->getQPushButton()->setIconSize(QSize(30, 30));
    ui->deleteBtn->getQPushButton()->setIconSize(QSize(30, 30));
    ui->addBtn->getQPushButton()->setIcon(
        ResourceManager::getResourceManager().getIdwShiftRightIcon());
    ui->deleteBtn->getQPushButton()->setIcon(
        ResourceManager::getResourceManager().getIdwShiftLeftIcon());
    
}

void IdpChipSelectorController::createAction() {
    this->installEventFilter(this);
    ui->idwCanvasContent->setAsyncCaller(getAsyncCaller());
    itsChildAndBeManaged(ui->idwCanvasContent, this);
    itsChildAndBeManaged(ui->idwCanvasContent->getCanvas(), ui->idwCanvasContent);
    helper = new ChipMapHelper(CANVAS);
    CANVAS->registSelectedHandler({nullptr, qint32(qRgb(255,255,0)), IDW_CM_CHIP, CHIP_SELECTED});
    CANVAS->registSelectedHandler({nullptr, qint32(qRgb(200,200,200)), IDW_CM_CHIP, CHIP_TMP_SELECTED});
    CANVAS->registSelectedHandler(
        {QSharedPointer<SelectNumberDrawerHandler>(new AlreadyCrossDrawer), 0, IDW_CM_CHIP, CHIP_CROSS_SELECTED});
    CCANVAS registSelectedHandler(
        {QSharedPointer<SelectNumberDrawerHandler>(IDW_DEFAULT_TEG_CHECK_STYLE),
         0,
         IDW_CM_SHOT,
         SHOT_TEG_CHECKED});
    CCANVAS registSelectedHandler(
        {QSharedPointer<SelectNumberDrawerHandler>(IDW_DEFAULT_TEG_CHECK_STYLE),
         0,
         IDW_CM_CHIP,
         CHIP_TEG_CHECKED});

    bindCloseBtn(ui->cancelBtn->getQPushButton());

    seqTableModel = new IdpMpPosTableModel(ui->seqTable);
    pNoTableModel = new ChipSelectorIdpPNoTableModel(ui->pNoTable);
    ui->pNoTable->setSelectionMode(QAbstractItemView::ExtendedSelection);

    templateTableModel = new ChipSelectorIdpTemplateTableModel(ui->templateTable);

    //向上
    connect(ui->moveBackBtn->getQPushButton(), &QPushButton::clicked, this, [this]() {
        QList<qint32> indices = ui->seqTable->getOrderedSelectedRows();
        if (indices.isEmpty()) {
            return;
        }
        ACaller->push(this,
                      [this, indices](TSC) mutable { seqTableModel->swapData(indices, true); });
    });
    //向下
    connect(ui->moveForwardBtn->getQPushButton(), &QPushButton::clicked, this, [this]() {
        QList<qint32> indices = ui->seqTable->getOrderedSelectedRows();
        if (indices.isEmpty()) {
            return;
        }
        ACaller->push(this,
                      [this, indices](TSC) mutable { seqTableModel->swapData(indices, false); });
    });

    connect(ui->addBtn->getQPushButton(), &QPushButton::clicked, this, [this]() {
        addMeasurementPoint();
    });
    connect(ui->deleteBtn->getQPushButton(), &QPushButton::clicked, this, [this]() {
        deleteMeasurementPoint();
    });

    connect(ui->pNoTable->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this,
            [this](const QItemSelection& selected, const QItemSelection& deselected) {
                Q_UNUSED(selected)
                Q_UNUSED(deselected)
                QList<int> indices = ui->pNoTable->getOrderedSelectedRows();
                if (indices.isEmpty()) {
                    return;
                }
                ACaller->push(this, [this, indices](TSC) {
                    // CANVAS->localGetCheckedPoints(CHIP_SELECTED)->clear();
                    CANVAS->localGetCheckedPoints(CHIP_CROSS_SELECTED)->clear();
                    QList<ChipSelectorIdpPNoData>& list = pNoTableModel->getDataList();
                    if (list.isEmpty()) {
                        return;
                    }
                    for (const int& index : indices) {
                        if (index < 0 || index >= list.size()) {
                            continue;
                        }
                        ChipSelectorIdpPNoData& data = list[index];
                        for (QSharedPointer<IdpMpPosListData>& d : data.posList) {
                            // CANVAS->localCheckItem(CHIP_SELECTED, d->chipNumber, 1);
                            CANVAS->localCheckItem(CHIP_CROSS_SELECTED, d->chipNumber, 1);
                        }
                        CANVAS->localUpdateWaferOnly();
                        MainThreadInvoker::getInvoker().blockRun([this, &data]() {
                            ui->countTextField->getQLineEdit()->setText(
                                QString::number(data.posList.size()));
                        });
                    }

                });
            });

    //初始化画布参数
    getAsyncCaller()->push(this, [this](TSC){
        CCANVAS localSetDisplayMode(IDW_DM_MIN);
        CCANVAS localSetMouseCheckUnit(IDW_CM_CHIP);
    });

    CANVAS->localSetItemCheckedCallback([this](const IDW_UnitPointInfo& info){
        if (info.repeat) {
            return;
        }
        if (info.number3 < 0) {
            return;
        }
        int number = info.number3;
        bool needClearOld = !shiftKeyPressed.load(std::memory_order_acquire) && !controlKeyPressed.load(std::memory_order_acquire);
        bool donothing = false;
        if (needClearOld) {
            IdwLinkedList* list = CCANVAS localGetCheckedPoints(CHIP_SELECTED);
            if (list->size() > 0) {
                donothing = list->contains(number);
                list->clear();
            }
        }
        if (!donothing)
            CCANVAS localCheckItem(CHIP_SELECTED, number);
        CCANVAS localUpdateWaferOnly();
        qint32 size = CCANVAS localGetCheckedPoints(CHIP_SELECTED)->size();
        MainThreadInvoker::getInvoker().runLater(
            [this, size]() { ui->countTextField->getQLineEdit()->setText(QString::number(size)); });
    });

    connect(CANVAS, &IdwCanvas::notifyMouseSelectRectRangeBegin, this, [this](){
        ACaller->push(this, [this](TSC){
            IdwLinkedList* list = CANVAS->localGetCheckedPoints(CHIP_SELECTED);
            const IdwLinkedNode* node = list->first();
            while (node) {
                beforeSelectedStatus.insert(node->data);
                node = node->next;
            }
        });
    });
    connect(CANVAS, &IdwCanvas::notifyMouseSelectRectRangeEnd, this, [this](){
        ACaller->push(this, [this](TSC){
            beforeSelectedStatus.clear();
        });
    });

    connect(CANVAS, &IdwCanvas::notifyMouseSelectRectRange, this, [this](const QRect& range){
        ACaller->stop(999);
        ATask.gid(999).object(this).push(ACaller)
            .task([this, range](TSC cmd){
                QList<qint32> list = CANVAS->localSearchChipByRange(cmd, range, true);
                for (qint32& n : lastSelectedStatus) {
                    if (cmd.isStop()) {
                        return;
                    }
                    CANVAS->localCheckItem(CHIP_SELECTED, n, beforeSelectedStatus.contains(n));
                }
                if (list.size() > 0) {
                    for (qint32& n : list) {
                        if (cmd.isStop()) {
                            return;
                        }
                        CANVAS->localCheckItem(CHIP_SELECTED, n, !beforeSelectedStatus.contains(n));
                    }
                }
                CCANVAS localUpdateWaferOnly();
                qint32 size = CCANVAS localGetCheckedPoints(CHIP_SELECTED)->size();
                MainThreadInvoker::getInvoker().runLater([this, size]() {
                    ui->countTextField->getQLineEdit()->setText(QString::number(size));
                });
                lastSelectedStatus = std::move(list);
            })
            .exec();
    });

    connect(ui->magBox->getQComboBox(), &UComboBox::currentIndexChanged, this, [this](int mag) {
        mag = mag + 1;
        getAsyncCaller()->push(this, [this, mag](TSC cmd){
            CCANVAS localSetMag(mag);
            CCANVAS localUpdate(cmd);
        });
    });

    CANVAS->setMousePressCallback([this](QMouseEvent* event) {
        if (event->button() == Qt::MiddleButton) {
            middlePressed = true;
            CANVAS->updateMouseInfo(event);
            QPoint pos(CANVAS->mouseX(), CANVAS->mouseY());
            event->accept();
            bool ctrlPressed = ui->idwCanvasContent->getCanvas()->isEnableMouseRanageSelected();
            ACaller->push(this, [this, pos, ctrlPressed](TSC) {
                IDW_UnitPointInfo result = CANVAS->localShotChipAt(pos);
                if (result.number3 < 0) {
                    return;
                }
                localMoveBeginPos = QPoint(result.x, result.y);
                if (ctrlPressed) {
                    IdwLinkedList* selectedList = CANVAS->localGetCheckedPoints(CHIP_SELECTED);
                    IdwLinkedList* grayList = CANVAS->localGetCheckedPoints(CHIP_TMP_SELECTED);
                    grayList->clear();
                    const IdwLinkedNode* n = selectedList->first();
                    while (n) {
                        grayList->append(n->data);
                        n = n->next;
                    }
                }
            });
            return;
        }
        CANVAS->doMousePress(event);
    });
    CANVAS->setMouseMoveCallback([this](QMouseEvent* event) {
        if (middlePressed) {
            CANVAS->updateMouseInfo(event);
            QPoint pos(CANVAS->mouseX(), CANVAS->mouseY());
            event->accept();
            ACaller->push(this, [this, pos](TSC) { localMiddleMoveChip(pos); });
            return;
        }
        CANVAS->doMouseMove(event);
    });
    CANVAS->setMouseReleaseCallback([this](QMouseEvent* event) {
        if (middlePressed && event->button() == Qt::MiddleButton) {
            middlePressed = false;
            CANVAS->updateMouseInfo(event);
            QPoint pos(CANVAS->mouseX(), CANVAS->mouseY());
            event->accept();
            ACaller->push(this, [this, pos](TSC) {
                localMiddleMoveChip(pos);
                IdwLinkedList* grayList = CANVAS->localGetCheckedPoints(CHIP_TMP_SELECTED);
                IdwLinkedList* selectedList = CANVAS->localGetCheckedPoints(CHIP_SELECTED);
                const IdwLinkedNode* n = grayList->first();
                while (n) {
                    selectedList->append(n->data);
                    n = n->next;
                }
                grayList->clear();
                CANVAS->localUpdateWaferOnly();
            });
            return;
        }
        CANVAS->doMouseRelease(event);
    });

    connect(ui->okBtn->getQPushButton(), &QPushButton::clicked, this, [this]() {
        if (okCallback) {
            ACaller->push(this, [this](TSC) {
                MainThreadInvoker::getInvoker().blockRun([this]() {
                    if (okCallback(seqTableModel->getDataList())) {
                        changeCloseStatusOnce(HW_OK);
                        close();
                    }
                });
            });
        }
    });

    connect(ui->chipNoCbox, &QCheckBox::toggled, this, [this](bool b) {
        ACaller->push(this, [this, b](TSC cmd) { CANVAS->localSetRendChipNumber(cmd, b ? 2 : 0); });
    });
}

bool IdpChipSelectorController::eventFilter(QObject *watched, QEvent *event) {
    if (watched != this || (event->type() != QEvent::KeyPress && event->type() != QEvent::KeyRelease))
        return CDE90Dialog::eventFilter(watched, event);
    QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(event);
    if (keyEvent->key() == Qt::Key_Shift) {
        bool pressed = event->type() == QEvent::KeyPress;
        shiftKeyPressed.store(pressed, std::memory_order_release);
        ui->idwCanvasContent->getCanvas()->setAllowMouseSlideAsPress(pressed);
        ui->idwCanvasContent->getCanvas()->setMousePressAsClick(pressed);
        return true;
    } else if (keyEvent->key() == Qt::Key_Control) {
        bool pressed = event->type() == QEvent::KeyPress;
        controlKeyPressed.store(pressed, std::memory_order_release);
        ui->idwCanvasContent->getCanvas()->enableMouseRanageSelected(pressed);
        return true;
    }
    return CDE90Dialog::eventFilter(watched, event);
}

void IdpChipSelectorController::localMiddleMoveChip(
    const QPoint& pos) {
    IDW_UnitPointInfo result = CANVAS->localShotChipAt(pos);
    if (result.repeat || result.number3 < 0) {
        return;
    }
    int shiftX = result.x - localMoveBeginPos.x();
    int shiftY = result.y - localMoveBeginPos.y();
    int moveCount = 2;
    if (shiftX == 0
        || !helper
                ->localShiftWork(CHIP_SELECTED,
                                 Qt::Horizontal,
                                 shiftX,
                                 false)
                .moved) {
        result.x = localMoveBeginPos.x();
        --moveCount;
    }
    if (shiftY == 0
        || !helper
                ->localShiftWork(CHIP_SELECTED,
                                 Qt::Vertical,
                                 shiftY,
                                 false)
                .moved) {
        result.y = localMoveBeginPos.y();
        --moveCount;
    }
    if (moveCount > 0) {
        localMoveBeginPos = QPoint(result.x, result.y);
        CANVAS->localUpdateWaferOnly();
    }
}

void IdpChipSelectorController::_addMeasurementPoint(int index, QList<ChipSelectorIdpPNoData>& pList) {
    if (index < 0 || index >= pList.size()) return;
    ChipSelectorIdpPNoData& pNoData = pList[index];
    QSharedPointer<IdpMpParamListData>& paramData = pNoData.data;
    IdwLinkedList* list = CANVAS->localGetCheckedPoints(CHIP_SELECTED);
    if (!list || list->size() == 0) {
        return;
    }
    QSet<qint32> existsNumber;
    QList<IdpMpTableModeData>& seqList = seqTableModel->getDataList();
    for (IdpMpTableModeData& d : seqList) {
        if (d.detailParamData != paramData) {
            continue;
        }
        existsNumber.insert(d.posData->chipNumber);
    }

    const IdwLinkedNode* node = list->first();
    bool found;
    QPoint ruler;
    int addCount = 0;
    while (node) {
        if (!existsNumber.contains(node->data)) {
            ruler = CANVAS->localFindChipRulerByNumber(node->data, &found);
            if (found) {
                IdpMpTableModeData newData;
                newData.posData = QSharedPointer<IdpMpPosListData>(new IdpMpPosListData);
                newData.posData->oPoint = getChipCoordinateByChipNumber(node->data,
                                                                        CANVAS->localShot().data(),
                                                                        CANVAS->localChip().data(),
                                                                        &found);
                if (found) {
                    newData.detailParamData = paramData;

                    newData.posData->chipX = ruler.x();
                    newData.posData->chipY = ruler.y();
                    //TODO 待修改
                    // newData.posData->dNumber = QString::number(paramData->pNumber);
                    newData.posData->chipNumber = node->data;
                    newData.posData->paramIndex = index;
                    auto it = oldPosDNumbers.find(QPoint(newData.posData->chipNumber, paramData->pNumber));
                    if (it == oldPosDNumbers.end()) {
                        int dCount = newData.detailParamData->mpTemplate.templateParam.exeParam.list.size();
                        newData.posData->dNumbers.resize(dCount);
                        if (dCount > 0) {
                            newData.dNumbers = QString::number(paramData->pNumber);
                            QList<int> dNums;
                            dNums.resize(1);
                            dNums[0] = paramData->pNumber;
                            for (int i = 0; i < dCount; ++ i) {
                                newData.posData->dNumbers.replace(i, dNums);
                            }
                        } else {
                            newData.dNumbers = "";
                        }
                    } else {
                        QList<int> list;
                        for (auto hit = it->begin(); hit != it->end(); ++ hit) {
                            for (int& d : *hit) {
                                if (list.contains(d)) continue;
                                list.append(d);
                                if (list.size() > 2) break;
                            }
                            if (list.size() > 2) break;
                        }
                        collectViewDName(*it, newData.dNumbers);
                    }
                    seqList.append(newData);
                    existsNumber.insert(node->data);
                    CANVAS->checkItem(CHIP_CROSS_SELECTED, node->data, 1);
                    pNoData.posList.append(newData.posData);
                    ++addCount;
                }
            }
        }
        node = node->next;
    }
    if (addCount > 0) {
        seqTableModel->resizeViewList(seqList.size());
        pNoData.dNumber = reCalcPNoDataDNumber(pNoData.data->pNumber, seqList, oldPosDNumbers);
        pNoTableModel->notifyDataChanged(index, index);
    }
}

void IdpChipSelectorController::addMeasurementPoint() {
    QList<int> indices = ui->pNoTable->getOrderedSelectedRows();
    if (indices.isEmpty()) {
        return;
    }
    ACaller->push(this, [this, indices](TSC) {
        QList<ChipSelectorIdpPNoData>& pList = pNoTableModel->getDataList();
        for (const int& index : indices) {
            _addMeasurementPoint(index, pList);
        }
        CANVAS->localUpdateWaferOnly();
    });
}

void IdpChipSelectorController::_deleteMeasurementPoint(int index, QList<ChipSelectorIdpPNoData>& pList) {
    if (index < 0 || index >= pList.size()) {
        return;
    }
    ChipSelectorIdpPNoData& pNoData = pList[index];
    QSharedPointer<IdpMpParamListData>& paramData = pNoData.data;
    IdwLinkedList* list = CANVAS->localGetCheckedPoints(CHIP_SELECTED);
    if (!list || list->size() == 0) {
        return;
    }
    QSet<qint32> checker;
    checker.reserve(list->size());
    const IdwLinkedNode* node = list->first();
    while (node) {
        checker.insert(node->data);
        node = node->next;
    }
    QList<IdpMpTableModeData>& seqList = seqTableModel->getDataList();
    QList<qint32> waitToRemoveList;
    waitToRemoveList.reserve(list->size());
    for (int i = 0; i < seqList.size(); ++i) {
        IdpMpTableModeData& d = seqList[i];
        if (d.detailParamData != paramData || !checker.contains(d.posData->chipNumber)) {
            continue;
        }
        waitToRemoveList.append(i);
        CANVAS->checkItem(CHIP_CROSS_SELECTED, d.posData->chipNumber, 0);
    }
    if (waitToRemoveList.isEmpty()) {
        return;
    }
    seqTableModel->removeDatas(waitToRemoveList);
    QList<QSharedPointer<IdpMpPosListData>>& posList = pNoData.posList;
    for (int i = 0; i < posList.size();) {
        QSharedPointer<IdpMpPosListData>& p = posList[i];
        if (checker.contains(p->chipNumber)) {
            posList.remove(i);
            continue;
        }
        ++i;
    }
    pNoData.dNumber = reCalcPNoDataDNumber(pNoData.data->pNumber, seqList, oldPosDNumbers);
    pNoTableModel->notifyDataChanged(index, index);
}

void IdpChipSelectorController::deleteMeasurementPoint() {
    QList<int> indices = ui->pNoTable->getOrderedSelectedRows();
    if (indices.isEmpty()) {
        return;
    }
    ACaller->push(this, [this, indices](TSC) {
        QList<ChipSelectorIdpPNoData>& pList = pNoTableModel->getDataList();
        for (const int& index : indices) {
            _deleteMeasurementPoint(index, pList);
        }
        CANVAS->localUpdateWaferOnly();
    });
}

void IdpChipSelectorController::initIdwTopId(quint64 topId, const QList<qint32>& selectedNumbers) {
    getAsyncCaller()->push(this, [this, topId, selectedNumbers](TSC cmd){
        simpleLoadingBox(this, "Working", "Loading", true)
        IDW_WaferService* service = ServiceManager::instance()->getDefaultService<IDW_WaferService>();
        int errCode;
        IDW_WaferInfoEntity entity = service->getWaferInfoByTopId(topId, &errCode);
        if (errCode) {
            simpleMessageBox(this, "Error", "Load IDW info error.");
            return;
        }
        waferDataFromEntity(entity, *waferData, *shotData, *chipData);
        CCANVAS localGetCheckedPoints(CHIP_SELECTED)->clear();
        for (const qint32& n : selectedNumbers) {
            CCANVAS localCheckItem(CHIP_SELECTED, n, 1);
        }
        qint32 gid = waferData->getTegType() == 0 ? SHOT_TEG_CHECKED : CHIP_TEG_CHECKED;
        const QSet<qint32>& tegNumbers = waferData->getTegNumbers();
        if (tegNumbers.size() > 0) {
            for (auto it = tegNumbers.begin(); it != tegNumbers.end(); ++it) {
                CCANVAS localCheckItem(gid, *it);
            }
        }
        CCANVAS localUpdate(cmd);
    });
}

void IdpChipSelectorController::copyDataToSeqTable(
    const QList<IdpMpTableModeData>& list) {
    //函数的调度在UI线程上，本类的设计是所有的table数据强制在任务线程上处理，因此需要二次调度线程确保数据安全。
    ACaller->push(this, [this, list](TSC) {
        //pNo. -> dName
        MainThreadInvoker::getInvoker().blockRun([this, &list]() {
            oldPosDNumbers.clear();
            seqTableModel->clear();
            QList<IdpMpTableModeData>& seqList = seqTableModel->getDataList();
            seqList = list;
            seqTableModel->resizeViewList(seqList.size());
        });

        QList<quint64> allStaticTemplateIds;
        QList<quint64> allDynamicTemplateIds;
        QHash<QSharedPointer<IdpMpParamListData>, QList<QSharedPointer<IdpMpPosListData>>> pNoChecker;
        QList<IdpMpTableModeData>& seqList = seqTableModel->getDataList();
        QList<ChipSelectorIdpPNoData>& pList = pNoTableModel->getDataList();
        pList.clear();
        pList.reserve(10);
        for (IdpMpTableModeData& data : seqList) {
            oldPosDNumbers.insert(QPoint(data.posData->chipNumber, data.detailParamData->pNumber), data.posData->dNumbers);
            auto it = pNoChecker.find(data.detailParamData);
            if (it == pNoChecker.end()) {
                it = pNoChecker.insert(data.detailParamData,
                                       QList<QSharedPointer<IdpMpPosListData>>());
                ChipSelectorIdpPNoData pData;
                pData.data = data.detailParamData;
                pList.append(pData);
            }
            if (data.detailParamData->mpipTemplateId > 0) {
                if (data.detailParamData->mpipTemplateRefType == 0) {
                    allDynamicTemplateIds.append(data.detailParamData->mpipTemplateId);
                } else {
                    allStaticTemplateIds.append(data.detailParamData->mpipTemplateId);
                }
            }
            it->append(data.posData);
        }
        for (ChipSelectorIdpPNoData& data : pList) {
            data.posList = pNoChecker[data.data];
            collectViewDName(data.posList, data.dNumber);
        }
        pNoTableModel->resizeViewList(pList.size());
        //由于模板列表仅展示，不做操作，因此在查询动态连接的模板时不必关心映射到具体的哪个数据
        if (allDynamicTemplateIds.size() > 0 || allStaticTemplateIds.size() > 0) {
            TemplateInfoService* tempService = ServiceManager::instance()
                                                   ->getDefaultService<TemplateInfoService>();

            QList<TemplateInfoEntity> sList;

            int errCode;
            if (allStaticTemplateIds.size() > 0) {
                sList = tempService->getByIds(allStaticTemplateIds, &errCode);
                if (errCode) {
                    simpleMessageBox(this, "Error", "Load template list error.");
                    return;
                }
            }
            {
                QList<TemplateInfoEntity> dList;
                if (allDynamicTemplateIds.size() > 0) {
                    dList = tempService->getDatasByIdsForNewVersion(allDynamicTemplateIds, &errCode);
                    if (errCode) {
                        simpleMessageBox(this, "Error", "Load template list error.");
                        return;
                    }
                }
                if (sList.isEmpty() && dList.isEmpty()) {
                    return;
                }
                sList.reserve(sList.size() + dList.size());
                sList.append(dList);
            }
            std::sort(sList.begin(),
                      sList.end(),
                      [](const TemplateInfoEntity& e1, const TemplateInfoEntity& e2) {
                          return e1.getId() < e2.getId();
                      });
            QList<ChipSelectorIdpTemplateData>& tmpViewList = templateTableModel->getDataList();
            tmpViewList.reserve(sList.size());
            ChipSelectorIdpTemplateData data;
            for (TemplateInfoEntity& entity : sList) {
                MPIPTemplateInfoModel param;
                if (templateInfoModelDataFromEntity(entity, param)) {
                    simpleMessageBox(this, "Error", "Load Address template list error.");
                    return;
                }
                data.dPoint = {param.dx, param.dy};
                data.templateName = entity.getTemplateName();
                tmpViewList.append(data);
            }
            templateTableModel->resizeViewList(tmpViewList.size());
        }
    });
}
