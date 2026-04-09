#ifndef IMG_ENV_SETTING_CONTENT_H
#define IMG_ENV_SETTING_CONTENT_H
#include <QWidget>
#include "ui_common_global.h"

namespace cdsem {
class UiImgEnvSettingContent;
}
struct ImageSaveEnvModel;
class CDE90ComboBox;

class UI_COMMON_EXPORT ImgEnvSettingContent : public QWidget {
    Q_OBJECT
private:
    cdsem::UiImgEnvSettingContent* ui = nullptr;
    ImageSaveEnvModel* param = nullptr;

    void create();
    void createAction();
public:
     explicit ImgEnvSettingContent(QWidget* parent = nullptr);
     ~ImgEnvSettingContent() noexcept;

     void initParam(ImageSaveEnvModel* param);
     QString collectData();
     void resetToComponent();
     QString collectData(ImageSaveEnvModel& param);
     void resetToComponent(const ImageSaveEnvModel&);

     void hideQuaility(bool b = true);

 signals:
     void overlayMeasurementCursorCBoxToggle(bool);
     void overlayCrossCursorCBoxToggle(bool);
     void overlayAreaCursorCBoxToggle(bool);
     void overlayMeasurementValueCBoxToggle(bool);
     void overlayLineProfileCBoxToggle(bool);
     void overlayUmMarkerCBoxToggle(bool);
     void saveSizeItemBoxIndexChanged(CDE90ComboBox*);
     void saveCompressionItemBoxIndexChanged(CDE90ComboBox*);
};
#endif
