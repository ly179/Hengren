#include "idp_measurement_arrange_setting_controller.h"
#include <QBrush>
#include <QFont>
#include <QFontMetrics>
#include <QHBoxLayout>
#include <QPaintEvent>
#include <QPainter>
#include <QPen>
#include <QPolygon>
#include <QScrollBar>
#include <QTransform>
#include "cdsem_global_resource.h"
#include "cdsem_number_validator_helper.h"
#include "idp_data_model.h"
#include "measure_sort_dialog.h"
#include "ui_idp_measurement_arrange_setting_controller.h"
#include <cmath>

IdpMeasurementArrangeSettingController::IdpMeasurementArrangeSettingController(QWidget* parent) : CDE90Dialog(parent) {
    create();
    createAction();
}

IdpMeasurementArrangeSettingController::~IdpMeasurementArrangeSettingController() noexcept {
    if (ui) {
        delete ui;
    }
}

void IdpMeasurementArrangeSettingController::create() {
    ui = new cdsem::UiIdpMeasurementArrangeSettingController;
    setContent(new QWidget);
    ui->init(getContent());
    ui->upLabel->setPixmap(ResourceManager::getResourceManager().getArrowUpIcon());
    ui->leftLabel->setPixmap(ResourceManager::getResourceManager().getArrowLeftIcon());
    ui->rightLabel->setPixmap(ResourceManager::getResourceManager().getArrowRightIcon());
    ui->downLabel->setPixmap(ResourceManager::getResourceManager().getArrowDownIcon());
    ui->centerLabel->setPixmap(ResourceManager::getResourceManager().getCrossCenterIcon());

    ui->leftUpLabel->setPixmap(ResourceManager::getResourceManager().getArrowLeftUpIcon());
    ui->leftDownLabel->setPixmap(ResourceManager::getResourceManager().getArrowLeftDownIcon());
    ui->rightUpLabel->setPixmap(ResourceManager::getResourceManager().getArrowRightUpIcon());
    ui->rightDownLabel->setPixmap(ResourceManager::getResourceManager().getArrowRightDownIcon());
    ui->xCenterLabel->setPixmap(ResourceManager::getResourceManager().getMxCenterIcon());

    sortView8 = new MeasureSortView(ui->orderContent);
    sortView8->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    ui->orderContent->layout()->addWidget(sortView8);

    sortView2 = new MeasurementSortDiagonalView(ui->orderContent);
    sortView2->setVisible(false);
    sortView2->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    ui->orderContent->layout()->addWidget(sortView2);

    
}

void IdpMeasurementArrangeSettingController::createAction() {
    CDSemNumberValidatorHelper::intFree()
        .put(ui->upTextField)
        .put(ui->downTextField)
        .put(ui->leftTextField)
        .put(ui->rightTextField)
        .put(ui->leftUpTextField)
        .put(ui->rightUpTextField)
        .put(ui->leftDownTextField)
        .put(ui->rightDownTextField)
        .autoFixMode();
    CDSemNumberValidatorHelper::doubleFree<1>()
        .put(ui->pitchXTextField)
        .put(ui->pitchYTextField)
        .autoFixMode();
    bindCloseBtn(ui->cancelBtn->getQPushButton());
    connect(ui->typeItemBox->getQComboBox(),
            &UComboBox::currentIndexChanged,
            this,
            [this](int index) {
                Q_UNUSED(index)
                int curr = ui->typeItemBox->currentValue();
                if (curr == 0) {
                    ui->diagonalContent->hide();
                    sortView2->hide();
                    ui->matrixContent->show();
                    ui->matrixInfoContent->show();
                    sortView8->show();
                } else {
                    ui->matrixContent->hide();
                    ui->matrixInfoContent->hide();
                    sortView8->hide();
                    ui->diagonalContent->show();
                    sortView2->show();
                }
            });
    connect(ui->leftDownTextField->getQLineEdit(),
            &CDE90TextField::onFocusIn,
            this,
            &IdpMeasurementArrangeSettingController::changeToDiagonalView1);
    connect(ui->rightUpTextField->getQLineEdit(),
            &CDE90TextField::onFocusIn,
            this,
            &IdpMeasurementArrangeSettingController::changeToDiagonalView1);
    connect(ui->leftUpTextField->getQLineEdit(),
            &CDE90TextField::onFocusIn,
            this,
            &IdpMeasurementArrangeSettingController::changeToDiagonalView2);
    connect(ui->rightDownTextField->getQLineEdit(),
            &CDE90TextField::onFocusIn,
            this,
            &IdpMeasurementArrangeSettingController::changeToDiagonalView2);
    connect(ui->leftTextField->getQLineEdit(),
            &QLineEdit::textChanged,
            this,
            [this](const QString& text) {
                ui->mxTextField->setText(text.toInt() + ui->rightTextField->text().toInt() + 1);
            });
    connect(ui->upTextField->getQLineEdit(),
            &QLineEdit::textChanged,
            this,
            [this](const QString& text) {
                ui->myTextField->setText(text.toInt() + ui->downTextField->text().toInt() + 1);
            });
    connect(ui->rightTextField->getQLineEdit(),
            &QLineEdit::textChanged,
            this,
            [this](const QString& text) {
                ui->mxTextField->setText(text.toInt() + ui->leftTextField->text().toInt() + 1);
            });
    connect(ui->downTextField->getQLineEdit(),
            &QLineEdit::textChanged,
            this,
            [this](const QString& text) {
                ui->myTextField->setText(text.toInt() + ui->upTextField->text().toInt() + 1);
            });

    connect(sortView2, &MeasurementSortDiagonalView::sortChanged, this, [this](int type) {
        if (type == MEASURE_SORT_LDTRU || type == MEASURE_SORT_RUTLD) {
            diagonal1 = type;
        } else if (type == MEASURE_SORT_LUTRD || type == MEASURE_SORT_RDTLU) {
            diagonal2 = type;
        }
    });
    connect(ui->okBtn->getQPushButton(), &QPushButton::clicked, this, [this]() {
        if (okCallback && okCallback()) {
            changeCloseStatusOnce(HW_OK);
            close();
        }
    });
}

void IdpMeasurementArrangeSettingController::changeToDiagonalView1() {
    sortView2->hideSort(MEASURE_SORT_LDTRU, false);
    sortView2->hideSort(MEASURE_SORT_RUTLD, false);
    sortView2->hideSort(MEASURE_SORT_LUTRD);
    sortView2->hideSort(MEASURE_SORT_RDTLU);
    sortView2->setChecked(diagonal1);
}
void IdpMeasurementArrangeSettingController::changeToDiagonalView2() {
    sortView2->hideSort(MEASURE_SORT_LDTRU);
    sortView2->hideSort(MEASURE_SORT_RUTLD);
    sortView2->hideSort(MEASURE_SORT_LUTRD, false);
    sortView2->hideSort(MEASURE_SORT_RDTLU, false);
    sortView2->setChecked(diagonal2);
}

void IdpMeasurementArrangeSettingController::resetToComponent(
    const IdpDuplicatePos& param) {
    ui->pitchXTextField->setText(param.pitchX.toQStringAndKeepFullDecimal());
    ui->pitchYTextField->setText(param.pitchY.toQStringAndKeepFullDecimal());
    if (param.type == 0) {
        ui->typeItemBox->setCurrentData(param.type);
        ui->upTextField->setText(param.r1.toString());
        ui->rightTextField->setText(param.r2.toString());
        ui->downTextField->setText(param.r3.toString());
        ui->leftTextField->setText(param.r4.toString());
        sortView8->setChecked(MEASURE_SORT_MATRIX(param.direction));
    } else if (param.type == 1) {
        ui->typeItemBox->setCurrentData(param.type);
        ui->rightUpTextField->setText(param.r1.toString());
        ui->rightDownTextField->setText(param.r2.toString());
        ui->leftDownTextField->setText(param.r3.toString());
        ui->leftUpTextField->setText(param.r4.toString());
        diagonal1 = IdpDuplicatePos::getDiagonalDirection(param.direction, 0);
        diagonal2 = IdpDuplicatePos::getDiagonalDirection(param.direction, 1);
        ui->rightUpTextField->setFocus();
        sortView2->setChecked(diagonal1);
    } else {
        ui->upTextField->getQLineEdit()->clear();
        ui->rightTextField->getQLineEdit()->clear();
        ui->downTextField->getQLineEdit()->clear();
        ui->leftTextField->getQLineEdit()->clear();
        ui->rightUpTextField->getQLineEdit()->clear();
        ui->rightDownTextField->getQLineEdit()->clear();
        ui->leftDownTextField->getQLineEdit()->clear();
        ui->leftUpTextField->getQLineEdit()->clear();
        diagonal1 = -1;
        diagonal2 = -1;
        sortView8->setChecked(MEASURE_SORT_MATRIX_NONE);
        sortView2->setChecked(diagonal1);
        ui->typeItemBox->setCurrentData(0);
    }
}
QString IdpMeasurementArrangeSettingController::collectData(
    IdpDuplicatePos& param) {
    param.type = ui->typeItemBox->currentValue();
    param.pitchX = ui->pitchXTextField->text();
    param.pitchY = ui->pitchYTextField->text();
    Integer32 tmp;
    bool ok;
    if (param.type == 0) {
        tmp = ui->upTextField->text().toInt(&ok);
        if (!ok) {
            tmp = nullptr;
        }
        param.r1 = tmp;
        tmp = ui->rightTextField->text().toInt(&ok);
        if (!ok) {
            tmp = nullptr;
        }
        param.r2 = tmp;
        tmp = ui->downTextField->text().toInt(&ok);
        if (!ok) {
            tmp = nullptr;
        }
        param.r3 = tmp;
        tmp = ui->leftTextField->text().toInt(&ok);
        if (!ok) {
            tmp = nullptr;
        }
        param.r4 = tmp;
        MEASURE_SORT_MATRIX sortDirection = sortView8->getChecked();
        if (param.r1 != nullptr || param.r2 != nullptr || param.r3 != nullptr
            || param.r4 != nullptr) {
            if (sortDirection == MEASURE_SORT_MATRIX_NONE) {
                return "You must specify the sort direction.";
            }
        }
        param.direction = sortDirection;
    } else if (param.type == 1) {
        tmp = ui->rightUpTextField->text().toInt(&ok);
        if (!ok) {
            tmp = nullptr;
        }
        param.r1 = tmp;
        tmp = ui->rightDownTextField->text().toInt(&ok);
        if (!ok) {
            tmp = nullptr;
        }
        param.r2 = tmp;
        tmp = ui->leftDownTextField->text().toInt(&ok);
        if (!ok) {
            tmp = nullptr;
        }
        param.r3 = tmp;
        tmp = ui->leftUpTextField->text().toInt(&ok);
        if (!ok) {
            tmp = nullptr;
        }
        param.r4 = tmp;
        if (param.r1 != nullptr || param.r3 != nullptr) {
            if (diagonal1 == -1) {
                return "You must specify the sort direction.";
            }
        }
        if (param.r2 != nullptr || param.r4 != nullptr) {
            if (diagonal2 == -1) {
                return "You must specify the sort direction.";
            }
        }
        param.direction = IdpDuplicatePos::genDiagonalDirection(diagonal1, diagonal2);
    } else {
        param.type = -1;
        param.direction = -1;
        param.pitchX.clear();
        param.pitchY.clear();
        param.r1 = nullptr;
        param.r2 = nullptr;
        param.r3 = nullptr;
        param.r4 = nullptr;
    }
    return "";
}

struct MeasurementSortDiagonalHelper {
    QPen paintPen;
    QBrush paintBrush;
    QPen checkedPen;
    QBrush checkedBrush;
    QPen bgPen;
    QBrush bgBrush;
    QPen lightBorderPen;
    QBrush lightBorderBrush;
    QPen shadowBorderPen;
    QBrush shadowBorderBrush;
    QFont pFont;
};

MeasurementSortDiagonalCanvas::MeasurementSortDiagonalCanvas(QWidget* parent) : QWidget(parent) {
    helper = new MeasurementSortDiagonalHelper;
    helper->paintBrush = QBrush(QColor(Qt::black));
    helper->paintPen = QPen(helper->paintBrush, 1);
    helper->checkedBrush = QBrush(QColor(Qt::green));
    helper->checkedPen = QPen(helper->checkedBrush, 1);
    helper->bgBrush = QBrush(QColor(Qt::white));
    helper->bgPen = QPen(helper->bgBrush, 1);
    helper->lightBorderBrush = QBrush(QColor(237, 237, 237));
    helper->lightBorderPen = QPen(helper->lightBorderBrush, 1);
    helper->shadowBorderBrush = QBrush(QColor(119, 119, 119));
    helper->shadowBorderPen = QPen(helper->shadowBorderBrush, 1);
    helper->pFont = font();
    helper->pFont.setPointSize(12);
}
MeasurementSortDiagonalCanvas::~MeasurementSortDiagonalCanvas() noexcept {
    delete helper;
}

void MeasurementSortDiagonalCanvas::paintEvent(QPaintEvent* ) {
    QPainter painter(this);
    painter.setFont(helper->pFont);
    int padding = 5;

    if (checked) {
        painter.setPen(helper->checkedPen);
        painter.setBrush(helper->checkedBrush);
    } else {
        painter.setPen(helper->bgPen);
        painter.setBrush(helper->bgBrush);
    }
    painter.drawRect(sem::getDrawRect(QRect(0,0, width(), height())));
    if (checked) {
        painter.setPen(helper->shadowBorderPen);
        painter.setBrush(helper->shadowBorderBrush);
    } else {
        painter.setPen(helper->lightBorderPen);
        painter.setBrush(helper->lightBorderBrush);
    }

    painter.drawPolygon(QPolygon()
                        << QPoint(0,0)
                        << QPoint(0, height())
                        << QPoint(borderWidth, height() - borderWidth)
                        << QPoint(borderWidth, borderWidth)
                        << QPoint(width() - borderWidth, borderWidth)
                        << QPoint(width(), 0)
                        );
    if (checked) {
        painter.setPen(helper->lightBorderPen);
        painter.setBrush(helper->lightBorderBrush);
    } else {
        painter.setPen(helper->shadowBorderPen);
        painter.setBrush(helper->shadowBorderBrush);
    }
    painter.drawPolygon(QPolygon()
                        << QPoint(0, height())
                        << QPoint(width(), height())
                        << QPoint(width(), 0)
                        << QPoint(width() - borderWidth, borderWidth)
                        << QPoint(width() - borderWidth, height() - borderWidth)
                        << QPoint(borderWidth, height() - borderWidth)
                        );
    painter.setPen(helper->paintPen);
    painter.setBrush(helper->paintBrush);
    QFontMetrics fontMetrics = painter.fontMetrics();
    int ascent = fontMetrics.ascent();
    int fontHeight = ascent + fontMetrics.descent();
    bool directionType = direction == MEASURE_SORT_LUTRD || direction == MEASURE_SORT_RDTLU;
    QString text;
    if (direction == MEASURE_SORT_LDTRU || direction == MEASURE_SORT_LUTRD) {
        text = "1";
    } else {
        text = "2";
    }
    if (directionType) {
        painter.drawText(padding, padding + ascent, text);
    } else {
        painter.drawText(padding, height() - padding - fontHeight + ascent, text);
    }

    if (direction == MEASURE_SORT_RUTLD || direction == MEASURE_SORT_RDTLU) {
        text = "1";
    } else {
        text = "2";
    }
    int fontWidth = fontMetrics.tightBoundingRect(text).width();
    if (directionType) {
        painter.drawText(width() - padding - fontWidth,
                         height() - padding - fontHeight + ascent,
                         text);
    } else {
        painter.drawText(width() - padding - fontWidth, padding + ascent, text);
    }

    QPoint cp1;
    QPoint cp2;
    if (directionType) {
        cp1 = QPoint(padding + fontWidth / 2, padding + fontHeight / 2);
        cp2 = QPoint(width() - padding - fontWidth / 2, height() - padding - fontHeight / 2);
    } else {
        cp1 = QPoint(padding + fontWidth / 2, height() - padding - fontHeight / 2);
        cp2 = QPoint(width() - padding - fontWidth / 2, padding + fontHeight / 2);
    }

    int tWidth = std::sqrt(std::pow(cp2.x() - cp1.x(), 2) + std::pow(cp1.y() - cp2.y(), 2));
    int bold = 4;
    int halfBold = bold / 2;
    int arrowWidth = tWidth * 0.2;
    int arrowDiff = 4;
    int fontWidthArrowGap = 10;
    QTransform transform = QTransform::fromTranslate(cp1.x(), cp1.y());
    qreal rot = std::atan(double(cp1.y() - cp2.y()) / double(cp2.x() - cp1.x())) * (180 / M_PI);
    transform.rotate(-rot);
    QPolygonF polygon;
    if (direction == MEASURE_SORT_LDTRU || direction == MEASURE_SORT_LUTRD) {
        polygon = QPolygonF()
                           << QPointF(fontWidthArrowGap, -halfBold)
                           << QPointF(fontWidthArrowGap, halfBold)
                           << QPointF(tWidth - arrowWidth - fontWidthArrowGap, halfBold)
                           << QPointF(tWidth - arrowWidth - fontWidthArrowGap, halfBold + arrowDiff)
                           << QPointF(tWidth - fontWidthArrowGap, 0)
                           << QPointF(tWidth - arrowWidth - fontWidthArrowGap, - halfBold - arrowDiff)
                           << QPointF(tWidth - arrowWidth - fontWidthArrowGap, - halfBold);

    } else {
        polygon = QPolygonF()
        << QPointF(tWidth - fontWidthArrowGap, -halfBold)
        << QPointF(tWidth - fontWidthArrowGap, halfBold)
        << QPointF(arrowWidth + fontWidthArrowGap, halfBold)
        << QPointF(arrowWidth + fontWidthArrowGap, halfBold + arrowDiff)
        << QPointF(fontWidthArrowGap, 0)
        << QPointF(arrowWidth + fontWidthArrowGap, - halfBold - arrowDiff)
        << QPointF(arrowWidth + fontWidthArrowGap, - halfBold);
    }
    painter.drawPolygon(transform.map(polygon));
}

void MeasurementSortDiagonalCanvas::mousePressEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton) {
        event->ignore();
        QWidget::mousePressEvent(event);
        return;
    }
    mousePressed = true;
}
void MeasurementSortDiagonalCanvas::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton || !mousePressed) {
        event->ignore();
        QWidget::mouseReleaseEvent(event);
        return;
    }
    checked = !checked;
    if (!signalsBlocked()) {
        emit toggled(checked);
    }
    mousePressed = false;
    this->update();
}

MeasurementSortDiagonalView::MeasurementSortDiagonalView(QWidget* parent) : CDE90ScrollArea(parent) {
    create();
    createAction();
}

MeasurementSortDiagonalView::~MeasurementSortDiagonalView() noexcept {
    DEFAULT_DELETE_ARR_PTR_BLOCK(directionCheckers)
}

void MeasurementSortDiagonalView::create() {
    areaContent = new QWidget(this);
    areaContent->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    areaLayout = new QHBoxLayout(areaContent);
    areaLayout->setSpacing(5);
    areaLayout->setContentsMargins(0, 0,0,0);
    areaContent->setLayout(areaLayout);
    setWidget(areaContent);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setFixedHeight(70 + horizontalScrollBar()->height());
}

void MeasurementSortDiagonalView::createAction() {
    directionCheckers = new MeasurementSortDiagonalCanvas*[MEASURE_SORT_RDTLU + 1];
    for (int i = 0; i <= MEASURE_SORT_RDTLU; ++i) {
        MeasurementSortDiagonalCanvas* c = new MeasurementSortDiagonalCanvas(areaContent);
        c->setDirection(MEASURE_SORT_DIAGONAL(i));
        c->setFixedSize(70,70);
        c->hide();
        directionCheckers[i] = c;
        connect(directionCheckers[i], &MeasurementSortDiagonalCanvas::toggled, this, [this, c, i](bool checked){
            if (checked) {
                if (lastChecked == i) {
                    return;
                }
                if (lastChecked >= 0) {
                    directionCheckers[lastChecked]->setChecked(false);
                }
                lastChecked = i;
                emit sortChanged(i);
            } else {
                c->blockSignals(true);
                c->setChecked(true);
                c->blockSignals(false);
            }
        });
        areaLayout->addWidget(directionCheckers[i]);
    }
}

void MeasurementSortDiagonalView::setChecked(
    int sortType) {
    if (sortType < 0 || sortType > MEASURE_SORT_RDTLU) {
        if (lastChecked >= 0) {
            directionCheckers[lastChecked]->setChecked(false);
            lastChecked = -1;
        }
        return;
    }
    int i = int(sortType);
    if (i == lastChecked) {
        return;
    }
    if (lastChecked >= 0) {
        directionCheckers[lastChecked]->setChecked(false);
    }
    this->lastChecked = i;
    directionCheckers[i]->setChecked(true);
}

int MeasurementSortDiagonalView::getChecked() {
    return MEASURE_SORT_DIAGONAL(lastChecked);
}

void MeasurementSortDiagonalView::hideSort(
    MEASURE_SORT_DIAGONAL i, bool hide) {
    directionCheckers[i]->setVisible(!hide);
    if (lastChecked == i) {
        directionCheckers[lastChecked]->setChecked(false);
        lastChecked = -1;
    }
}
