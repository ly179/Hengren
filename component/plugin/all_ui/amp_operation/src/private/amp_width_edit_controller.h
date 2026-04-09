#ifndef AMP_WIDTH_EDIT_CONTROLLER_H
#define AMP_WIDTH_EDIT_CONTROLLER_H
#include <QWidget>
#include "amp_data_model.h"

namespace cdsem {
class UiAmpWidthEditController;
};
class AsyncCaller;
class AmpDenseSettingEditController;

class AmpWidthEditController : public QWidget {
private:
    cdsem::UiAmpWidthEditController* ui = nullptr;
    AmpWidthParam *param = nullptr;
    AmpDenseSettingEditController* denseSettingController = nullptr;
    AsyncCaller* caller = nullptr;
    int oldAcdType = -1;
    int oldMeasureDirection = -1;
    int oldKind = -1;
    int oldMeasurePoint = -1;
    int oldPreProcess = -1;

    void create();
    void createAction();
    void initRealTimeEdit();

    void refreshAcdType();
    void refreshMeasureDirection();
    void refreshKind();
    void refreshMeasurePos();
    void refreshPreProcess();

public:
    explicit AmpWidthEditController(QWidget* parent = nullptr);
    ~AmpWidthEditController() noexcept;

    void initAsyncCaller(AsyncCaller* caller);

    QString collectData(AmpWidthParam& param);
    void resetToComponent();
};
#endif
