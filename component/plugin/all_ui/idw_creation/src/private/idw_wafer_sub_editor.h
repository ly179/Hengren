#ifndef IDW_WAFER_SUB_EDITOR_H
#define IDW_WAFER_SUB_EDITOR_H
#include <QSet>
#include "cde90_window.h"
#include "idw_wafer_data_model.h"
#include "sem_global.h"

struct IdwWaferSubEditorDataHelper;
class IdwDetailDataMode;
class IdwWaferSubEditor : public CDE90Window {
    friend struct IdwWaferSubEditorDataHelper;
    Q_OBJECT
private:
    IdwWaferSubEditorDataHelper* dataHelper;

    void create(AsyncCaller* canvasCaller);
    void createAction();

    void localCreateItems(const IDW_CreateDataParam&);
    void collectDataAndCreate();
    void autoFillSize();
    void autoFillSizeStep2();
    void autoFillSizeStep3();
    void autoFillSizeStep4();
    void swapRows(bool forward);

    void closeEvent(QCloseEvent *event) override;
public:
    IdwWaferSubEditor(IDW_WaferData* waferData, AsyncCaller* canvasCaller, IdwWaferSubEditor* chipEditor, QWidget* parent = nullptr);
    ~IdwWaferSubEditor() noexcept;

    //IdwCreateDataParam param 该参数要么使用IdwCreateDataParam&要么就纯粹的值拷贝，因为入参需要修改。
    bool localCreateData(IDW_CreateDataParam& param);

    //设置输入框中的值
    void setInputData(const IDW_CreateDataParam& param);

    void updateCanvas();

    IDW_ShotChipData& localGetShotChipData();

    //如果是直接通过localGetShotChipData()在外部修改的数据，则需要通知数据变更内容
    IdwDetailDataMode& getTableDataMode();

    //在chip创建时，需要shot矩阵参数，该参数仅chip editor有效
    void setShotSize(qint32 size);
signals:
    void onClose();
};

#endif // IDW_WAFER_SUB_EDITOR_H
