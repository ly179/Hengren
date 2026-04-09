#ifndef AMP_EDGE_ROUGHNESS_EDIT_CONTROLLER_H
#define AMP_EDGE_ROUGHNESS_EDIT_CONTROLLER_H
#include <QWidget>
#include "amp_dense_setting_edit_controller.h"

namespace cdsem {
class UiAmpEdgeRoughnessEditController;
};
class AsyncCaller;

class AmpEdgeRoughnessEditController : public QWidget {
private:
    AmpEdgeRoughnessParam* param = nullptr;
    cdsem::UiAmpEdgeRoughnessEditController* ui = nullptr;
    AmpDenseSettingEditController* denseSettingController = nullptr;

    AsyncCaller* caller = nullptr;
    int oldAcdType = -1;
    int oldMeasurePoint = -1;
    int oldData = -1;

    void create();
    void createAction();
    void initRealTimeEdit();

    void refreshAcdType();
    void refreshMeasurePos();
    void refreshData();
    void refreshDense();

public:
     explicit AmpEdgeRoughnessEditController(QWidget* parent = nullptr);
     ~AmpEdgeRoughnessEditController() noexcept;

     void initAsyncCaller(AsyncCaller* caller);

     void resetToComponent();
     QString collectData(AmpEdgeRoughnessParam&);
};
#endif
