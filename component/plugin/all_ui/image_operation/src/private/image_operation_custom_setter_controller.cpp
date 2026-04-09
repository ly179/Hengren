#include "image_operation_custom_setter_controller.h"
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "cbox_global_keys.h"
#include "cde90_button.h"
#include "cde90_frame.h"
#include "cde90_label.h"
#include "cde90_radio_button.h"
#include "cde90_spliter_line.h"
#include "database_combo_box.h"

ImageOperationCustomSetterController::ImageOperationCustomSetterController(QWidget* parent): CDE90Dialog(parent) {
    connect(this, &CDE90Dialog::asyncCallerChanged, this, [this](){
        init();
    }, Qt::SingleShotConnection);
    create();
    createAction();
}
ImageOperationCustomSetterController::~ImageOperationCustomSetterController() noexcept {}

void ImageOperationCustomSetterController::create() {
    content = new QWidget;
    contentLayout = new QVBoxLayout(content);
    contentLayout->setContentsMargins(0,0,0,0);
    contentLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    contentLayout->setSpacing(0);
    content->setLayout(contentLayout);

    mainContent = new QWidget(content);
    mainLayout = new QVBoxLayout(mainContent);
    mainLayout->setContentsMargins(5,0,5,0);
    mainLayout->setSpacing(0);
    mainContent->setLayout(mainLayout);
    contentLayout->addWidget(mainContent);

    cursorModeTitleLabel = new CDE90Label("Cursor Mode", mainContent);
    mainLayout->addWidget(cursorModeTitleLabel);

    cursorModeContent = new CDE90Frame(mainContent);
    cursorModeContent->setFrameStyle(QFrame::Panel | QFrame::Plain);
    cursorModeContent->setLineWidth(2);
    cursorModeLayout = new QGridLayout(cursorModeContent);
    cursorModeLayout->setContentsMargins(5,5,5,5);
    cursorModeLayout->setHorizontalSpacing(5);
    cursorModeLayout->setVerticalSpacing(5);
    cursorModeContent->setLayout(cursorModeLayout);
    mainLayout->addWidget(cursorModeContent);

    advanceBtn = new CDE90RadioButton("Advanced", cursorModeContent);
    conventionalBtn = new CDE90RadioButton("Conventional(Rectangular)", cursorModeContent);
    pixelSizeLabel = new CDE90Label("Pixel Size", cursorModeContent);
    axLabel = new CDE90Label("X", cursorModeContent);
    ayLabel = new CDE90Label("Y", cursorModeContent);
    cxLabel = new CDE90Label("X", cursorModeContent);
    cyLabel = new CDE90Label("Y", cursorModeContent);
    axBtn = new DITEMBOX(cursorModeContent);
    ayBtn = new DITEMBOX(cursorModeContent);
    cxBtn = new DITEMBOX(cursorModeContent);
    cyBtn = new DITEMBOX(cursorModeContent);
    cursorModeLayout->addWidget(advanceBtn, 0, 0);
    cursorModeLayout->addWidget(pixelSizeLabel, 0, 1, Qt::AlignRight);
    cursorModeLayout->addWidget(axLabel, 0, 2, Qt::AlignRight);
    cursorModeLayout->addWidget(axBtn, 0, 3);
    cursorModeLayout->addWidget(ayLabel, 1, 0, 1, 3, Qt::AlignRight);
    cursorModeLayout->addWidget(ayBtn, 1, 3);
    cursorModeLayout->addWidget(conventionalBtn, 2, 0, 1, 2);
    cursorModeLayout->addWidget(cxLabel, 2, 2, Qt::AlignRight);
    cursorModeLayout->addWidget(cxBtn, 2, 3);
    cursorModeLayout->addWidget(cyLabel, 3, 0, 1, 3, Qt::AlignRight);
    cursorModeLayout->addWidget(cyBtn, 3, 3);

    scanParamTitleLabel = new CDE90Label("Scan Parameters", mainContent);
    mainLayout->addWidget(scanParamTitleLabel);

    scanParamContent = new QWidget(mainContent);
    scanParamLayout = new QGridLayout(scanParamContent);
    scanParamLayout->setContentsMargins(0,0,0,0);
    scanParamLayout->setHorizontalSpacing(5);
    scanParamLayout->setVerticalSpacing(5);
    scanParamContent->setLayout(scanParamLayout);
    mainLayout->addWidget(scanParamContent);

    scanTitleLabel = new CDE90Label("Scan", scanParamContent);
    frameTitleLabel = new CDE90Label("Frame", scanParamContent);
    filterTitleLabel = new CDE90Label("Filter", scanParamContent);
    flatTitleLabel = new CDE90Label("Flat", scanParamContent);
    dcTitleLabel = new CDE90Label("D.C.", scanParamContent);
    scanRateBtn = new DITEMBOX(scanParamContent);
    frameBtn = new DITEMBOX(scanParamContent);
    filterBtn = new DITEMBOX(scanParamContent);
    flatBtn = new DITEMBOX(scanParamContent);
    dcBtn = new DITEMBOX(scanParamContent);
    scanParamLayout->addWidget(scanTitleLabel, 0, 0);
    scanParamLayout->addWidget(scanRateBtn, 0, 1);
    scanParamLayout->addWidget(frameTitleLabel, 0, 2);
    scanParamLayout->addWidget(frameBtn, 0, 3);
    scanParamLayout->addWidget(filterTitleLabel, 1, 0);
    scanParamLayout->addWidget(filterBtn, 1, 1);
    scanParamLayout->addWidget(flatTitleLabel, 1, 2);
    scanParamLayout->addWidget(flatBtn, 1, 3);
    scanParamLayout->addWidget(dcTitleLabel, 2, 0);
    scanParamLayout->addWidget(dcBtn, 2, 1);

    contentLayout->addWidget(new CDE90SpliterHLine(content));

    operationContent = new QWidget(content);
    operationLayout = new QVBoxLayout(operationContent);
    operationLayout->setContentsMargins(5,0,5,0);
    operationLayout->setSpacing(0);
    operationLayout->setAlignment(Qt::AlignLeft);
    operationContent->setLayout(operationLayout);
    contentLayout->addWidget(operationContent);

    operationTitleLabel = new CDE90Label("Operation Buttom", operationContent);
    operationLayout->addWidget(operationTitleLabel);

    operationBtnContent = new QWidget(operationContent);
    operationBtnLayout = new QHBoxLayout(operationContent);
    operationBtnLayout->setSpacing(5);
    operationBtnLayout->setContentsMargins(5,5,5,5);
    operationBtnContent->setLayout(operationBtnLayout);
    operationLayout->addWidget(operationBtnContent);

    defMagBtn = new CDE90ButtonBox("Def. Mag", operationBtnContent);
    roiSelectionBtn = new CDE90ButtonBox("ROI Selection", operationBtnContent);
    roiSelectionBtn->getQPushButton()->setCheckable(true);
    centerBtn = new CDE90ButtonBox("Centering", operationBtnContent);
    scanBtn = new CDE90ButtonBox("Scan", operationBtnContent);
    operationBtnLayout->addWidget(defMagBtn);
    operationBtnLayout->addWidget(roiSelectionBtn);
    operationBtnLayout->addWidget(centerBtn);
    operationBtnLayout->addWidget(scanBtn);

    contentLayout->addWidget(new CDE90SpliterHLine(content));

    bottomContent = new QWidget(content);
    bottomLayout = new QHBoxLayout(bottomContent);
    bottomLayout->setContentsMargins(5,5,5,5);
    bottomLayout->setAlignment(Qt::AlignRight);
    bottomContent->setLayout(bottomLayout);
    contentLayout->addWidget(bottomContent);
    closeBtn = new CDE90ButtonBox("Close", bottomContent);
    bottomLayout->addWidget(closeBtn);

    setContent(content);
}
void ImageOperationCustomSetterController::createAction() {
    connect(closeBtn->getQPushButton(), &CDE90Button::clicked, this, [this](){
        this->close();
    });
}

void ImageOperationCustomSetterController::init() {
    axBtn->setAsyncCaller(getAsyncCaller());
    ayBtn->setAsyncCaller(getAsyncCaller());
    cxBtn->setAsyncCaller(getAsyncCaller());
    cyBtn->setAsyncCaller(getAsyncCaller());
    scanRateBtn->setAsyncCaller(getAsyncCaller());
    frameBtn->setAsyncCaller(getAsyncCaller());
    filterBtn->setAsyncCaller(getAsyncCaller());
    flatBtn->setAsyncCaller(getAsyncCaller());
    dcBtn->setAsyncCaller(getAsyncCaller());

    itsChildAndBeManaged(axBtn, this);
    itsChildAndBeManaged(ayBtn, this);
    itsChildAndBeManaged(cxBtn, this);
    itsChildAndBeManaged(cyBtn, this);
    itsChildAndBeManaged(scanRateBtn, this);
    itsChildAndBeManaged(frameBtn, this);
    itsChildAndBeManaged(filterBtn, this);
    itsChildAndBeManaged(flatBtn, this);
    itsChildAndBeManaged(dcBtn, this);
}

void ImageOperationCustomSetterController::refreshView() {
    axBtn->loadItemFromDatabase(CBOX_TYPE_SEM_IMG_X_PIX_SIZE);
    ayBtn->loadItemFromDatabase(CBOX_TYPE_SEM_IMG_Y_PIX_SIZE);
    cxBtn->loadItemFromDatabase(CBOX_TYPE_SEM_IMG_X_PIX_COUNT);
    cyBtn->loadItemFromDatabase(CBOX_TYPE_SEM_IMG_Y_PIX_COUNT);
    scanRateBtn->loadItemFromDatabase(CBOX_TYPE_SEM_IMG_SCAN_RATE);
    frameBtn->loadItemFromDatabase(CBOX_TYPE_SEM_IMG_FRAME);
    filterBtn->loadItemFromDatabase(CBOX_TYPE_SEM_IMG_FILTER);
    flatBtn->loadItemFromDatabase(CBOX_TYPE_SEM_IMG_FLAT);
    dcBtn->loadItemFromDatabase(CBOX_TYPE_SEM_IMG_DC);
}
