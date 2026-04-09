#ifndef AMP_CORNER_EDIT_CONTROLLER_H
#define AMP_CORNER_EDIT_CONTROLLER_H
#include <QWidget>
#include "amp_data_model.h"

namespace cdsem {
class UiAmpCornerEditController;
};

class AmpCornerEditController : public QWidget {
private:
    AmpCornerParam* param = nullptr;
    cdsem::UiAmpCornerEditController* ui = nullptr;

    void create();
    void createAction();
    void initRealTimeEdit();
public:
     explicit AmpCornerEditController(QWidget* parent = nullptr);
     ~AmpCornerEditController() noexcept;

     QString collectData(AmpCornerParam& param);
     void resetToComponent();
};
#endif
