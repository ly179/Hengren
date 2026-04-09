#ifndef IMAGE_OPERATION_VIEW_SETTER_CONTROLLER_H
#define IMAGE_OPERATION_VIEW_SETTER_CONTROLLER_H
#include <QHash>
#include "cbox_config_entity.h"
#include "cde90_dialog.h"
#include "scan_mode_data_model.h"
#include "ui_image_operation_global.h"
#include "view_data_mode.h"

class QVBoxLayout;
class CDE90Label;
class CDE90ButtonBox;
class QHBoxLayout;
class CDE90TableView;
class TableCellMenuHelper;
class CDSEM_GlobalEnvSemImageDataModel;

struct ViewScanDataModeHelper;
struct ScanModeTableData {
    ScanModeDataModel data;
    QString scanRateText;
    QString filterText;
    QString dcText;
    QString flatText;
};

class ViewScanDataMode : public SimpleRowTableModel<ScanModeTableData> {
private:
    ViewScanDataModeHelper* helper;
    bool viewMode = false;
    QHash<CBoxMixKey, QString>* valueToText = nullptr;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    virtual QVariant getItemData(qint32 row, qint32 column) const override;

public:
    explicit ViewScanDataMode(bool viewMode,
                              QHash<CBoxMixKey, QString>* textValue,
                              QObject* parent = nullptr);
    ~ViewScanDataMode() noexcept;

    void updateView();
};

class UI_IMAGE_OPERATION_EXPORT ImageOperationViewSetterController : public CDE90Dialog{
    Q_OBJECT
private:
    QHash<CBoxMixKey, QString> valueToText;

    QWidget* content;
    QVBoxLayout* contentLayout;

    QHBoxLayout* topLayout;
    QWidget* topContent;

    CDE90Label* defaultLabel;
    CDE90ButtonBox* saveBtn;
    CDE90ButtonBox* loadBtn;

    QVBoxLayout* viewModeLayout;
    QWidget* viewModeContent;
    QHBoxLayout* viewModeTitleLayout;
    QWidget* viewModeTitleContent;
    CDE90Label* viewModeLabel;
    CDE90ButtonBox* viewScanBtn;

    CDE90TableView* viewTable;
    ViewScanDataMode* viewDataMode;

    QVBoxLayout* integModeLayout;
    QWidget* integModeContent;
    QHBoxLayout* integModeTitleLayout;
    QWidget* integModeTitleContent;
    CDE90Label* integModeLabel;
    CDE90ButtonBox* integScanBtn;

    CDE90TableView* integTable;
    ViewScanDataMode* integDataMode;

    QHBoxLayout* bottomLayout;
    QWidget* bottomContent;

    CDE90ButtonBox* applyBtn;
    CDE90ButtonBox* closeBtn;

    TableCellMenuHelper* viewTableMenuHelper;
    TableCellMenuHelper* integTableMenuHelper;

    bool localNeedLoadMenu = true;

    void create();
    void createAction();
    void setProcedureRunningStatus();
    bool localRefreshView(QList<ScanModeTableData>&);
    bool localRefreshOneView(ScanModeTableData&);
    void localLoadMenu();

    void doSelectScan(CDE90TableView* table, int value);
    void doSelectFrame(CDE90TableView* table, int value);
    void doSelectPixelX(CDE90TableView* table, int value);
    void doSelectPixelY(CDE90TableView* table, int value);
    void doSelectFilter(CDE90TableView* table, int value);
    void doSelectFlat(CDE90TableView* table, int value);
    void doSelectDC(CDE90TableView* table, int value);

    void saveData();

    void localCopyDataToTable(const QList<ScanModeDataModel>&);
    void localCopyTableDataToGlobal();

    //从数据库读取数据
    int localLoadDataFromDatabase();
    int localLoadDataFromGlobal();

public:
    explicit ImageOperationViewSetterController(QWidget* parent = nullptr);
    ~ImageOperationViewSetterController() noexcept;

    void initData();

signals:
    void dataApplied();
};

#endif // IMAGE_OPERATION_VIEW_SETTER_CONTROLLER_H
