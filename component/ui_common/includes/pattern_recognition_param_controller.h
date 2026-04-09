#ifndef PATTERN_RECOGNITION_PARAM_CONTROLLER_H
#define PATTERN_RECOGNITION_PARAM_CONTROLLER_H
#include "cde90_dialog.h"
#include "common_act_param_helper.h"
#include "template_data_model.h"
#include "ui_common_global.h"
#include "cdsem_data_common.h"

namespace cdsem {
class UiPatternRecognitionParamController;
};

struct PatternRegParam {
    PatternRecognitionParamSettingParam patternRegParam;
    QImage image;
    bool imgUpdated = false;
};

struct PatternRecognitionParamControllerDataHelper;

class ImageOperationImageCanvas;
class PatternRegInfoController;
class PatternRecognitionController;
struct NormalImageRegResult;
class AmpCommonData;
class UI_COMMON_EXPORT PatternRecognitionParamController : public CDE90Dialog{
    friend struct PatternRecognitionParamControllerDataHelper;
    SEM_SET_INIT_PARAM(
        PatternRegParam)
    SEM_OK_CALL(
        PatternRegParam)
private:
    PatternRecognitionParamControllerDataHelper* dataHelper = nullptr;
    cdsem::UiPatternRecognitionParamController* ui = nullptr;

    void create();
    void createAction();
    void resetToComponent();
    QString collectData();

public:
    explicit PatternRecognitionParamController(QWidget* parent = nullptr);
    ~PatternRecognitionParamController() noexcept;

    void setMode(cdsem::PatternRegParamOpenMode);

    void setLoadStatus(bool = true);
    void setImageOnly(const QImage& image);
    void setImageAdditionOnly(const ImageAddtionDataModel& param);

    void hideNextBtn(bool b = true);
    void hideBackBtn(bool b = true);

    //仅MP时需要使用
    void setDefaultDNumber(int);
    const QImage& getMeasureImage() const;
    const ImageAddtionDataModel& getMeasureImageAddition() const;
    const AmpCommonData* getAmpCommonData() const;
};
#endif
