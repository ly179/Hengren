#ifndef DETECTION_COMMON_SETTING_CONTROLLER_H
#define DETECTION_COMMON_SETTING_CONTROLLER_H
#include <QWidget>
#include "amp_data_model.h"
namespace cdsem {
class UiDetectionCommonSettingController;
}
class AsyncCaller;

using DetectionAfterLoadedFunc = std::function<void(QSet<int>&)>;
class DetectionCommonSettingController : public QWidget {
    Q_OBJECT
private:
    cdsem::UiDetectionCommonSettingController* ui = nullptr;
    DetectionAfterLoadedFunc afterEdgeSearchLoaded;
    DetectionAfterLoadedFunc afterShapeLoaded;

    AsyncCaller* caller = nullptr;
    AmpMethodDetectionCommonParam* param = nullptr;

    int eVal1 = -1;
    int eVal2 = -1;
    int sVal1 = -1;
    int sVal2 = -1;

    void create();
    void createAction();

    void initRealTimeEdit();
public:
     explicit DetectionCommonSettingController(QWidget* parent = nullptr);
     ~DetectionCommonSettingController() noexcept;

     inline void initParam(AmpMethodDetectionCommonParam* param) { this->param = param; }

     void setTitle(const QString& t1, const QString& t2);
     void setShapeVisible(bool b);
     void setFlyerVisible(bool b);
     void disableFlyer(bool b = true);

     void removeEdgeDirect(QSet<int>& val);
     void removeShape(int);

     void initAsyncCaller(AsyncCaller* caller);

     inline void setAfterEdgeSearchLoaded(const DetectionAfterLoadedFunc& c) {
         afterEdgeSearchLoaded = c;
     }
     inline void setAfterShapeLoaded(const DetectionAfterLoadedFunc& c) { afterShapeLoaded = c; }

     void refreshEdgeSearchData();
     void refreshShapeData();

     void resetToComponent();
     QString collectData(AmpMethodDetectionCommonParam&);

     void disableLeft(bool = true);
     void disableRight(bool = true);

 signals:
     //paramPtr: AmpMethodDetectionCommonParam中的字段地址
     void dataChanged(QPointer<QObject> sender, void* paramPtr);
};
#endif
