#ifndef AXIS_ALIGN_WAFER_SELECTOR_CONTROLLER_H
#define AXIS_ALIGN_WAFER_SELECTOR_CONTROLLER_H

#include <QList>
#include "cde90_window.h"
#include "float_number.h"
#include "sem_common.h"

class QVBoxLayout;
class QHBoxLayout;
class IdwCanvasContent;
class CDE90Label;
class CDE90TextFieldBox;
class CDE90ComboBox;
class CDE90ButtonBox;

struct AxisAlignSelectedChip {
    IDW_PointF oPoint;
    qint32 number;
    qint32 x;
    qint32 y;
};
using AxisAlignWaferSelectorOk = sem::ONE_PARAM_ARG_FUNC<const AxisAlignSelectedChip&>;

class AxisAlignWaferSelectorController : public CDE90Window {
private:
    AxisAlignWaferSelectorOk okCallback;

    QWidget* content;
    QVBoxLayout* contentLayout;

    QWidget* canvasContent;
    QHBoxLayout* canvasLayout;
    IdwCanvasContent* idwCanvasContent = nullptr;

    QWidget* controlContent;
    QHBoxLayout* controlLayout;

    QWidget* leftContent;
    QHBoxLayout* leftLayout;
    CDE90Label* chipPosLabel;
    CDE90Label* chipPosLabelRight;
    CDE90TextFieldBox* chipPosTextField;

    QWidget* rightContent;
    QHBoxLayout* rightLayout;
    CDE90Label* magnifyLabel;
    CDE90ComboBox* magBtn;

    QWidget* bottomContent;
    QHBoxLayout* bottomLayout;
    CDE90ButtonBox* okBtn;
    CDE90ButtonBox* cancelBtn;

    void create();
    void createAction();

    void init();

public:
    explicit AxisAlignWaferSelectorController(QWidget* parent = nullptr);
    ~AxisAlignWaferSelectorController() noexcept;

    void setOkCallback(const AxisAlignWaferSelectorOk&);

    void initIdwId(quint64, const Integer32& x, const Integer32& y);
};

#endif // AXIS_ALIGN_WAFER_SELECTOR_CONTROLLER_H
