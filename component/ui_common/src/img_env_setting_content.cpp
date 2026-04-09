#include "img_env_setting_content.h"
#include "cdsem_number_validator_helper.h"
#include "image_save_model.h"
#include "ui_img_env_setting_content.h"

ImgEnvSettingContent::ImgEnvSettingContent(
    QWidget* parent)
    : QWidget(parent) {
    create();
    createAction();
}

ImgEnvSettingContent::~ImgEnvSettingContent() noexcept {
    if (ui) {
        delete ui;
    }
}

void ImgEnvSettingContent::create() {
    ui = new cdsem::UiImgEnvSettingContent;
    ui->init(this);
}

void ImgEnvSettingContent::createAction() {
    CDSemNumberValidatorHelper::range(0, 100).put(ui->qualityTextField).autoFixMode();
    connect(ui->overlayMeasurementCursorCBox,
            &QCheckBox::toggled,
            this,
            &ImgEnvSettingContent::overlayMeasurementCursorCBoxToggle);
    connect(ui->overlayCrossCursorCBox,
            &QCheckBox::toggled,
            this,
            &ImgEnvSettingContent::overlayCrossCursorCBoxToggle);
    connect(ui->overlayAreaCursorCBox,
            &QCheckBox::toggled,
            this,
            &ImgEnvSettingContent::overlayAreaCursorCBoxToggle);
    connect(ui->overlayMeasurementValueCBox,
            &QCheckBox::toggled,
            this,
            &ImgEnvSettingContent::overlayMeasurementValueCBoxToggle);
    connect(ui->overlayLineProfileCBox,
            &QCheckBox::toggled,
            this,
            &ImgEnvSettingContent::overlayLineProfileCBoxToggle);
    connect(ui->overlayUmMarkerCBox,
            &QCheckBox::toggled,
            this,
            &ImgEnvSettingContent::overlayUmMarkerCBoxToggle);

    connect(ui->saveSizeItemBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int) {
        emit saveSizeItemBoxIndexChanged(ui->saveSizeItemBox);
    });
    connect(ui->saveCompressionItemBox->getQComboBox(),
            &QComboBox::currentIndexChanged,
            this,
            [this](int) { emit saveCompressionItemBoxIndexChanged(ui->saveCompressionItemBox); });
}

void ImgEnvSettingContent::initParam(
    ImageSaveEnvModel* param) {
    this->param = param;
}
QString ImgEnvSettingContent::collectData() {
    if (!param) {
        return "";
    }
    return collectData(*param);
}
void ImgEnvSettingContent::resetToComponent() {
    if (!param) {
        return;
    }
    resetToComponent(*param);
}

QString ImgEnvSettingContent::collectData(
    ImageSaveEnvModel& param) {
    int val = ui->saveCompressionItemBox->currentValue();
    if (val < 0) {
        return "Compression is not specified.";
    }
    param.compression = val;
    val = ui->saveSizeItemBox->currentValue();
    if (val < 0) {
        return "Size is not specified.";
    }
    param.size = val;
    param.enableMeaureCursor = ui->overlayMeasurementCursorCBox->isChecked();
    param.enableCrossCursor = ui->overlayCrossCursorCBox->isChecked();
    param.enableAreaCursor = ui->overlayAreaCursorCBox->isChecked();
    param.enableMeasureValue = ui->overlayMeasurementValueCBox->isChecked();
    param.enableLineProfile = ui->overlayLineProfileCBox->isChecked();
    param.enableUmMarker = ui->overlayUmMarkerCBox->isChecked();
    QString str = ui->qualityTextField->text();
    if (str.isEmpty()) {
        param.quality = nullptr;
    } else {
        param.quality = str.toInt();
    }
    return "";
}
void ImgEnvSettingContent::resetToComponent(
    const ImageSaveEnvModel& param) {
    ui->overlayMeasurementCursorCBox->blockSignals(true);
    ui->overlayMeasurementCursorCBox->setChecked(param.enableMeaureCursor);
    ui->overlayMeasurementCursorCBox->blockSignals(false);

    ui->overlayCrossCursorCBox->blockSignals(true);
    ui->overlayCrossCursorCBox->setChecked(param.enableCrossCursor);
    ui->overlayCrossCursorCBox->blockSignals(false);

    ui->overlayAreaCursorCBox->blockSignals(true);
    ui->overlayAreaCursorCBox->setChecked(param.enableAreaCursor);
    ui->overlayAreaCursorCBox->blockSignals(false);

    ui->overlayMeasurementValueCBox->blockSignals(true);
    ui->overlayMeasurementValueCBox->setChecked(param.enableMeasureValue);
    ui->overlayMeasurementValueCBox->blockSignals(false);

    ui->overlayLineProfileCBox->blockSignals(true);
    ui->overlayLineProfileCBox->setChecked(param.enableLineProfile);
    ui->overlayLineProfileCBox->blockSignals(false);

    ui->overlayUmMarkerCBox->blockSignals(true);
    ui->overlayUmMarkerCBox->setChecked(param.enableUmMarker);
    ui->overlayUmMarkerCBox->blockSignals(false);

    ui->saveSizeItemBox->blockSignals(true);
    ui->saveSizeItemBox->setCurrentData(param.size);
    ui->saveSizeItemBox->blockSignals(false);

    ui->saveCompressionItemBox->blockSignals(true);
    ui->saveCompressionItemBox->setCurrentData(param.compression);
    ui->saveCompressionItemBox->blockSignals(false);

    ui->qualityTextField->setText(param.quality.toString());
}

void ImgEnvSettingContent::hideQuaility(
    bool b) {
    ui->qualityLabel->setVisible(!b);
    ui->qualityTextField->setVisible(!b);
}
