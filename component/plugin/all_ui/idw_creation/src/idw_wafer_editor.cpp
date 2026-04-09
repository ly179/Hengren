#include "idw_wafer_editor.h"
#include <QActionGroup>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QVBoxLayout>
#include <QWidget>
#include "cde90_button.h"
#include "cde90_combo_box.h"
#include "cde90_frame.h"
#include "cde90_label.h"
#include "cde90_menu.h"
#include "cde90_menu_bar.h"
#include "cde90_message_box.h"
#include "cde90_process_box.h"
#include "cde90_radio_button.h"
#include "cde90_text_field.h"
#include "cdsem_global_resource.h"
#include "cdsem_textfield_number_validator.h"
#include "class_service.h"
#include "idw_canvas_item_check_style.h"
#include "idw_model_data_convert.h"
#include "idw_wafer_service.h"
#include "measure_sort_dialog.h"
#include "rotate_notch_dialog.h"
#include "service_manager.h"
#include "sort_direction_enum.h"
#include "private/idw_detail_data_model.h"

#include "idw_canvas.h"
#include "private/idw_wafer_sub_editor.h"

#define shotData dataHelper->shotEditor->localGetShotChipData()
#define chipData dataHelper->chipEditor->localGetShotChipData()
#define GID(T) int(idw_canvas::T)
#define CANVAS_CALLER dataHelper->canvasContent->getCanvas()->getAsyncCaller().push
#define CANVAS_STOP(T) dataHelper->canvasContent->getCanvas()->getAsyncCaller().stop(GID(T));
#define CANVAS dataHelper->canvasContent->getCanvas()
#define CCANVAS dataHelper->canvasContent->getCanvas()->
//UPDATE_LOCAL可能存在无法让其他线程中断的问题，因此不推荐使用了
// #define CANVAS_UPDATE_LOCAL\
//     CANVAS_STOP(UPDATE)\
//     CANVAS_STOP(UPDATE_WAFER)\
//     if (CCANVAS isLocalThread()) {\
//         CCANVAS localUpdate(c);\
//     } else {\
//         CANVAS_CALLER({GID(UPDATE), [this](TSC c){CCANVAS localUpdate(c);}});\
//     }
#define CANVAS_UPDATE \
    CANVAS->getAsyncCaller().stop(CANVAS->getFullyUpdateGid()); \
    CANVAS_CALLER(CANVAS->getFullyUpdateGid(), [this](TSC c){CCANVAS localUpdate(c);});
#define CANVAS_UPDATE_WAFER \
    CANVAS_STOP(UPDATE_WAFER) \
    CANVAS_CALLER([this](TSC){CCANVAS localUpdateWaferOnly();});

struct IdwWaferEditorDataHelper {
    IdwWaferEditor* d;
    IdwWaferEditorDataHelper* dataHelper;
    //shotData和chipData分别放到shot editor和chip editor上
    IDW_WaferData waferData;

    //用于保存最后依次鼠标点击选择的shot/chip
    IDW_UnitPointInfo lastCheckedItem;

    IDW_BaseData baseData;

    sem::NO_PARAM_ARG_FUNC exitCallback;

    QWidget* content;
    QVBoxLayout* contentLayout;

    QWidget* centerContent;
    QHBoxLayout* centerLayout;

    QWidget* centerRightContent;
    QVBoxLayout* centerRightLayout;

    IdwCanvasContent* canvasContent;

    QWidget* bottomContent;
    QHBoxLayout* bottomLayout;

    CDE90MenuBar* menuBar;
    CDE90Menu* fileMenu;
    CDE90Menu* viewMenu;

    QAction* saveAction;
    QAction* saveAsAction;
    QAction* exitAction;

    CDE90Menu* displayChipNoMenu;
    QAction* displayChipOnAct;
    QAction* displayChipOffAct;

    QGridLayout* classDeviceLayout;
    QWidget* classDeviceContent;

    CDE90Label* classLabel;
    CDE90TextFieldBox* classTextField;
    CDE90Label* deviceLabel;
    CDE90TextFieldBox* deviceTextField;
    CDE90Label* idwLabel;
    CDE90TextFieldBox* idwTextField;

    QVBoxLayout* inputLayout;
    QWidget* inputContent;
    CDE90Frame* createContent;
    QGridLayout* createLayout;

    QHBoxLayout* waferSizeLayout;
    QWidget* waferSizeWidget;
    QHBoxLayout* invalidLayout;
    QWidget* invalidWidget;
    QHBoxLayout* offsetLayout;
    QWidget* offsetWidget;
    QHBoxLayout* notchLayout;
    QWidget* notchWidget;
    QGridLayout* arrayEditLayout;

    CDE90Label* waferSizeLabel;
    CDE90ComboBox* waferSizeBtn;
    CDE90Label* invalidLabel;
    CDE90Label* invalidUnit;
    CDE90TextFieldBox* invalidTextField;
    CDE90Label* offsetXLabel;
    CDE90TextFieldBox* offsetXTextField;
    CDE90Label* offsetYLabel;
    CDE90TextFieldBox* offsetYTextField;
    CDE90Label* offsetUnit;
    CDE90Label* notchLabel;
    CDE90TextFieldBox* notchTextField;
    CDE90Label* notchUnit;
    CDE90ButtonBox* notchBtn;
    CDE90Label* arrayEditLabel;
    CDE90Label* arrayLabel;
    CDE90Label* arrayXLabel;
    CDE90TextFieldBox* arrayXTextField;
    CDE90Label* arrayYLabel;
    CDE90TextFieldBox* arrayYTextField;
    CDE90Label* arrayUnitLabel;
    CDE90Label* unitSizeLabel;
    CDE90Label* unitSizeXLabel;
    CDE90TextFieldBox* unitSizeXTextField;
    CDE90Label* unitSizeYLabel;
    CDE90TextFieldBox* unitSizeYTextField;
    CDE90Label* unitSizeUnitLabel;
    CDE90ButtonBox* createBtn;
    CDE90ButtonBox* editBtn;

    CDE90Label* advanceTitle;
    CDE90Frame* advanceContent;
    QHBoxLayout* advanceLayout;
    QGridLayout* advanceAdjustLayout;
    QWidget* advanceAdjustWidget;
    QWidget* shiftTitleContent;
    QVBoxLayout* shiftTitleLayout;
    CDE90Label* shiftTitleLabel;
    QWidget* shiftContent;
    QHBoxLayout* shiftLayout;
    QWidget* shiftUpDownContnet;
    QVBoxLayout* shiftUpDownLayout;
    CDE90ButtonBox* upBtn;
    CDE90ButtonBox* leftBtn;
    CDE90ButtonBox* rightBtn;
    CDE90ButtonBox* downBtn;
    CDE90Label* deletionTitle;
    CDE90Frame* deletionSectionContent;
    QVBoxLayout* deletionSectionLayout;
    CDE90RadioButton* shotUnit;
    CDE90RadioButton* chipUnit;
    QWidget* partSectionContent;
    QVBoxLayout* partSectionLayout;
    CDE90ButtonBox* fullyBtn;
    CDE90ButtonBox* partlyBtn;
    QWidget* initialContent;
    QVBoxLayout* initialLayout;
    CDE90ButtonBox* initialBtn;

    QWidget* numberTitleContent;
    QHBoxLayout* numberTitleLayout;
    CDE90Label* numberTitle;
    QWidget* numberBtnContent;
    QHBoxLayout* numberBtnLayout;
    CDE90ComboBox* numberBtn;

    QWidget* magContent;
    QHBoxLayout* magLayout;
    CDE90Label* magTitle;
    CDE90ComboBox* magBtn;


    QWidget* chipNoContent;
    QHBoxLayout* chipNoLayout;
    CDE90Label* chipNoTitle;
    CDE90TextFieldBox* chipNoShotTextField;
    CDE90Label* chipNoSpliter;
    CDE90TextFieldBox* chipNoChipTextField;
    CDE90Label* chipNoEnd;

    CDE90Frame* shotChipSelectionContent;
    QHBoxLayout* shotChipSelectionLayout;
    CDE90RadioButton* shotSelectionBtn;
    CDE90RadioButton* chipSelectionBtn;

    QWidget* otherSelectionBtnContent;
    QHBoxLayout* otherSelectionBtnLayout;
    CDE90RadioButton* shiftSelectionBtn;
    CDE90RadioButton* nonExistsSelectionBtn;
    CDE90RadioButton* tegSelectionBtn;
    CDE90RadioButton* originSelectionBtn;

    AsyncCaller* canvasCaller;
    AsyncCaller* editShotCanvasCaller;
    AsyncCaller* editChipCanvasCaller;

    IdwWaferSubEditor* shotEditor = nullptr;
    IdwWaferSubEditor* chipEditor = nullptr;

    CDE90InputBox* saveAsBox = nullptr;

    bool couldExit = false;
    bool shiftKeyPressed = false;
    bool controlKeyPressed = false;

    bool localSingleSelectChecker(bool needClearOld, IdwLinkedList* list, int number) {
        bool exists = false;
        if (needClearOld && list) {
            if (list->size() > 0) {
                exists = list->contains(number);
                list->clear();
            }
        }
        return exists;
    }
};

IdwWaferEditor::IdwWaferEditor(QWidget* parent)
    : RelationIDWWaferEditor(parent) {
    dataHelper = new IdwWaferEditorDataHelper;
    dataHelper->d = this;
    initAsyncCaller([this](){
        if (dataHelper->exitCallback) {
            dataHelper->exitCallback();
        }
        dataHelper->couldExit = true;
    });
    dataHelper->canvasCaller = new AsyncCaller([this](){
        getAsyncCaller()->exit();
    });
    dataHelper->editShotCanvasCaller = new AsyncCaller([this](){
        dataHelper->canvasCaller->exit();
    });
    dataHelper->editChipCanvasCaller = new AsyncCaller([this](){
        dataHelper->editShotCanvasCaller->exit();
    });
    create();
    createAction();
    initCanvas();
    triggerAction();
    installEventFilter(this);
}

IdwWaferEditor::~IdwWaferEditor() noexcept {
    delete dataHelper->editChipCanvasCaller;
    delete dataHelper->editShotCanvasCaller;
    delete dataHelper->canvasCaller;
    delete dataHelper;
}

void IdwWaferEditor::create() {

    dataHelper->content = new QWidget;
    dataHelper->contentLayout = new QVBoxLayout(dataHelper->content);
    dataHelper->contentLayout->setContentsMargins(0,0,0,0);
    dataHelper->contentLayout->setSpacing(0);
    dataHelper->content->setLayout(dataHelper->contentLayout);

    dataHelper->menuBar = new CDE90MenuBar(dataHelper->content);
    dataHelper->menuBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    dataHelper->contentLayout->addWidget(dataHelper->menuBar);
    dataHelper->fileMenu = new CDE90Menu("File", dataHelper->menuBar);
    dataHelper->viewMenu = new CDE90Menu("View", dataHelper->menuBar);
    menuStyle(dataHelper->viewMenu);
    dataHelper->saveAction = dataHelper->fileMenu->addAction("Save");
    dataHelper->saveAsAction = dataHelper->fileMenu->addAction("Save As...");
    dataHelper->exitAction = dataHelper->fileMenu->addAction("Exit");

    dataHelper->displayChipNoMenu = new CDE90Menu("Display Chip No.", dataHelper->viewMenu);
    dataHelper->displayChipOnAct = dataHelper->displayChipNoMenu->addAction("ON");
    dataHelper->displayChipOnAct->setCheckable(true);
    dataHelper->displayChipOffAct = dataHelper->displayChipNoMenu->addAction("OFF");
    dataHelper->displayChipOffAct->setCheckable(true);
    dataHelper->displayChipOffAct->setChecked(true);
    dataHelper->viewMenu->addMenu(dataHelper->displayChipNoMenu);

    QActionGroup* viewGroup = new QActionGroup(dataHelper->displayChipNoMenu);
    viewGroup->addAction(dataHelper->displayChipOnAct);
    viewGroup->addAction(dataHelper->displayChipOffAct);

    dataHelper->menuBar->addMenu(dataHelper->fileMenu);
    dataHelper->menuBar->addMenu(dataHelper->viewMenu);

    dataHelper->centerContent = new QWidget(dataHelper->content);
    dataHelper->centerContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    dataHelper->centerLayout = new QHBoxLayout(dataHelper->centerContent);
    dataHelper->centerLayout->setContentsMargins(5,5,5,5);
    dataHelper->centerLayout->setSpacing(5);
    dataHelper->centerContent->setLayout(dataHelper->centerLayout);
    dataHelper->contentLayout->addWidget(dataHelper->centerContent);

    dataHelper->canvasContent = new IdwCanvasContent(dataHelper->centerContent);
    dataHelper->canvasContent->setAsyncCaller(dataHelper->canvasCaller);
    dataHelper->canvasContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    dataHelper->centerLayout->addWidget(dataHelper->canvasContent);

    dataHelper->centerRightContent = new QWidget(dataHelper->centerContent);
    dataHelper->centerRightContent->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    dataHelper->centerRightLayout = new QVBoxLayout(dataHelper->centerRightContent);
    dataHelper->centerRightLayout->setContentsMargins(5,5,5,5);
    dataHelper->centerRightLayout->setSpacing(10);
    dataHelper->centerRightContent->setLayout(dataHelper->centerRightLayout);
    dataHelper->centerLayout->addWidget(dataHelper->centerRightContent);

    dataHelper->classDeviceContent = new QWidget(dataHelper->centerRightContent);
    dataHelper->classDeviceContent->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    dataHelper->classDeviceLayout = new QGridLayout(dataHelper->classDeviceContent);
    dataHelper->classDeviceLayout->setContentsMargins(0,0,0,0);
    dataHelper->classDeviceLayout->setHorizontalSpacing(10);
    dataHelper->classDeviceLayout->setVerticalSpacing(0);
    dataHelper->classDeviceContent->setLayout(dataHelper->classDeviceLayout);
    dataHelper->centerRightLayout->addWidget(dataHelper->classDeviceContent);

    dataHelper->deviceLabel = new CDE90Label("Device", dataHelper->classDeviceContent);
    dataHelper->classLabel = new CDE90Label("Class", dataHelper->classDeviceContent);
    dataHelper->idwLabel = new CDE90Label("IDW", dataHelper->classDeviceContent);
    dataHelper->deviceTextField = new CDE90TextFieldBox(dataHelper->classDeviceContent);
    dataHelper->deviceTextField->expanding();
    dataHelper->deviceTextField->getQLineEdit()->setDisabled(true);
    dataHelper->classTextField = new CDE90TextFieldBox(dataHelper->classDeviceContent);
    dataHelper->classTextField->expanding();
    dataHelper->classTextField->getQLineEdit()->setDisabled(true);
    dataHelper->idwTextField = new CDE90TextFieldBox(dataHelper->classDeviceContent);
    dataHelper->idwTextField->expanding();
    dataHelper->idwTextField->getQLineEdit()->setDisabled(true);
    dataHelper->classDeviceLayout->addWidget(dataHelper->deviceLabel, 0, 0);
    dataHelper->classDeviceLayout->addWidget(dataHelper->classLabel, 1, 0);
    dataHelper->classDeviceLayout->addWidget(dataHelper->idwLabel, 2, 0);
    dataHelper->classDeviceLayout->addWidget(dataHelper->deviceTextField, 0, 1);
    dataHelper->classDeviceLayout->addWidget(dataHelper->classTextField, 1, 1);
    dataHelper->classDeviceLayout->addWidget(dataHelper->idwTextField, 2, 1);

    dataHelper->inputContent = new QWidget(dataHelper->centerRightContent);
    dataHelper->inputLayout = new QVBoxLayout(dataHelper->inputContent);
    dataHelper->inputLayout->setContentsMargins(0,0,0,0);
    dataHelper->inputLayout->setSpacing(3);
    dataHelper->inputContent->setLayout(dataHelper->inputLayout);
    dataHelper->centerRightLayout->addWidget(dataHelper->inputContent);

    dataHelper->waferSizeWidget = new QWidget(dataHelper->inputContent);
    dataHelper->waferSizeLayout = new QHBoxLayout(dataHelper->waferSizeWidget);
    dataHelper->waferSizeLayout->setContentsMargins(0,0,0,0);
    dataHelper->waferSizeLayout->setSpacing(5);
    dataHelper->waferSizeLayout->setAlignment(Qt::AlignLeft);
    dataHelper->waferSizeWidget->setLayout(dataHelper->waferSizeLayout);
    dataHelper->inputLayout->addWidget(dataHelper->waferSizeWidget);
    dataHelper->waferSizeLabel = new CDE90Label("Wafer Size", dataHelper->waferSizeWidget);
    dataHelper->waferSizeBtn = new CDE90ComboBox(dataHelper->waferSizeWidget);
    //12寸
    dataHelper->waferSizeBtn->addItem("300mm", 300);
    //8寸
    dataHelper->waferSizeBtn->addItem("200mm", 200);
    //6寸
    dataHelper->waferSizeBtn->addItem("150mm", 150);
    //5寸
    dataHelper->waferSizeBtn->addItem("125mm", 125);
    //4寸
    dataHelper->waferSizeBtn->addItem("100mm", 100);
    //3寸
    dataHelper->waferSizeBtn->addItem("75mm", 75);
    dataHelper->waferSizeLayout->addWidget(dataHelper->waferSizeLabel);
    dataHelper->waferSizeLayout->addWidget(dataHelper->waferSizeBtn);

    dataHelper->invalidWidget = new QWidget(dataHelper->inputContent);
    dataHelper->invalidLayout = new QHBoxLayout(dataHelper->invalidWidget);
    dataHelper->invalidLayout->setAlignment(Qt::AlignLeft);
    dataHelper->invalidLayout->setContentsMargins(0,0,0,0);
    dataHelper->invalidLayout->setSpacing(5);
    dataHelper->invalidWidget->setLayout(dataHelper->invalidLayout);
    dataHelper->inputLayout->addWidget(dataHelper->invalidWidget);
    dataHelper->invalidLabel = new CDE90Label("Invalid Area", dataHelper->invalidWidget);
    dataHelper->invalidUnit = new CDE90Label("mm", dataHelper->invalidWidget);
    dataHelper->invalidTextField = new CDE90TextFieldBox(dataHelper->invalidWidget);
    dataHelper->invalidTextField->setMaximumWidth(100);
    dataHelper->invalidTextField->getQLineEdit()->setText("0.00");
    CDSemDoubleValidator<2>* invalidValidator = new CDSemDoubleValidator<2>(dataHelper->invalidTextField->getQLineEdit());
    invalidValidator->setTop(10);
    invalidValidator->setBottom(0);
    invalidValidator->setMode(cdsem::VALIDATOR_AUTO_FIX);
    dataHelper->invalidTextField->getQLineEdit()->setValidator(invalidValidator);
    dataHelper->invalidLayout->addWidget(dataHelper->invalidLabel);
    dataHelper->invalidLayout->addWidget(dataHelper->invalidTextField);
    dataHelper->invalidLayout->addWidget(dataHelper->invalidUnit);

    dataHelper->offsetWidget = new QWidget(dataHelper->inputContent);
    dataHelper->offsetLayout = new QHBoxLayout(dataHelper->offsetWidget);
    dataHelper->offsetLayout->setContentsMargins(0,0,0,0);
    dataHelper->offsetLayout->setSpacing(5);
    dataHelper->offsetLayout->setAlignment(Qt::AlignLeft);
    dataHelper->offsetWidget->setLayout(dataHelper->offsetLayout);
    dataHelper->inputLayout->addWidget(dataHelper->offsetWidget);
    dataHelper->offsetXLabel = new CDE90Label("Map Offset X", dataHelper->offsetWidget);
    dataHelper->offsetYLabel = new CDE90Label("Y", dataHelper->offsetWidget);
    dataHelper->offsetUnit = new CDE90Label("μm", dataHelper->offsetWidget);
    dataHelper->offsetXTextField = new CDE90TextFieldBox(dataHelper->offsetWidget);
    dataHelper->offsetYTextField = new CDE90TextFieldBox(dataHelper->offsetWidget);
    dataHelper->offsetXTextField->setMaximumWidth(100);
    dataHelper->offsetYTextField->setMaximumWidth(100);
    dataHelper->offsetLayout->addWidget(dataHelper->offsetXLabel);
    dataHelper->offsetLayout->addWidget(dataHelper->offsetXTextField);
    dataHelper->offsetLayout->addWidget(dataHelper->offsetYLabel);
    dataHelper->offsetLayout->addWidget(dataHelper->offsetYTextField);
    dataHelper->offsetLayout->addWidget(dataHelper->offsetUnit);
    CDSemDoubleValidator<1>* offsetXYValidator = new CDSemDoubleValidator<1>(dataHelper->offsetXTextField->getQLineEdit());
    offsetXYValidator->setMode(cdsem::VALIDATOR_AUTO_FIX);
    dataHelper->offsetXTextField->getQLineEdit()->setValidator(offsetXYValidator);
    dataHelper->offsetYTextField->getQLineEdit()->setValidator(offsetXYValidator);
    dataHelper->offsetXTextField->getQLineEdit()->setText("0.0");
    dataHelper->offsetYTextField->getQLineEdit()->setText("0.0");

    dataHelper->notchWidget = new QWidget(dataHelper->inputContent);
    dataHelper->notchLayout = new QHBoxLayout(dataHelper->notchWidget);
    dataHelper->notchLayout->setAlignment(Qt::AlignLeft);
    dataHelper->notchLayout->setContentsMargins(0,0,0,0);
    dataHelper->notchLayout->setSpacing(5);
    dataHelper->notchWidget->setLayout(dataHelper->notchLayout);
    dataHelper->inputLayout->addWidget(dataHelper->notchWidget);
    dataHelper->notchLabel = new CDE90Label("Notch Direction", dataHelper->notchWidget);
    dataHelper->notchUnit = new CDE90Label("°", dataHelper->notchWidget);
    dataHelper->notchTextField = new CDE90TextFieldBox(dataHelper->notchWidget);
    dataHelper->notchTextField->setMaximumWidth(100);
    dataHelper->notchTextField->getQLineEdit()->setText("0.0");
    CDSemDoubleValidator<1>* notchValidator = new CDSemDoubleValidator<1>(dataHelper->offsetXTextField->getQLineEdit());
    notchValidator->setMode(cdsem::VALIDATOR_AUTO_FIX);
    notchValidator->setTop("359.9");
    notchValidator->setBottom(0);
    dataHelper->notchTextField->getQLineEdit()->setValidator(notchValidator);
    dataHelper->notchTextField->getQLineEdit()->setText("0.0");
    dataHelper->notchBtn = new CDE90ButtonBox("...", dataHelper->notchWidget);
    dataHelper->notchBtn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    dataHelper->notchLayout->addWidget(dataHelper->notchLabel);
    dataHelper->notchLayout->addWidget(dataHelper->notchTextField);
    dataHelper->notchLayout->addWidget(dataHelper->notchUnit);
    dataHelper->notchLayout->addWidget(dataHelper->notchBtn);

    QWidget* arrayEditContent = new QWidget(dataHelper->centerRightContent);
    QVBoxLayout* arrayEditLayout = new QVBoxLayout(arrayEditContent);
    arrayEditLayout->setContentsMargins(0,0,0,0);
    arrayEditLayout->setSpacing(2);
    arrayEditContent->setLayout(arrayEditLayout);
    dataHelper->centerRightLayout->addWidget(arrayEditContent);
    dataHelper->arrayEditLabel = new CDE90Label("Shot Array Definition", arrayEditContent);
    dataHelper->createContent = new CDE90Frame(arrayEditContent);
    dataHelper->createContent->setFrameStyle(QFrame::Panel | QFrame::Raised);
    dataHelper->createContent->setLineWidth(2);
    dataHelper->createLayout = new QGridLayout(dataHelper->createContent);
    dataHelper->createLayout->setHorizontalSpacing(5);
    dataHelper->createLayout->setVerticalSpacing(0);
    dataHelper->createLayout->setContentsMargins(5,5,5,5);
    dataHelper->createContent->setLayout(dataHelper->createLayout);
    arrayEditLayout->addWidget(dataHelper->arrayEditLabel);
    arrayEditLayout->addWidget(dataHelper->createContent);

    dataHelper->arrayLabel = new CDE90Label("Array", dataHelper->createContent);
    dataHelper->arrayXLabel = new CDE90Label("X", dataHelper->createContent);
    dataHelper->arrayYLabel = new CDE90Label("Y", dataHelper->createContent);
    dataHelper->arrayUnitLabel = new CDE90Label("/Wafer", dataHelper->createContent);
    dataHelper->arrayXTextField = new CDE90TextFieldBox(dataHelper->createContent);
    dataHelper->arrayXTextField->setMaximumWidth(100);
    dataHelper->arrayYTextField = new CDE90TextFieldBox(dataHelper->createContent);
    dataHelper->arrayYTextField->setMaximumWidth(100);

    CDSemIntValidator* arrayXYValidator = new CDSemIntValidator(dataHelper->arrayXTextField->getQLineEdit());
    arrayXYValidator->setBottom(1);
    arrayXYValidator->setTop(IDW_MAX_ARRAY);
    arrayXYValidator->setMode(cdsem::VALIDATOR_AUTO_FIX);
    dataHelper->arrayXTextField->getQLineEdit()->setValidator(arrayXYValidator);
    dataHelper->arrayYTextField->getQLineEdit()->setValidator(arrayXYValidator);


    dataHelper->unitSizeLabel = new CDE90Label("Size", dataHelper->createContent);
    dataHelper->unitSizeXLabel = new CDE90Label("X", dataHelper->createContent);
    dataHelper->unitSizeYLabel = new CDE90Label("Y", dataHelper->createContent);
    dataHelper->unitSizeUnitLabel = new CDE90Label("μm", dataHelper->createContent);
    dataHelper->unitSizeXTextField = new CDE90TextFieldBox(dataHelper->createContent);
    dataHelper->unitSizeXTextField->setMaximumWidth(100);
    dataHelper->unitSizeYTextField = new CDE90TextFieldBox(dataHelper->createContent);
    dataHelper->unitSizeYTextField->setMaximumWidth(100);

    CDSemDoubleValidator<1>* sizeXYValidator = new CDSemDoubleValidator<1>(dataHelper->unitSizeXTextField->getQLineEdit());
    sizeXYValidator->setMode(cdsem::VALIDATOR_AUTO_FIX);
    sizeXYValidator->setBottom(FloatNumber1("0.1"));
    dataHelper->unitSizeXTextField->getQLineEdit()->setValidator(sizeXYValidator);
    dataHelper->unitSizeYTextField->getQLineEdit()->setValidator(sizeXYValidator);

    QWidget* createBtnContent = new QWidget(dataHelper->createContent);
    QHBoxLayout* createBtnLayout = new QHBoxLayout(createBtnContent);
    createBtnLayout->setContentsMargins(0,10,0,0);
    createBtnLayout->setSpacing(0);
    createBtnContent->setLayout(createBtnLayout);
    dataHelper->createBtn = new CDE90ButtonBox("Create", createBtnContent);
    createBtnLayout->addWidget(dataHelper->createBtn);
    QWidget* editBtnContent = new QWidget(dataHelper->createContent);
    QHBoxLayout* editBtnLayout = new QHBoxLayout(editBtnContent);
    editBtnLayout->setContentsMargins(0,10,0,0);
    editBtnLayout->setSpacing(0);
    editBtnLayout->setAlignment(Qt::AlignRight);
    dataHelper->editBtn = new CDE90ButtonBox("Array Editor...", editBtnContent);
    editBtnLayout->addWidget(dataHelper->editBtn);

    dataHelper->createLayout->addWidget(dataHelper->arrayLabel, 0, 0);
    dataHelper->createLayout->addWidget(dataHelper->arrayXLabel, 0, 1);
    dataHelper->createLayout->addWidget(dataHelper->arrayXTextField, 0, 2);
    dataHelper->createLayout->addWidget(dataHelper->arrayYLabel, 0, 3);
    dataHelper->createLayout->addWidget(dataHelper->arrayYTextField, 0, 4);
    dataHelper->createLayout->addWidget(dataHelper->arrayUnitLabel, 0, 5);

    dataHelper->createLayout->addWidget(dataHelper->unitSizeLabel, 1, 0);
    dataHelper->createLayout->addWidget(dataHelper->unitSizeXLabel, 1, 1);
    dataHelper->createLayout->addWidget(dataHelper->unitSizeXTextField, 1, 2);
    dataHelper->createLayout->addWidget(dataHelper->unitSizeYLabel, 1, 3);
    dataHelper->createLayout->addWidget(dataHelper->unitSizeYTextField, 1, 4);
    dataHelper->createLayout->addWidget(dataHelper->unitSizeUnitLabel, 1, 5);

    dataHelper->createLayout->addWidget(createBtnContent, 2, 2);
    dataHelper->createLayout->addWidget(editBtnContent, 2, 4, 1, 2);

    QWidget* advanceRootContent = new QWidget(dataHelper->centerRightContent);
    QVBoxLayout* advanceRootLayout = new QVBoxLayout(advanceRootContent);
    advanceRootLayout->setSpacing(0);
    advanceRootLayout->setContentsMargins(0,5,0,0);
    advanceRootContent->setLayout(advanceRootLayout);
    dataHelper->centerRightLayout->addWidget(advanceRootContent);
    dataHelper->advanceTitle = new CDE90Label("Advanced Setting", advanceRootContent);
    advanceRootLayout->addWidget(dataHelper->advanceTitle);
    dataHelper->advanceContent = new CDE90Frame(advanceRootContent);
    dataHelper->advanceContent->setFrameStyle(QFrame::Panel | QFrame::Raised);
    dataHelper->advanceContent->setLineWidth(2);

    dataHelper->advanceLayout = new QHBoxLayout(dataHelper->advanceContent);
    dataHelper->advanceLayout->setSpacing(0);
    dataHelper->advanceLayout->setContentsMargins(5,5,5,5);
    dataHelper->advanceLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    dataHelper->advanceContent->setLayout(dataHelper->advanceLayout);
    advanceRootLayout->addWidget(dataHelper->advanceContent);

    dataHelper->advanceAdjustWidget = new QWidget(dataHelper->advanceContent);
    dataHelper->advanceAdjustWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    dataHelper->advanceAdjustLayout = new QGridLayout(dataHelper->advanceAdjustWidget);
    dataHelper->advanceAdjustLayout->setContentsMargins(0,0,0,0);
    dataHelper->advanceAdjustLayout->setSpacing(0);
    dataHelper->advanceAdjustLayout->setHorizontalSpacing(0);
    dataHelper->advanceAdjustLayout->setVerticalSpacing(0);
    dataHelper->advanceAdjustWidget->setLayout(dataHelper->advanceAdjustLayout);
    dataHelper->advanceLayout->addWidget(dataHelper->advanceAdjustWidget);

    dataHelper->shiftTitleContent = new QWidget(dataHelper->advanceAdjustWidget);
    dataHelper->shiftTitleLayout = new QVBoxLayout(dataHelper->shiftTitleContent);
    dataHelper->shiftTitleLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    dataHelper->shiftTitleLayout->setContentsMargins(0,0,0,0);
    dataHelper->shiftTitleLayout->setSpacing(0);
    dataHelper->shiftTitleContent->setLayout(dataHelper->shiftTitleLayout);
    dataHelper->advanceAdjustLayout->addWidget(dataHelper->shiftTitleContent, 0 ,0);

    dataHelper->shiftTitleLabel = new CDE90Label("Shot Shift", dataHelper->shiftTitleContent);
    dataHelper->shiftTitleLayout->addWidget(dataHelper->shiftTitleLabel);

    dataHelper->shiftContent = new QWidget(dataHelper->advanceAdjustWidget);
    dataHelper->shiftLayout = new QHBoxLayout(dataHelper->shiftContent);
    dataHelper->shiftLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    dataHelper->shiftLayout->setContentsMargins(0,0,0,0);
    dataHelper->shiftLayout->setSpacing(0);
    dataHelper->shiftContent->setLayout(dataHelper->shiftLayout);
    dataHelper->advanceAdjustLayout->addWidget(dataHelper->shiftContent, 1, 1);

    dataHelper->leftBtn = new CDE90ButtonBox(dataHelper->shiftContent);
    dataHelper->leftBtn->getQPushButton()->setIcon(ResourceManager::getResourceManager().getIdwShiftLeftIcon());
    dataHelper->leftBtn->getQPushButton()->setIconSize(QSize(30,30));
    dataHelper->leftBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    dataHelper->shiftLayout->addWidget(dataHelper->leftBtn);

    dataHelper->shiftUpDownContnet = new QWidget(dataHelper->shiftContent);
    dataHelper->shiftUpDownLayout = new QVBoxLayout(dataHelper->shiftUpDownContnet);
    dataHelper->shiftUpDownLayout->setAlignment(Qt::AlignCenter);
    dataHelper->shiftUpDownLayout->setContentsMargins(0,0,0,0);
    dataHelper->shiftUpDownLayout->setSpacing(0);
    dataHelper->shiftUpDownContnet->setLayout(dataHelper->shiftUpDownLayout);

    dataHelper->upBtn = new CDE90ButtonBox(dataHelper->shiftUpDownContnet);
    dataHelper->upBtn->getQPushButton()->setIconSize(QSize(30,30));
    dataHelper->upBtn->getQPushButton()->setIcon(ResourceManager::getResourceManager().getIdwShiftUpIcon());
    dataHelper->upBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    dataHelper->downBtn = new CDE90ButtonBox(dataHelper->shiftUpDownContnet);
    dataHelper->downBtn->getQPushButton()->setIconSize(QSize(30,30));
    dataHelper->downBtn->getQPushButton()->setIcon(ResourceManager::getResourceManager().getIdwShiftDownIcon());
    dataHelper->downBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    dataHelper->shiftUpDownLayout->addWidget(dataHelper->upBtn);
    dataHelper->shiftUpDownLayout->addWidget(dataHelper->downBtn);
    dataHelper->shiftLayout->addWidget(dataHelper->shiftUpDownContnet);
    dataHelper->rightBtn = new CDE90ButtonBox(dataHelper->shiftContent);
    dataHelper->rightBtn->getQPushButton()->setIconSize(QSize(30,30));
    dataHelper->rightBtn->getQPushButton()->setIcon(ResourceManager::getResourceManager().getIdwShiftRightIcon());
    dataHelper->rightBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    dataHelper->shiftLayout->addWidget(dataHelper->rightBtn);

    dataHelper->deletionTitle = new CDE90Label("Auto Deletion", dataHelper->advanceAdjustWidget);
    dataHelper->advanceAdjustLayout->addWidget(dataHelper->deletionTitle, 2, 0, 1, 1);

    QWidget* deletionSectionRoot = new QWidget(dataHelper->advanceAdjustWidget);
    QHBoxLayout* deletionSectionRootLayout = new QHBoxLayout(deletionSectionRoot);
    deletionSectionRootLayout->setContentsMargins(0,0,0,0);
    deletionSectionRootLayout->setSpacing(0);
    deletionSectionRootLayout->setAlignment(Qt::AlignCenter);
    deletionSectionRoot->setLayout(deletionSectionRootLayout);
    dataHelper->advanceAdjustLayout->addWidget(deletionSectionRoot, 3, 0);
    dataHelper->deletionSectionContent = new CDE90Frame(dataHelper->advanceAdjustWidget);
    dataHelper->deletionSectionContent->setFrameStyle(QFrame::Panel | QFrame::Plain);
    dataHelper->deletionSectionContent->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    dataHelper->deletionSectionContent->setLineWidth(2);
    dataHelper->deletionSectionLayout = new QVBoxLayout(dataHelper->deletionSectionContent);
    dataHelper->deletionSectionLayout->setContentsMargins(0,0,0,0);
    dataHelper->deletionSectionLayout->setSpacing(0);
    dataHelper->deletionSectionLayout->setAlignment(Qt::AlignCenter);
    dataHelper->deletionSectionContent->setLayout(dataHelper->deletionSectionLayout);
    dataHelper->deletionSectionContent->setStyleSheet("QRadioButton {padding: 2 5 2 2;}");
    deletionSectionRootLayout->addWidget(dataHelper->deletionSectionContent);
    dataHelper->shotUnit = new CDE90RadioButton("Shot", dataHelper->deletionSectionContent);
    dataHelper->chipUnit = new CDE90RadioButton("Chip", dataHelper->deletionSectionContent);
    dataHelper->deletionSectionLayout->addWidget(dataHelper->shotUnit);
    dataHelper->deletionSectionLayout->addWidget(dataHelper->chipUnit);

    QWidget* partSectionRoot = new QWidget(dataHelper->advanceAdjustWidget);
    QHBoxLayout* partSectionRootLayout = new QHBoxLayout(partSectionRoot);
    partSectionRootLayout->setContentsMargins(0,0,0,0);
    partSectionRootLayout->setSpacing(0);
    partSectionRootLayout->setAlignment(Qt::AlignCenter);
    partSectionRoot->setLayout(partSectionRootLayout);
    dataHelper->advanceAdjustLayout->addWidget(partSectionRoot, 3, 1);

    dataHelper->partSectionContent = new QWidget(partSectionRoot);
    dataHelper->partSectionContent->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    dataHelper->partSectionLayout = new QVBoxLayout(dataHelper->partSectionContent);
    dataHelper->partSectionLayout->setContentsMargins(0,0,0,0);
    dataHelper->partSectionLayout->setSpacing(0);
    dataHelper->partSectionContent->setLayout(dataHelper->partSectionLayout);
    partSectionRootLayout->addWidget(dataHelper->partSectionContent);

    dataHelper->fullyBtn = new CDE90ButtonBox("Fully Out", dataHelper->partSectionContent);
    dataHelper->partlyBtn = new CDE90ButtonBox("Partly Out", dataHelper->partSectionContent);
    dataHelper->partlyBtn->setStyleSheet("QPushButton {padding: 2 20;}");
    dataHelper->partSectionLayout->addWidget(dataHelper->fullyBtn);
    dataHelper->partSectionLayout->addWidget(dataHelper->partlyBtn);

    dataHelper->initialContent = new QWidget(dataHelper->advanceAdjustWidget);
    dataHelper->initialLayout = new QVBoxLayout(dataHelper->initialContent);
    dataHelper->initialLayout->setContentsMargins(0,0,0,0);
    dataHelper->initialLayout->setSpacing(0);
    dataHelper->initialLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    dataHelper->initialContent->setLayout(dataHelper->initialLayout);
    dataHelper->advanceAdjustLayout->addWidget(dataHelper->initialContent, 3, 2);
    dataHelper->initialBtn = new CDE90ButtonBox("Initial", dataHelper->initialContent);
    dataHelper->initialLayout->addWidget(dataHelper->initialBtn);

    dataHelper->numberTitleContent = new QWidget(dataHelper->advanceAdjustWidget);
    dataHelper->numberTitleLayout = new QHBoxLayout(dataHelper->numberTitleContent);
    dataHelper->numberTitleLayout->setContentsMargins(0,10,0,0);
    dataHelper->numberTitleLayout->setSpacing(0);
    dataHelper->numberTitleLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    dataHelper->numberTitleContent->setLayout(dataHelper->numberTitleLayout);
    dataHelper->numberTitle = new CDE90Label("Numbering", dataHelper->numberTitleContent);
    dataHelper->numberTitle->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    dataHelper->numberTitleLayout->addWidget(dataHelper->numberTitle);
    dataHelper->numberBtnContent = new QWidget(dataHelper->advanceAdjustWidget);
    dataHelper->numberBtnLayout = new QHBoxLayout(dataHelper->numberBtnContent);
    dataHelper->numberBtnLayout->setContentsMargins(0,10,0,0);
    dataHelper->numberBtnLayout->setSpacing(0);
    dataHelper->numberBtnLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    dataHelper->numberBtnContent->setLayout(dataHelper->numberBtnLayout);
    dataHelper->advanceAdjustLayout->addWidget(dataHelper->numberTitleContent, 4, 0);
    dataHelper->advanceAdjustLayout->addWidget(dataHelper->numberBtnContent, 4, 1, 2, 1);
    dataHelper->numberBtn = new CDE90ComboBox(dataHelper->numberBtnContent);
    dataHelper->numberBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    dataHelper->numberBtn->addItems(QStringList() << "(Column,Row)" << "(Shot,Chip)");
    dataHelper->numberBtnLayout->addWidget(dataHelper->numberBtn);

    dataHelper->magContent = new QWidget(dataHelper->centerRightContent);
    dataHelper->magLayout = new QHBoxLayout(dataHelper->magContent);
    dataHelper->magLayout->setSpacing(5);
    dataHelper->magLayout->setContentsMargins(0,0,0,0);
    dataHelper->magLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    dataHelper->magContent->setLayout(dataHelper->magLayout);
    dataHelper->centerRightLayout->addWidget(dataHelper->magContent);
    dataHelper->magTitle = new CDE90Label("Magnify", dataHelper->magContent);
    dataHelper->magBtn = new CDE90ComboBox(dataHelper->magContent);
    dataHelper->magLayout->addWidget(dataHelper->magTitle);
    dataHelper->magLayout->addWidget(dataHelper->magBtn);

    dataHelper->centerRightLayout->addSpacerItem(new QSpacerItem(10, 0, QSizePolicy::Preferred, QSizePolicy::Expanding));

    dataHelper->bottomContent = new QWidget(dataHelper->content);
    dataHelper->bottomLayout = new QHBoxLayout(dataHelper->bottomContent);
    dataHelper->bottomLayout->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    dataHelper->bottomLayout->setSpacing(5);
    dataHelper->bottomLayout->setContentsMargins(5,0,0,5);
    dataHelper->bottomContent->setLayout(dataHelper->bottomLayout);
    dataHelper->contentLayout->addWidget(dataHelper->bottomContent);

    dataHelper->chipNoContent = new QWidget(dataHelper->bottomContent);
    dataHelper->chipNoLayout = new QHBoxLayout(dataHelper->chipNoContent);
    dataHelper->chipNoLayout->setContentsMargins(0,0,10,0);
    dataHelper->chipNoLayout->setSpacing(5);
    dataHelper->chipNoContent->setLayout(dataHelper->chipNoLayout);
    dataHelper->bottomLayout->addWidget(dataHelper->chipNoContent);
    dataHelper->chipNoTitle = new CDE90Label("Chip No. (", dataHelper->chipNoContent);
    dataHelper->chipNoShotTextField = new CDE90TextFieldBox(dataHelper->chipNoContent);
    dataHelper->chipNoShotTextField->getQLineEdit()->setFixedWidth(50);
    dataHelper->chipNoShotTextField->getQLineEdit()->setDisabled(true);
    dataHelper->chipNoSpliter = new CDE90Label(",", dataHelper->chipNoContent);
    dataHelper->chipNoChipTextField = new CDE90TextFieldBox(dataHelper->chipNoContent);
    dataHelper->chipNoChipTextField->getQLineEdit()->setFixedWidth(50);
    dataHelper->chipNoChipTextField->getQLineEdit()->setDisabled(true);
    dataHelper->chipNoEnd = new CDE90Label(")", dataHelper->chipNoContent);
    dataHelper->chipNoLayout->addWidget(dataHelper->chipNoTitle);
    dataHelper->chipNoLayout->addWidget(dataHelper->chipNoShotTextField);
    dataHelper->chipNoLayout->addWidget(dataHelper->chipNoSpliter);
    dataHelper->chipNoLayout->addWidget(dataHelper->chipNoChipTextField);
    dataHelper->chipNoLayout->addWidget(dataHelper->chipNoEnd);

    dataHelper->shotChipSelectionContent = new CDE90Frame(dataHelper->bottomContent);
    dataHelper->shotChipSelectionContent->setFrameStyle(QFrame::Panel | QFrame::Plain);
    dataHelper->shotChipSelectionContent->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    dataHelper->shotChipSelectionContent->setLineWidth(2);
    dataHelper->shotChipSelectionContent->setStyleSheet("QRadioButton {padding: 2 5 2 2;}");
    dataHelper->shotChipSelectionLayout = new QHBoxLayout(dataHelper->shotChipSelectionContent);
    dataHelper->shotChipSelectionLayout->setContentsMargins(0,0,0,0);
    dataHelper->shotChipSelectionLayout->setSpacing(0);
    dataHelper->shotChipSelectionContent->setLayout(dataHelper->shotChipSelectionLayout);
    dataHelper->bottomLayout->addWidget(dataHelper->shotChipSelectionContent);

    dataHelper->shotSelectionBtn = new CDE90RadioButton("Shot", dataHelper->shotChipSelectionContent);
    dataHelper->chipSelectionBtn = new CDE90RadioButton("Chip", dataHelper->shotChipSelectionContent);
    dataHelper->shotChipSelectionLayout->addWidget(dataHelper->shotSelectionBtn);
    dataHelper->shotChipSelectionLayout->addWidget(dataHelper->chipSelectionBtn);

    dataHelper->otherSelectionBtnContent = new QWidget(dataHelper->bottomContent);
    dataHelper->otherSelectionBtnLayout = new QHBoxLayout(dataHelper->otherSelectionBtnContent);
    dataHelper->otherSelectionBtnLayout->setContentsMargins(0,0,0,0);
    dataHelper->otherSelectionBtnLayout->setSpacing(5);
    dataHelper->otherSelectionBtnLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    dataHelper->otherSelectionBtnContent->setLayout(dataHelper->otherSelectionBtnLayout);
    dataHelper->bottomLayout->addWidget(dataHelper->otherSelectionBtnContent);
    dataHelper->shiftSelectionBtn = new CDE90RadioButton("Shift", dataHelper->otherSelectionBtnContent);
    dataHelper->shiftSelectionBtn->setProperty("checked-color", qRgb(255, 255, 0));
    dataHelper->otherSelectionBtnLayout->addWidget(dataHelper->shiftSelectionBtn);
    dataHelper->nonExistsSelectionBtn = new CDE90RadioButton("Non Exist", dataHelper->otherSelectionBtnContent);
    dataHelper->nonExistsSelectionBtn->setProperty("checked-color", qRgb(180, 180, 180));
    dataHelper->otherSelectionBtnLayout->addWidget(dataHelper->nonExistsSelectionBtn);
    dataHelper->tegSelectionBtn = new CDE90RadioButton("TEG", dataHelper->otherSelectionBtnContent);
    dataHelper->tegSelectionBtn->setProperty("checked-color", qRgb(0, 234, 255));
    dataHelper->otherSelectionBtnLayout->addWidget(dataHelper->tegSelectionBtn);
    dataHelper->originSelectionBtn = new CDE90RadioButton("Origin", dataHelper->otherSelectionBtnContent);
    dataHelper->otherSelectionBtnLayout->addWidget(dataHelper->originSelectionBtn);
    setContent(dataHelper->content);

    dataHelper->chipEditor = new IdwWaferSubEditor(&dataHelper->waferData, dataHelper->editChipCanvasCaller, nullptr, this);
    dataHelper->chipEditor->initAsyncCaller(getAsyncCaller());
    dataHelper->chipEditor->setWindowModality(Qt::WindowModal);
    dataHelper->chipEditor->setWindowTitle("Sub Array Editor");
    dataHelper->chipEditor->setResizable(false);
    dataHelper->chipEditor->hideMenuBtn(false);
    dataHelper->shotEditor = new IdwWaferSubEditor(&dataHelper->waferData, dataHelper->editShotCanvasCaller, dataHelper->chipEditor, this);
    dataHelper->shotEditor->initAsyncCaller(getAsyncCaller());
    dataHelper->shotEditor->setWindowModality(Qt::WindowModal);
    dataHelper->shotEditor->setWindowTitle("Array Editor");
    dataHelper->shotEditor->setResizable(false);
    dataHelper->shotEditor->hideMenuBtn(false);
}

void IdwWaferEditor::createAction() {
    connect(dataHelper->shotEditor, &IdwWaferSubEditor::onClose, this, [this](){
        getAsyncCaller()->push(this, [this](TSC){
            afterCreate();
            MainThreadInvoker::getInvoker().blockRun([this](){
                dataHelper->arrayXTextField->getQLineEdit()->setText(QString::number(shotData.getArrayX()));
                dataHelper->arrayYTextField->getQLineEdit()->setText(QString::number(shotData.getArrayY()));
                dataHelper->unitSizeXTextField->getQLineEdit()->setText(shotData.getSizeX().toQStringAndKeepFullDecimal());
                dataHelper->unitSizeYTextField->getQLineEdit()->setText(shotData.getSizeY().toQStringAndKeepFullDecimal());
            });
        });
    });
    connect(dataHelper->notchBtn->getQPushButton(), &CDE90Button::clicked, this, [this](){
        RotateNotchDialog d(dataHelper->notchLabel->text().startsWith("Notch"), this);
        d.setWindowTitle(dataHelper->notchLabel->text());
        d.setSelectedNotch(dataHelper->notchTextField->getQLineEdit()->text());
        int degree = d.exec();
        if (degree >= 0) {
            dataHelper->notchTextField->getQLineEdit()->setText(QString::number(degree) + ".0");
            if (!dataHelper->notchTextField->getQLineEdit()->signalsBlocked()) {
                emit dataHelper->notchTextField->getQLineEdit()->editingFinished();
            }
        }
    });

    connect(dataHelper->notchTextField->getQLineEdit(), &CDE90TextField::editingFinished, this, [this](){
        FloatNumber1 degree = dataHelper->notchTextField->getQLineEdit()->text();
        getAsyncCaller()->push(this, [this, degree](TSC){
            dataHelper->waferData.setNotchDirection(degree);
            CANVAS_STOP(WAFER_NOTCH)
            CANVAS_CALLER(GID(WAFER_NOTCH), [this](TSC){
                getAsyncCaller()->pushBlock(this, [this](TSC){
                    CCANVAS localWafer()->setNotchDirection(dataHelper->waferData.getNotchDirection());
                });
                CANVAS_STOP(UPDATE_WAFER)
                CCANVAS localUpdateWaferOnly();
            });
        });
    });

    connect(dataHelper->offsetXTextField->getQLineEdit(), &CDE90TextField::editingFinished, this, [this](){
        FloatNumber1 offset = dataHelper->offsetXTextField->getQLineEdit()->text();
        getAsyncCaller()->push(this, [this, offset](TSC){
            FloatNumberPointF<> point = dataHelper->waferData.getMapOffset();
            point.x = offset;
            dataHelper->waferData.setMapOffset(point);
            CANVAS_STOP(WAFER_MAP_OFFSET)
            CANVAS_CALLER(GID(WAFER_MAP_OFFSET), [this](TSC){
                getAsyncCaller()->pushBlock(this, [this](TSC){
                    CCANVAS localWafer()->setMapOffset(dataHelper->waferData.getMapOffset());
                });
                CANVAS_STOP(UPDATE_WAFER)
                CCANVAS localUpdateWaferOnly();
            });
        });
    });
    connect(dataHelper->offsetYTextField->getQLineEdit(), &CDE90TextField::editingFinished, this, [this](){
        FloatNumber1 offset = dataHelper->offsetYTextField->getQLineEdit()->text();
        getAsyncCaller()->push(this, [this, offset](TSC){
            FloatNumberPointF<> point = dataHelper->waferData.getMapOffset();
            point.y = offset;
            dataHelper->waferData.setMapOffset(point);
            CANVAS_STOP(WAFER_MAP_OFFSET)
            CANVAS_CALLER(GID(WAFER_MAP_OFFSET), [this](TSC){
                getAsyncCaller()->pushBlock(this, [this](TSC){
                    CCANVAS localWafer()->setMapOffset(dataHelper->waferData.getMapOffset());
                });
                CANVAS_STOP(UPDATE_WAFER)
                CCANVAS localUpdateWaferOnly();
            });
        });
    });

    connect(dataHelper->chipSelectionBtn, &QRadioButton::toggled, this, [this](bool checked){
        dataHelper->originSelectionBtn->setDisabled(!checked);
        if (checked) {
            CANVAS_CALLER([this](TSC){
                CCANVAS localSetMouseCheckUnit(IDW_CM_CHIP);
                CANVAS_UPDATE_WAFER
            });
        }
    });
    connect(dataHelper->shotSelectionBtn, &QRadioButton::toggled, this, [this](bool checked){
        bool sChecked = checked && dataHelper->shiftSelectionBtn->isChecked();
        dataHelper->shiftSelectionBtn->setDisabled(!checked);
        dataHelper->upBtn->getQPushButton()->setDisabled(!sChecked);
        dataHelper->downBtn->getQPushButton()->setDisabled(!sChecked);
        dataHelper->leftBtn->getQPushButton()->setDisabled(!sChecked);
        dataHelper->rightBtn->getQPushButton()->setDisabled(!sChecked);
        if (checked) {
            CANVAS_CALLER([this](TSC){
                CCANVAS localSetMouseCheckUnit(IDW_CM_SHOT);
                CANVAS_UPDATE_WAFER
            });
        }
    });
    connect(dataHelper->shiftSelectionBtn, &QRadioButton::toggled, this, [this](bool checked){
        bool sChecked = checked && dataHelper->shotSelectionBtn->isChecked();
        dataHelper->upBtn->getQPushButton()->setDisabled(!sChecked);
        dataHelper->downBtn->getQPushButton()->setDisabled(!sChecked);
        dataHelper->leftBtn->getQPushButton()->setDisabled(!sChecked);
        dataHelper->rightBtn->getQPushButton()->setDisabled(!sChecked);
    });

    dataHelper->magBtn->addItems(QStringList() << "x1" << "x2" << "x4" << "x8" << "x16");

    connect(dataHelper->createBtn->getQPushButton(), &CDE90Button::clicked, this, [this](){
        IDW_CreateDataParam param;
        QString str = dataHelper->arrayXTextField->getQLineEdit()->text();
        if (str.isEmpty()) {
            return;
        }
        param.arrayX = str.toInt();
        str = dataHelper->arrayYTextField->getQLineEdit()->text();
        if (str.isEmpty()) {
            return;
        }
        param.arrayY = str.toInt();
        str = dataHelper->unitSizeXTextField->getQLineEdit()->text();
        if (str.isEmpty()) {
            return;
        }
        param.sizeX = str;
        str = dataHelper->unitSizeYTextField->getQLineEdit()->text();
        if (str.isEmpty()) {
            return;
        }
        param.sizeY = str;
        param.pitchX = 0;
        param.pitchY = 0;
        param.direction = char(MEASURE_SORT_2431);
        dataHelper->createBtn->loading();
        getAsyncCaller()->push(this, [this, param](TSC) mutable{
            sem::AfterGc gc([this](){
                MainThreadInvoker::getInvoker().runLater([this](){
                    dataHelper->createBtn->overLoading();
                });
            });
            dataHelper->shotEditor->localCreateData(param);
            dataHelper->shotEditor->setInputData(param);
            afterCreate();
        });
    });
    connect(dataHelper->editBtn->getQPushButton(), &CDE90Button::clicked, this, [this](){
        dataHelper->shotEditor->show();
        MainThreadInvoker::getInvoker().moveToScreenCenter(dataHelper->shotEditor);
        dataHelper->shotEditor->updateCanvas();
    });
    connect(dataHelper->waferSizeBtn->getQComboBox(), &UComboBox::currentIndexChanged, this, [this](int i){
        int n = getCheckedWaferSize(i);
        getAsyncCaller()->push(this, [this, n](TSC){
            dataHelper->waferData.setWaferDiameter(n * 1000);
            WaferDirectionMode mode = CCANVAS getDirectionModeFromAuto(dataHelper->waferData.getWaferDiameter());
            MainThreadInvoker::getInvoker().runLater([this, mode](){
                if (mode == WDM_NOTCH) {
                    dataHelper->notchLabel->setText("Notch Direction");
                } else {
                    dataHelper->notchLabel->setText("Flat Direction");
                }
            });
            CANVAS_STOP(WAFER_SIZE)
            CANVAS_CALLER(GID(WAFER_SIZE), [this](TSC){
                getAsyncCaller()->pushBlock(this, [this](TSC){
                    CCANVAS localWafer()->setWaferDiameter(dataHelper->waferData.getWaferDiameter());
                });
                CANVAS_UPDATE
            });
        });
    });
    connect(dataHelper->invalidTextField->getQLineEdit(), &CDE90TextField::editingFinished, this, [this](){
        FloatNumber2 num = dataHelper->invalidTextField->getQLineEdit()->text();
        num *= 1000;
        getAsyncCaller()->push(this, [this, num](TSC){
            dataHelper->waferData.setInvalidArea(num);
            CANVAS_STOP(WAFER_INVALID_AREA)
            CANVAS_CALLER(GID(WAFER_INVALID_AREA), [this](TSC){
                getAsyncCaller()->pushBlock(this, [this](TSC){
                    CCANVAS localWafer()->setInvalidArea(dataHelper->waferData.getInvalidArea());
                });
                CANVAS_STOP(UPDATE_WAFER)
                CCANVAS localUpdateWaferOnly();
            });
        });
    });

    CCANVAS registSelectedHandler({nullptr, int(qRgb(255,255,0)), IDW_CM_SHOT, GID(SHOT_SHIFT)});
    CCANVAS registSelectedHandler({QSharedPointer<SelectNumberDrawerHandler>(new CheckPointSelectedBg(Qt::black)),
                                   0,
                                   IDW_CM_SHOT,
                                   GID(SHOT_INVALID)});
    CCANVAS registSelectedHandler({QSharedPointer<SelectNumberDrawerHandler>(new CheckPointSelectedBg(Qt::black)),
                                   0,
                                   IDW_CM_CHIP,
                                   GID(CHIP_INVALID)});
    CCANVAS registSelectedHandler(
        {QSharedPointer<SelectNumberDrawerHandler>(IDW_DEFAULT_ORIGIN_CHECK_STYLE),
         0,
         IDW_CM_CHIP,
         GID(CHIP_ORIGIN)});
    CCANVAS registSelectedHandler(
        {QSharedPointer<SelectNumberDrawerHandler>(IDW_DEFAULT_TEG_CHECK_STYLE),
         0,
         IDW_CM_SHOT,
         GID(SHOT_TEG_CHECKED)});
    CCANVAS registSelectedHandler(
        {QSharedPointer<SelectNumberDrawerHandler>(IDW_DEFAULT_TEG_CHECK_STYLE),
         0,
         IDW_CM_CHIP,
         GID(CHIP_TEG_CHECKED)});

    connect(CANVAS, &IdwCanvas::itemChecked, this, [this](const IDW_UnitPointInfo& info){
        if (info.repeat) {
            return;
        }
        qDebug() << "checked chip: " << info.number3;
        dataHelper->lastCheckedItem = info;
        if (info.type == IDW_CM_SHOT && info.number1 >= 0 && dataHelper->shotSelectionBtn->isChecked()) {
            int number = info.number1;
            if (dataHelper->shiftSelectionBtn->isChecked()) {
                bool needClearOld = !dataHelper->shiftKeyPressed && !dataHelper->controlKeyPressed;
                CANVAS_CALLER([this, number, needClearOld](TSC){
                    if (dataHelper->localSingleSelectChecker(needClearOld, CCANVAS localGetCheckedPoints(GID(SHOT_SHIFT)), number)) {
                        CANVAS_UPDATE_WAFER
                        return;
                    }
                    CCANVAS checkItem(GID(SHOT_SHIFT), number);
                    CANVAS_UPDATE_WAFER
                });
            } else if (dataHelper->nonExistsSelectionBtn->isChecked()) {
                bool needClearOld = !dataHelper->shiftKeyPressed && !dataHelper->controlKeyPressed;
                CANVAS_CALLER([this, number, needClearOld](TSC){
                    IdwLinkedList* list = CCANVAS localGetCheckedPoints(GID(CHIP_INVALID));
                    if (!list->isEmpty()) {
                        list->clear();
                    }
                    if (dataHelper->localSingleSelectChecker(needClearOld, CCANVAS localGetCheckedPoints(GID(SHOT_INVALID)), number)) {
                        CANVAS_UPDATE_WAFER
                        return;
                    }
                    bool r = CCANVAS checkItem(GID(SHOT_INVALID), number);
                    CANVAS_UPDATE_WAFER
                    getAsyncCaller()->push(this, [this, number, r](TSC) {
                        if (dataHelper->waferData.getInvalidType() != 0) {
                            dataHelper->waferData.setInvalidType(0);
                            dataHelper->waferData.getInvalidNumbers().clear();
                        }
                        if (r) {
                            dataHelper->waferData.getInvalidNumbers().insert(number);
                        } else {
                            dataHelper->waferData.getInvalidNumbers().remove(number);
                        }
                    });
                });
            } else if (dataHelper->tegSelectionBtn->isChecked()) {
                //关于TEG，尚不清楚是针对Chip的还是二者都可以，目前暂时放在Chip下
                bool needClearOld = !dataHelper->shiftKeyPressed && !dataHelper->controlKeyPressed;
                CANVAS_CALLER([this, number, needClearOld](TSC) {
                    IdwLinkedList* list = CCANVAS localGetCheckedPoints(GID(CHIP_TEG_CHECKED));
                    if (!list->isEmpty()) {
                        list->clear();
                    }
                    if (dataHelper->localSingleSelectChecker(needClearOld, CCANVAS localGetCheckedPoints(GID(SHOT_TEG_CHECKED)), number)) {
                        CANVAS_UPDATE_WAFER
                        return;
                    }
                    if (dataHelper->waferData.getTegType() != 0) {
                        dataHelper->waferData.setTegType(0);
                        dataHelper->waferData.getTegNumbers().clear();
                    }
                    bool r = CCANVAS checkItem(GID(SHOT_TEG_CHECKED), number);
                    CANVAS_UPDATE_WAFER
                    if (r) {
                        dataHelper->waferData.getTegNumbers().insert(number);
                    } else {
                        dataHelper->waferData.getTegNumbers().remove(number);
                    }
                });
            }
        } else if (info.type == IDW_CM_CHIP && info.number2 >= 0 && dataHelper->chipSelectionBtn->isChecked()) {
            int number = info.number3;
            if (dataHelper->originSelectionBtn->isChecked()) {
                getAsyncCaller()->push(this, [this, number](TSC) {
                    dataHelper->waferData.setOriginalNumber(number);
                });
                bool needRerendNumber = dataHelper->displayChipOnAct->isChecked()
                                        && dataHelper->numberBtn->currentIndex() == 0;
                CANVAS_CALLER([this, number, needRerendNumber](TSC cmd) {
                    CCANVAS localGetCheckedPoints(GID(CHIP_ORIGIN))->clear();
                    CCANVAS checkItem(GID(CHIP_ORIGIN), number);
                    CCANVAS localWafer()->setOriginalNumber(number);
                    if (needRerendNumber) {
                        CANVAS->localSetRendChipNumber(cmd, 2);
                    } else {
                        CANVAS_UPDATE_WAFER
                    }
                });
                dataHelper->lastCheckedItem.x = 0;
                dataHelper->lastCheckedItem.y = 0;

            } else if (dataHelper->nonExistsSelectionBtn->isChecked()) {
                bool needClearOld = !dataHelper->shiftKeyPressed && !dataHelper->controlKeyPressed;
                CANVAS_CALLER([this, number, needClearOld](TSC) {
                    IdwLinkedList* list = CCANVAS localGetCheckedPoints(GID(SHOT_INVALID));
                    if (!list->isEmpty()) {
                        list->clear();
                    }
                    if (dataHelper->localSingleSelectChecker(needClearOld, CCANVAS localGetCheckedPoints(GID(CHIP_INVALID)), number)) {
                        CANVAS_UPDATE_WAFER
                        return;
                    }
                    bool r = CCANVAS checkItem(GID(CHIP_INVALID), number);
                    CANVAS_UPDATE_WAFER
                    getAsyncCaller()->push(this, [this, number, r](TSC) {
                        if (dataHelper->waferData.getInvalidType() != 1) {
                            dataHelper->waferData.setInvalidType(1);
                            dataHelper->waferData.getInvalidNumbers().clear();
                        }
                        if (r) {
                            dataHelper->waferData.getInvalidNumbers().insert(number);
                        } else {
                            dataHelper->waferData.getInvalidNumbers().remove(number);
                        }
                    });
                });
            } else if (dataHelper->tegSelectionBtn->isChecked()) {
                bool needClearOld = !dataHelper->shiftKeyPressed && !dataHelper->controlKeyPressed;
                CANVAS_CALLER([this, number, needClearOld](TSC) {
                    IdwLinkedList* list = CCANVAS localGetCheckedPoints(GID(SHOT_TEG_CHECKED));
                    if (!list->isEmpty()) {
                        list->clear();
                    }
                    if (dataHelper->waferData.getTegType() != 1) {
                        dataHelper->waferData.setTegType(1);
                        dataHelper->waferData.getTegNumbers().clear();
                    }
                    if (dataHelper->localSingleSelectChecker(needClearOld, CCANVAS localGetCheckedPoints(GID(CHIP_TEG_CHECKED)), number)) {
                        CANVAS_UPDATE_WAFER
                        return;
                    }
                    bool r = CCANVAS checkItem(GID(CHIP_TEG_CHECKED), number);
                    CANVAS_UPDATE_WAFER
                    if (r) {
                        dataHelper->waferData.getTegNumbers().insert(number);
                    } else {
                        dataHelper->waferData.getTegNumbers().remove(number);
                    }
                });
            }
        }
        updateNumberLabel(dataHelper->lastCheckedItem);
    });
    connect(dataHelper->upBtn->getQPushButton(), &CDE90Button::clicked, this, [this](){
        shiftShot(false, true);
    });
    connect(dataHelper->downBtn->getQPushButton(), &CDE90Button::clicked, this, [this](){
        shiftShot(false, false);
    });
    connect(dataHelper->leftBtn->getQPushButton(), &CDE90Button::clicked, this, [this](){
        shiftShot(true, false);
    });
    connect(dataHelper->rightBtn->getQPushButton(), &CDE90Button::clicked, this, [this](){
        shiftShot(true, true);
    });
    connect(dataHelper->partlyBtn->getQPushButton(), &CDE90Button::clicked, this, [this](){
        autoCalcInvalid(true);
    });
    connect(dataHelper->fullyBtn->getQPushButton(), &CDE90Button::clicked, this, [this](){
        autoCalcInvalid(false);
    });
    connect(dataHelper->initialBtn->getQPushButton(), &CDE90Button::clicked, this, [this]() {
        getAsyncCaller()->push(this, [this](TSC) {
            dataHelper->waferData.getInvalidNumbers().clear();
            CANVAS_CALLER([this](TSC){
                CCANVAS localGetCheckedPoints(GID(SHOT_INVALID))->clear();
                CCANVAS localGetCheckedPoints(GID(CHIP_INVALID))->clear();
                CANVAS_UPDATE_WAFER
            });
        });
    });
    connect(dataHelper->numberBtn->getQComboBox(), &UComboBox::currentIndexChanged, this, [this](int index) {
        updateNumberLabel(dataHelper->lastCheckedItem);
        if (dataHelper->displayChipOnAct->isChecked()) {
            char status = index == 0 ? 2 : 1;
            CANVAS_CALLER(
                [this, status](TSC cmd) { CANVAS->localSetRendChipNumber(cmd, status, true); });
        }
    });
    connect(dataHelper->magBtn->getQComboBox(), &UComboBox::currentIndexChanged, this, [this](int) {
        CANVAS_CALLER([this](TSC) { CCANVAS localSetMag(std::pow(2, dataHelper->magBtn->currentIndex())); });
        CANVAS_UPDATE
    });

    connect(dataHelper->exitAction, &QAction::triggered, this, [this]() {
        dataHelper->editChipCanvasCaller->exit();
    });

    connect(dataHelper->saveAction, &QAction::triggered, this, [this](){
        IDW_WaferData& waferData = dataHelper->waferData;
        IDW_BaseData& baseData = dataHelper->baseData;
        getAsyncCaller()->push(this, [this, waferData, baseData](TSC){
            simpleLoadingBox(this, "Working", "Saving...", true)
            IDW_WaferService* service = ServiceManager::instance()->getDefaultService<IDW_WaferService>();
            int errCode;
            quint64 existsId = service->getWaferIdByName(baseData.waferName, baseData.classId, &errCode);
            if (errCode) {
                simpleMessageBox(this, "Error", "Error to check wafer name.");
                return;
            }
            if ((baseData.waferId == 0 && existsId) || (baseData.waferId > 0 && existsId > 0 && baseData.waferId != existsId)) {
                simpleMessageBox(this, "Error", "Wafer name already exists, please use \"Save as\" instead.");
                return;
            }
            IDW_WaferInfoEntity entity;
            waferDataToEntity(entity, waferData, shotData, chipData);
            entity.setWaferName(baseData.waferName);
            entity.setClassId(baseData.classId);
            entity.setId(baseData.waferId);

            errCode = service->saveWithTransaction(entity);
            if (errCode) {
                simpleMessageBox(this, "Error", errCode == FILE_NAME_REPEAT_ERROR_CODE ? "Wafer name already exists, please use \"Save as\" instead." : "Save error.");
                qCritical() << "wafer save data at save action in wafer editor while database: " << errCode;
                return;
            }
            MainThreadInvoker::getInvoker().blockRun([this,&entity](){
                dataHelper->baseData.waferId = entity.getId();
                simpleMessageBox(this, "Info", "Save complete.");
            });

        });
    });

    connect(dataHelper->saveAsAction, &QAction::triggered, this, [this]() {
        if (!dataHelper->saveAsBox) {
            dataHelper->saveAsBox = new CDE90InputBox(this);
            dataHelper->saveAsBox->setWindowTitle("Save as...");
            dataHelper->saveAsBox->setEditTitle("Create IDW");
            dataHelper->saveAsBox->setInitText(dataHelper->baseData.waferName);
            dataHelper->saveAsBox->setInputLength(FILE_NAME_LENGTH);
            dataHelper->saveAsBox->setInputCallback([this](const QString& waferName){
                if (waferName.isEmpty()) {
                    simpleMessageBox(this, "Message", "Please input IDW name.");
                    return false;
                }
                IDW_BaseData &baseData = dataHelper->baseData;
                IDW_WaferData &waferData = dataHelper->waferData;
                getAsyncCaller()->push(this, [this, waferName, baseData, waferData](TSC){
                    simpleLoadingBox(this, "Working", "Saving...", true)
                    IDW_WaferInfoEntity entity;
                    waferDataToEntity(entity, waferData, shotData, chipData);
                    entity.setWaferName(waferName);
                    entity.setClassId(baseData.classId);
                    entity.setId(0);
                    IDW_WaferService* service = ServiceManager::instance()->getDefaultService<IDW_WaferService>();
                    int errCode;
                    quint64 existsId = service->getWaferIdByName(waferName, baseData.classId, &errCode);
                    if (errCode) {
                        simpleMessageBox(this, "Error", "Error to check wafer name.");
                        return;
                    }
                    if (existsId) {
                        simpleMessageBox(this, "Error", "Wafer name already exists.");
                        return;
                    }
                    errCode = service->saveWithTransaction(entity);
                    if (errCode) {
                        simpleMessageBox(this, "Error", errCode == FILE_NAME_REPEAT_ERROR_CODE ? "Wafer name already exists." : "Save error.");
                        qCritical() << "wafer save data at save action in wafer editor while database: " << errCode;
                        return;
                    }
                    MainThreadInvoker::getInvoker().blockRun([this, &entity, &waferName](){
                        dataHelper->baseData.waferId = entity.getId();
                        dataHelper->baseData.waferName = waferName;
                        dataHelper->idwTextField->getQLineEdit()->setText(waferName);
                        dataHelper->saveAsBox->close();
                        simpleMessageBox(this, "Info", "Save complete.");
                        dataHelper->saveAction->setVisible(true);
                    });
                });
                return false;
            });
            dataHelper->saveAsBox->resize(300,200);
        }
        dataHelper->saveAsBox->show();
    });

    connect(dataHelper->displayChipOffAct, &QAction::toggled, this, [this](bool b) {
        if (b)
            CANVAS_CALLER([this](TSC cmd) { CANVAS->localSetRendChipNumber(cmd, 0, true); });
    });
    connect(dataHelper->displayChipOnAct, &QAction::toggled, this, [this](bool b) {
        if (!b) {
            return;
        }
        char status = dataHelper->numberBtn->currentIndex() == 0 ? 2 : 1;
        CANVAS_CALLER(
            [this, status](TSC cmd) { CANVAS->localSetRendChipNumber(cmd, status, true); });
    });
}

void IdwWaferEditor::triggerAction() {
    dataHelper->shotUnit->setChecked(true);
    dataHelper->chipSelectionBtn->setChecked(false);
    dataHelper->shotSelectionBtn->setChecked(true);

    emit dataHelper->shotSelectionBtn->toggled(dataHelper->shotSelectionBtn->isChecked());
    emit dataHelper->chipSelectionBtn->toggled(dataHelper->chipSelectionBtn->isChecked());
    emit dataHelper->shiftSelectionBtn->toggled(dataHelper->shiftSelectionBtn->isChecked());
    emit dataHelper->waferSizeBtn->getQComboBox()->currentIndexChanged(dataHelper->waferSizeBtn->getQComboBox()->currentIndex());
}

void IdwWaferEditor::initCanvas() {
    IDW_WaferData collectedData = collectWaferData();
    getAsyncCaller()->push(this, [this, collectedData](TSC){
        dataHelper->waferData.setInvalidArea(collectedData.getInvalidArea());
        dataHelper->waferData.setNotchDirection(collectedData.getNotchDirection());
        dataHelper->waferData.setInvalidType(collectedData.getInvalidType());
        dataHelper->waferData.setMapOffset(collectedData.getMapOffset());
        dataHelper->waferData.setWaferDiameter(collectedData.getWaferDiameter());
        CANVAS_STOP(CANVAS_INIT)
        CANVAS_CALLER(GID(CANVAS_INIT), [this](TSC){
            getAsyncCaller()->pushBlock(this, [this](TSC){
                *CCANVAS localWafer() = dataHelper->waferData;
                *CCANVAS localShot() = shotData;
                *CCANVAS localChip() = chipData;
            });
            CANVAS_UPDATE
        });
    });
}

qint32 IdwWaferEditor::getCheckedWaferSize(int i) {
    if (i < 0) {
        i = dataHelper->waferSizeBtn->currentIndex();
    }
    return dataHelper->waferSizeBtn->getQComboBox()->itemData(i).toInt();
}

IDW_WaferData IdwWaferEditor::collectWaferData() {
    IDW_WaferData waferData;
    QString str = dataHelper->notchTextField->getQLineEdit()->text();
    if (str.isEmpty()) {
        str = "0";
    }
    waferData.setNotchDirection(str);
    str = dataHelper->invalidTextField->getQLineEdit()->text();
    if (str.isEmpty()) {
        str = "0";
    }
    waferData.setInvalidArea(str);
    waferData.setWaferDiameter(getCheckedWaferSize());
    FloatNumberPointF<> mapOffset{0,0};
    str = dataHelper->offsetXTextField->getQLineEdit()->text();
    if (str.length() > 0) {
        mapOffset.x = str;
    }
    str = dataHelper->offsetYTextField->getQLineEdit()->text();
    if (str.length() > 0) {
        mapOffset.y = str;
    }
    waferData.setMapOffset(mapOffset);
    waferData.setInvalidType(dataHelper->shotUnit->isChecked() ? 0 : 1);
    return waferData;
}

void IdwWaferEditor::shiftShot(bool x, bool up) {
    CANVAS_CALLER(GID(SHOT_UPDATE), [this, x, up](TSC){
        IdwLinkedList* list = CCANVAS localGetCheckedPoints(GID(SHOT_SHIFT));
        if (!list) {
            return;
        }
        getAsyncCaller()->pushBlock(this, [this, list, x, up](TSC){
            QList<IDW_Data_Item> &shotItems = *shotData.getDataList();
            if (shotItems.isEmpty()) {
                return;
            }
            QList<IDW_Data_Item>* canvasItems = CCANVAS localShot()->getDataList();
            FloatNumber1 moveUnit = x ? chipData.getSizeX(): chipData.getSizeY();
            if (!up) {
                moveUnit = -moveUnit;
            }
            const IdwLinkedNode* node = list->first();
            while (node) {
                if (node->data >= shotItems.size()) {
                    continue;
                }
                IDW_Data_Item& item = shotItems[node->data];
                if (x) {
                    item.setOffsetX(item.getOffsetX() + moveUnit);
                } else {
                    item.setOffsetY(item.getOffsetY() + moveUnit);
                }
                if (canvasItems) {
                    IDW_Data_Item& canvasItem = (*canvasItems)[node->data];
                    if (x) {
                        canvasItem.setOffsetX(item.getOffsetX());
                    } else {
                        canvasItem.setOffsetY(item.getOffsetY());
                    }
                }
                node = node->next;
            }
        });
        CANVAS_UPDATE
    });
}

inline bool checkPointInSpace(qreal x, qreal y, qreal space) {
    return std::pow(x, 2) + std::pow(y, 2) <= std::pow(space, 2);
}

void IdwWaferEditor::localCalcInValidChip(bool partly) {
    dataHelper->waferData.getInvalidNumbers().clear();
    dataHelper->waferData.setInvalidType(1);
    qreal radius = (dataHelper->waferData.getWaferDiameter().toDouble() - dataHelper->waferData.getInvalidArea().toDouble() * 2) / 2;
    qreal pointX, pointY;
    qint32 invalidCount;
    qreal startX, startY;
    qint32 chipNum = chipData.getDataList()->size();
    qint32 chipIndex;

    qreal chipOffsetX, chipOffsetY, chipSizeX, chipSizeY;
    for (QList<IDW_Data_Item>::iterator shotIter = shotData.getDataList()->begin(); shotIter != shotData.getDataList()->end(); ++ shotIter) {
        startX = shotIter->getOffsetX().toDouble();
        startY = shotIter->getOffsetY().toDouble();
        for (QList<IDW_Data_Item>::iterator iter = chipData.getDataList()->begin(); iter != chipData.getDataList()->end(); ++ iter) {
            chipIndex = shotIter->getNumber() * chipNum + iter->getNumber();
            invalidCount = 0;
            chipOffsetX = iter->getOffsetX().toDouble();
            chipOffsetY = iter->getOffsetY().toDouble();
            pointX = chipOffsetX + startX;
            pointY = chipOffsetY + startY;
            if (!checkPointInSpace(pointX, pointY, radius)) {
                if (partly) {
                    dataHelper->waferData.getInvalidNumbers().insert(chipIndex);
                    continue;
                }
                ++ invalidCount;
            }
            chipSizeX = iter->getSizeX().toDouble();
            pointX = chipOffsetX + chipSizeX + startX;
            pointY = chipOffsetY + startY;
            if (!checkPointInSpace(pointX, pointY, radius)) {
                if (partly) {
                    dataHelper->waferData.getInvalidNumbers().insert(chipIndex);
                    continue;
                }
                ++ invalidCount;
            }

            chipSizeY = iter->getSizeY().toDouble();
            pointX = chipOffsetX + startX;
            pointY = chipOffsetY + chipSizeY + startY;
            if (!checkPointInSpace(pointX, pointY, radius)) {
                if (partly) {
                    dataHelper->waferData.getInvalidNumbers().insert(chipIndex);
                    continue;
                }
                ++ invalidCount;
            }

            pointX = chipOffsetX + chipSizeX + startX;
            pointY = chipOffsetY + chipSizeY + startY;
            if (!checkPointInSpace(pointX, pointY, radius)) {
                if (partly) {
                    dataHelper->waferData.getInvalidNumbers().insert(chipIndex);
                    continue;
                }
                ++ invalidCount;
            }
            if (invalidCount == 4) {
                dataHelper->waferData.getInvalidNumbers().insert(chipIndex);
            }
        }
    }
}

void IdwWaferEditor::localCalcInValidShot(bool partly) {
    dataHelper->waferData.getInvalidNumbers().clear();
    dataHelper->waferData.setInvalidType(0);
    FloatNumber1 radius = (dataHelper->waferData.getWaferDiameter() - dataHelper->waferData.getInvalidArea() * 2) / 2;
    FloatNumber1 pointX, pointY;
    qint32 invalidCount;
    for (QList<IDW_Data_Item>::iterator iter = shotData.getDataList()->begin(); iter != shotData.getDataList()->end(); ++ iter) {
        invalidCount = 0;
        pointX = iter->getOffsetX();
        pointY = iter->getOffsetY();
        if (!checkPointInSpace(pointX.toDouble(), pointY.toDouble(), radius.toDouble())) {
            if (partly) {
                dataHelper->waferData.getInvalidNumbers().insert(iter->getNumber());
                continue;
            }
            ++ invalidCount;
        }
        pointX = iter->getOffsetX() + iter->getSizeX();
        pointY = iter->getOffsetY();
        if (!checkPointInSpace(pointX.toDouble(), pointY.toDouble(), radius.toDouble())) {
            if (partly) {
                dataHelper->waferData.getInvalidNumbers().insert(iter->getNumber());
                continue;
            }
            ++ invalidCount;
        }

        pointX = iter->getOffsetX();
        pointY = iter->getOffsetY() + iter->getSizeY();
        if (!checkPointInSpace(pointX.toDouble(), pointY.toDouble(), radius.toDouble())) {
            if (partly) {
                dataHelper->waferData.getInvalidNumbers().insert(iter->getNumber());
                continue;
            }
            ++ invalidCount;
        }

        pointX = iter->getOffsetX() + iter->getSizeX();
        pointY = iter->getOffsetY() + iter->getSizeY();
        if (!checkPointInSpace(pointX.toDouble(), pointY.toDouble(), radius.toDouble())) {
            if (partly) {
                dataHelper->waferData.getInvalidNumbers().insert(iter->getNumber());
                continue;
            }
            ++ invalidCount;
        }
        if (invalidCount == 4) {
            dataHelper->waferData.getInvalidNumbers().insert(iter->getNumber());
        }
    }
}

void IdwWaferEditor::autoCalcInvalid(bool partly) {
    bool isShot = dataHelper->shotUnit->isChecked();
    dataHelper->fullyBtn->loading();
    dataHelper->partlyBtn->loading();
    getAsyncCaller()->push(this, [this, isShot, partly](TSC){
        sem::AfterGc gc([this](){
            MainThreadInvoker::getInvoker().runLater([this](){
                dataHelper->fullyBtn->overLoading();
                dataHelper->partlyBtn->overLoading();
            });
        });
        if (dataHelper->shotUnit->isChecked()) {
            localCalcInValidShot(partly);
        } else {
            localCalcInValidChip(partly);
        }
        CANVAS_CALLER([this, isShot](TSC){
            getAsyncCaller()->pushBlock(this, [this, isShot](TSC){
                qint32 gid;
                if (isShot) {
                    gid = GID(SHOT_INVALID);
                } else {
                    gid = GID(CHIP_INVALID);
                }
                CCANVAS localGetCheckedPoints(GID(SHOT_INVALID))->clear();
                CCANVAS localGetCheckedPoints(GID(CHIP_INVALID))->clear();
                const QSet<qint32>& invalids = dataHelper->waferData.getInvalidNumbers();
                if (invalids.size() > 0) {
                    for (auto it = invalids.begin(); it != invalids.end(); ++it) {
                        CCANVAS localCheckItem(gid, *it);
                    }
                }
            });
            CANVAS_UPDATE_WAFER
        });
    });
}

void IdwWaferEditor::updateNumberLabel(const IDW_UnitPointInfo& info) {
    if (dataHelper->numberBtn->currentIndex() == 0) {
        if (info.x == std::numeric_limits<qint32>::min()) {
            dataHelper->chipNoShotTextField->getQLineEdit()->clear();
        } else {
            dataHelper->chipNoShotTextField->getQLineEdit()->setText(QString::number(info.x));
        }
        if (info.y == std::numeric_limits<qint32>::min()) {
            dataHelper->chipNoChipTextField->getQLineEdit()->clear();
        } else {
            dataHelper->chipNoChipTextField->getQLineEdit()->setText(QString::number(info.y));
        }
    } else {
        if (info.number1 < 0) {
            dataHelper->chipNoShotTextField->getQLineEdit()->clear();
        } else {
            dataHelper->chipNoShotTextField->getQLineEdit()->setText(QString::number(info.number1));
        }
        if (info.number2 < 0) {
            dataHelper->chipNoChipTextField->getQLineEdit()->clear();
        } else {
            dataHelper->chipNoChipTextField->getQLineEdit()->setText(QString::number(info.number2));
        }
    }
}

void IdwWaferEditor::afterCreate() {
    CANVAS_STOP(SHOT_CREATE)
    CANVAS_STOP(SHOT_UPDATE)
    CANVAS_CALLER(GID(SHOT_CREATE), [this](TSC){
        getAsyncCaller()->pushBlock(this, [this](TSC){
            {
                WaferDirectionMode mode = CCANVAS getDirectionModeFromAuto(dataHelper->waferData.getWaferDiameter());
                MainThreadInvoker::getInvoker().runLater([this, mode](){
                    if (mode == WDM_NOTCH) {
                        dataHelper->notchLabel->setText("Notch Direction");
                    } else {
                        dataHelper->notchLabel->setText("Flat Direction");
                    }
                });
            }
            *CCANVAS localShot() = shotData;
            *CCANVAS localChip() = chipData;
            *CCANVAS localWafer() = dataHelper->waferData;
            CCANVAS localGetCheckedPoints(GID(SHOT_SHIFT))->clear();
            CCANVAS localGetCheckedPoints(GID(CHIP_ORIGIN))->clear();
            CCANVAS localGetCheckedPoints(GID(SHOT_INVALID))->clear();
            CCANVAS localGetCheckedPoints(GID(CHIP_INVALID))->clear();
            CCANVAS localGetCheckedPoints(GID(SHOT_TEG_CHECKED))->clear();
            CCANVAS localGetCheckedPoints(GID(CHIP_TEG_CHECKED))->clear();
            CCANVAS localCheckItem(GID(CHIP_ORIGIN), dataHelper->waferData.getOriginalNumber());
            int gid = dataHelper->waferData.getInvalidType() == 0 ? GID(SHOT_INVALID) : GID(CHIP_INVALID);
            const QSet<qint32>& invalids = dataHelper->waferData.getInvalidNumbers();
            if (invalids.size() > 0) {
                for (auto it = invalids.begin(); it != invalids.end(); ++it) {
                    CCANVAS localCheckItem(gid, *it);
                }
            }
            gid = dataHelper->waferData.getTegType() == 0 ? GID(SHOT_TEG_CHECKED) : GID(CHIP_TEG_CHECKED);
            const QSet<qint32>& tegNumbers = dataHelper->waferData.getTegNumbers();
            if (tegNumbers.size() > 0) {
                for (auto it = tegNumbers.begin(); it != tegNumbers.end(); ++it) {
                    CCANVAS localCheckItem(gid, *it);
                }
            }
        });
        CANVAS_UPDATE
    });
}

void IdwWaferEditor::closeEvent(QCloseEvent* event) {
    if (dataHelper->couldExit) {
        event->accept();
        CDE90Window::closeEvent(event);
    } else {
        dataHelper->exitAction->trigger();
        event->ignore();
    }
}

bool IdwWaferEditor::eventFilter(QObject *watched, QEvent *event) {
    if (watched != this || (event->type() != QEvent::KeyPress && event->type() != QEvent::KeyRelease)) return RelationIDWWaferEditor::eventFilter(watched, event);
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
    if (keyEvent->key() == Qt::Key_Shift) {
        dataHelper->shiftKeyPressed = event->type() == QEvent::KeyPress;
        dataHelper->canvasContent->getCanvas()->setMousePressAsClick(dataHelper->shiftKeyPressed);
        dataHelper->canvasContent->getCanvas()->setAllowMouseSlideAsPress(dataHelper->shiftKeyPressed);
    } else if (keyEvent->key() == Qt::Key_Control) {
        dataHelper->controlKeyPressed = event->type() == QEvent::KeyPress;
    } else {
        return RelationIDWWaferEditor::eventFilter(watched, event);
    }
    return true;
}

void IdwWaferEditor::setBaseData(const IDW_BaseData& baseData) {
    getAsyncCaller()->push(this, [this, baseData](TSC){
        dataHelper->baseData = baseData;
        MainThreadInvoker::getInvoker().blockRun([this](){
            dataHelper->classTextField->getQLineEdit()->setText(dataHelper->baseData.className);
            dataHelper->idwTextField->getQLineEdit()->setText(dataHelper->baseData.waferName);
        });
    });
}

void IdwWaferEditor::setExitCallback(const sem::NO_PARAM_ARG_FUNC& callback) {
    dataHelper->exitCallback = callback;
}

void IdwWaferEditor::loadData(quint64 id) {
    getAsyncCaller()->push(this, [this, id](TSC){
        simpleLoadingBox(this, "Working", "Loading...", true);
        IDW_WaferService* service = ServiceManager::instance()->getDefaultService<IDW_WaferService>();
        int errCode;
        IDW_WaferInfoEntity entity = service->getWaferInfoById(id, &errCode);
        if (errCode) {
            simpleMessageBox(this, "Error", "Query IDW info error.");
            return;
        }
        dataHelper->baseData.waferName = entity.getWaferName();
        dataHelper->baseData.classId = entity.getClassId();
        dataHelper->baseData.waferId = id;
        ClassService* classService = ServiceManager::instance()->getDefaultService<ClassService>();
        ClassEntity classEntity = classService->getById(entity.getClassId(), &errCode);
        if (!errCode) {
            dataHelper->baseData.className = classEntity.getClassName();
        } else {
            dataHelper->baseData.className = "";
        }
        dataHelper->shotEditor->getTableDataMode().clear();
        dataHelper->chipEditor->getTableDataMode().clear();
        waferDataFromEntity(entity, dataHelper->waferData, shotData, chipData);
        dataHelper->shotEditor->getTableDataMode().resizeViewList(shotData.getDataList()->size());
        dataHelper->chipEditor->getTableDataMode().resizeViewList(chipData.getDataList()->size());
        MainThreadInvoker::getInvoker().blockRun([this](){
            dataHelper->waferSizeBtn->setCurrentData(dataHelper->waferData.getWaferDiameter() / 1000);
            dataHelper->invalidTextField->getQLineEdit()->setText(dataHelper->waferData.getInvalidArea().toQStringAndKeepFullDecimal());
            dataHelper->offsetXTextField->getQLineEdit()->setText(dataHelper->waferData.getMapOffset().x.toQStringAndKeepFullDecimal());
            dataHelper->offsetYTextField->getQLineEdit()->setText(dataHelper->waferData.getMapOffset().y.toQStringAndKeepFullDecimal());
            dataHelper->notchTextField->getQLineEdit()->setText(dataHelper->waferData.getNotchDirection().toQStringAndKeepFullDecimal());
            dataHelper->arrayXTextField->getQLineEdit()->setText(QString::number(shotData.getArrayX()));
            dataHelper->arrayYTextField->getQLineEdit()->setText(QString::number(shotData.getArrayY()));
            dataHelper->unitSizeXTextField->getQLineEdit()->setText(shotData.getSizeX().toQStringAndKeepFullDecimal());
            dataHelper->unitSizeYTextField->getQLineEdit()->setText(shotData.getSizeY().toQStringAndKeepFullDecimal());
            dataHelper->idwTextField->getQLineEdit()->setText(dataHelper->baseData.waferName);
            dataHelper->classTextField->getQLineEdit()->setText(dataHelper->baseData.className);
            IDW_CreateDataParam param;
            param.arrayX = shotData.getArrayX();
            param.arrayY = shotData.getArrayY();
            param.sizeX = shotData.getSizeX();
            param.sizeY = shotData.getSizeY();
            param.pitchX = shotData.getPitchX();
            param.pitchY = shotData.getPitchY();
            dataHelper->shotEditor->setInputData(param);
            param.arrayX = chipData.getArrayX();
            param.arrayY = chipData.getArrayY();
            param.sizeX = chipData.getSizeX();
            param.sizeY = chipData.getSizeY();
            param.pitchX = chipData.getPitchX();
            param.pitchY = chipData.getPitchY();
            dataHelper->chipEditor->setInputData(param);
            dataHelper->chipEditor->setShotSize(shotData.getArrayX() * shotData.getArrayY());
        });
        afterCreate();
    });
}

void IdwWaferEditor::disableCoverSave(bool b) {
    dataHelper->saveAction->setVisible(!b);
}

quint64 IdwWaferEditor::getIdwId() const {
    return dataHelper->baseData.waferId;
}
