#ifndef AMP_GAP_EDIT_CONTROLLER_H
#define AMP_GAP_EDIT_CONTROLLER_H
#include <QWidget>
#include "amp_data_model.h"

namespace cdsem {
class UiAmpGapEditController;
};
class AsyncCaller;

class AmpGapEditController : public QWidget {
private:
    AmpGapParam* param = nullptr;
    cdsem::UiAmpGapEditController* ui = nullptr;

    AsyncCaller* caller = nullptr;
    void create();
    void createAction();
    void initRealTimeEdit();

    void refreshAcdType();

public:
     explicit AmpGapEditController(QWidget* parent = nullptr);
     ~AmpGapEditController() noexcept;

     void initAsyncCaller(AsyncCaller* caller);

     QString collectData(AmpGapParam& param);
     void resetToComponent();
};
#endif
