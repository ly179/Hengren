#ifndef FEM_MEASUREMENT_CANVAS_H
#define FEM_MEASUREMENT_CANVAS_H

#include "pixel_canvas.h"

class FemMeasurementCanvas : public PixelCanvas {
private:
    QBrush checkedBrush;
    QPen linePen;
    qreal lastDeviceRatio = 1;
    qint32 fontPadding = 10;
    qint32 mapPadding = 20;

    bool fromCenter = false;
    //0 - x focus, 1 - y focus, 2 - focus only
    char type = 0;
    void paintEvent(QPaintEvent*) override;

public:
    explicit FemMeasurementCanvas(QWidget* parent = nullptr);
    ~FemMeasurementCanvas() noexcept;

    void toFromCenter(bool = true);
    void setType(char t);
};

#endif // FEM_MEASUREMENT_CANVAS_H
