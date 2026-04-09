#ifndef IDP_AP_WAFER_SELECTOR_CONTROLLER_H
#define IDP_AP_WAFER_SELECTOR_CONTROLLER_H
#include "cde90_dialog.h"
#include "float_number.h"

struct IdpApSelectedChip{
    IDW_PointF oPoint;
    qint32 number;
    qint32 x;
    qint32 y;
};
using IdpApWaferSelectorOk = sem::ONE_PARAM_ARG_FUNC<const QList<IdpApSelectedChip>&>;

struct IdpApWaferSelectDataHelper;
/**
 * IDP AP IDW选择器，本类的异步加载类绘制线程和数据线程使用的是同一个线程，且数据仅在画布中一份
 * @brief The IdpApWaferSelectorController class
 */
class IdpApWaferSelectorController : public CDE90Dialog{
private:

    IdpApWaferSelectDataHelper* dataHelper = nullptr;

    void create();
    void createAction();

    void init();

    bool eventFilter(QObject *watched, QEvent *event) override;
public:
    explicit IdpApWaferSelectorController(QWidget* parent = nullptr);
    ~IdpApWaferSelectorController() noexcept;

    void setOkCallback(const IdpApWaferSelectorOk&);

    void initIdwTopId(quint64, const QList<qint32>& selectedNumbers);

    void setSingleSelectMode(bool b);
};

#endif // IDP_AP_WAFER_SELECTOR_CONTROLLER_H
