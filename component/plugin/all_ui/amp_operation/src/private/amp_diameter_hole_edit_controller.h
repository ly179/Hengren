#ifndef AMP_DIAMETER_HOLE_EDIT_CONTROLLER_H
#define AMP_DIAMETER_HOLE_EDIT_CONTROLLER_H
#include <QWidget>
#include "amp_data_model.h"

namespace cdsem {
class UiAmpDiameterHoleEditController;
}; // namespace cdsem
class AsyncCaller;

class AmpDiameterHoleEditController : public QWidget {
private:
    AmpDiameterHoleParam* param = nullptr;
    cdsem::UiAmpDiameterHoleEditController* ui = nullptr;

    AsyncCaller* caller = nullptr;
    int oldMeasurePoint = -1;
    int oldAcdType = -1;

    void create();
    void createAction();
    void initRealTimeEdit();

    void refreshMeasurePos();
    void refreshAcdType();

public:
     explicit AmpDiameterHoleEditController(QWidget* parent = nullptr);
     ~AmpDiameterHoleEditController() noexcept;

     void initAsyncCaller(AsyncCaller* caller);

     QString collectData(AmpDiameterHoleParam& param);
     void resetToComponent();
};
#endif
