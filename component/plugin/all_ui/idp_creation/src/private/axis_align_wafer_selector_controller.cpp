#include "axis_align_wafer_selector_controller.h"
#include <QVBoxLayout>
#include "cde90_button.h"
#include "cde90_combo_box.h"
#include "cde90_label.h"
#include "cde90_message_box.h"
#include "cde90_process_box.h"
#include "cde90_spliter_line.h"
#include "cde90_text_field.h"
#include "idw_canvas.h"
#include "idw_model_data_convert.h"
#include "idw_wafer_service.h"
#include "service_manager.h"
#include "idw_wafer_data_model.h"
#include "sem_global.h"

#define CANVAS idwCanvasContent->getCanvas()
#define CCANVAS idwCanvasContent->getCanvas()->
#define waferData CCANVAS localWafer()
#define shotData CCANVAS localShot()
#define chipData CCANVAS localChip()

AxisAlignWaferSelectorController::AxisAlignWaferSelectorController(
    QWidget* parent)
    : CDE90Window(parent) {
    connect(
        this, &CDE90Window::asyncCallerChanged, this, [this]() { init(); }, Qt::SingleShotConnection);
    create();
    createAction();
}

AxisAlignWaferSelectorController::~AxisAlignWaferSelectorController() noexcept {}

void AxisAlignWaferSelectorController::create() {
    content = new QWidget;
    contentLayout = new QVBoxLayout(content);
    contentLayout->setSpacing(5);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    content->setLayout(contentLayout);

    canvasContent = new QWidget(content);
    canvasContent->setMinimumSize(500, 500);
    canvasContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    canvasLayout = new QHBoxLayout(canvasContent);
    canvasLayout->setContentsMargins(5, 5, 5, 5);
    canvasContent->setLayout(canvasLayout);
    contentLayout->addWidget(canvasContent);

    controlContent = new QWidget(content);
    controlLayout = new QHBoxLayout(controlContent);
    controlLayout->setContentsMargins(5, 5, 5, 5);
    controlLayout->setSpacing(5);
    controlContent->setLayout(controlLayout);
    contentLayout->addWidget(controlContent);

    leftContent = new QWidget(controlContent);
    leftLayout = new QHBoxLayout(leftContent);
    leftLayout->setAlignment(Qt::AlignTop);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftContent->setLayout(leftLayout);
    controlLayout->addWidget(leftContent);
    chipPosLabel = new CDE90Label("Chip X,Y (", leftContent);
    chipPosLabelRight = new CDE90Label(")", leftContent);
    chipPosTextField = new CDE90TextFieldBox(leftContent);
    chipPosTextField->setFixedWidth(100);
    chipPosTextField->getQLineEdit()->setAlignment(Qt::AlignCenter);
    leftLayout->addWidget(chipPosLabel);
    leftLayout->addWidget(chipPosTextField);
    leftLayout->addWidget(chipPosLabelRight);

    rightContent = new QWidget(controlContent);
    rightLayout = new QHBoxLayout(rightContent);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(5);
    rightContent->setLayout(rightLayout);
    controlLayout->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Preferred));
    controlLayout->addWidget(rightContent);

    magnifyLabel = new CDE90Label("Magnify", rightContent);
    magBtn = new CDE90ComboBox(rightContent);
    rightLayout->addWidget(magnifyLabel);
    rightLayout->addWidget(magBtn);

    contentLayout->addWidget(new CDE90SpliterHLine(content));

    bottomContent = new QWidget(content);
    bottomLayout = new QHBoxLayout(bottomContent);
    bottomLayout->setContentsMargins(5, 5, 5, 5);
    bottomLayout->setSpacing(5);
    bottomContent->setLayout(bottomLayout);
    contentLayout->addWidget(bottomContent);

    okBtn = new CDE90ButtonBox("OK", bottomContent);
    cancelBtn = new CDE90ButtonBox("Cancel", bottomContent);
    bottomLayout->addWidget(okBtn);
    bottomLayout->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Preferred));
    bottomLayout->addWidget(cancelBtn);

    setContent(content);
}
void AxisAlignWaferSelectorController::createAction() {
    magBtn->addItems(QStringList() << "1x" << "2x" << "3x" << "4x");
    bindCloseBtn(cancelBtn->getQPushButton());
    connect(okBtn->getQPushButton(), &CDE90Button::clicked, this, [this]() {
        if (okCallback) {
            okBtn->loading();
            cancelBtn->getQPushButton()->setDisabled(true);
            bool succ = getAsyncCaller()->push(this, [this](TSC) {
                IdwLinkedList* items = CANVAS->localGetCheckedPoints(1);
                if (items->isEmpty()) {
                    MainThreadInvoker::getInvoker().runLater([this]() {
                        okBtn->overLoading();
                        cancelBtn->getQPushButton()->setDisabled(false);
                        simpleMessageBox(this,
                                         "Note",
                                         "It is required to specify at least one chip.");
                    });
                    return;
                }
                const IdwLinkedNode* node = items->first();
                AxisAlignSelectedChip r;
                r.number = node->data;
                QPoint p = CANVAS->localFindChipRulerByNumber(r.number);
                r.x = p.x();
                r.y = p.y();
                bool succ;
                r.oPoint = getChipCoordinateByChipNumber(r.number,
                                                         CANVAS->localShot().data(),
                                                         CANVAS->localChip().data(),
                                                         &succ);
                if (!succ) {
                    simpleMessageBox(this, "Error", "Data error, cannot load chip coordinate.");
                    return;
                }
                MainThreadInvoker::getInvoker().blockRun([this, &r]() {
                    okCallback(r);
                    changeCloseStatusOnce(HW_OK);
                    close();
                });
            });
            if (!succ) {
                okBtn->overLoading();
                cancelBtn->setDisabled(false);
                simpleMessageBox(this, "Error", "Operation failed.");
            }
            return;
        }
        close();
    });
}

void AxisAlignWaferSelectorController::init() {
    if (idwCanvasContent) {
        return;
    }
    idwCanvasContent = new IdwCanvasContent(canvasContent);
    getAsyncCaller()->manage(idwCanvasContent, this);
    getAsyncCaller()->manage(idwCanvasContent->getCanvas(), idwCanvasContent);
    idwCanvasContent->setAsyncCaller(getAsyncCaller());
    idwCanvasContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    canvasLayout->addWidget(idwCanvasContent);
    CANVAS->registSelectedHandler({nullptr, qint32(qRgb(255, 255, 0)), IDW_CM_CHIP, 1});

    //初始化画布参数
    getAsyncCaller()->push(this, [this](TSC) {
        CCANVAS localSetDisplayMode(IDW_DM_MIN);
        CCANVAS localSetMouseCheckUnit(IDW_CM_CHIP);
    });

    connect(CANVAS, &IdwCanvas::itemChecked, this, [this](const IDW_UnitPointInfo& info) {
        if (info.repeat) {
            return;
        }
        if (info.number3 < 0) {
            chipPosTextField->getQLineEdit()->clear();
            return;
        }
        chipPosTextField->getQLineEdit()->setText(QString::number(info.x) + ", "
                                                  + QString::number(info.y));
        int number = info.number3;
        getAsyncCaller()->push(this, [this, number](TSC) {
            CCANVAS localGetCheckedPoints(1)->clear();
            CCANVAS localCheckItem(1, number);
            CCANVAS localUpdateWaferOnly();
        });
    });

    connect(magBtn->getQComboBox(), &UComboBox::currentIndexChanged, this, [this](int mag) {
        mag = mag + 1;
        getAsyncCaller()->push(this, [this, mag](TSC cmd) {
            CCANVAS localSetMag(mag);
            CCANVAS localUpdate(cmd);
        });
    });
}

void AxisAlignWaferSelectorController::setOkCallback(
    const AxisAlignWaferSelectorOk& callback) {
    okCallback = callback;
}

void AxisAlignWaferSelectorController::initIdwId(
    quint64 id, const Integer32& x, const Integer32& y) {
    getAsyncCaller()->push(this, [this, id, x, y](TSC cmd) {
        simpleLoadingBox(this, "Working", "Loading", true) IDW_WaferService* service
            = ServiceManager::instance()->getDefaultService<IDW_WaferService>();
        int errCode;
        IDW_WaferInfoEntity entity = service->getWaferInfoById(id, &errCode);
        if (errCode) {
            simpleMessageBox(this, "Error", "Load IDW info error.");
            return;
        }
        waferDataFromEntity(entity, *waferData, *shotData, *chipData);
        IdwLinkedList* list = CCANVAS localGetCheckedPoints(1);
        list->clear();
        CCANVAS localUpdate(cmd);
        if (x != nullptr && y != nullptr) {
            const QList<qint32>& list = CANVAS->localFindChipByRuler(QPoint(x.value(), y.value()));
            if (list.size() > 0) {
                //单选取1个，如果chip真的有重叠，那就让它重叠去，无视这个问题
                CCANVAS localCheckItem(1, list[0], 1);
                MainThreadInvoker::getInvoker().blockRun([this, &x, &y]() {
                    chipPosTextField->setText(x.toString() + ", " + y.toString());
                });
            }
        }
    });
}
