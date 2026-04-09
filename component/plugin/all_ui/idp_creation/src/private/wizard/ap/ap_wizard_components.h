//
// Created by Shisan on 2026/1/22.
//

#ifndef HENGREN_CD_SEM_AP_WIZARD_COMPONENTS_H
#define HENGREN_CD_SEM_AP_WIZARD_COMPONENTS_H

#include "sem_common.h"
#include "wizard_component.h"
#include "cdsem_global_data.h"

namespace cdsem {
    class UiApWizardContentController;
    struct TmpImageData;
}

struct PatternRecognitionParamSettingParam;
struct AutoFocusSettingParam;
struct ApWizardDataParam {
    std::unique_ptr<PatternRecognitionParamSettingParam> param;
    IDW_PointF dPoint;
    bool valid = false;
};

struct ApWizardDataResult {
    ApWizardDataParam omParam;
    ApWizardDataParam semParam;
    std::unique_ptr<AutoFocusSettingParam> semFocusParam;
    cdsem::PyShotChipCoordinate chipCoord;
};

struct ApWizardComponentsDataHelper;

class ApWizardContent;
class QStackedLayout;
class ApWizardComponents : public WizardComponent{
    friend struct ApWizardComponentsDataHelper;
    Q_OBJECT
private:
    std::unique_ptr<ApWizardComponentsDataHelper> dataHelper;
    cdsem::UiApWizardContentController* ui;

    void createAction();

    void initSteps() override;

    //点击Skip按钮时触发的动作
    void getSkip(int&) override;
    //点击back按钮时要返回到的步骤，传入的int为当前步骤的上一步，如果需要返回到更多，则修改该值
    void getPrev(int&) override;

    void stepFinished() override;

    void stepCodeRefresh(int) override;
public:
    explicit ApWizardComponents(QWidget* parent = nullptr);
    ~ApWizardComponents() noexcept override;

    const ApWizardDataResult& getResult() const;
};


#endif //HENGREN_CD_SEM_AP_WIZARD_COMPONENTS_H