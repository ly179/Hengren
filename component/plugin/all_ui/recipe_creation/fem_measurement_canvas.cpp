#include "fem_measurement_canvas.h"
#include <QPainter>
#include "sem_view_common.h"

const QString exposureStr = "Exposure";
const QString focusStr = "Focus";

FemMeasurementCanvas::FemMeasurementCanvas(
    QWidget *parent)
    : PixelCanvas(parent) {
    QFont f = font();
    f.setPointSize(20);
    setFont(f);

    checkedBrush = QBrush(QColor(102, 102, 102));
}

FemMeasurementCanvas::~FemMeasurementCanvas() noexcept {}

void FemMeasurementCanvas::paintEvent(
    QPaintEvent *) {
    refreshMapFromRoot(true);
    painterBegin();
    if (lastDeviceRatio != getDevicePixelRatioCache()) {
        lastDeviceRatio = getDevicePixelRatioCache();
        linePen.setWidth(lastDeviceRatio * 2);
    }

    const QString *leftStr = nullptr;
    const QString *bottomStr = nullptr;
    if (type == 0) {
        leftStr = &exposureStr;
        bottomStr = &focusStr;
    } else if (type == 1) {
        bottomStr = &exposureStr;
        leftStr = &focusStr;
    } else {
        bottomStr = &focusStr;
    }

    qint32 fontPadding = this->fontPadding * getDevicePixelRatioCache();
    qint32 mapPadding = this->mapPadding * getDevicePixelRatioCache();
    QPainter &painter = *this->painter;
    painter.setFont(font());
    qint32 width = canvasWidthInPainting();
    qint32 height = canvasHeightInPainting();
    painter.fillRect(QRect(0, 0, width, height), Qt::white);
    QFontMetrics fontMetrics = painter.fontMetrics();
    qint32 ascent = fontMetrics.ascent();
    qint32 fHeight = ascent + fontMetrics.descent();
    qint32 itemFontPadding = fontPadding * 2 + fHeight;
    qint32 itemGap = itemFontPadding + fontPadding + mapPadding;
    qint32 realLeft = fontPadding, realTop = mapPadding, realSize = std::min(width, height);
    if (width > height) {
        realLeft = (width - height) / 2 + fontPadding;
    } else if (height > width) {
        realTop = (height - width) / 2 + mapPadding;
    }
    qint32 itemWidth = realSize - itemGap;
    if (itemWidth <= 0) {
        return;
    }
    itemWidth -= itemWidth % 5;
    qint32 item = itemWidth / 5;
    qint32 startX = realLeft;
    qint32 startY = realTop;

    qint32 hLineY = startY + itemWidth + fontPadding;
    qint32 hLineStartX = itemFontPadding + startX;
    qint32 hLineEndX = itemWidth + itemFontPadding + startX;

    QRect leftRect;
    if (leftStr) {
        leftRect = fontMetrics.tightBoundingRect(*leftStr);
    }
    QRect bottomRect = fontMetrics.tightBoundingRect(*bottomStr);
    if (leftStr) {
        qint32 tw = itemWidth / 2 + realTop;
        //padding - left
        qint32 th = realLeft;
        painter.translate(th, tw);
        painter.rotate(270);
        painter.drawText(-leftRect.width() / 2, ascent, *leftStr);
        painter.rotate(-270);
        painter.translate(-th, -tw);
    }

    painter.drawText(hLineStartX + itemWidth / 2 - bottomRect.width() / 2,
                     hLineY + fontPadding + ascent,
                     *bottomStr);

    painter.setPen(linePen);

    //从上到下的横向线条
    for (int i = 0; i < 6; ++i) {
        painter.drawLine(sem::getLeftTopViewPoint(itemFontPadding + startX, linePen.width()),
                         sem::getLeftTopViewPoint(item * i + startY, linePen.width()),
                         sem::getRightDownViewPoint(itemWidth + itemFontPadding + startX
                                                        + linePen.width(),
                                                    linePen.width()),
                         sem::getLeftTopViewPoint(item * i + startY, linePen.width()));
    }
    //从左到右的纵向线条
    for (int i = 0; i < 6; ++i) {
        painter.drawLine(sem::getLeftTopViewPoint(itemFontPadding + startX + item * i,
                                                  linePen.width()),
                         sem::getLeftTopViewPoint(startY, linePen.width()),
                         sem::getLeftTopViewPoint(itemFontPadding + startX + item * i,
                                                  linePen.width()),
                         sem::getRightDownViewPoint(startY + itemWidth + linePen.width(),
                                                    linePen.width()));
    }
    qint32 arrowW = 5 * getDevicePixelRatioCache();
    qint32 arrowH = 10 * getDevicePixelRatioCache();
    if (leftStr) {
        //绘制纵向箭头，从下到上绘制
        painter.drawLine(itemFontPadding - fontPadding + realLeft,
                         startY + itemWidth,
                         itemFontPadding - fontPadding + realLeft,
                         startY);

        //左撇
        painter.drawLine(itemFontPadding - fontPadding + realLeft,
                         startY - linePen.width() / 2,
                         itemFontPadding - fontPadding + realLeft - arrowW,
                         startY + arrowH);
        //右撇
        painter.drawLine(itemFontPadding - fontPadding + realLeft,
                         startY - linePen.width() / 2,
                         itemFontPadding - fontPadding + realLeft + arrowW,
                         startY + arrowH);
    }
    //绘制横向箭头，从左到右绘制
    painter.drawLine(hLineStartX, hLineY, hLineEndX, hLineY);
    //上撇
    painter.drawLine(hLineEndX, hLineY, hLineEndX - arrowH, hLineY - arrowW);
    //下撇
    painter.drawLine(hLineEndX, hLineY, hLineEndX - arrowH, hLineY + arrowW);

    // painter.setPen(QPen(QColor(255, 0, 0), linePen.width()));
    painter.setBrush(checkedBrush);
    if (fromCenter) {
        painter.drawRect(sem::getDrawRect(QRect(itemFontPadding + startX + item * 2,
                                                item * 2 + startY,
                                                item + linePen.width(),
                                                item + linePen.width()),
                                          linePen.width()));
    } else {
        painter.drawRect(sem::getDrawRect(QRect(itemFontPadding + startX /* + item * 0*/,
                                                item * 4 + startY,
                                                item + linePen.width(),
                                                item + linePen.width()),
                                          linePen.width()));
    }
    painter.end();
}

void FemMeasurementCanvas::toFromCenter(
    bool d) {
    fromCenter = d;
    update();
}
void FemMeasurementCanvas::setType(
    char t) {
    type = t;
    update();
}
