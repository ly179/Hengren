#include "image_operation_controller_ui_helper.h"
#include "image_operation_controller.h"
#include <QPainter>
#include <QHBoxLayout>
#include "image_operation_widget_canvas.h"
#include "cde90_button.h"
#include "cde90_label.h"
#include "cde90_frame.h"
#include "param_horizontal_slider.h"

ImageCanvasBorderWidget::ImageCanvasBorderWidget(QWidget *parent) : PixelCanvas(parent) {}
ImageCanvasBorderWidget::~ImageCanvasBorderWidget() noexcept {}

void ImageCanvasBorderWidget::paintEvent(QPaintEvent *) {
    refreshMapFromRoot(true);
    painterBegin();
    painter->setPen(pen);
    painter->setBrush(brush);
    painter->drawRect(getDrawRect(QRect(0, 0, canvasWidthInPainting(), borderWidth)));
    painter->drawRect(
        getDrawRect(QRect(0, borderWidth, borderWidth, canvasHeightInPainting() - borderWidth)));
    painter->drawRect(getDrawRect(QRect(borderWidth,
                                        canvasHeightInPainting() - borderWidth,
                                        canvasWidthInPainting() - borderWidth,
                                        borderWidth)));
    painter->drawRect(getDrawRect(QRect(canvasWidthInPainting() - borderWidth,
                                        borderWidth,
                                        borderWidth,
                                        canvasHeightInPainting() - borderWidth * 2)));
    painter->end();
}

void ImageCanvasBorderWidget::setColor(const QColor& color) {
    brush = QBrush(color);
    pen = QPen((brush), 1);
    update();
}



QWidget *ImageOperationControllerUiHelper::initImageCanvas(
    QWidget *centerContent) {
    // QHBoxLayout* canvasLayout;
    // ImageOperationCanvas* canvas;
    canvasContent = new ImageCanvasBorderWidget(centerContent);
    QHBoxLayout *canvasContentLayout = new QHBoxLayout(canvasContent);
    canvasContentLayout->setContentsMargins(5, 5, 5, 5);
    canvasContent->setLayout(canvasContentLayout);

    canvas = new ImageOperationWidgetCanvas(canvasContent);
    canvas->setMinimumSize(QSize(512, 512));
    canvas->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    canvasContentLayout->addWidget(canvas);

    QVBoxLayout *canvasLayout = new QVBoxLayout(canvas);
    canvasLayout->setContentsMargins(0, 0, 0, 0);
    canvasLayout->setSpacing(0);
    canvasLayout->setAlignment(Qt::AlignTop);
    canvas->setLayout(canvasLayout);

    canvasToolWidget = new QWidget(canvas);
    canvasToolWidget->setVisible(false);
    QHBoxLayout *canvasToolLayout = new QHBoxLayout(canvasToolWidget);
    canvasToolLayout->setContentsMargins(0, 0, 0, 0);
    canvasToolLayout->setSpacing(0);
    canvasToolLayout->setAlignment(Qt::AlignRight);
    canvasToolWidget->setLayout(canvasToolLayout);

    CDE90Frame *canvasToolContent = new CDE90Frame(canvasToolWidget);
    canvasToolContent->setStyleSheet("QPushButton {border-width: 1;}"
                                     "QPushButton:pressed {border-width: 1;}");
    canvasToolContent->setContentsMargins(1, 1, 1, 1);
    canvasToolContent->setLineWidth(1);
    canvasToolContent->setFrameStyle(QFrame::Sunken | QFrame::Box);
    QHBoxLayout *canvasToolContentLayout = new QHBoxLayout(canvasToolContent);
    canvasToolContentLayout->setContentsMargins(0, 0, 0, 0);
    canvasToolContentLayout->setSpacing(0);
    canvasToolContentLayout->setAlignment(Qt::AlignRight);
    canvasToolContent->setLayout(canvasToolContentLayout);
    alignBtn = new CDE90Button(canvasToolContent);
    alignBtn->setIconSize(QSize(25, 25));
    alignBtn->setFixedSize(QSize(25, 25));
    alignBtn->setIcon(QIcon(":/icon/align.png"));
    alignBtn->setVisible(false);
    detectBtn = new CDE90Button(canvasToolContent);
    detectBtn->setIconSize(QSize(25, 25));
    detectBtn->setFixedSize(QSize(25, 25));
    detectBtn->setIcon(QIcon(":/icon/detect.png"));
    detectBtn->setVisible(false);
    sortBtn = new CDE90Button(canvasToolContent);
    sortBtn->setIconSize(QSize(25, 25));
    sortBtn->setFixedSize(QSize(25, 25));
    sortBtn->setIcon(QIcon(":/icon/sort.png"));
    lockCursorMoveBtn = new CDE90Button(canvasToolContent);
    lockCursorMoveBtn->setIconSize(QSize(25, 25));
    lockCursorMoveBtn->setFixedSize(QSize(25, 25));
    lockCursorMoveBtn->setCheckable(true);
    lockCursorMoveBtn->setIcon(QIcon(":/icon/free_move.png"));
    gatherToCenterBtn = new CDE90Button(canvasToolContent);
    gatherToCenterBtn->setIconSize(QSize(25, 25));
    gatherToCenterBtn->setFixedSize(QSize(25, 25));
    gatherToCenterBtn->setIcon(QIcon(":/icon/cursor_center.png"));
    cursorToolViewBtn = new CDE90Button(canvasToolContent);
    cursorToolViewBtn->setIconSize(QSize(12, 25));
    cursorToolViewBtn->setFixedSize(QSize(12, 25));

    canvasToolContentLayout->addWidget(alignBtn);
    canvasToolContentLayout->addWidget(detectBtn);
    canvasToolContentLayout->addWidget(sortBtn);
    canvasToolContentLayout->addWidget(lockCursorMoveBtn);
    canvasToolContentLayout->addWidget(gatherToCenterBtn);
    canvasToolContentLayout->addSpacerItem(
        cursorToolLeftMargin = new QSpacerItem(3, 0, QSizePolicy::Fixed, QSizePolicy::Preferred));
    canvasToolContentLayout->addWidget(cursorToolViewBtn);
    canvasToolLayout->addWidget(canvasToolContent);

    canvasLayout->addSpacerItem(
        new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding));
    canvasLayout->addWidget(canvasToolWidget);

    return canvasContent;
}
