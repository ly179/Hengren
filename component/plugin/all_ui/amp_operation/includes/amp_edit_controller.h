#ifndef AMP_EDIT_CONTROLLER_H
#define AMP_EDIT_CONTROLLER_H
#include "ui_amp_operation_global.h"
#include "relation_amp_edit_controller.h"

namespace cdsem {
class UiAmpEditController;
};

class AmpDataSelectController;
class AmpWidthEditController;
class AmpDiameterHoleEditController;
class AmpCornerEditController;
class AmpEllipseEditController;
class AmpGapEditController;
class AmpMppcEditController;
class AmpOpenCloseHoleEditController;
class AmpEdgeRoughnessEditController;
class AmpWidthRoughnessEditController;
class AmpLwrAnalysisEditController;
struct AmpCommonData;
class UI_AMP_OPERATION_EXPORT AmpEditController : public RelatoinAmpEditController {
    Q_OBJECT
private:
    cdsem::UiAmpEditController* ui = nullptr;

    AmpWidthEditController* widthController = nullptr;
    AmpDiameterHoleEditController* diameterHoleController = nullptr;
    AmpCornerEditController* cornerController = nullptr;
    AmpEllipseEditController* ellipseController = nullptr;
    AmpGapEditController* gapController = nullptr;
    AmpMppcEditController* mppcController = nullptr;
    AmpOpenCloseHoleEditController* openCloseHoleController = nullptr;
    AmpEdgeRoughnessEditController* edgeRoughnessController = nullptr;
    AmpWidthRoughnessEditController* widthRoughnessController = nullptr;
    AmpLwrAnalysisEditController* lwrAnalysisController = nullptr;

    AmpDataSelectController* dataSelector = nullptr;

    QWidget* currentController = nullptr;

    quint64 dataId = 0;

    bool dataSelectorAsSave = false;

    void create();
    void createAction();

    void createDataSelector();

    void switchTool(cdsem::MeasureType, bool resetComponent = true);

    void checkName(const QString& inputText);
    void saveAmpNoCaller(quint64 id, const QString& name);
    void loadAmpNoCaller(quint64 id);

    QString prepareAmpDataToSave(AmpCommonData&);

    void closeEvent(QCloseEvent* event) override;
    void showEvent(QShowEvent* event) override;

public:
     explicit AmpEditController(QWidget* parent = nullptr);
     ~AmpEditController() noexcept;

     void loadAmp(cdsem::MeasureType, const QString& json) override;
     // void loadAmp(const AmpCommonData&);
     //如果AMS启用，则使用TMP进行load
     //如果AMS没启用，则使用global进行load
     void loadAmp() override;

};
#endif
