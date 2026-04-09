#ifndef IMAGE_SCAN_SETTING_CONTROLLER_H
#define IMAGE_SCAN_SETTING_CONTROLLER_H
#include "cde90_dialog.h"
#include "common_act_param_helper.h"
#include "ui_common_global.h"
#include "float_number.h"

namespace cdsem {
class UiImageScanSettingContent;
};
struct ImageScanModel;
class CDE90ButtonBox;
class AsyncCaller;

class UI_COMMON_EXPORT ImageScanSettingContent : public QWidget {
    Q_OBJECT
private:
    cdsem::UiImageScanSettingContent* ui = nullptr;
    AsyncCaller* caller = nullptr;

    void create();
    void createAction();

public:
    explicit ImageScanSettingContent(QWidget* parent = nullptr);
    ~ImageScanSettingContent() noexcept;

    void initAsyncCaller(AsyncCaller* caller);

    QWidget* getTitleContent() const;
    QWidget* getDetailContent() const;

    QString collectData(ImageScanModel& param);
    void resetToComponent(const ImageScanModel& param);
    void resetComponent();

    QString getEnergyFilterText() const;
    QString getBsePrecentText() const;
    QString getMagText() const;
    QString getRotText() const;
    bool isEnableEnergyFilter() const;
    FloatNumber1 getBrightness() const;
    FloatNumber1 getContrast() const;
    int getScanRate() const;
    bool isEnableFlatMode() const;
    int getFrame() const;
    int getFilter() const;
    int getPixelX() const;
    int getPixelY() const;

signals:
    void energyFilterChanged(bool);
    void energyFilterValueChanged(const QString&);
    void bsePrecentChanged(const QString&);
    void brightnessChanged(int);
    void contrastChanged(int);
    void magChanaged(const QString&);
    void rotChanaged(const QString&);
    void scanModeChanged(int);
    void flatModeChanged(bool);
    void frameChanged(int);
    void filterChanged(int);
    void pixelXChanged(int);
    void pixelYChanged(int);
};

class UI_COMMON_EXPORT ImageScanSettingController : public CDE90Dialog {
    SEM_OK_CALL(ImageScanModel)
private:
    ImageScanSettingContent* imageScanSettingContent;
    CDE90ButtonBox* okBtn;
    CDE90ButtonBox* cancelBtn;

    void create();
    void createAction();

public:
    explicit ImageScanSettingController(QWidget* parent = nullptr);
    ~ImageScanSettingController() noexcept;

    QString collectData(ImageScanModel& param);
    void resetToComponent(const ImageScanModel& param);
};
#endif
