#ifndef AMP_LWR_ANALYSIS_EDIT_CONTROLLER_H
#define AMP_LWR_ANALYSIS_EDIT_CONTROLLER_H
#include <QWidget>
#include "amp_data_model.h"

namespace cdsem {
class UiAmpLwrAnalysisEditController;
};
class AsyncCaller;

class AmpLwrAnalysisEditController : public QWidget {
private:
    AmpLwrAnalysisParam* param = nullptr;
    cdsem::UiAmpLwrAnalysisEditController* ui = nullptr;

    AsyncCaller* caller = nullptr;

    void create();
    void createAction();
    void initRealTimeEdit();
public:
     explicit AmpLwrAnalysisEditController(QWidget* parent = nullptr);
     ~AmpLwrAnalysisEditController() noexcept;

     void initAsyncCaller(AsyncCaller* caller);

     void resetToComponent();
     QString collectData(AmpLwrAnalysisParam& param);
};
#endif
