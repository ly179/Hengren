#include "idw_canvas_item_check_style.h"

CheckPointSelectedBg::CheckPointSelectedBg(
    const QColor& color, bool startEven) : startEven(startEven) {
    brush = QBrush(color);
    pen = QPen(brush, 1);
}

void CheckPointSelectedBg::begin(
    QPainter& painter) {
    painter.setPen(pen);
    painter.setBrush(brush);
}

void CheckPointSelectedBg::paint(
    const QRect& rect, QPainter& painter, qint32) {
    int startX = rect.x();
    int startY = rect.y();
    int endX = startX + rect.width() - 1;
    int endY = startY + rect.height() - 1;

    bool even = startEven;
    for (int y = startY; y <= endY; y += width) {
        for (int x = even ? startX : startX + width; x <= endX; x += width * 2) {
            painter.drawRect(sem::getDrawRect(QRect(x,
                                                    y,
                                                    std::min(width, endX - x + 1),
                                                    std::min(width, endY - y + 1)),
                                              1));
        }
        even = !even;
    }
}
