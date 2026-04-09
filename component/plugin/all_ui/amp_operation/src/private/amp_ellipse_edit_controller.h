#ifndef AMP_ELLIPSE_EDIT_CONTROLLER_H
#define AMP_ELLIPSE_EDIT_CONTROLLER_H
#include <QWidget>
#include "amp_data_model.h"

namespace cdsem {
class UiAmpEllipseEditController;
};
class AsyncCaller;

class AmpEllipseEditController : public QWidget {
private:
    AmpEllipseParam* param = nullptr;
    cdsem::UiAmpEllipseEditController* ui = nullptr;

    AsyncCaller* caller = nullptr;
    int oldAcdType = -1;

    void create();
    void createAction();
    void refreshAcdType();

    void initRealTimeEdit();

public:
     explicit AmpEllipseEditController(QWidget* parent = nullptr);
     ~AmpEllipseEditController() noexcept;

     void initAsyncCaller(AsyncCaller* caller);

     QString collectData(AmpEllipseParam& param);
     void resetToComponent();
};
#endif
