#include "image_scan_setting_controller.h"
#include <QVBoxLayout>
#include "cbox_global_keys.h"
#include "cde90_button.h"
#include "cde90_message_box.h"
#include "cde90_spliter_line.h"
#include "cdsem_number_validator_helper.h"
#include "image_save_model.h"
#include "sem_global.h"
#include "ui_image_scan_setting_content.h"
#include "cdsem_global_data.h"
#include "cdsem_global_env_data_model.h"
#include "image_file_temp_handler.h"
#include "image_type_defition.h"

ImageScanSettingContent::ImageScanSettingContent(QWidget* parent)
    : QWidget(parent) {}

ImageScanSettingContent::~ImageScanSettingContent() noexcept {
    if (ui) {
        delete ui;
    }
}

void ImageScanSettingContent::create() {
    ui = new cdsem::UiImageScanSettingContent;
    ui->init(this);
}

void ImageScanSettingContent::createAction() {
    CDSemNumberValidatorHelper::min(0).put(ui->magTextField->getQLineEdit()).autoFixMode();
    CDSemNumberValidatorHelper::doubleFree<1>().put(ui->rotTextField).autoFixMode();
    CDSemNumberValidatorHelper::range(-100, 100)
        .put(ui->energyFilterTextField->getQLineEdit())
        .autoFixMode();
    CDSemNumberValidatorHelper::range(0, 100)
        .put(ui->bTextField->getQLineEdit())
        .put(ui->cTextField->getQLineEdit())
        .autoFixMode();
    CDSemNumberValidatorHelper::range(0, 100).put(ui->bseTextField->getQLineEdit()).autoFixMode();
    connect(ui->energyFilterCheckBox, &QCheckBox::toggled, this, [this](bool b) {
        ui->energyFilterTextField->setDisabled(!b);
        ui->bseTextField->setDisabled(!b);
    });

    caller->manage(ui->filterItemBox, this);
    caller->manage(ui->scanRateItemBox, this);
    caller->manage(ui->frameItemBox, this);
    caller->manage(ui->pixelXItemBox, this);
    caller->manage(ui->pixelYItemBox, this);

    ui->filterItemBox->setAsyncCaller(caller);
    ui->scanRateItemBox->setAsyncCaller(caller);
    ui->frameItemBox->setAsyncCaller(caller);
    ui->pixelXItemBox->setAsyncCaller(caller);
    ui->pixelYItemBox->setAsyncCaller(caller);

    ui->filterItemBox->loadItemFromDatabase(CBOX_TYPE_SEM_IMG_FILTER);
    ui->scanRateItemBox->loadItemFromDatabase(CBOX_TYPE_SEM_IMG_SCAN_RATE);
    ui->frameItemBox->loadItemFromDatabase(CBOX_TYPE_SEM_IMG_FRAME);
    ui->pixelXItemBox->loadItemFromDatabase(CBOX_TYPE_SEM_IMG_X_PIX_COUNT);
    ui->pixelYItemBox->loadItemFromDatabase(CBOX_TYPE_SEM_IMG_Y_PIX_COUNT);

    connect(ui->energyFilterCheckBox,
            &QCheckBox::toggled,
            this,
            &ImageScanSettingContent::energyFilterChanged);
    connect(ui->energyFilterTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this]() {
        emit energyFilterValueChanged(ui->energyFilterTextField->text());
    });
    connect(ui->bseTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this]() {
        emit bsePrecentChanged(ui->bseTextField->text());
    });
    connect(ui->bTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this]() {
        emit brightnessChanged(fromTextFieldText(ui->bTextField, VEVM_WITH_PLACE_HOLD).toInt());
    });
    connect(ui->cTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this]() {
        emit contrastChanged(fromTextFieldText(ui->cTextField, VEVM_WITH_PLACE_HOLD).toInt());
    });
    connect(ui->magTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this]() {
        emit magChanaged(ui->magTextField->text());
    });
    connect(ui->rotTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this]() {
        emit rotChanaged(ui->rotTextField->text());
    });
    connect(ui->scanRateItemBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this]() {
        emit scanModeChanged(ui->scanRateItemBox->currentValue());
    });
    connect(ui->flatModeCheckBox,
            &QCheckBox::toggled,
            this,
            &ImageScanSettingContent::flatModeChanged);
    connect(ui->frameItemBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this]() {
        emit frameChanged(ui->frameItemBox->currentValue());
    });
    connect(ui->filterItemBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this]() {
        emit filterChanged(ui->filterItemBox->currentValue());
    });
    connect(ui->pixelXItemBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this]() {
        emit pixelXChanged(ui->pixelXItemBox->currentValue());
    });
    connect(ui->pixelYItemBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this]() {
        emit pixelYChanged(ui->pixelYItemBox->currentValue());
    });
    connect(ui->readBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        ui->bTextField->setText(int(std::round(SEM_PY_ENV.getBrightness())));
        ui->cTextField->setText(int(std::round(SEM_PY_ENV.getContrast())));
        ui->magTextField->setText(SEM_PY_ENV.getMag());
        ui->rotTextField->setText(FloatNumber1(SEM_PY_ENV.getBeamRot()).toMinString());
        ui->scanRateItemBox->safeSetCurrentData(SEM_PY_ENV.getScanRate());
        ui->frameItemBox->safeSetCurrentData(SEM_PY_ENV.getFrame());

        cdsem::CDSEM_GlobalDataHelper::get()->invokeImage([this](){
            const cdsem::TmpImageData* image;
            cdsem::CDSEM_GlobalDataHelper::get()->getImage(IMG_OPT_TYPE_SEM, image);
            QSize size = image->originSize;
            if (!size.isValid()) {
                size = image->size;
            }
            MainThreadInvoker::getInvoker().runLater([this, size](){
                ui->pixelXItemBox->safeSetCurrentData(size.width());
                ui->pixelYItemBox->safeSetCurrentData(size.height());
            });
        });
    });
}

void ImageScanSettingContent::initAsyncCaller(AsyncCaller* caller) {
    Q_ASSERT_X(this->caller == nullptr,
               "ImageScanSettingContent::initAsyncCaller",
               "caller只能被初始化一次");
    this->caller = caller;
    create();
    createAction();
}

QWidget* ImageScanSettingContent::getTitleContent() const {
    return ui->titleContent;
}

QWidget* ImageScanSettingContent::getDetailContent() const {
    return ui->detailContent;
}

QString ImageScanSettingContent::collectData(ImageScanModel& param) {
    param.rot = fromTextFieldText(ui->rotTextField, VEVM_EMPTY, "0");
    param.energyFilterValue = fromTextFieldToInt(ui->energyFilterTextField, 0);
    param.bsePrecent = fromTextFieldToInt(ui->bseTextField, 0);
    param.b = fromTextFieldText(ui->bTextField, VEVM_WITH_PLACE_HOLD).toInt();
    param.c = fromTextFieldText(ui->cTextField, VEVM_WITH_PLACE_HOLD).toInt();
    param.mag = fromTextFieldToInt(ui->magTextField, 0);
    param.scanRate = ui->scanRateItemBox->currentValue();
    if (param.scanRate < 0) {
        return "Scan mode is not specified.";
    }
    param.frame = ui->frameItemBox->currentValue();
    if (param.frame < 0) {
        return "Frame is not specified.";
    }
    param.filter = ui->filterItemBox->currentValue();
    if (param.filter < 0) {
        return "Filter is not specified.";
    }
    param.pixelX = ui->pixelXItemBox->currentValue();
    if (param.pixelX < 0) {
        return "Pixel x is not specified.";
    }
    param.pixelY = ui->pixelYItemBox->currentValue();
    if (param.pixelY < 0) {
        return "Pixel y is not specified.";
    }
    param.enableEnergyFilter = ui->energyFilterCheckBox->isChecked();
    param.enableFlatModel = ui->flatModeCheckBox->isChecked();
    return "";
}

void ImageScanSettingContent::resetToComponent(const ImageScanModel& param) {
    ui->rotTextField->setText(param.rot.toQStringAndKeepFullDecimal());
    ui->energyFilterTextField->setText(param.energyFilterValue.toString());
    ui->bseTextField->setText(param.bsePrecent.toString());
    ui->bTextField->setText(param.b.toMinString());
    ui->cTextField->setText(param.c.toMinString());
    ui->magTextField->setText(param.mag.toString());
    ui->scanRateItemBox->safeSetCurrentData(param.scanRate == -1 ? 0 : param.scanRate);
    ui->frameItemBox->safeSetCurrentData(param.frame == -1 ? 0 : param.frame);
    ui->filterItemBox->safeSetCurrentData(param.filter == -1 ? 0 : param.filter);
    ui->pixelXItemBox->safeSetCurrentData(param.pixelX == -1 ? 0 : param.pixelX);
    ui->pixelYItemBox->safeSetCurrentData(param.pixelY == -1 ? 0 : param.pixelY);
    ui->energyFilterCheckBox->setChecked(param.enableEnergyFilter);
    ui->flatModeCheckBox->setChecked(param.enableFlatModel);
}

void ImageScanSettingContent::resetComponent() {
    ui->rotTextField->getQLineEdit()->clear();
    ui->energyFilterTextField->getQLineEdit()->clear();
    ui->bseTextField->getQLineEdit()->clear();
    ui->bTextField->getQLineEdit()->clear();
    ui->cTextField->getQLineEdit()->clear();
    ui->magTextField->getQLineEdit()->clear();
    ui->scanRateItemBox->safeSetCurrentIndex(0);
    ui->frameItemBox->safeSetCurrentIndex(0);
    ui->filterItemBox->safeSetCurrentIndex(0);
    ui->pixelXItemBox->setCurrentIndex(0);
    ui->pixelYItemBox->setCurrentIndex(0);
    ui->energyFilterCheckBox->setChecked(false);
    ui->flatModeCheckBox->setChecked(false);
}

QString ImageScanSettingContent::getEnergyFilterText() const {
    return ui->energyFilterTextField->text();
}
QString ImageScanSettingContent::getBsePrecentText() const {
    return ui->bseTextField->text();
}
QString ImageScanSettingContent::getMagText() const {
    return ui->magTextField->text();
}
QString ImageScanSettingContent::getRotText() const {
    return ui->rotTextField->text();
}
bool ImageScanSettingContent::isEnableEnergyFilter() const {
    return ui->energyFilterCheckBox->isChecked();
}
FloatNumber1 ImageScanSettingContent::getBrightness() const {
    return fromTextFieldText(ui->bTextField, VEVM_WITH_PLACE_HOLD);
}
FloatNumber1 ImageScanSettingContent::getContrast() const {
    return fromTextFieldText(ui->cTextField, VEVM_WITH_PLACE_HOLD);
}
int ImageScanSettingContent::getScanRate() const {
    return ui->scanRateItemBox->currentValue();
}
bool ImageScanSettingContent::isEnableFlatMode() const {
    return ui->flatModeCheckBox->isChecked();
}
int ImageScanSettingContent::getFrame() const {
    return ui->frameItemBox->currentValue();
}
int ImageScanSettingContent::getFilter() const {
    return ui->filterItemBox->currentValue();
}
int ImageScanSettingContent::getPixelX() const {
    return ui->pixelXItemBox->currentValue();
}
int ImageScanSettingContent::getPixelY() const {
    return ui->pixelYItemBox->currentValue();
}

ImageScanSettingController::ImageScanSettingController(QWidget* parent)
    : CDE90Dialog(parent) {
    connect(this, &ImageScanSettingController::asyncCallerChanged, this, [this]() {
        create();
        createAction();
    });
}

ImageScanSettingController::~ImageScanSettingController() noexcept {}

void ImageScanSettingController::create() {
    QWidget* content = new QWidget;
    setContent(content);

    QVBoxLayout* rootLayout = new QVBoxLayout(content);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(5);
    setLayout(rootLayout);

    QWidget* contentWidget = new QWidget(content);
    QVBoxLayout* contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(5, 5, 5, 5);
    contentWidget->setLayout(contentLayout);
    rootLayout->addWidget(contentWidget);

    imageScanSettingContent = new ImageScanSettingContent(contentWidget);
    contentLayout->addWidget(imageScanSettingContent);

    rootLayout->addWidget(new CDE90SpliterHLine(content));

    QWidget* bottomContent = new QWidget(content);
    QHBoxLayout* bottomLayout = new QHBoxLayout(bottomContent);
    bottomLayout->setContentsMargins(5, 5, 5, 5);
    bottomLayout->setSpacing(5);
    bottomContent->setLayout(bottomLayout);
    rootLayout->addWidget(bottomContent);

    okBtn = new CDE90ButtonBox("OK", bottomContent);
    cancelBtn = new CDE90ButtonBox("Cancel", bottomContent);
    bottomLayout->addWidget(okBtn);
    bottomLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
    bottomLayout->addWidget(cancelBtn);

    
}
void ImageScanSettingController::createAction() {
    bindCloseBtn(cancelBtn->getQPushButton());
    itsChildAndBeManaged(imageScanSettingContent, this);
    imageScanSettingContent->initAsyncCaller(ACaller);

    connect(okBtn->getQPushButton(), &QPushButton::clicked, this, [this]() {
        ImageScanModel param;
        QString errMsg = collectData(param);
        if (errMsg.size() > 0) {
            simpleMessageBox(this, "Error", errMsg);
            return;
        }
        if (okCallback && okCallback(param)) {
            close();
        }
    });
}

QString ImageScanSettingController::collectData(ImageScanModel& param) {
    return imageScanSettingContent->collectData(param);
}
void ImageScanSettingController::resetToComponent(const ImageScanModel& param) {
    imageScanSettingContent->resetToComponent(param);
}
