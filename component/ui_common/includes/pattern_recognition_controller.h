#ifndef PATTERN_RECOGNITION_CONTROLLER_H
#define PATTERN_RECOGNITION_CONTROLLER_H
#include "cde90_dialog.h"
#include "common_act_param_helper.h"
#include "template_data_model.h"
#include "ui_common_global.h"

namespace cdsem {
class UiPatternRecognitionController;
};

class UI_COMMON_EXPORT PatternRecognitionController : public CDE90Dialog{
    SEM_OK_CALL(PatternRecognitionAlgorithmSettingParam)
    SEM_SET_INIT_PARAM(PatternRecognitionAlgorithmSettingParam)
private:
    cdsem::UiPatternRecognitionController* ui = nullptr;

    cdsem::PatternRegParamOpenMode mode = cdsem::PRPOM_AP_SEM;

    void create();
    void createAction();
    void resetToComponent();
    QString collectData();

    void kindChanged(int cData = -1);
public:
    explicit PatternRecognitionController(cdsem::PatternRegParamOpenMode mode, QWidget* parent = nullptr);
    ~PatternRecognitionController() noexcept;

    void hideDefault(bool b = true);
};
#endif
