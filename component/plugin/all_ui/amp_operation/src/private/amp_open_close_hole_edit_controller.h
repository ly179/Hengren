#ifndef AMP_OPEN_CLOSE_HOLE_EDIT_CONTROLLER_H
#define AMP_OPEN_CLOSE_HOLE_EDIT_CONTROLLER_H
#include <QWidget>
#include "amp_data_model.h"

namespace cdsem {
class UiAmpOpenCloseHoleEditController;
};
class AsyncCaller;

class AmpOpenCloseHoleEditController : public QWidget {
private:
    AmpOpenCloseHoleParam* param = nullptr;
    cdsem::UiAmpOpenCloseHoleEditController* ui = nullptr;

    void create();
    void createAction();
    void initRealTimeEdit();
public:
     explicit AmpOpenCloseHoleEditController(QWidget* parent = nullptr);
     ~AmpOpenCloseHoleEditController() noexcept;

     void resetToComponent();
     QString collectData(AmpOpenCloseHoleParam& param);
};
#endif
