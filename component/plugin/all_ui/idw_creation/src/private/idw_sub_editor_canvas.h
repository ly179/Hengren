#ifndef IDW_SUB_EDITOR_CANVAS_H
#define IDW_SUB_EDITOR_CANVAS_H

#include "pixel_canvas.h"
struct IdwSubEditorCanvasDataHelper;

struct IDW_EditUnitPointInfo {
    qint32 number;
    //表示在鼠标按压之后抬起之前选择中，多次选择总是选中同一个单元
    bool repeat = false;
};

namespace idw_edit_canvas {
enum IdwTaskGid {
    UPDATE = 1,
    SELECTED_UPDATE,
    DATA_UPDATE
};
enum EditDataType {
    SHOT,
    CHIP
};
}
struct PrepareDrawParam;
class AsyncCaller;
class IDW_ShotChipData;
class IDW_WaferData;
class TaskStopCmd;
//编辑窗口的画布
class IdwSubEditorCanvas : public PixelCanvas {
    friend struct IdwSubEditorCanvasDataHelper;
    Q_OBJECT
private:
    IdwSubEditorCanvasDataHelper* dataHelper;


    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

    void updateShotOrigin();
    bool prepareDrawParam(PrepareDrawParam &shotParam);
    bool prepareRectIndex(const PrepareDrawParam& param, qint32 itemCount);
    void putDataToIndex(const PrepareDrawParam& param, TaskStopCmd& cmd);
    void rend(TaskStopCmd& cmd);
    void mergePixmap();
    IDW_EditUnitPointInfo unitAt(const QPoint& pos);

public:
    IdwSubEditorCanvas(AsyncCaller* caller, QWidget* parent = nullptr);
    ~IdwSubEditorCanvas() noexcept;

    AsyncCaller& getAsyncCaller();

    IDW_ShotChipData& localEditData();

    IDW_WaferData& localWaferData();

    bool isLocalThread();

    //该函数的number接收一个全局的number编号，即如果是shot编号则无需关心
    //如果是chip编号需要shot之外的编号
    //char = -1 表示自行决定，如果是已选中则取消选中，如果是未选中则选中
    //char = 1 表示无视情况直接选中
    //char = 0 表示无视情况直接取消选中
    void checkItem(qint32 number, char = -1);
    void localCheckItem(qint32 number, char = -1);
    void localClearCheckedItem();
    //这些不带local开头的推荐放在UI线程上调用
    //注册全量更新任务组ID
    void registFullyUpdateGid(int);
    void registSelectedUpdateGid(int);

    //任何内容的更新需要刷新画布都需要按需调用下列的两个函数
    //数据更新完毕后，需要手动调用更细函数进行更新画布
    void localUpdate(TaskStopCmd& cmd);
    void localUpdateCheckOnly();

    void setDataType(idw_edit_canvas::EditDataType dataType);
signals:
    //一定在UI线程上
    void itemChecked(const IDW_EditUnitPointInfo&);
};
#endif // IDW_SUB_EDITOR_CANVAS_H
