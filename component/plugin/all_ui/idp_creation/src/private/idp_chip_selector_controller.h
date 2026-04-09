#ifndef IDP_CHIP_SELECTOR_CONTROLLER_H
#define IDP_CHIP_SELECTOR_CONTROLLER_H
#include <QSet>
#include "cde90_dialog.h"
#include "common_act_param_helper.h"
#include "idp_mp_data_table_model.h"

namespace cdsem {
class UiIdpChipSelectorController;
};

struct ChipSelectorIdpPNoData {
    QSharedPointer<IdpMpParamListData> data;
    QList<QSharedPointer<IdpMpPosListData>> posList;
    QString dNumber;
};

struct ChipSelectorIdpTemplateData {
    IDW_PointF dPoint;
    QString templateName;
};

class ChipSelectorIdpPNoTableModel;
class ChipSelectorIdpTemplateTableModel;
class ChipMapHelper;
class IdpMpPosTableModel;

class IdpChipSelectorController : public CDE90Dialog{

    SEM_OK_CALL(
        QList<IdpMpTableModeData>)

private:
    QPoint localMoveBeginPos;

    //用于保存初始化的一些chip,pNo 对应的dNumber，
    //比如手快把chip删了再加回来，我希望它能恢复原来的dNumber而不是同一设置成pNo
    //key: x: chipNumber y: pNo
    QHash<QPoint, QList<QList<int>>> oldPosDNumbers;

    std::atomic_bool controlKeyPressed = false;
    std::atomic_bool shiftKeyPressed = false;

    //鼠标框选相关辅助
    QSet<qint32> beforeSelectedStatus;
    QList<qint32> lastSelectedStatus;
    cdsem::UiIdpChipSelectorController* ui = nullptr;
    ChipMapHelper* helper = nullptr;
    IdpMpPosTableModel* seqTableModel;
    ChipSelectorIdpPNoTableModel* pNoTableModel;
    ChipSelectorIdpTemplateTableModel* templateTableModel;

    bool middlePressed = false;

    void create();
    void createAction();

    bool eventFilter(QObject *watched, QEvent *event);
    void localMiddleMoveChip(const QPoint& pos);

    void _addMeasurementPoint(int index, QList<ChipSelectorIdpPNoData>&);
    void addMeasurementPoint();
    void _deleteMeasurementPoint(int index, QList<ChipSelectorIdpPNoData>&);
    void deleteMeasurementPoint();

public:
    explicit IdpChipSelectorController(QWidget* parent = nullptr);
    ~IdpChipSelectorController() noexcept;

    void initIdwTopId(quint64 topId, const QList<qint32>& selectedNumbers);

    void copyDataToSeqTable(const QList<IdpMpTableModeData>& list);
};
#endif
