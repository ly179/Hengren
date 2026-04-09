#include "idw_wafer_sub_editor.h"
#include <QGridLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QSpacerItem>
#include "cde90_button.h"
#include "cde90_frame.h"
#include "cde90_label.h"
#include "cde90_message_box.h"
#include "cde90_spliter_line.h"
#include "cde90_table_view.h"
#include "cde90_text_field.h"
#include "cdsem_global_data.h"
#include "cdsem_global_resource.h"
#include "image_measurement.h"
#include "image_save_model.h"
#include "measure_sort_dialog.h"
#include "qdatetime.h"
#include <QDir>
#include "sort_direction_enum.h"
#include "tmp_image_data_converter.h"
#include "cde90_menu.h"
#include "idw_sub_editor_canvas.h"
#include "scan_mode_data_model.h"
#include "cdsem_textfield_number_validator.h"
#include "idw_detail_data_model.h"
#include "image_operation_relation.h"
#include "image_type_defition.h"

#define GID(T) int(idw_edit_canvas::T)
#define CANVAS_CALLER dataHelper->canvas->getAsyncCaller().push
#define CANVAS_STOP(T) dataHelper->canvas->getAsyncCaller().stop(GID(T));
#define CANVAS_UPDATE \
CANVAS_STOP(UPDATE) \
    CANVAS_STOP(SELECTED_UPDATE) \
    CANVAS_CALLER(GID(UPDATE), [this](TSC c){dataHelper->canvas->localUpdate(c);});

struct IdwWaferSubEditorDataHelper {
    sem::ONE_PARAM_ARG_FUNC<IDW_CreateDataParam&> originPointCalc;

    QWidget* content;
    QVBoxLayout* contentLayout;

    QWidget* createAreaContent;
    QHBoxLayout* createAreaLayout;

    CDE90Frame* canvasContent;
    QHBoxLayout* canvasLayout;
    IdwSubEditorCanvas* canvas;

    QWidget* createContent;
    QGridLayout* createLayout;

    CDE90Label* arrayTitle;
    CDE90Label* arrayXTitle;
    CDE90Label* arrayYTitle;
    CDE90TextFieldBox* arrayXTextField;
    CDE90TextFieldBox* arrayYTextField;

    CDE90Label* sizeTitle;
    CDE90Label* sizeXTitle;
    CDE90Label* sizeYTitle;
    CDE90TextFieldBox* sizeXTextField;
    CDE90TextFieldBox* sizeYTextField;

    CDE90Label* pitchTitle;
    CDE90Label* pitchXTitle;
    CDE90Label* pitchYTitle;
    CDE90TextFieldBox* pitchXTextField;
    CDE90TextFieldBox* pitchYTextField;

    QWidget* createBtnContent;
    QHBoxLayout* createBtnLayout;
    CDE90ButtonBox* createBtn;

    QWidget* tableContent;
    QHBoxLayout* tableLayout;
    QVBoxLayout* iTableLayout;
    QWidget* iTableContent;
    CDE90TableView* table;
    QWidget* upDownContent;
    QVBoxLayout* upDownLayout;
    CDE90ButtonBox* upBtn;
    CDE90ButtonBox* downBtn;

    QGridLayout* editLayout;
    QWidget* editContent;
    CDE90TextFieldBox* eNoTextField;
    CDE90TextFieldBox* eOffsetXTextField;
    CDE90TextFieldBox* eOffsetYTextField;
    CDE90TextFieldBox* eSizeXTextField;
    CDE90TextFieldBox* eSizeYTextField;

    QWidget* settingContent;
    QHBoxLayout* settingLayout;
    CDE90Label* settingNotify;
    CDE90ButtonBox* applyBtn;
    CDE90ButtonBox* sortBtn;

    QWidget* shiftContent;
    QHBoxLayout* shiftLayout;
    CDE90ButtonBox* shiftBtn;
    CDE90Label* shiftXLabel;
    CDE90Label* shiftXUnitLabel;
    CDE90Label* shiftYLabel;
    CDE90Label* shiftYUnitLabel;
    CDE90TextFieldBox* shiftXTextField;
    CDE90TextFieldBox* shiftYTextField;

    QWidget* windowHandleContent;
    QHBoxLayout* windowHandleLayout;
    CDE90ButtonBox* subArrayBtn = nullptr;
    CDE90ButtonBox* closeBtn;

    IdwDetailDataMode* tableMode;

    MeasureSortDialog* sortDialog = nullptr;

    IDW_ShotChipData editData;
    IDW_WaferData* waferData;
    IdwWaferSubEditor* chipEditor = nullptr;

    CDSemIntValidator* arrayValidator;
    CDSemDoubleValidator<1>* offsetValidator;
    CDSemDoubleValidator<1>* sizeValidator;
    CDSemDoubleValidator<1>* pitchXValidator;
    CDSemDoubleValidator<1>* pitchYValidator;

    qint32 defTextFieldWidth = 92;

    //在chip创建时，需要shot矩阵参数，该参数仅chip editor有效
    qint32 shotSize = 0;
    QImage tempImage;
    QImage firstImage;
    MoveStageParam firstImagePosition;
    ScanModeDataModel firstImageScanMode;

    QImage rightImage;
    MoveStageParam rightImagePosition;
    ScanModeDataModel rightImageScanMode;

    QImage downImage;
    MoveStageParam downImagePosition;
    ScanModeDataModel downImageScanMode;
};

IdwWaferSubEditor::IdwWaferSubEditor(IDW_WaferData* waferData,
                                     AsyncCaller* canvasCaller,
                                     IdwWaferSubEditor* chipEditor,
                                     QWidget* parent)
    : CDE90Window(parent) {
    dataHelper = new IdwWaferSubEditorDataHelper;
    dataHelper->waferData = waferData;
    dataHelper->chipEditor = chipEditor;
    create(canvasCaller);
    createAction();
    if (chipEditor) {
        dataHelper->originPointCalc = [](IDW_CreateDataParam& param){
            param.origin = defaultShotOriginPoint(param);
        };
        dataHelper->settingNotify->setText("Offset means distance between wafer center and shot center");
        dataHelper->canvas->setDataType(idw_edit_canvas::SHOT);
    } else {
        dataHelper->originPointCalc = [](IDW_CreateDataParam& param){
            param.origin = defaultChipOriginPoint();
        };
        dataHelper->settingNotify->setText("Offset means distance between lower-left of shot and lower-left of chip");
        dataHelper->canvas->setDataType(idw_edit_canvas::CHIP);
    }
    dataHelper->editData.setDataList(&dataHelper->tableMode->getDataList());
}

IdwWaferSubEditor::~IdwWaferSubEditor() noexcept{
    dataHelper->editData.setDataList(nullptr);
    delete dataHelper;
}

void IdwWaferSubEditor::create(AsyncCaller* canvasCaller) {
    dataHelper->content = new QWidget;
    dataHelper->contentLayout = new QVBoxLayout(dataHelper->content);
    dataHelper->contentLayout->setContentsMargins(0,0,0,0);
    dataHelper->contentLayout->setSpacing(0);
    dataHelper->content->setLayout(dataHelper->contentLayout);

    dataHelper->createAreaContent = new QWidget(dataHelper->content);
    dataHelper->createAreaContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    dataHelper->createAreaLayout = new QHBoxLayout(dataHelper->createAreaContent);
    dataHelper->createAreaLayout->setContentsMargins(5,5,5,5);
    dataHelper->createAreaLayout->setSpacing(5);
    dataHelper->createAreaLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    dataHelper->createAreaContent->setLayout(dataHelper->createAreaLayout);
    dataHelper->contentLayout->addWidget(dataHelper->createAreaContent);

    dataHelper->canvasContent = new CDE90Frame(dataHelper->createAreaContent);
    dataHelper->canvasContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    dataHelper->canvasContent->setLineWidth(2);
    dataHelper->canvasContent->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    dataHelper->canvasLayout = new QHBoxLayout(dataHelper->canvasContent);
    dataHelper->canvasLayout->setSpacing(0);
    dataHelper->canvasLayout->setContentsMargins(0,0,0,0);
    dataHelper->canvasContent->setLayout(dataHelper->canvasLayout);
    dataHelper->createAreaLayout->addWidget(dataHelper->canvasContent);
    dataHelper->canvas = new IdwSubEditorCanvas(canvasCaller, dataHelper->canvasContent);
    dataHelper->canvas->registFullyUpdateGid(GID(UPDATE));
    dataHelper->canvas->registSelectedUpdateGid(GID(SELECTED_UPDATE));
    dataHelper->canvasLayout->addWidget(dataHelper->canvas);

    dataHelper->createContent = new QWidget(dataHelper->createAreaContent);
    dataHelper->createContent->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    dataHelper->createLayout = new QGridLayout(dataHelper->createContent);
    dataHelper->createLayout->setContentsMargins(5,0,5,5);
    dataHelper->createLayout->setSpacing(0);
    dataHelper->createLayout->setHorizontalSpacing(3);
    dataHelper->createLayout->setVerticalSpacing(8);
    dataHelper->createContent->setLayout(dataHelper->createLayout);
    dataHelper->createAreaLayout->addWidget(dataHelper->createContent);

    dataHelper->arrayTitle = new CDE90Label("Array", dataHelper->createContent);
    dataHelper->arrayXTitle = new CDE90Label("X", dataHelper->createContent);
    dataHelper->arrayYTitle = new CDE90Label("Y", dataHelper->createContent);
    dataHelper->sizeTitle = new CDE90Label("Size [μm]", dataHelper->createContent);
    dataHelper->sizeXTitle = new CDE90Label("X", dataHelper->createContent);
    dataHelper->sizeYTitle = new CDE90Label("Y", dataHelper->createContent);
    dataHelper->pitchTitle = new CDE90Label("Pitch [μm]", dataHelper->createContent);
    dataHelper->pitchXTitle = new CDE90Label("X", dataHelper->createContent);
    dataHelper->pitchYTitle = new CDE90Label("Y", dataHelper->createContent);

    dataHelper->arrayXTextField = new CDE90TextFieldBox(dataHelper->createContent);
    dataHelper->arrayYTextField = new CDE90TextFieldBox(dataHelper->createContent);
    dataHelper->sizeXTextField = new CDE90TextFieldBox(dataHelper->createContent);
    dataHelper->sizeYTextField = new CDE90TextFieldBox(dataHelper->createContent);
    dataHelper->pitchXTextField = new CDE90TextFieldBox(dataHelper->createContent);
    dataHelper->pitchYTextField = new CDE90TextFieldBox(dataHelper->createContent);
    dataHelper->arrayXTextField->getQLineEdit()->setFixedWidth(dataHelper->defTextFieldWidth);
    dataHelper->arrayYTextField->getQLineEdit()->setFixedWidth(dataHelper->defTextFieldWidth);
    dataHelper->sizeXTextField->getQLineEdit()->setFixedWidth(dataHelper->defTextFieldWidth);
    dataHelper->sizeYTextField->getQLineEdit()->setFixedWidth(dataHelper->defTextFieldWidth);
    dataHelper->pitchXTextField->getQLineEdit()->setFixedWidth(dataHelper->defTextFieldWidth);
    dataHelper->pitchYTextField->getQLineEdit()->setFixedWidth(dataHelper->defTextFieldWidth);
    dataHelper->pitchXTextField->getQLineEdit()->setPlaceholderText("*");
    dataHelper->pitchYTextField->getQLineEdit()->setPlaceholderText("*");

    dataHelper->createBtnContent = new QWidget(dataHelper->createContent);
    dataHelper->createBtnLayout = new QHBoxLayout(dataHelper->createBtnContent);
    dataHelper->createBtnLayout->setContentsMargins(0,0,0,0);
    dataHelper->createBtnLayout->setSpacing(0);
    dataHelper->createBtnLayout->setAlignment(Qt::AlignCenter);
    dataHelper->createBtnContent->setLayout(dataHelper->createBtnLayout);
    dataHelper->createLayout->addWidget(dataHelper->createBtnContent);
    dataHelper->createBtn = new CDE90ButtonBox("Create", dataHelper->createBtnContent);
    dataHelper->createBtnLayout->addWidget(dataHelper->createBtn);

    dataHelper->createLayout->addWidget(dataHelper->arrayTitle, 0, 0, 1, 4);
    dataHelper->createLayout->addWidget(dataHelper->arrayXTitle, 1,0);
    dataHelper->createLayout->addWidget(dataHelper->arrayXTextField, 1,1);
    dataHelper->createLayout->addWidget(dataHelper->arrayYTitle, 1,2);
    dataHelper->createLayout->addWidget(dataHelper->arrayYTextField, 1,3);

    dataHelper->createLayout->addWidget(dataHelper->sizeTitle, 2, 0, 1, 2);
    dataHelper->createLayout->addWidget(dataHelper->sizeXTitle, 3,0);
    dataHelper->createLayout->addWidget(dataHelper->sizeXTextField, 3,1);
    dataHelper->createLayout->addWidget(dataHelper->sizeYTitle, 3,2);
    dataHelper->createLayout->addWidget(dataHelper->sizeYTextField, 3,3);

    dataHelper->createLayout->addWidget(dataHelper->pitchTitle, 4,0, 1, 4);
    dataHelper->createLayout->addWidget(dataHelper->pitchXTitle, 5,0);
    dataHelper->createLayout->addWidget(dataHelper->pitchXTextField, 5,1);
    dataHelper->createLayout->addWidget(dataHelper->pitchYTitle, 5,2);
    dataHelper->createLayout->addWidget(dataHelper->pitchYTextField, 5,3);

    dataHelper->createLayout->addWidget(dataHelper->createBtnContent, 6, 0, 1, 4);
    dataHelper->contentLayout->addWidget(new CDE90SpliterHLine(dataHelper->content));

    dataHelper->tableContent = new QWidget(dataHelper->content);
    dataHelper->tableContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    dataHelper->tableLayout = new QHBoxLayout(dataHelper->tableContent);
    dataHelper->tableLayout->setSpacing(3);
    dataHelper->tableLayout->setContentsMargins(5,5,3,0);
    dataHelper->tableContent->setLayout(dataHelper->tableLayout);
    dataHelper->contentLayout->addWidget(dataHelper->tableContent);

    dataHelper->iTableContent = new QWidget(dataHelper->tableContent);
    dataHelper->iTableLayout = new QVBoxLayout(dataHelper->iTableContent);
    dataHelper->iTableLayout->setContentsMargins(0,0,0,0);
    dataHelper->iTableLayout->setSpacing(3);
    dataHelper->iTableContent->setLayout(dataHelper->iTableLayout);
    dataHelper->tableLayout->addWidget(dataHelper->iTableContent);

    dataHelper->tableMode = new IdwDetailDataMode;
    dataHelper->table = new CDE90TableView(dataHelper->tableMode, dataHelper->iTableContent);
    dataHelper->table->setMinimumHeight(200);
    //开启隔行颜色交替显示
    dataHelper->table->setAlternatingRowColors(true);
    dataHelper->tableMode->insertHead("No.", 0, 0);
    dataHelper->tableMode->insertHead("Offset X, Y [μm]", 1, 2);
    dataHelper->tableMode->insertHead("Size X, Y [μm]", 3, 4);
    dataHelper->table->horizontalHeader()->resizeSection(0, 80);
    // table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
    // table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
    // table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Interactive);
    // table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Interactive);
    // table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Interactive);
    dataHelper->iTableLayout->addWidget(dataHelper->table);

    dataHelper->upDownContent = new QWidget(dataHelper->tableContent);
    dataHelper->upDownContent->setStyleSheet("QPushButton {padding: 3 0;}");
    dataHelper->upDownLayout = new QVBoxLayout(dataHelper->upDownContent);
    dataHelper->upDownLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    dataHelper->upDownLayout->setSpacing(10);
    dataHelper->upDownLayout->setContentsMargins(0,10,0,0);
    dataHelper->upDownContent->setLayout(dataHelper->upDownLayout);
    dataHelper->tableLayout->addWidget(dataHelper->upDownContent);

    dataHelper->upBtn = new CDE90ButtonBox(dataHelper->upDownContent);
    dataHelper->downBtn = new CDE90ButtonBox(dataHelper->upDownContent);
    dataHelper->upBtn->getQPushButton()->setIcon(ResourceManager::getResourceManager().getIdwShiftUpIcon());
    dataHelper->downBtn->getQPushButton()->setIcon(ResourceManager::getResourceManager().getIdwShiftDownIcon());
    dataHelper->upDownLayout->addWidget(dataHelper->upBtn);
    dataHelper->upDownLayout->addWidget(dataHelper->downBtn);

    dataHelper->editContent = new QWidget(dataHelper->iTableContent);
    dataHelper->editLayout = new QGridLayout(dataHelper->editContent);
    dataHelper->editLayout->setHorizontalSpacing(3);
    dataHelper->editLayout->setVerticalSpacing(0);
    dataHelper->editLayout->setContentsMargins(0,5,0,0);
    dataHelper->editContent->setLayout(dataHelper->editLayout);
    dataHelper->iTableLayout->addWidget(dataHelper->editContent);

    dataHelper->eNoTextField = new CDE90TextFieldBox(dataHelper->editContent);
    dataHelper->eNoTextField->getQLineEdit()->setFixedWidth(50);
    dataHelper->eNoTextField->getQLineEdit()->setDisabled(true);
    dataHelper->eNoTextField->getQLineEdit()->setPlaceholderText("*");
    dataHelper->eOffsetXTextField = new CDE90TextFieldBox(dataHelper->editContent);
    dataHelper->eOffsetXTextField->getQLineEdit()->setFixedWidth(dataHelper->defTextFieldWidth);
    dataHelper->eOffsetXTextField->getQLineEdit()->setPlaceholderText("*");
    dataHelper->eOffsetYTextField = new CDE90TextFieldBox(dataHelper->editContent);
    dataHelper->eOffsetYTextField->getQLineEdit()->setFixedWidth(dataHelper->defTextFieldWidth);
    dataHelper->eOffsetYTextField->getQLineEdit()->setPlaceholderText("*");
    dataHelper->eSizeXTextField = new CDE90TextFieldBox(dataHelper->editContent);
    dataHelper->eSizeXTextField->getQLineEdit()->setFixedWidth(dataHelper->defTextFieldWidth);
    dataHelper->eSizeXTextField->getQLineEdit()->setPlaceholderText("*");
    dataHelper->eSizeYTextField = new CDE90TextFieldBox(dataHelper->editContent);
    dataHelper->eSizeYTextField->getQLineEdit()->setFixedWidth(dataHelper->defTextFieldWidth);
    dataHelper->eSizeYTextField->getQLineEdit()->setPlaceholderText("*");
    dataHelper->editLayout->addWidget(dataHelper->eNoTextField, 0, 0);
    dataHelper->editLayout->addWidget(dataHelper->eOffsetXTextField, 0, 1);
    dataHelper->editLayout->addWidget(dataHelper->eOffsetYTextField, 0, 2);
    dataHelper->editLayout->addWidget(dataHelper->eSizeXTextField, 0, 3);
    dataHelper->editLayout->addWidget(dataHelper->eSizeYTextField, 0, 4);

    dataHelper->settingContent = new QWidget(dataHelper->content);
    dataHelper->settingLayout = new QHBoxLayout(dataHelper->settingContent);
    dataHelper->settingLayout->setContentsMargins(5,0,5,0);
    dataHelper->settingLayout->setSpacing(5);
    dataHelper->settingContent->setLayout(dataHelper->settingLayout);
    dataHelper->contentLayout->addWidget(dataHelper->settingContent);
    dataHelper->settingNotify = new CDE90Label(dataHelper->settingContent);
    dataHelper->settingNotify->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    dataHelper->settingNotify->setWordWrap(true);
    dataHelper->applyBtn = new CDE90ButtonBox("Apply", dataHelper->settingContent);
    dataHelper->sortBtn = new CDE90ButtonBox("Sorting...", dataHelper->settingContent);
    dataHelper->settingLayout->addWidget(dataHelper->settingNotify);
    dataHelper->settingLayout->addWidget(dataHelper->applyBtn);
    dataHelper->settingLayout->addWidget(dataHelper->sortBtn);

    dataHelper->shiftContent = new QWidget(dataHelper->content);
    dataHelper->shiftLayout = new QHBoxLayout(dataHelper->shiftContent);
    dataHelper->shiftLayout->setContentsMargins(5,5,5,7);
    dataHelper->shiftLayout->setSpacing(5);
    dataHelper->shiftLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    dataHelper->shiftContent->setLayout(dataHelper->shiftLayout);
    dataHelper->contentLayout->addWidget(dataHelper->shiftContent);
    dataHelper->shiftBtn = new CDE90ButtonBox("Shift", dataHelper->shiftContent);
    dataHelper->shiftXLabel = new CDE90Label("X", dataHelper->shiftContent);
    dataHelper->shiftXTextField = new CDE90TextFieldBox(dataHelper->shiftContent);
    dataHelper->shiftXTextField->getQLineEdit()->setFixedWidth(dataHelper->defTextFieldWidth);
    dataHelper->shiftXUnitLabel = new CDE90Label("μm", dataHelper->shiftContent);
    dataHelper->shiftYLabel = new CDE90Label("Y", dataHelper->shiftContent);
    dataHelper->shiftYTextField = new CDE90TextFieldBox(dataHelper->shiftContent);
    dataHelper->shiftYTextField->getQLineEdit()->setFixedWidth(dataHelper->defTextFieldWidth);
    dataHelper->shiftYUnitLabel = new CDE90Label("μm", dataHelper->shiftContent);
    dataHelper->shiftLayout->addWidget(dataHelper->shiftBtn);
    dataHelper->shiftLayout->addWidget(dataHelper->shiftXLabel);
    dataHelper->shiftLayout->addWidget(dataHelper->shiftXTextField);
    dataHelper->shiftLayout->addWidget(dataHelper->shiftXUnitLabel);
    QWidget* gapWidget = new QWidget(dataHelper->shiftContent);
    gapWidget->setFixedSize(10,1);
    dataHelper->shiftLayout->addWidget(gapWidget);
    dataHelper->shiftLayout->addWidget(dataHelper->shiftYLabel);
    dataHelper->shiftLayout->addWidget(dataHelper->shiftYTextField);
    dataHelper->shiftLayout->addWidget(dataHelper->shiftYUnitLabel);

    dataHelper->contentLayout->addWidget(new CDE90SpliterHLine(dataHelper->content));

    dataHelper->windowHandleContent = new QWidget(dataHelper->content);
    dataHelper->windowHandleLayout = new QHBoxLayout(dataHelper->windowHandleContent);
    dataHelper->windowHandleLayout->setContentsMargins(5,3,5,3);
    dataHelper->windowHandleContent->setLayout(dataHelper->windowHandleLayout);
    dataHelper->contentLayout->addWidget(dataHelper->windowHandleContent);

    if (dataHelper->chipEditor) {
        dataHelper->subArrayBtn = new CDE90ButtonBox("Sub Array...", dataHelper->windowHandleContent);
        dataHelper->windowHandleLayout->addWidget(dataHelper->subArrayBtn);
    }
    dataHelper->closeBtn = new CDE90ButtonBox("Close", dataHelper->windowHandleContent);
    dataHelper->windowHandleLayout->addItem(new QSpacerItem(0,10,QSizePolicy::Expanding, QSizePolicy::Preferred));
    dataHelper->windowHandleLayout->addWidget(dataHelper->closeBtn);
    setContent(dataHelper->content);
}

void IdwWaferSubEditor::createAction() {
    dataHelper->shiftXTextField->getQLineEdit()->setText("0.0");
    dataHelper->shiftYTextField->getQLineEdit()->setText("0.0");
    dataHelper->arrayValidator = new CDSemIntValidator(dataHelper->arrayXTextField->getQLineEdit());
    dataHelper->arrayValidator->setBottom(0);
    dataHelper->arrayValidator->setTop(IDW_MAX_ARRAY);
    dataHelper->arrayValidator->setMode(cdsem::VALIDATOR_AUTO_FIX);

    dataHelper->offsetValidator = new CDSemDoubleValidator<1>(dataHelper->eOffsetXTextField->getQLineEdit());
    dataHelper->offsetValidator->setMode(cdsem::VALIDATOR_AUTO_FIX);

    dataHelper->sizeValidator = new CDSemDoubleValidator<1>(dataHelper->sizeXTextField->getQLineEdit());
    dataHelper->sizeValidator->setBottom(0);
    dataHelper->sizeValidator->setMode(cdsem::VALIDATOR_AUTO_FIX);

    dataHelper->pitchXValidator = new CDSemDoubleValidator<1>(dataHelper->pitchXTextField->getQLineEdit());
    dataHelper->pitchXValidator->setBottom(0);
    dataHelper->pitchXValidator->setMode(cdsem::VALIDATOR_AUTO_FIX);
    dataHelper->pitchYValidator = new CDSemDoubleValidator<1>(dataHelper->pitchXTextField->getQLineEdit());
    dataHelper->pitchYValidator->setBottom(0);
    dataHelper->pitchYValidator->setMode(cdsem::VALIDATOR_AUTO_FIX);

    connect(dataHelper->sizeXTextField->getQLineEdit(), &CDE90TextField::editingFinished, this, [this](){
        QString sizeStr = dataHelper->sizeXTextField->getQLineEdit()->text();
        if (sizeStr.isEmpty()) {
            dataHelper->pitchXValidator->setBottom(0);
            return;
        }
        dataHelper->pitchXValidator->setBottom(sizeStr);
        QString pitchStr = dataHelper->pitchXTextField->getQLineEdit()->text();
        if (pitchStr.isEmpty()) {
            return;
        }
        if (FloatNumber1(pitchStr) < FloatNumber1(sizeStr)) {
            dataHelper->pitchXTextField->getQLineEdit()->setText(sizeStr);
        }
    });
    connect(dataHelper->sizeYTextField->getQLineEdit(), &CDE90TextField::editingFinished, this, [this](){
        QString sizeStr = dataHelper->sizeYTextField->getQLineEdit()->text();
        if (sizeStr.isEmpty()) {
            dataHelper->pitchYValidator->setBottom(0);
            return;
        }
        dataHelper->pitchYValidator->setBottom(sizeStr);
        QString pitchStr = dataHelper->pitchYTextField->getQLineEdit()->text();
        if (pitchStr.isEmpty()) {
            return;
        }
        if (FloatNumber1(pitchStr) < FloatNumber1(sizeStr)) {
            dataHelper->pitchYTextField->getQLineEdit()->setText(sizeStr);
        }
    });
    dataHelper->arrayXTextField->getQLineEdit()->setValidator(dataHelper->arrayValidator);
    dataHelper->arrayYTextField->getQLineEdit()->setValidator(dataHelper->arrayValidator);
    dataHelper->sizeXTextField->getQLineEdit()->setValidator(dataHelper->sizeValidator);
    dataHelper->sizeYTextField->getQLineEdit()->setValidator(dataHelper->sizeValidator);
    dataHelper->pitchXTextField->getQLineEdit()->setValidator(dataHelper->pitchXValidator);
    dataHelper->pitchYTextField->getQLineEdit()->setValidator(dataHelper->pitchYValidator);
    dataHelper->eOffsetXTextField->getQLineEdit()->setValidator(dataHelper->offsetValidator);
    dataHelper->eOffsetYTextField->getQLineEdit()->setValidator(dataHelper->offsetValidator);
    dataHelper->eSizeXTextField->getQLineEdit()->setValidator(dataHelper->sizeValidator);
    dataHelper->eSizeYTextField->getQLineEdit()->setValidator(dataHelper->sizeValidator);
    dataHelper->shiftXTextField->getQLineEdit()->setValidator(dataHelper->offsetValidator);
    dataHelper->shiftYTextField->getQLineEdit()->setValidator(dataHelper->offsetValidator);

    connect(dataHelper->createBtn->getQPushButton(), &CDE90Button::clicked, this, [this](){
        collectDataAndCreate();
    });

    connect(dataHelper->sortBtn->getQPushButton(), &CDE90Button::clicked, this, [this]() {
        if (!dataHelper->sortDialog) {
            dataHelper->sortDialog = new MeasureSortDialog(this);
            dataHelper->sortDialog->setWindowTitle("Sorting");
            dataHelper->sortDialog->setApplyCallback([this](MEASURE_SORT_MATRIX t) {
                getAsyncCaller()->push(this, [this, t](TSC c) {
                    dataHelper->editData.setDirection(t);
                    QList<IDW_Data_Item>& list = dataHelper->tableMode->getDataList();
                    sortDirection(list, t);
                    dataHelper->canvas->localEditData() = dataHelper->editData;
                    dataHelper->tableMode->notifyDataChanged(0, list.size() - 1);
                    dataHelper->canvas->localUpdate(c);
                });
            });
        }
        dataHelper->sortDialog->show();
        dataHelper->sortDialog->setInitSelected(MEASURE_SORT_MATRIX(dataHelper->editData.getDirection()));
        dataHelper->sortDialog->resize(330, dataHelper->sortDialog->height());
        MainThreadInvoker::getInvoker().moveToScreenCenter(dataHelper->sortDialog);
    });
    connect(dataHelper->table->selectionModel(), &QItemSelectionModel::selectionChanged, this, [this](const QItemSelection &selected, const QItemSelection &deselected){
        int top, bottom;
        for (auto it = selected.begin(); it != selected.end(); ++ it) {
            top = (*it).top();
            bottom = (*it).bottom();
            for (; top <= bottom; ++ top) {
                dataHelper->canvas->checkItem(top, 1);
            }
        }
        for (auto it = deselected.begin(); it != deselected.end(); ++ it) {
            top = (*it).top();
            bottom = (*it).bottom();
            for (; top <= bottom; ++ top) {
                dataHelper->canvas->checkItem(top, 0);
            }
        }
        dataHelper->canvas->getAsyncCaller().push([this](TSC){
            dataHelper->canvas->localUpdateCheckOnly();
        });

        QModelIndexList list = dataHelper->table->selectionModel()->selectedRows();
        if (list.size() != 1) {
            dataHelper->eNoTextField->getQLineEdit()->clear();
            dataHelper->eOffsetXTextField->getQLineEdit()->clear();
            dataHelper->eOffsetYTextField->getQLineEdit()->clear();
            dataHelper->eSizeXTextField->getQLineEdit()->clear();
            dataHelper->eSizeYTextField->getQLineEdit()->clear();
            return;
        }
        int index = list[0].row();
        if (index < 0) {
            return;
        }
        getAsyncCaller()->push(this, [this, index](TSC){
            QList<IDW_Data_Item>& list = dataHelper->tableMode->getDataList();
            if (index >= list.size()) {
                return;
            }
            IDW_Data_Item& item = list[index];
            MainThreadInvoker::getInvoker().blockRun([this, &item](){
                dataHelper->eNoTextField->getQLineEdit()->setText(QString::number(item.getNumber()));
                dataHelper->eOffsetXTextField->getQLineEdit()->setText(item.getOffsetX().toQStringAndKeepFullDecimal());
                dataHelper->eOffsetYTextField->getQLineEdit()->setText(item.getOffsetY().toQStringAndKeepFullDecimal());
                dataHelper->eSizeXTextField->getQLineEdit()->setText(item.getSizeX().toQStringAndKeepFullDecimal());
                dataHelper->eSizeYTextField->getQLineEdit()->setText(item.getSizeY().toQStringAndKeepFullDecimal());
            });
        });
    });
    connect(dataHelper->applyBtn->getQPushButton(), &CDE90Button::clicked, this, [this](){
        QString str = dataHelper->eNoTextField->getQLineEdit()->text();
        if (str.isEmpty()) {
            return;
        }
        int number = str.toInt();
        if (number < 0) {
            return;
        }
        str = dataHelper->eOffsetXTextField->getQLineEdit()->text();
        if (str.isEmpty()) {
            return;
        }
        FloatNumber1 offsetX = str;
        str = dataHelper->eOffsetYTextField->getQLineEdit()->text();
        if (str.isEmpty()) {
            return;
        }
        FloatNumber1 offsetY = str;
        str = dataHelper->eSizeXTextField->getQLineEdit()->text();
        if (str.isEmpty()) {
            return;
        }
        FloatNumber1 sizeX = str;
        str = dataHelper->eSizeYTextField->getQLineEdit()->text();
        if (str.isEmpty()) {
            return;
        }
        FloatNumber1 sizeY = str;
        getAsyncCaller()->push(this, [=](TSC){
            QList<IDW_Data_Item> &list = dataHelper->tableMode->getDataList();
            if (number < list.size()) {
                IDW_Data_Item& item = list[number];
                item.setOffsetX(offsetX);
                item.setOffsetY(offsetY);
                item.setSizeX(sizeX);
                item.setSizeY(sizeY);
                MainThreadInvoker::getInvoker().runLater([this, number](){
                    dataHelper->tableMode->notifyDataChanged(number, number);
                });
                CANVAS_CALLER(this, [this, item, number](TSC){
                    QList<IDW_Data_Item>* dataList = dataHelper->canvas->localEditData().getDataList();
                    if (dataList && number < dataList->size()) {
                        (*dataList)[number] = item;
                    }
                    CANVAS_UPDATE
                });
            }
        });
    });
    connect(dataHelper->upBtn->getQPushButton(), &CDE90Button::clicked, this, [this](){
        getAsyncCaller()->push(this, [this](TSC){
            swapRows(true);
        });
    });
    connect(dataHelper->downBtn->getQPushButton(), &CDE90Button::clicked, this, [this](){
        getAsyncCaller()->push(this, [this](TSC){
            swapRows(false);
        });
    });
    connect(dataHelper->shiftBtn->getQPushButton(), &CDE90Button::clicked, this, [this](){
        QModelIndexList list = dataHelper->table->selectionModel()->selectedRows();
        if (list.isEmpty()) {
            return;
        }
        QList<qint32> numbers(list.size());
        for (int i = 0; i < list.size(); ++ i) {
            numbers.replace(i, list[i].row());
        }
        QString strX = dataHelper->shiftXTextField->getQLineEdit()->text();
        if (strX.isEmpty()) {
            strX = "0";
        }
        QString strY = dataHelper->shiftYTextField->getQLineEdit()->text();
        if (strY.isEmpty()) {
            strY = "0";
        }
        if (strX == "0" && strY == "0") {
            return;
        }
        getAsyncCaller()->push(this, [this, numbers, strX, strY](TSC) mutable{
            dataHelper->tableMode->updateDatas(numbers, [&strX, &strY](int, IDW_Data_Item& item){
                item.setOffsetX(item.getOffsetX() + strX);
                item.setOffsetY(item.getOffsetY() + strY);
            });
            //TODO 全体更新，待优化
            CANVAS_STOP(DATA_UPDATE)
            CANVAS_CALLER(GID(DATA_UPDATE), [this](TSC){
                getAsyncCaller()->pushBlock(this, [this](TSC){
                    dataHelper->canvas->localEditData() = dataHelper->editData;
                });
                CANVAS_UPDATE
            });
        });
    });
    connect(dataHelper->canvas, &IdwSubEditorCanvas::itemChecked, this, [this](const IDW_EditUnitPointInfo& info){
        if (info.repeat || info.number < 0) {
            return;
        }
        int number = info.number;
        getAsyncCaller()->push(this, [this, number](TSC){
            if (dataHelper->tableMode->getDataList().size() > number) {
                MainThreadInvoker::getInvoker().runLater([this, number](){
                    dataHelper->table->selectRow(number);
                });
            }
        });
    });
    connect(dataHelper->closeBtn->getQPushButton(), &CDE90Button::clicked, this, [this](){
        this->close();
    });

    QPushButton* iconBtn = iconButton();
    iconBtn->setIcon(ResourceManager::instance()->getSudokuIcon());
    CDE90Menu* iconMenu = new CDE90Menu(iconBtn);
    iconBtn->setMenu(iconMenu);
    QAction* act = iconMenu->addAction("Pitch Measure...");
    connect(act, &QAction::triggered, this, [this]() {
        autoFillSize();
    });


    if (dataHelper->subArrayBtn) {
        connect(dataHelper->subArrayBtn->getQPushButton(), &CDE90Button::clicked, this, [this](){
            dataHelper->chipEditor->show();
            MainThreadInvoker::getInvoker().moveToScreenCenter(dataHelper->chipEditor);
            dataHelper->chipEditor->updateCanvas();
        });
    }
}


void IdwWaferSubEditor::localCreateItems(const IDW_CreateDataParam& param) {
    qint32 listSize = param.arrayX * param.arrayY;
    if (listSize <= 0) {
        return;
    }
    FloatNumber1 currStartX = param.origin.x;
    FloatNumber1 currStartY = param.origin.y;
    FloatNumber1 pitchX = param.pitchX < param.sizeX ? param.sizeX : param.pitchX;
    FloatNumber1 pitchY = param.pitchY < param.sizeY ? param.sizeY : param.pitchY;

    dataHelper->tableMode->resizeDataList(listSize);
    QList<IDW_Data_Item> &itemList = *(dataHelper->editData.getDataList());
    for (int i=0; i < listSize; ++ i) {
        if (!(i%param.arrayY)) {
            currStartY = param.origin.y;
            if (i > 0) {
                currStartX += pitchX;
            }
        } else {
            currStartY += pitchY;
        }
        IDW_Data_Item& item = itemList[i];
        item.setNumber(i);
        item.setOffsetX(currStartX);
        item.setOffsetY(currStartY);
        item.setSizeX(param.sizeX);
        item.setSizeY(param.sizeY);
    }
    if (dataHelper->editData.getDirection() != 0) {
        sortDirection(*(dataHelper->editData.getDataList()), MEASURE_SORT_MATRIX(dataHelper->editData.getDirection()));
    }
    dataHelper->tableMode->notifyDataChanged(0, listSize - 1);
}
int openSimpleMessage(QWidget* parent, const QString& text) {
    CDE90MessageBox d(parent);
    d.setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    d.setWindowTitle("Waiting input");
    d.setText(text);
    d.addLeftButton("OK", true);
    d.addRightButton("Cancel", false);
    d.setResizable(false);
    d.getContent()->setStyleSheet(
        "QLabel {color: rgb(79, 73, 246);} QPushButton {color: rgb(79, 73, 246);}");
    return d.exec();
};
void saveTempImage(const QImage& image, const QString head, const QString end) {
    QDateTime now = QDateTime::currentDateTime();
    QString fileName = "./temp_img/" + now.toString("yyyy_MM_dd/") + head
                       + now.toString("HH_mm_ss_zzz") + end;
    QFile file(fileName);
    QDir dir = QFileInfo(file).absoluteDir();
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            return;
        }
    }
    QString s = dir.absolutePath() + "/" + QFileInfo(file).fileName();
    image.save(s);
}
void IdwWaferSubEditor::autoFillSize() {
    bool succ;
    MainThreadInvoker::getInvoker()
        .blockRun([&succ]() {
            succ = image_operation::openSingleImageOperation(true).ok;
        });
    if (!succ) {
        simpleMessageBox(this, "Error", "Error to open Image Operation Window");
        return;
    }
    int r = openSimpleMessage(nullptr,
                              "Move the stage to a unique chip patern and "
                              "click the [OK] button.");
    if (r)
        return;
    cdsem::CDSEM_GlobalDataHelper::get()->getImage(IMG_OPT_TYPE_SEM,
        [this](const cdsem::TmpImageData& imageData, const ImageAddtionDataModel& otherData) {
            tmpToQImage(imageData, dataHelper->firstImage);
            dataHelper->firstImagePosition.x = otherData.stagePos.x();
            dataHelper->firstImagePosition.y = otherData.stagePos.y();
            dataHelper->firstImagePosition.r = otherData.stageRot;
            dataHelper->firstImagePosition.z = otherData.stageZ;
            dataHelper->firstImagePosition.t = otherData.stageTilt;
            dataHelper->firstImageScanMode.setPixelX(otherData.deviceImageSize.width());
            dataHelper->firstImageScanMode.setPixelY(otherData.deviceImageSize.height());
            dataHelper->firstImageScanMode.setPixelSize(otherData.pixelSize);
            dataHelper->firstImageScanMode.setRotation(otherData.imageRot);
            dataHelper->firstImageScanMode.setScanRate(otherData.scanRate);
            dataHelper->firstImageScanMode.setFrame(otherData.frame);
            QRect tempRect;
            tempRect.setLeft(dataHelper->firstImage.width() / 4);
            tempRect.setTop(dataHelper->firstImage.height() / 4);
            tempRect.setWidth(dataHelper->firstImage.width() / 2);
            tempRect.setHeight(dataHelper->firstImage.height() / 2);
            dataHelper->tempImage = dataHelper->firstImage.copy(tempRect);
            saveTempImage(dataHelper->firstImage, "autoFillSize", "first.bmp");
            saveTempImage(dataHelper->tempImage, "autoFillSize", "temp.bmp");
            MainThreadInvoker::getInvoker().runLater([this]() { autoFillSizeStep2(); });
        });
}
void IdwWaferSubEditor::autoFillSizeStep2() {
    int r = openSimpleMessage(
        nullptr,
        "Move the stage to the right chip whih the same unique patern int the center of view and "
        "click the [OK] button.");
    if (r)
        return;
    //这里最好按照第一张图的放大倍率再拍一张
    cdsem::CDSEM_GlobalDataHelper::get()->getImage(IMG_OPT_TYPE_SEM,
        [this](const cdsem::TmpImageData& imageData, const ImageAddtionDataModel& otherData) {
            tmpToQImage(imageData, dataHelper->rightImage);
            dataHelper->rightImagePosition.x = otherData.stagePos.x();
            dataHelper->rightImagePosition.y = otherData.stagePos.y();
            dataHelper->rightImagePosition.r = otherData.stageRot;
            dataHelper->rightImagePosition.z = otherData.stageZ;
            dataHelper->rightImagePosition.t = otherData.stageTilt;
            dataHelper->rightImageScanMode.setPixelX(otherData.deviceImageSize.width());
            dataHelper->rightImageScanMode.setPixelY(otherData.deviceImageSize.height());
            dataHelper->rightImageScanMode.setPixelSize(otherData.pixelSize);
            dataHelper->rightImageScanMode.setRotation(otherData.imageRot);
            dataHelper->rightImageScanMode.setScanRate(otherData.scanRate);
            dataHelper->rightImageScanMode.setFrame(otherData.frame);
            MatchPatternResult rightResult = algo::searchFromTemplate(dataHelper->rightImage, dataHelper->tempImage);
            saveTempImage(dataHelper->rightImage, "autoFillSize", "right.bmp");
            QPointF viewPos = cdsem::CDSEM_GlobalDataHelper::get()->viewShiftToStage(
                QPointF(rightResult.shift.x() * dataHelper->rightImageScanMode.getPixelSize().toDouble(),
                        rightResult.shift.y() * dataHelper->rightImageScanMode.getPixelSize().toDouble()));
            //单位是纳米
            dataHelper->rightImagePosition.x = dataHelper->rightImagePosition.x + viewPos.x() / 1000 / 1000 / 1000;
            dataHelper->rightImagePosition.y = dataHelper->rightImagePosition.y + viewPos.y() / 1000 / 1000 / 1000;
            MainThreadInvoker::getInvoker().runLater([this]() { autoFillSizeStep3(); });
        });
}
void IdwWaferSubEditor::autoFillSizeStep3() {
    int r = openSimpleMessage(
        nullptr,
        "Move the stage to the down chip whih the same unique patern int the center of view and "
        "click the [OK] button.");
    if (r)
        return;
    cdsem::CDSEM_GlobalDataHelper::get()->getImage(IMG_OPT_TYPE_SEM,
        [this](const cdsem::TmpImageData& imageData, const ImageAddtionDataModel& otherData) {
            tmpToQImage(imageData, dataHelper->downImage);
            dataHelper->downImagePosition.x = otherData.stagePos.x();
            dataHelper->downImagePosition.y = otherData.stagePos.y();
            dataHelper->downImagePosition.r = otherData.stageRot;
            dataHelper->downImagePosition.z = otherData.stageZ;
            dataHelper->downImagePosition.t = otherData.stageTilt;
            dataHelper->downImageScanMode.setPixelX(otherData.deviceImageSize.width());
            dataHelper->downImageScanMode.setPixelY(otherData.deviceImageSize.height());
            dataHelper->downImageScanMode.setPixelSize(otherData.pixelSize);
            dataHelper->downImageScanMode.setRotation(otherData.imageRot);
            dataHelper->downImageScanMode.setScanRate(otherData.scanRate);
            dataHelper->downImageScanMode.setFrame(otherData.frame);
            MatchPatternResult downResult = algo::searchFromTemplate(dataHelper->downImage, dataHelper->tempImage);
            saveTempImage(dataHelper->downImage, "autoFillSize", "down.bmp");

            QPointF viewPos = cdsem::CDSEM_GlobalDataHelper::get()->viewShiftToStage(
                QPointF(downResult.shift.x() * dataHelper->downImageScanMode.getPixelSize().toDouble(),
                        downResult.shift.y() * dataHelper->downImageScanMode.getPixelSize().toDouble()));
            //单位是纳米
            dataHelper->downImagePosition.x = dataHelper->downImagePosition.x + viewPos.x() / 1000 / 1000 / 1000;
            dataHelper->downImagePosition.y = dataHelper->downImagePosition.y + viewPos.y() / 1000 / 1000 / 1000;
            MainThreadInvoker::getInvoker().runLater([this]() { autoFillSizeStep4(); });
        });
}
void IdwWaferSubEditor::autoFillSizeStep4() {
    QPointF firstPosition = cdsem::CDSEM_GlobalDataHelper::get()->getWaferPosition(
        dataHelper->firstImagePosition);
    QPointF rightPosition = cdsem::CDSEM_GlobalDataHelper::get()->getWaferPosition(
        dataHelper->rightImagePosition);
    QPointF downPosition = cdsem::CDSEM_GlobalDataHelper::get()->getWaferPosition(dataHelper->downImagePosition);

    double chipWidth = sqrt(
        (firstPosition.x() - rightPosition.x()) * (firstPosition.x() - rightPosition.x())
        + (firstPosition.y() - rightPosition.y()) * (firstPosition.y() - rightPosition.y()));
    double chipHeight = sqrt(
        (downPosition.x() - rightPosition.x()) * (downPosition.x() - rightPosition.x())
        + (downPosition.y() - rightPosition.y()) * (downPosition.y() - rightPosition.y()));

    int r = openSimpleMessage(nullptr,
                              QString("chipWidth=%0, chipHeight=%1").arg(chipWidth).arg(chipHeight));
    if (r)
        return;
}
void IdwWaferSubEditor::collectDataAndCreate() {
    IDW_CreateDataParam param;
    QString str = dataHelper->arrayXTextField->getQLineEdit()->text();
    if (str.isEmpty()) {
        return;
    }
    param.arrayX = str.toInt();
    str = dataHelper->arrayYTextField->getQLineEdit()->text();
    param.arrayY = str.toInt();
    if (str.isEmpty()) {
        return;
    }
    str = dataHelper->sizeXTextField->getQLineEdit()->text();
    if (str.isEmpty()) {
        return;
    }
    param.sizeX = str;
    str = dataHelper->sizeYTextField->getQLineEdit()->text();
    if (str.isEmpty()) {
        return;
    }
    param.sizeY = str;
    if (!dataHelper->chipEditor && param.arrayX * param.arrayY * dataHelper->shotSize > IDW_SHOT_CHIP_MAX_ARRAY) {
        simpleMessageBox(this, "Warn", "The matrix has exceeded the maximum value of 50000.");
        return;
    }

    str = dataHelper->pitchXTextField->getQLineEdit()->text();
    if (!str.isEmpty()) {
        param.pitchX = str;
        if (param.pitchX < param.sizeX) {
            return;
        }
    } else {
        param.pitchX = 0;
    }
    str = dataHelper->pitchYTextField->getQLineEdit()->text();
    if (!str.isEmpty()) {
        param.pitchY = str;
        if (param.pitchY < param.sizeY) {
            return;
        }
    } else {
        param.pitchY = 0;
    }
    param.direction = MEASURE_SORT_2431;
    dataHelper->createBtn->loading();
    getAsyncCaller()->push(this, [this, param](TSC) mutable{
        sem::AfterGc gc([this](){
            MainThreadInvoker::getInvoker().runLater([this](){
                dataHelper->createBtn->overLoading();
            });
        });
        if (!localCreateData(param)) {
            return;
        }
        CANVAS_STOP(DATA_UPDATE)
        CANVAS_CALLER(GID(DATA_UPDATE), [this](TSC){
            getAsyncCaller()->pushBlock(this, [this](TSC){
                dataHelper->canvas->localEditData() = dataHelper->editData;
                dataHelper->canvas->localWaferData() = *dataHelper->waferData;
            });
            CANVAS_UPDATE
        });
    });
}

void IdwWaferSubEditor::swapRows(bool forward) {
    QModelIndexList list = dataHelper->table->selectionModel()->selectedRows();
    if (list.isEmpty()) {
        return;
    }
    QList<qint32> indices(list.size());
    for (int i = 0; i < list.size(); ++ i) {
        indices.replace(i, list[i].row());
    }
    bool hasChanged = dataHelper->tableMode->swapData(indices, forward, true, [](IDW_Data_Item& item1, IDW_Data_Item& item2){
        qint32 n = item1.getNumber();
        item1.setNumber(item2.getNumber());
        item2.setNumber(n);
    });
    if (hasChanged) {
        //如果都使用DATA_UPDATE这个ID作为组ID更新，不要在这里call stop。
        CANVAS_CALLER(GID(DATA_UPDATE), [this, indices, forward](TSC) mutable{
            QList<IDW_Data_Item>* list = dataHelper->canvas->localEditData().getDataList();
            if (!list || list->isEmpty()) {
                return;
            }
            dataHelper->canvas->localClearCheckedItem();
            sem::swapListRows<IDW_Data_Item>(*list, indices, forward, false, nullptr, nullptr, [this](IDW_Data_Item& item1, IDW_Data_Item& item2){
                qint32 n = item1.getNumber();
                item1.setNumber(item2.getNumber());
                item2.setNumber(n);
                dataHelper->canvas->localCheckItem(item2.getNumber(), 1);
            });
            CANVAS_UPDATE
        });
    }
}

void IdwWaferSubEditor::closeEvent(QCloseEvent *event) {
    Q_UNUSED(event)
    emit onClose();
}

bool IdwWaferSubEditor::localCreateData(IDW_CreateDataParam& param) {
    qint32 listSize = param.arrayX * param.arrayY;
    if (listSize <= 0) {
        return false;
    }
    dataHelper->editData.setDirection(param.direction);
    dataHelper->editData.setArrayX(param.arrayX);
    dataHelper->editData.setArrayY(param.arrayY);
    dataHelper->editData.setSizeX(param.sizeX);
    dataHelper->editData.setSizeY(param.sizeY);
    dataHelper->editData.setPitchX(param.pitchX);
    dataHelper->editData.setPitchY(param.pitchY);
    dataHelper->originPointCalc(param);
    localCreateItems(param);
    if (dataHelper->chipEditor) {
        dataHelper->chipEditor->setShotSize(param.arrayX * param.arrayY);
        IDW_CreateDataParam chipParam;
        chipParam.arrayX = 1;
        chipParam.arrayY = 1;
        chipParam.sizeX = param.sizeX;
        chipParam.sizeY = param.sizeY;
        chipParam.pitchX = 0;
        chipParam.pitchY = 0;
        chipParam.direction = char(MEASURE_SORT_2431);
        dataHelper->chipEditor->localCreateData(chipParam);
        dataHelper->chipEditor->setInputData(chipParam);
        dataHelper->waferData->getInvalidNumbers().clear();
        dataHelper->waferData->getTegNumbers().clear();
        dataHelper->waferData->setOriginalNumber(0);
    }
    return true;
}

void IdwWaferSubEditor::setInputData(const IDW_CreateDataParam& param) {
    if (!MainThreadInvoker::getInvoker().isMainThread()) {
        MainThreadInvoker::getInvoker().blockRun([this, &param](){
            setInputData(param);
        });
        return;
    }
    dataHelper->arrayXTextField->getQLineEdit()->setText(QString::number(param.arrayX));
    dataHelper->arrayYTextField->getQLineEdit()->setText(QString::number(param.arrayY));
    dataHelper->sizeXTextField->getQLineEdit()->setText(param.sizeX.toQStringAndKeepFullDecimal());
    dataHelper->sizeYTextField->getQLineEdit()->setText(param.sizeY.toQStringAndKeepFullDecimal());
    dataHelper->pitchXValidator->setBottom(param.sizeX);
    dataHelper->pitchYValidator->setBottom(param.sizeY);
    if (param.pitchX < param.sizeX) {
        dataHelper->pitchXTextField->getQLineEdit()->clear();
    } else {
        dataHelper->pitchXTextField->getQLineEdit()->setText(param.pitchX.toQStringAndKeepFullDecimal());
    }
    if (param.pitchY < param.sizeY) {
        dataHelper->pitchYTextField->getQLineEdit()->clear();
    } else {
        dataHelper->pitchYTextField->getQLineEdit()->setText(param.pitchY.toQStringAndKeepFullDecimal());
    }
}

void IdwWaferSubEditor::updateCanvas() {
    CANVAS_CALLER(GID(DATA_UPDATE), [this](TSC){
        getAsyncCaller()->pushBlock(this, [this](TSC){
            dataHelper->canvas->localEditData() = dataHelper->editData;
            dataHelper->canvas->localWaferData() = *dataHelper->waferData;
        });
        CANVAS_UPDATE
    });
}

IDW_ShotChipData& IdwWaferSubEditor::localGetShotChipData() {
    return dataHelper->editData;
}

IdwDetailDataMode& IdwWaferSubEditor::getTableDataMode() {
    return *dataHelper->tableMode;
}

void IdwWaferSubEditor::setShotSize(qint32 size) {
    dataHelper->shotSize = size;
}
