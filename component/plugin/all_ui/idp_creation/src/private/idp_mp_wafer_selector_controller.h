#ifndef IDP_MP_WAFER_SELECTOR_CONTROLLER_H
#define IDP_MP_WAFER_SELECTOR_CONTROLLER_H
#include <QHash>
#include <QMap>
#include "cde90_dialog.h"
#include "common_act_param_helper.h"
#include "float_number.h"

class CDSemIntValidator;
namespace cdsem {
class UiIdpMpWaferSelectorController;
};

struct IdpMpSelectedChip{
    IDW_PointF oPoint;
    qint32 number;
    qint32 x;
    qint32 y;
};
class ChipMapHelper;
struct IdpMpWaferSelectHelper;
class IdpMpWaferSelectorController : public CDE90Dialog{
    SEM_OK_CALL(QList<IdpMpSelectedChip>)
private:
    IdpMpWaferSelectHelper* dataHelper = nullptr;
    cdsem::UiIdpMpWaferSelectorController* ui = nullptr;

    void create();
    void createAction();

    bool eventFilter(QObject *watched, QEvent *event) override;

public:
    explicit IdpMpWaferSelectorController(QWidget* parent = nullptr);
    ~IdpMpWaferSelectorController() noexcept;

    void initIdwTopId(quint64 topId, const QList<qint32>& selectedNumbers, const QList<qint32>& grayNumbers = QList<qint32>());

    void setSingleSelectMode(bool b);

    //此函数仅能初始化完毕后调用，修改过程中调用会出问题。
    void setStartSeq(int);
};
#endif
