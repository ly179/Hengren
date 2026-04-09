#include "idp_ap_wafer_selector_controller.h"
#include <QGridLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include "cde90_button.h"
#include "cde90_combo_box.h"
#include "cde90_label.h"
#include "cde90_message_box.h"
#include "cde90_process_box.h"
#include "cde90_spliter_line.h"
#include "cde90_text_field.h"
#include "idw_canvas.h"
#include "idw_canvas_item_check_style.h"
#include "idw_model_data_convert.h"
#include "idw_wafer_service.h"
#include "service_manager.h"
#include "idw_wafer_data_model.h"
#include "sem_global.h"

#define CHIP_TEG_CHECKED 6
#define SHOT_TEG_CHECKED 5

#define CANVAS idwCanvasContent->getCanvas()
#define CCANVAS idwCanvasContent->getCanvas()->
#define waferData CCANVAS localWafer()
#define shotData CCANVAS localShot()
#define chipData CCANVAS localChip()

struct IdpApWaferSelectDataHelper {
    IdpApWaferSelectorOk okCallback;

    QWidget* content;
    QVBoxLayout* contentLayout;

    QWidget* canvasContent;
    QHBoxLayout* canvasLayout;
    IdwCanvasContent* idwCanvasContent = nullptr;

    QWidget* controlContent;
    QHBoxLayout* controlLayout;

    QWidget* leftContent;
    QHBoxLayout* leftLayout;
    CDE90Label* chipPosLabel;
    CDE90Label* chipPosLabelRight;
    CDE90TextFieldBox* chipPosTextField;

    QWidget* rightContent;
    QGridLayout* rightLayout;
    CDE90Label* countLabel;
    CDE90TextFieldBox* countTextField;
    CDE90Label* magnifyLabel;
    CDE90ComboBox* magBtn;

    QWidget* bottomContent;
    QHBoxLayout* bottomLayout;
    CDE90ButtonBox* okBtn;
    CDE90ButtonBox* cancelBtn;

    std::atomic_bool singleSelectionMode = false;

    bool shiftKeyPressed = false;
    bool controlKeyPressed = false;
};

IdpApWaferSelectorController::IdpApWaferSelectorController(QWidget* parent): CDE90Dialog(parent) {
    connect(this, &CDE90Dialog::asyncCallerChanged, this, [this](){
        init();
    },Qt::SingleShotConnection);
    dataHelper = new IdpApWaferSelectDataHelper;
    create();
    createAction();
}
IdpApWaferSelectorController::~IdpApWaferSelectorController() noexcept {
    qDebug() << "ap wafer selector release.";
    if (dataHelper) delete dataHelper;
}

void IdpApWaferSelectorController::create() {
    dataHelper->content = new QWidget;
    dataHelper->contentLayout = new QVBoxLayout(dataHelper->content);
    dataHelper->contentLayout->setSpacing(5);
    dataHelper->contentLayout->setContentsMargins(0,0,0,0);
    dataHelper->content->setLayout(dataHelper->contentLayout);

    dataHelper->canvasContent = new QWidget(dataHelper->content);
    dataHelper->canvasContent->setMinimumSize(500,500);
    dataHelper->canvasContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    dataHelper->canvasLayout = new QHBoxLayout(dataHelper->canvasContent);
    dataHelper->canvasLayout->setContentsMargins(5,5,5,5);
    dataHelper->canvasContent->setLayout(dataHelper->canvasLayout);
    dataHelper->contentLayout->addWidget(dataHelper->canvasContent);

    dataHelper->controlContent = new QWidget(dataHelper->content);
    dataHelper->controlLayout = new QHBoxLayout(dataHelper->controlContent);
    dataHelper->controlLayout->setContentsMargins(5,5,5,5);
    dataHelper->controlLayout->setSpacing(5);
    dataHelper->controlContent->setLayout(dataHelper->controlLayout);
    dataHelper->contentLayout->addWidget(dataHelper->controlContent);

    dataHelper->leftContent = new QWidget(dataHelper->controlContent);
    dataHelper->leftLayout = new QHBoxLayout(dataHelper->leftContent);
    dataHelper->leftLayout->setAlignment(Qt::AlignTop);
    dataHelper->leftLayout->setContentsMargins(0,0,0,0);
    dataHelper->leftContent->setLayout(dataHelper->leftLayout);
    dataHelper->controlLayout->addWidget(dataHelper->leftContent);
    dataHelper->chipPosLabel = new CDE90Label("Chip X,Y (", dataHelper->leftContent);
    dataHelper->chipPosLabelRight = new CDE90Label(")", dataHelper->leftContent);
    dataHelper->chipPosTextField = new CDE90TextFieldBox(dataHelper->leftContent);
    dataHelper->chipPosTextField->setFixedWidth(100);
    dataHelper->chipPosTextField->getQLineEdit()->setAlignment(Qt::AlignCenter);
    dataHelper->leftLayout->addWidget(dataHelper->chipPosLabel);
    dataHelper->leftLayout->addWidget(dataHelper->chipPosTextField);
    dataHelper->leftLayout->addWidget(dataHelper->chipPosLabelRight);

    dataHelper->rightContent = new QWidget(dataHelper->controlContent);
    dataHelper->rightLayout = new QGridLayout(dataHelper->rightContent);
    dataHelper->rightLayout->setContentsMargins(0,0,0,0);
    dataHelper->rightLayout->setVerticalSpacing(5);
    dataHelper->rightLayout->setHorizontalSpacing(5);
    dataHelper->rightContent->setLayout(dataHelper->rightLayout);
    dataHelper->controlLayout->addItem(new QSpacerItem(10,10, QSizePolicy::Expanding, QSizePolicy::Preferred));
    dataHelper->controlLayout->addWidget(dataHelper->rightContent);

    dataHelper->countLabel = new CDE90Label("Count", dataHelper->rightContent);
    dataHelper->magnifyLabel = new CDE90Label("Magnify", dataHelper->rightContent);
    dataHelper->countTextField = new CDE90TextFieldBox(dataHelper->rightContent);
    dataHelper->countTextField->getQLineEdit()->setDisabled(true);
    dataHelper->countTextField->getQLineEdit()->setFixedWidth(50);
    dataHelper->magBtn = new CDE90ComboBox(dataHelper->rightContent);
    dataHelper->rightLayout->addWidget(dataHelper->countLabel, 0,0);
    dataHelper->rightLayout->addWidget(dataHelper->countTextField, 0,1);
    dataHelper->rightLayout->addWidget(dataHelper->magnifyLabel, 1,0);
    dataHelper->rightLayout->addWidget(dataHelper->magBtn, 1,1);

    dataHelper->contentLayout->addWidget(new CDE90SpliterHLine(dataHelper->content));

    dataHelper->bottomContent = new QWidget(dataHelper->content);
    dataHelper->bottomLayout = new QHBoxLayout(dataHelper->bottomContent);
    dataHelper->bottomLayout->setContentsMargins(5,5,5,5);
    dataHelper->bottomLayout->setSpacing(5);
    dataHelper->bottomContent->setLayout(dataHelper->bottomLayout);
    dataHelper->contentLayout->addWidget(dataHelper->bottomContent);

    dataHelper->okBtn = new CDE90ButtonBox("OK", dataHelper->bottomContent);
    dataHelper->cancelBtn = new CDE90ButtonBox("Cancel", dataHelper->bottomContent);
    dataHelper->bottomLayout->addWidget(dataHelper->okBtn);
    dataHelper->bottomLayout->addItem(new QSpacerItem(10,10, QSizePolicy::Expanding, QSizePolicy::Preferred));
    dataHelper->bottomLayout->addWidget(dataHelper->cancelBtn);

    setContent(dataHelper->content);
}

void IdpApWaferSelectorController::createAction() {
    dataHelper->magBtn->addItems(QStringList() << "1x" << "2x" << "3x" << "4x");
    bindCloseBtn(dataHelper->cancelBtn->getQPushButton());
    connect(dataHelper->okBtn->getQPushButton(), &CDE90Button::clicked, this, [this](){
        if (dataHelper->okCallback) {
            dataHelper->okBtn->loading();
            dataHelper->cancelBtn->getQPushButton()->setDisabled(true);
            bool succ = getAsyncCaller()->push(this, [this](TSC){
                IdwLinkedList* items = dataHelper->CANVAS->localGetCheckedPoints(1);
                if (items->isEmpty()) {
                    MainThreadInvoker::getInvoker().runLater([this](){
                        dataHelper->okBtn->overLoading();
                        dataHelper->cancelBtn->getQPushButton()->setDisabled(false);
                        simpleMessageBox(this, "Note", "It is required to specify at least one chip.");
                    });
                    return;
                }
                const IdwLinkedNode* node = items->first();
                QList<IdpApSelectedChip> list(items->size());
                int i = 0;
                QPoint p;
                bool succ;
                while (node) {
                    IdpApSelectedChip &r = list[i];
                    r.number = node->data;
                    p = dataHelper->CANVAS->localFindChipRulerByNumber(r.number);
                    r.x = p.x();
                    r.y = p.y();
                    r.oPoint = getChipCoordinateByChipNumber(r.number, dataHelper->CANVAS->localShot().data(), dataHelper->CANVAS->localChip().data(), &succ);
                    if (!succ) {
                        simpleMessageBox(this, "Error", "Data error, cannot load chip coordinate.");
                        return;
                    }
                    node = node->next;
                    ++ i;
                }
                MainThreadInvoker::getInvoker().blockRun([this, &list](){
                    dataHelper->okCallback(list);
                    changeCloseStatusOnce(HW_OK);
                    close();
                });
            });
            if (!succ) {
                dataHelper->okBtn->overLoading();
                dataHelper->cancelBtn->setDisabled(false);
                simpleMessageBox(this, "Error", "Operation failed.");
            }
            return;
        }
        close();
    });
}

void IdpApWaferSelectorController::init() {
    if (dataHelper->idwCanvasContent) {
        return;
    }
    dataHelper->idwCanvasContent = new IdwCanvasContent(dataHelper->canvasContent);
    getAsyncCaller()->manage(dataHelper->idwCanvasContent, this);
    getAsyncCaller()->manage(dataHelper->idwCanvasContent->getCanvas(), dataHelper->idwCanvasContent);
    dataHelper->idwCanvasContent->setAsyncCaller(getAsyncCaller());
    dataHelper->idwCanvasContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    dataHelper->canvasLayout->addWidget(dataHelper->idwCanvasContent);
    dataHelper->CANVAS->registSelectedHandler({nullptr, qint32(qRgb(255,255,0)), IDW_CM_CHIP, 1});
    dataHelper->CCANVAS registSelectedHandler(
        {QSharedPointer<SelectNumberDrawerHandler>(IDW_DEFAULT_TEG_CHECK_STYLE),
         0,
         IDW_CM_SHOT,
         SHOT_TEG_CHECKED});
    dataHelper->CCANVAS registSelectedHandler(
        {QSharedPointer<SelectNumberDrawerHandler>(IDW_DEFAULT_TEG_CHECK_STYLE),
         0,
         IDW_CM_CHIP,
         CHIP_TEG_CHECKED});

    //初始化画布参数
    getAsyncCaller()->push(this, [this](TSC){
        dataHelper->CCANVAS localSetDisplayMode(IDW_DM_MIN);
        dataHelper->CCANVAS localSetMouseCheckUnit(IDW_CM_CHIP);
    });

    connect(dataHelper->CANVAS, &IdwCanvas::itemChecked, this, [this](const IDW_UnitPointInfo& info){
        if (info.repeat) {
            return;
        }
        if (info.number3 < 0) {
            dataHelper->chipPosTextField->getQLineEdit()->clear();
            return;
        }
        dataHelper->chipPosTextField->getQLineEdit()->setText(QString::number(info.x) + ", " + QString::number(info.y));
        int number = info.number3;
        bool needClearOld = !dataHelper->shiftKeyPressed && !dataHelper->controlKeyPressed;
        getAsyncCaller()->push(this, [this, number, needClearOld](TSC){
            IdwLinkedList* list = dataHelper->CCANVAS localGetCheckedPoints(1);
            bool donothing = false;
            if (dataHelper->singleSelectionMode.load(std::memory_order_acquire)) {
                list->clear();
            } else if (needClearOld) {
                donothing = list->contains(number);
                list->clear();
            }
            if (!donothing)
                dataHelper->CCANVAS localCheckItem(1, number);
            dataHelper->CCANVAS localUpdateWaferOnly();
            qint32 size = dataHelper->CCANVAS localGetCheckedPoints(1)->size();
            MainThreadInvoker::getInvoker().runLater([this, size](){
                dataHelper->countTextField->getQLineEdit()->setText(QString::number(size));
            });
        });
    });

    connect(dataHelper->magBtn->getQComboBox(), &UComboBox::currentIndexChanged, this, [this](int mag){
        mag = mag + 1;
        getAsyncCaller()->push(this, [this, mag](TSC cmd){
            dataHelper->CCANVAS localSetMag(mag);
            dataHelper->CCANVAS localUpdate(cmd);
        });
    });
    installEventFilter(this);
}

bool IdpApWaferSelectorController::eventFilter(QObject *watched, QEvent *event) {
    if (watched != this || (event->type() != QEvent::KeyPress && event->type() != QEvent::KeyRelease))
        return CDE90Dialog::eventFilter(watched, event);
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
    if (keyEvent->key() == Qt::Key_Shift) {
        dataHelper->shiftKeyPressed = keyEvent->type() == QEvent::KeyPress;
        dataHelper->CCANVAS setMousePressAsClick(dataHelper->shiftKeyPressed);
        dataHelper->CCANVAS setAllowMouseSlideAsPress(dataHelper->shiftKeyPressed);
        return true;
    } else if (keyEvent->key() == Qt::Key_Control) {
        dataHelper->controlKeyPressed = keyEvent->type() == QEvent::KeyPress;
        return true;
    }
    return CDE90Dialog::eventFilter(watched, event);
}

void IdpApWaferSelectorController::setOkCallback(const IdpApWaferSelectorOk& callback) {
    dataHelper->okCallback = callback;
}

void IdpApWaferSelectorController::initIdwTopId(quint64 topId, const QList<qint32>& selectedNumbers) {
    getAsyncCaller()->push(this, [this, topId, selectedNumbers](TSC cmd){
        simpleLoadingBox(this, "Working", "Loading", true)
        IDW_WaferService* service = ServiceManager::instance()->getDefaultService<IDW_WaferService>();
        int errCode;
        IDW_WaferInfoEntity entity = service->getWaferInfoByTopId(topId, &errCode);
        if (errCode) {
            simpleMessageBox(this, "Error", "Load IDW info error.");
            return;
        }
        waferDataFromEntity(entity, *dataHelper->waferData, *dataHelper->shotData, *dataHelper->chipData);
        IdwLinkedList* list = dataHelper->CCANVAS localGetCheckedPoints(1);
        list->clear();
        for (const qint32& n : selectedNumbers) {
            dataHelper->CCANVAS localCheckItem(1, n, 1);
        }
        qint32 gid = dataHelper->waferData->getTegType() == 0 ? SHOT_TEG_CHECKED : CHIP_TEG_CHECKED;
        const QSet<qint32>& tegNumbers = dataHelper->waferData->getTegNumbers();
        if (tegNumbers.size() > 0) {
            for (auto it = tegNumbers.begin(); it != tegNumbers.end(); ++it) {
                dataHelper->CCANVAS localCheckItem(gid, *it);
            }
        }
        dataHelper->CCANVAS localUpdate(cmd);
        int size = list->size();
        MainThreadInvoker::getInvoker().runLater([this, size](){
            dataHelper->countTextField->setText(size);
        });
    });
}

void IdpApWaferSelectorController::setSingleSelectMode(bool b) {
    dataHelper->singleSelectionMode.store(b, std::memory_order_release);
}
