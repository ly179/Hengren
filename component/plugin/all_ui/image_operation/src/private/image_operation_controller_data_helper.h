#ifndef IMAGE_OPERATION_CONTROLLER_DATA_HELPER_H
#define IMAGE_OPERATION_CONTROLLER_DATA_HELPER_H

#include <QList>
#include "image_measurement_tool_interface.h"
#include <QPointer>

class ImageMeasurementTool;
class ImageOperationController;
class ImageMeasurementDoubleRectTool;
class ImageOperationWidgetCanvas;
class CursorHandler;
class WidthCursorHandler;
class WidthRoughnessCursorHandler;
class EdgeRoughnessCursorHandler;
class DiameterHoleCursorHandler;
class EllipseCursorHandler;
class QPushButton;
class DoubleBackgroundColorAnimation;
struct OperationResult;
class ScanModeDataModel;

struct AmpMeasureCursors {
    //测量结果标记，基本都是十字
    QList<ImageMeasurementTool*> results;
    QList<ImageMeasurementTool*> doubleBoxTools;
    QList<ImageMeasurementTool*> oneBoxTools;
    QList<ImageMeasurementTool*> annulusTools;
    QList<ImageMeasurementTool*> distanceTools;
    QList<ImageMeasurementTool*>* currentTools = nullptr;
};

struct ImageOperationControllerDataHelper {

    AmpMeasureCursors ampCursors;

    //Assist区域
    MeasurementRect assistRange;

    //![0] AMP光标相关操作变量
    // 每个光标在鼠标按压那一个在画布上的坐标
    QList<MeasurementPoint> beginPoints;

    QString resultText = "";

    ImageOperationController* d;

    WidthCursorHandler* widthCursorHandler;
    WidthRoughnessCursorHandler* widthRoughnessCursorHandler;
    EdgeRoughnessCursorHandler* edgeRoughnessCursorHandler;
    DiameterHoleCursorHandler* diameterHoleCursorHandler;
    EllipseCursorHandler* ellipseCursorHandler;

    int scanParamLabelTextVersion = 0;

    // 标记光标是移动状态还是resize状态
    bool moving = false;

    //![1] AMP的ACD在画布上的工具栏打开和关闭状态
    // 仅当ACD选择Auto，且Target > 1时工具栏显现，该字段表示显示/最小化按钮标记状态
    // 默认为显示
    bool canvasAcdPaneStatus = true;

    //stage移动结束时需要排一张图变量标识
    //仅SEM图使用
    bool needGrabImageForStageOver = false;

    //true表示已请求关闭窗口，正在处理关闭事宜
    bool closeRequested = false;

    CursorHandler* getCursorHandler();

    void amsBtnToYellow();
    void amsBtnToGreen();

    void amsBtnToNormal();

    void btnToGreen(QPushButton*);
    void btnRemoveGreen(QPushButton*);

    void initDoubleBoxBeginPos();

    void initOneBoxBeginPos();

    void initAnnualBoxBeginPos();

    void removeAllAmpCursor(QList<ImageMeasurementTool*>& list);

    void removeAllAmpCursor();

    void exchangeCurrentAmpCursor(QList<ImageMeasurementTool*>* targetTools);

    void moveDoubleBoxTool(int cursorDirection, bool moving);

    void moveOneBoxTool();

    void moveAnnualTool();

    void recordAssistCenterPos();

    void clearAssistCenterPos();

    void amsNextStep();

    void markWidthLineTools();

    static bool ampCursorSortFunc(const MeasurementPoint& o1, const MeasurementPoint& o2);

    void markMeasureCursor();
    void startAutoMeasure(const sem::ONE_PARAM_ARG_FUNC<const OperationResult&>&);
    void overAutoMeasure();
    void stopAutoMeasure();

    bool collectCenters();

    void notifyAmpChanged(QPointer<QObject> obj, void* pPtr);
    void sortAmpCursor();
    void allAmpCursorToCenter();
    void lockAcdCursorsMouse(bool = true);

    void setWorkDistanceFromSlider(int);
    void setWorkDistanceFromTextField(const QString&);
    void refreshRot();

    void reloadData();

    //s = 0 closeShiftTool
    //s = 1 to BeamShift Tool
    //2 = 2 to StageShift Tool
    void switchShiftFieldModel(int s);
    void switchShiftFieldBtnView(int s);

    //true procedure is running.
    void setProcedureRunningStatus();

    void setAutoFocusBtnStatus(bool);

    void imageOperationTypeChanged();

    void writeSemImage();
    void writeOmImage();
    void rewriteImage(int imageType = -1);
};

#endif // IMAGE_OPERATION_CONTROLLER_DATA_HELPER_H
