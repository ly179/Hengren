#ifndef IDW_CANVAS_ITEM_CHECK_STYLE_H
#define IDW_CANVAS_ITEM_CHECK_STYLE_H

#include "idw_canvas.h"
#include "ui_common_global.h"

#define IDW_DEFAULT_ORIGIN_CHECK_STYLE new CheckPointSelectedBg(Qt::green, true)
#define IDW_DEFAULT_TEG_CHECK_STYLE new CheckPointSelectedBg(QColor(0, 234, 255))

class UI_COMMON_EXPORT CheckPointSelectedBg : public SelectNumberDrawerHandler {
private:
    QPen pen;
    QBrush brush;
    int width = 4;
    bool startEven = false;

public:
    CheckPointSelectedBg(const QColor& color = Qt::green, bool startEven = false);
    void begin(QPainter& painter) override;

    virtual void paint(const QRect& rect, QPainter& painter, qint32) override;
};

#endif // IDW_CANVAS_ITEM_CHECK_STYLE_H
