#ifndef AMP_MPPC_EDIT_CONTROLLER_H
#define AMP_MPPC_EDIT_CONTROLLER_H
#include <QWidget>
#include "amp_data_model.h"

namespace cdsem {
class UiAmpMppcEditController;
};
class AsyncCaller;

class AmpMppcEditController : public QWidget {
private:
    AmpMppcParam* param = nullptr;
    cdsem::UiAmpMppcEditController* ui = nullptr;
    AsyncCaller* caller = nullptr;
    int oldData = -1;

    void create();
    void createAction();
    void initRealTimeEdit();

    void refreshData();
    void refreshMeasurePos();

public:
     explicit AmpMppcEditController(QWidget* parent = nullptr);
     ~AmpMppcEditController() noexcept;

     void initAsyncCaller(AsyncCaller* caller);

     void resetToComponent();
     QString collectData(AmpMppcParam& param);

};
#endif
