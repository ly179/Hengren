#ifndef IDP_MEASUREMENT_ARRANGE_SETTING_CONTROLLER_H
#define IDP_MEASUREMENT_ARRANGE_SETTING_CONTROLLER_H
#include "cde90_dialog.h"
#include "cde90_scroll_area.h"
#include "common_act_param_helper.h"
#include "sort_direction_enum.h"

class MeasurementSortDiagonalView;
class MeasurementSortDiagonalCanvas;
class MeasureSortView;
class QHBoxLayout;
struct IdpDuplicatePos;

namespace cdsem {
class UiIdpMeasurementArrangeSettingController;
};

class IdpMeasurementArrangeSettingController : public CDE90Dialog{
    Q_OBJECT
    CDSEM_OK_CALL
private:
    cdsem::UiIdpMeasurementArrangeSettingController* ui = nullptr;

    MeasureSortView* sortView8;
    MeasurementSortDiagonalView* sortView2;
    int diagonal1 = -1;
    int diagonal2 = -1;

    void create();
    void createAction();

private slots:
    void changeToDiagonalView1();
    void changeToDiagonalView2();

public:
     explicit IdpMeasurementArrangeSettingController(QWidget* parent = nullptr);
     ~IdpMeasurementArrangeSettingController() noexcept;

     void resetToComponent(const IdpDuplicatePos&);
     QString collectData(IdpDuplicatePos&);
};

struct MeasurementSortDiagonalHelper;

class MeasurementSortDiagonalCanvas : public QWidget {
    Q_OBJECT
private:
    MeasurementSortDiagonalHelper* helper;
    int borderWidth = 2;

    MEASURE_SORT_DIAGONAL direction = MEASURE_SORT_LDTRU;
    bool checked = false;
    bool mousePressed = false;

protected:
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
public:
    explicit MeasurementSortDiagonalCanvas(QWidget* parent = nullptr);
    virtual ~MeasurementSortDiagonalCanvas() noexcept;

    inline void setDirection(MEASURE_SORT_DIAGONAL d) {
        direction = d;
        update();
    }
    inline MEASURE_SORT_DIAGONAL getDirection() const {
        return direction;
    }

    inline void setChecked(bool checked) {
        this->checked = checked;
        update();
    }

    inline bool isChecked() const {
        return checked;
    }

signals:
    void toggled(bool checked);
};

class MeasurementSortDiagonalView : public CDE90ScrollArea{
    Q_OBJECT
private:
    MeasurementSortDiagonalCanvas** directionCheckers;
    int lastChecked = -1;
    QWidget* areaContent;
    QHBoxLayout* areaLayout;

    void create();
    void createAction();

public:
    explicit MeasurementSortDiagonalView(QWidget* parent = nullptr);
    ~MeasurementSortDiagonalView() noexcept;

    void setChecked(int sortType);
    int getChecked();

    void hideSort(MEASURE_SORT_DIAGONAL, bool = true);

signals:
    void sortChanged(int sortType);
};

#endif
