#ifndef AMP_WIDTH_ROUGHNESS_EDIT_CONTROLLER_H
#define AMP_WIDTH_ROUGHNESS_EDIT_CONTROLLER_H
#include <QWidget>
#include "amp_data_model.h"

namespace cdsem {
class UiAmpWidthRoughnessEditController;
};
class AsyncCaller;
class AmpDenseSettingEditController;

class AmpWidthRoughnessEditController : public QWidget {
private:
    cdsem::UiAmpWidthRoughnessEditController* ui = nullptr;
    AmpWidthRoughnessParam* param = nullptr;
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
    void refreshCursorBox();

public:
     explicit AmpWidthRoughnessEditController(QWidget* parent = nullptr);
     ~AmpWidthRoughnessEditController() noexcept;

     void initAsyncCaller(AsyncCaller* caller);

     QString collectData(AmpWidthRoughnessParam& param);
     void resetToComponent();
};
#endif
