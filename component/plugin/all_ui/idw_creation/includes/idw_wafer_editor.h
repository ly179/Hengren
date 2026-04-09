#ifndef IDW_WAFER_EDITOR_H
#define IDW_WAFER_EDITOR_H
#include "idw_view_data_model.h"
#include "idw_wafer_data_model.h"
#include "ui_idw_creation_global.h"
#include "relation_idw_wafer_editor.h"

namespace idw_canvas {
enum IdwTaskGid {
    UPDATE_WAFER = 1,
    CANVAS_INIT,
    SHOT_CREATE,
    SHOT_UPDATE,
    WAFER_SIZE,
    WAFER_INVALID_AREA,
    WAFER_NOTCH,
    WAFER_MAP_OFFSET
};
enum IdwSelectionGid {
    SHOT_SHIFT = 1,
    CHIP_ORIGIN,
    SHOT_INVALID,
    CHIP_INVALID,
    SHOT_TEG_CHECKED,
    CHIP_TEG_CHECKED
};
}
//! 本IDW功能将数据与画布数据剥离，即数据和画布各自维护自己的数据，这样的好处是可以让操作数据的界面不会因画布绘制效率而造成
//! 数据操作的卡顿（即需要等待画布的响应后才能继续操作数据）
//! IDW执行器有两个，一个是数据执行器，一个是画布执行器
//! 数据执行器用于数据创建和修改，画布执行器用于将数据复制到画布，并创建索引和画布缓存
//! 此外UI线程用于绘制画布缓存。
//! 经过验证，数据执行器中的任务都很快，不需要使用条件停止，而画布执行器可能需要，所以为画布执行器创建了任务组ID，用于任务停止。
//! 在需要将数据执行器中的数据复制到画布上时，需要通过画布执行器反向调用数据执行器，这样可以让画布执行器空闲后主动访问数据执行器而不是让数据执行器等待画布的响应。
//! 任务的停止需要视情况而定，对于全量更新可以使用，对于增量更新则不要使用，否则会造成操作丢失

struct IdwWaferEditorDataHelper;
class UI_IDW_EXPORT IdwWaferEditor : public RelationIDWWaferEditor {
    friend struct IdwWaferEditorDataHelper;
private:
    IdwWaferEditorDataHelper* dataHelper;
    void create();
    void createAction();
    void triggerAction();
    void initCanvas();
    //返回微米单位
    qint32 getCheckedWaferSize(int = -1);
    IDW_WaferData collectWaferData();
    void shiftShot(bool x, bool up);

    void localCalcInValidChip(bool partly);
    void localCalcInValidShot(bool partly);
    void autoCalcInvalid(bool partly);
    void updateNumberLabel(const IDW_UnitPointInfo&);

    void afterCreate();

    void closeEvent(QCloseEvent*) override;

    bool eventFilter(QObject *watched, QEvent *event) override;

public:
    IdwWaferEditor(QWidget* parent = nullptr);
    ~IdwWaferEditor() noexcept;

    //虚函数方便作为插件对象让其他方调用，而不是为了继承。
    void setBaseData(const IDW_BaseData&) override;
    void setExitCallback(const sem::NO_PARAM_ARG_FUNC&) override;
    void loadData(quint64 id) override;
    void disableCoverSave(bool = true) override;
    quint64 getIdwId() const override;
};

#endif // IDW_WAFER_EDITOR_H
