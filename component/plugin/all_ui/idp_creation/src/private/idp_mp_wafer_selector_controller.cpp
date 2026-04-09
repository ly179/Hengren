#include "idp_mp_wafer_selector_controller.h"
#include "cde90_message_box.h"
#include "cde90_process_box.h"
#include "cdsem_global_resource.h"
#include "cdsem_number_validator_helper.h"
#include "cdsem_textfield_number_validator.h"
#include "chip_map_helper.h"
#include "font_metrics_content.h"
#include "idw_canvas_item_check_style.h"
#include "idw_model_data_convert.h"
#include "idw_wafer_data_model.h"
#include "idw_wafer_service.h"
#include "service_manager.h"
#include "ui_idp_mp_wafer_selector_controller.h"
#include "sem_global.h"

#define CHIP_TEG_CHECKED 1
#define SHOT_TEG_CHECKED 2
#define CHIP_GRAY_CHECKED 3
#define CHIP_NORMAL_CHECKED 4

#define CANVAS ui->idwCanvasContent->getCanvas()
#define CCANVAS ui->idwCanvasContent->getCanvas()->
#define waferData CCANVAS localWafer()
#define shotData CCANVAS localShot()
#define chipData CCANVAS localChip()

struct IdpMpWaferSelectHelper {
    ChipMapHelper* helper = nullptr;
    CDSemIntValidator* seqValidator;

    std::atomic_bool singleSelectionMode = false;
    std::atomic_bool shiftKeyPressed = false;
    std::atomic_bool controlKeyPressed = false;
    std::atomic_int32_t nextSeq = 1;

    int insertNumber = -1;

    int startSeq = 1;

    inline void clearInsertNumber() {
        if (insertNumber >= 0) insertNumber = -1;
    }
};

class MpSelectedBgHandler : public SelectNumberDrawerHandler {
private:
    QBrush bgBrush;
    QPen bgPen;
    QPen textPen;
    QTextOption textOption;
    FontMetricsContent fontMetrics;
    IdpMpWaferSelectHelper* dataHelper = nullptr;
    int order;
    int height;
public:
    explicit MpSelectedBgHandler(IdpMpWaferSelectHelper* dataHelper) : dataHelper(dataHelper) {
        bgBrush = QBrush(QColor(255, 255, 0));
        bgPen = QPen(bgBrush, 1);
        textPen = QPen(Qt::black);
        textOption = QTextOption(Qt::AlignCenter);
        rendOrdered = true;
    }
    void begin(QPainter& painter) override{
        painter.setBrush(bgBrush);
        order = dataHelper->startSeq;
        fontMetrics = painter.fontMetrics();
        height = fontMetrics->ascent() + fontMetrics->descent();
    }

    void paint(const QRect& rect, QPainter& painter, qint32) override{
        painter.setPen(bgPen);
        painter.drawRect(sem::getDrawRect(rect));
        if (dataHelper && dataHelper->singleSelectionMode.load(std::memory_order_acquire)) {
            return;
        }
        QString text = QString::number(order);
        QRect range = fontMetrics->tightBoundingRect(text);
        if (range.width() <= rect.width() - 5 && range.height() <= height - 5) {
            painter.setPen(textPen);
            painter.drawText(rect.x() + rect.width() / 2 - range.width() / 2, rect.y() + rect.height() / 2 - height / 2 + fontMetrics->ascent(), text);
        }
        ++ order;
    }
};

IdpMpWaferSelectorController::IdpMpWaferSelectorController(QWidget* parent) : CDE90Dialog(parent) {
    connect(this, &CDE90Dialog::asyncCallerChanged, this, [this](){
        create();
        createAction();
    }, Qt::SingleShotConnection);
}

IdpMpWaferSelectorController::~IdpMpWaferSelectorController() noexcept {
    if (ui) {
        delete ui;
    }
    if (dataHelper) {
        if (dataHelper->helper) {
            delete dataHelper->helper;
        }
        delete dataHelper;
    }
}

void IdpMpWaferSelectorController::create() {
    dataHelper = new IdpMpWaferSelectHelper;
    ui = new cdsem::UiIdpMpWaferSelectorController;
    setContent(new QWidget);
    ui->init(getContent());
    ui->upBtn->getQPushButton()->setIconSize(QSize(25, 25));
    ui->upBtn->getQPushButton()->setIcon(ResourceManager::getResourceManager().getIdwShiftUpIcon());
    ui->downBtn->getQPushButton()->setIconSize(QSize(25, 25));
    ui->downBtn->getQPushButton()->setIcon(
        ResourceManager::getResourceManager().getIdwShiftDownIcon());
    ui->leftBtn->getQPushButton()->setIconSize(QSize(25, 25));
    ui->leftBtn->getQPushButton()->setIcon(
        ResourceManager::getResourceManager().getIdwShiftLeftIcon());
    ui->rightBtn->getQPushButton()->setIconSize(QSize(25, 25));
    ui->rightBtn->getQPushButton()->setIcon(
        ResourceManager::getResourceManager().getIdwShiftRightIcon());
    ui->seqUpBtn->getQPushButton()->setIconSize(QSize(20, 20));
    ui->seqUpBtn->getQPushButton()->setIcon(
        ResourceManager::getResourceManager().getIdwShiftUpIcon());
    ui->seqDownBtn->getQPushButton()->setIconSize(QSize(20, 20));
    ui->seqDownBtn->getQPushButton()->setIcon(
        ResourceManager::getResourceManager().getIdwShiftDownIcon());

}

void IdpMpWaferSelectorController::createAction() {
    dataHelper->seqValidator = CDSemNumberValidatorHelper::range(dataHelper->startSeq, dataHelper->startSeq)
                       .autoFixMode()
                       .put(ui->seqNoTextField->getQLineEdit())
                       .intValiator();
    dataHelper->helper = new ChipMapHelper(CANVAS);
    bindCloseBtn(ui->cancelBtn->getQPushButton());
    ui->idwCanvasContent->setAsyncCaller(getAsyncCaller());
    itsChildAndBeManaged(ui->idwCanvasContent, this);
    itsChildAndBeManaged(ui->idwCanvasContent->getCanvas(), ui->idwCanvasContent);
    CANVAS->registSelectedHandler(
        {QSharedPointer<SelectNumberDrawerHandler>(new MpSelectedBgHandler(dataHelper)), 0, IDW_CM_CHIP, CHIP_NORMAL_CHECKED});
    CANVAS->registSelectedHandler({nullptr, qint32(qRgb(215, 215, 215)), IDW_CM_CHIP, CHIP_GRAY_CHECKED});
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
    //初始化画布参数
    getAsyncCaller()->push(this, [this](TSC){
        CCANVAS localSetDisplayMode(IDW_DM_MIN);
        CCANVAS localSetMouseCheckUnit(IDW_CM_CHIP);
    });

    connect(ui->seqNoTextField->getQLineEdit(), &CDE90TextField::editingFinished, this, [this]() {
        dataHelper->clearInsertNumber();
    });

    connect(ui->seqUpBtn->getQPushButton(), &CDE90Button::clicked, this, [this]() {
        qint32 seq = ui->seqNoTextField->text().toInt() + 1;
        if (seq > dataHelper->seqValidator->top()) {
            return;
        }
        ui->seqNoTextField->setText(seq);
    });

    connect(ui->seqDownBtn->getQPushButton(), &CDE90Button::clicked, this, [this]() {
        qint32 seq = ui->seqNoTextField->text().toInt() - 1;
        if (seq < dataHelper->seqValidator->bottom()) {
            return;
        }
        ui->seqNoTextField->setText(seq);
    });

    connect(ui->seqNoTextField->getQLineEdit(), &QLineEdit::textChanged, this, [this](const QString& text) {
        if (text.isEmpty()) return;
        bool ok;
        int i = text.toInt(&ok);
        if (!ok) return;
        dataHelper->nextSeq.store(i, std::memory_order_release);
        dataHelper->clearInsertNumber();
    });

    CANVAS->localSetItemCheckedCallback([this](const IDW_UnitPointInfo& info){
        if (info.repeat) {
            return;
        }
        if (info.number3 < 0) {
            MainThreadInvoker::getInvoker().runLater(
                [this]() { ui->chipNoTextField->getQLineEdit()->clear(); });
            return;
        }
        QString chipLabelInfo = QString::number(info.x) + ", " + QString::number(info.y);
        MainThreadInvoker::getInvoker().runLater([this, chipLabelInfo]() mutable {
            ui->chipNoTextField->getQLineEdit()->setText(chipLabelInfo);
        });
        int number = info.number3;
        // CCANVAS localCheckItem(1, number);
        int nextSeq = dataHelper->nextSeq.load(std::memory_order_acquire);
        IdwLinkedList* list = CANVAS->localGetCheckedPoints(CHIP_NORMAL_CHECKED);
        if (dataHelper->singleSelectionMode.load(std::memory_order_acquire)) {
            list->clear();
            list->append(number);
            nextSeq = list->size() + dataHelper->startSeq;
        } else if (!dataHelper->shiftKeyPressed.load(std::memory_order_acquire) && !dataHelper->controlKeyPressed.load(std::memory_order_acquire)) {
            bool needAppend = !list->contains(number);
            list->clear();
            if (needAppend)
                list->append(number);
            nextSeq = list->size() + dataHelper->startSeq;
        } else {
            if (list->contains(number)) {
                list->remove(number);
                dataHelper->clearInsertNumber();
                if (nextSeq > list->size() + dataHelper->startSeq) {
                    nextSeq = list->size() + dataHelper->startSeq;
                }
            } else {
                if (nextSeq > list->size()) {
                    list->append(number);
                    dataHelper->clearInsertNumber();
                    nextSeq = list->size() + dataHelper->startSeq;
                } else {
                    if (dataHelper->insertNumber < 0) {
                        int insertIndex = 0;
                        const IdwLinkedNode* node = list->first();
                        while (node) {
                            if (insertIndex == nextSeq - dataHelper->startSeq) {
                                dataHelper->insertNumber = node->data;
                                break;
                            }
                            node = node->next;
                            ++ insertIndex;
                        }
                    }
                    list->insert(dataHelper->insertNumber, number);
                    dataHelper->insertNumber = number;
                    qDebug() << "insert next seq: " << nextSeq;
                }
            }
        }
        CCANVAS localUpdateWaferOnly();
        qint32 size = list->size();
        MainThreadInvoker::getInvoker().runLater([this, size, nextSeq](){
            dataHelper->seqValidator->setTop(size + 1);
            ui->countTextField->setText(size);
            ui->seqNoTextField->setText(nextSeq);
            dataHelper->nextSeq.store(nextSeq, std::memory_order_release);
        });
    });

    connect(ui->magBox->getQComboBox(), &UComboBox::currentIndexChanged, this, [this](int mag) {
        mag = mag + 1;
        getAsyncCaller()->push(this, [this, mag](TSC cmd){
            CCANVAS localSetMag(mag);
            CCANVAS localUpdate(cmd);
        });
    });

    connect(ui->allBtn->getQPushButton(), &CDE90Button::clicked, this, [this]() {
        getAsyncCaller()->push(this, [this](TSC){
            QList<IDW_Data_Item>* shotList = CANVAS->localShot()->getDataList();
            QList<IDW_Data_Item>* chipList = CANVAS->localChip()->getDataList();
            const QSet<qint32>& invalidNumbers = CANVAS->localGetInvalidChipNumbers();
            if (!shotList || !chipList) {
                return;
            }
            int total = shotList->size() * chipList->size() - invalidNumbers.size();
            int chipNumber;
            bool succ;
            for (IDW_Data_Item& shotItem : *shotList) {
                for (IDW_Data_Item& chipItem : *chipList) {
                    chipNumber = getChipNumberByShotChipNumber(shotItem.getNumber(), chipItem.getNumber(), CANVAS->localChip().data(), &succ);
                    if (!succ) {
                        simpleMessageBox(this, "Error", "Data error, cannot load chip coordinate.");
                        return;
                    }
                    if (invalidNumbers.contains(chipNumber)) {
                        continue;
                    }
                    CANVAS->localCheckItem(CHIP_NORMAL_CHECKED, chipNumber, true);
                }
            }
            CANVAS->localUpdateWaferOnly();
            MainThreadInvoker::getInvoker().runLater([this, total]() {
                ui->countTextField->setText(total);
                ui->seqNoTextField->setText(total + 1);
            });
        });
    });

    connect(ui->sortBtn->getQPushButton(), &CDE90Button::clicked, this, [this]() {
        int orderIndex = ui->sortOrderBox->currentIndex();
        getAsyncCaller()->push(this, [this, orderIndex](TSC){
            IdwLinkedList* list = CANVAS->localGetCheckedPoints(CHIP_NORMAL_CHECKED);
            if (!list || list->isEmpty()) {
                return;
            }
            QList<qint32> numbers;
            numbers.reserve(list->size());
            const IdwLinkedNode* node = list->first();
            while (node) {
                numbers.append(node->data);
                node = node->next;
            }
            std::sort(numbers.begin(),
                      numbers.end(),
                      orderIndex == 0 ? dataHelper->helper->getLocalColumnSortFunc()
                                      : dataHelper->helper->getLocalRowSortFunc());
            list->clear();
            for (qint32& number : numbers) {
                CANVAS->localCheckItem(CHIP_NORMAL_CHECKED, number, true);
            }
            CANVAS->localUpdateWaferOnly();
        });
    });

    connect(ui->upBtn->getQPushButton(), &CDE90Button::clicked, this, [this]() {
        ACaller->push(this, [this](TSC) {
            ChipMapHandlerResult result = dataHelper->helper->localShiftWork(CHIP_NORMAL_CHECKED, Qt::Vertical, 1);
            if (result.error) {
                simpleMessageBox(this, result.title, result.msg);
            } else if (result.moved) {
                CANVAS->localUpdateWaferOnly();
            }
        });
    });
    connect(ui->downBtn->getQPushButton(), &CDE90Button::clicked, this, [this]() {
        ACaller->push(this, [this](TSC) {
            ChipMapHandlerResult result = dataHelper->helper->localShiftWork(CHIP_NORMAL_CHECKED, Qt::Vertical, -1);
            if (result.error) {
                simpleMessageBox(this, result.title, result.msg);
            } else if (result.moved) {
                CANVAS->localUpdateWaferOnly();
            }
        });
    });
    connect(ui->leftBtn->getQPushButton(), &CDE90Button::clicked, this, [this]() {
        ACaller->push(this, [this](TSC) {
            ChipMapHandlerResult result = dataHelper->helper->localShiftWork(CHIP_NORMAL_CHECKED, Qt::Horizontal, -1);
            if (result.error) {
                simpleMessageBox(this, result.title, result.msg);
            } else if (result.moved) {
                CANVAS->localUpdateWaferOnly();
            }
        });
    });
    connect(ui->rightBtn->getQPushButton(), &CDE90Button::clicked, this, [this]() {
        ACaller->push(this, [this](TSC) {
            ChipMapHandlerResult result = dataHelper->helper->localShiftWork(CHIP_NORMAL_CHECKED, Qt::Horizontal, 1);
            if (result.error) {
                simpleMessageBox(this, result.title, result.msg);
            } else if (result.moved) {
                CANVAS->localUpdateWaferOnly();
            }
        });
    });

    connect(ui->okBtn->getQPushButton(), &CDE90Button::clicked, this, [this]() {
        if (okCallback) {
            ui->cancelBtn->getQPushButton()->setDisabled(true);
            bool succ = getAsyncCaller()->push(this, [this](TSC){
                IdwLinkedList* items = CANVAS->localGetCheckedPoints(CHIP_NORMAL_CHECKED);
                if (items->isEmpty()) {
                    MainThreadInvoker::getInvoker().runLater([this]() {
                        simpleMessageBox(this, "Note", "It is required to specify at least one chip.");
                    });
                    return;
                }
                const IdwLinkedNode* node = items->first();
                QList<IdpMpSelectedChip> list(items->size());
                int i = 0;
                QPoint p;
                bool succ;
                while (node) {
                    IdpMpSelectedChip &r = list[i];
                    r.number = node->data;
                    p = CANVAS->localFindChipRulerByNumber(r.number);
                    r.x = p.x();
                    r.y = p.y();
                    r.oPoint = getChipCoordinateByChipNumber(r.number, CANVAS->localShot().data(), CANVAS->localChip().data(), &succ);
                    if (!succ) {
                        simpleMessageBox(this, "Error", "Data error, cannot load chip coordinate.");
                        return;
                    }
                    node = node->next;
                    ++ i;
                }
                MainThreadInvoker::getInvoker().blockRun([this, &list](){
                    if (!okCallback(list)) return;
                    changeCloseStatusOnce(HW_OK);
                    close();
                });
            });
            if (succ) {
                succ = getAsyncCaller()->pushFocus(this, [this](TSC){
                    MainThreadInvoker::getInvoker().runLater([this](){
                        ui->okBtn->overLoading();
                        ui->cancelBtn->setDisabled(false);
                    });
                });
            }
            if (!succ) {
                ui->okBtn->overLoading();
                ui->cancelBtn->setDisabled(false);
                simpleMessageBox(this, "Error", "Operation failed.");
            }
            return;
        }
        close();
    });

    installEventFilter(this);
}

bool IdpMpWaferSelectorController::eventFilter(QObject *watched, QEvent *event) {
    if (this != watched || (event->type() != QEvent::KeyPress && event->type() != QEvent::KeyRelease))
        return CDE90Dialog::eventFilter(watched, event);
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
    if (keyEvent->key() == Qt::Key_Shift) {
        bool pressed = keyEvent->type() == QEvent::KeyPress;
        dataHelper->shiftKeyPressed.store(pressed, std::memory_order_release);
        ui->idwCanvasContent->getCanvas()->setAllowMouseSlideAsPress(pressed);
        ui->idwCanvasContent->getCanvas()->setMousePressAsClick(pressed);
        return true;
    } else if (keyEvent->key() == Qt::Key_Control) {
        dataHelper->controlKeyPressed.store(keyEvent->type() == QEvent::KeyPress, std::memory_order_release);
        return true;
    }
    return CDE90Dialog::eventFilter(watched, event);
}


void IdpMpWaferSelectorController::initIdwTopId(quint64 topId, const QList<qint32>& selectedNumbers, const QList<qint32>& grayNumbers) {
    getAsyncCaller()->push(this, [this, topId, selectedNumbers, grayNumbers](TSC cmd){
        simpleLoadingBox(this, "Working", "Loading", true)
        IDW_WaferService* service = ServiceManager::instance()->getDefaultService<IDW_WaferService>();
        int errCode;
        IDW_WaferInfoEntity entity = service->getWaferInfoByTopId(topId, &errCode);
        if (errCode) {
            simpleMessageBox(this, "Error", "Load IDW info error.");
            return;
        }
        waferDataFromEntity(entity, *waferData, *shotData, *chipData);
        IdwLinkedList* list = CCANVAS localGetCheckedPoints(CHIP_NORMAL_CHECKED);
        list->clear();
        for (const qint32& n : selectedNumbers) {
            CCANVAS localCheckItem(CHIP_NORMAL_CHECKED, n, 1);
        }
        list = CCANVAS localGetCheckedPoints(CHIP_GRAY_CHECKED);
        list->clear();
        for (const qint32& n : grayNumbers) {
            CCANVAS localCheckItem(CHIP_GRAY_CHECKED, n, 1);
        }
        qint32 gid = waferData->getTegType() == 0 ? SHOT_TEG_CHECKED : CHIP_TEG_CHECKED;
        const QSet<qint32>& tegNumbers = waferData->getTegNumbers();
        if (tegNumbers.size() > 0) {
            for (auto it = tegNumbers.begin(); it != tegNumbers.end(); ++it) {
                CCANVAS localCheckItem(gid, *it);
            }
        }
        CCANVAS localUpdate(cmd);
        int size = list->size();
        MainThreadInvoker::getInvoker().runLater([this, size](){
            dataHelper->seqValidator->setTop(size + dataHelper->startSeq);
            ui->countTextField->setText(size);
            ui->seqNoTextField->setText(size + dataHelper->startSeq);
        });
    });
}

void IdpMpWaferSelectorController::setSingleSelectMode(bool b) {
    dataHelper->singleSelectionMode.store(b, std::memory_order_release);
    ui->sortContent->setVisible(!b);
}

void IdpMpWaferSelectorController::setStartSeq(int seq) {
    ACaller->push(this, [this, seq](TSC){
        dataHelper->startSeq = seq;
    });
    dataHelper->seqValidator->setBottom(seq);
    dataHelper->seqValidator->setTop(seq);
    ui->seqNoTextField->setText(seq);
    dataHelper->nextSeq.store(seq, std::memory_order_release);
}
