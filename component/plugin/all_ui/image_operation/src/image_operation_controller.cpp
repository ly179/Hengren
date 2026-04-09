#include "image_operation_controller.h"
#include "ui_image_operation_controller.h"
#include <QButtonGroup>
#include <QPainter>
#include "cdsem_global_resource.h"
#include <QGridLayout>
#include "cde90_button.h"
#include "cde90_combo_box.h"
#include "cde90_label.h"
#include "cde90_menu.h"
#include "cde90_message_box.h"
#include "cde90_text_field.h"
#include "cde90_window.h"
#include "image_measurement_annulus_tool.h"
#include "image_measurement_circle_tool.h"
#include "image_measurement_cross_tool.h"
#include "image_measurement_distance_tool.h"
#include "image_measurement_double_rect_tool.h"
#include "image_measurement_lines_tool.h"
#include "image_measurement_rect_text_tool.h"
#include "image_measurement_rect_tool.h"
#include "image_measurement_sector_tool.h"
#include "image_operation_custom_setter_controller.h"
#include "image_operation_view_setter_controller.h"
#include "image_operation_widget_canvas.h"
#include "cdsem_global_data.h"
#include "cdsem_data_common.h"
#include "image_scan_live_mode_helper.h"
#include "cdsem_control_helper.h"
#include "optics_editor_relation.h"
#include "stream_image_supplier.h"
#include "cdsem_global_env_data_model.h"
#include "cdsem_number_validator_helper.h"
#include "param_horizontal_slider.h"
#include "image_operation_data_model.h"
#include "image_file_temp_handler.h"
#include "image_save_model.h"
#include "result_data_model.h"
#include "image_view_measure_tool_draw_helper.h"
#include "signal_hub_single_object_manager.h"
#include "private/image_operation_controller_ui_helper.h"

#include "private/image_operation_controller_data_helper.h"
#include "private/width_cursor_handler.h"
#include "private/width_roughness_cursor_handler.h"
#include "private/edge_roughness_cursor_handler.h"
#include "private/diameter_hole_cursor_handler.h"
#include "private/ellipse_cursor_handler.h"
#include "square_size_fix_helper.h"

#include "amp_editor_relation.h"
#include "image_type_defition.h"

#include "auto_function_helper.h"
#include "single_timer_filter.h"

int addSemMag(int m) {
    int mag = IOPT->getSemMag();
    if (m > 0) {
        if (mag >= 500000) {
            return -1;
        }
        mag += m;
        if (mag > 500000) mag = 500000;
        return mag;
    } else if (m < 0) {
        if (mag <= 10) {
            return -1;
        }
        mag += m;
        if (mag < 10) mag = 10;
        return mag;
    }
    return -1;
}

struct MeasurementLines {
    MeasurementNumber left = 0;
    MeasurementNumber right = 0;
    MeasurementNumber top = 0;
    MeasurementNumber bottom = 0;
};

class ImageMeasurementPoundLinesTool : public ImageMeasurementLinesTool {
private:
    ImageMeasurementLineTool* hLines[2];
    ImageMeasurementLineTool* vLines[2];

    void refreshLines(const QSize& size) {
        qint32 spaceX = size.width() / 3;
        qint32 spaceY = size.height() / 3;
        qint32 rx = size.width() % 3;
        qint32 ry = size.height() % 3;
        qint32 x, y;
        for (int i = 1; i < 3; ++i) {
            y = spaceY * i + (ry - i >= 0 ? 1 : 0);
            hLines[i - 1]->setStart({0, y}, false);
            hLines[i - 1]->setEnd({size.width() - 1, y}, false);
        }
        for (int i = 1; i < 3; ++i) {
            x = spaceX * i + (rx - i >= 0 ? 1 : 0);
            vLines[i - 1]->setStart({x, 0}, false);
            vLines[i - 1]->setEnd({x, size.height() - 1}, false);
        }
    }

public:
    explicit ImageMeasurementPoundLinesTool(
        ImageOperationCanvas* canvas)
        : ImageMeasurementLinesTool(canvas) {
        for (int i = 0; i < 2; ++i) {
            hLines[i] = new ImageMeasurementLineTool(this);
            hLines[i]->setWeight(2, false);
            hLines[i]->setColor(QColor(255, 255, 255, 125), false);
            vLines[i] = new ImageMeasurementLineTool(this);
            vLines[i]->setWeight(2, false);
            vLines[i]->setColor(QColor(255, 255, 255, 125), false);
            addLine(hLines[i], false);
            addLine(vLines[i], false);
        }
        disableMouse();
    }
    ~ImageMeasurementPoundLinesTool() noexcept {}

    void update(bool canvasUpdate = true) override {
        refreshLines(paintParam.getOImgSize());
        ImageMeasurementLinesTool::update(canvasUpdate);
    }

    void paintEvent(
        AfterImagePaintEvent& event) override {
        if (paintParam != event) {
            refreshLines(event.oImgSize);
        }
        ImageMeasurementLinesTool::paintEvent(event);
    }
};

ImageOperationController::ImageOperationController(QWidget* parent) : RelationImageOperationController(parent){
    // 直接初始化，不等待信号
    dataHelper = new ImageOperationControllerDataHelper;
    dataHelper->widthCursorHandler = new WidthCursorHandler(dataHelper);
    dataHelper->widthRoughnessCursorHandler = new WidthRoughnessCursorHandler(dataHelper);
    dataHelper->edgeRoughnessCursorHandler = new EdgeRoughnessCursorHandler(dataHelper);
    dataHelper->diameterHoleCursorHandler = new DiameterHoleCursorHandler(dataHelper);
    dataHelper->ellipseCursorHandler = new EllipseCursorHandler(dataHelper);
    dataHelper->d = this;
    create();
    createAction();
    triggerAction();

    // 保留信号连接以备不时之需
    connect(this, &CDE90Window::asyncCallerChanged, this, [this](){
        // 如果已经初始化了，就不需要再执行
        if (!dataHelper) {
        }
    });
}
ImageOperationController::~ImageOperationController() noexcept {
    if (uiHelper->viewSetter) {
        uiHelper->viewSetter->release();
    }
    if (uiHelper->customSetter) {
        uiHelper->customSetter->release();
    }
    if (dataHelper) {
        delete dataHelper->widthCursorHandler;
        delete dataHelper->widthRoughnessCursorHandler;
        delete dataHelper->edgeRoughnessCursorHandler;
        delete dataHelper->diameterHoleCursorHandler;
        delete dataHelper->ellipseCursorHandler;
        delete dataHelper;
    }
    if (uiHelper) delete uiHelper;
    if (ui) delete ui;
}

void ImageOperationController::closeEvent(QCloseEvent* event) {
    if (isCloseDisabled()) {
        CDE90Window::closeEvent(event);
        return;
    }
    if (!dataHelper->closeRequested) {
        ui->closeBtn->loading();
        this->setDisabled(true);
        disableClose(true);
        dataHelper->closeRequested = true;

        cdsem::CDSEM_GlobalDataHelper::get()->invokeImage([this]() {
            cdsem::CDSEM_GlobalDataHelper::get()->unRegistImageArrivedCallback(IMG_OPT_TYPE_SEM, IMG_OPT_BUSINESS_TYPE_VIEW_WINDOW_);
            cdsem::CDSEM_GlobalDataHelper::get()->unRegistImageArrivedCallback(IMG_OPT_TYPE_OM, IMG_OPT_BUSINESS_TYPE_VIEW_WINDOW_);
            cdsem::CDSEM_GlobalDataHelper::get()->unRegistImageArrivedCallback(IMG_OPT_TYPE_IM, IMG_OPT_BUSINESS_TYPE_VIEW_WINDOW_);
            MainThreadInvoker::getInvoker().runLater([this](){
                disableClose(false);
                this->close();
            });
        });
    }
    CDE90Window::closeEvent(event);
}

void ImageOperationController::showEvent(QShowEvent* event) {
     reloadData();
     CDE90Window::showEvent(event);
 }


void ImageOperationController::createDistenceTool(Qt::Orientation o) {
    clearTool();
    QSize oSize = uiHelper->canvas->imageStream()->currentImageSize();
    ImageMeasurementDistanceTool* dTool = new ImageMeasurementDistanceTool(uiHelper->canvas);
    dTool->setLineWeight(2, false);
    dTool->setColor(QColor(255, 255, 255, 125), false);
    dTool->setLineStyle(MTBS_DASH, false);
    dTool->setLine1(10, false);
    dTool->setLine2((oSize.isNull() ? (o == Qt::Horizontal ? uiHelper->canvas->canvasWidth() : uiHelper->canvas->canvasHeight()) : (o == Qt::Horizontal ? oSize.width() : oSize.height())) - 10, false);
    dTool->setOrientation(o);
    connect(dTool, &ImageMeasurementDistanceTool::posChanged, this, [dTool, this](bool){
        ui->resultLabel->setText(cdsem::PixelDistanceConvert::instance()->getNMSizeFromPixel(dTool->getDistance().toInt(), IOPT->getPixelSize()).toQString(2) + "nm");
    });
    uiHelper->currentTool = dTool;
    ui->resultLabel->setText(cdsem::PixelDistanceConvert::instance()->getNMSizeFromPixel(dTool->getDistance().toInt(), IOPT->getPixelSize()).toQString(2) + "nm");
    addMeasurementTool(dTool);
    dTool->update();
}

void ImageOperationController::createRectAreaTool() {
    resetRectCursorMode = false;
    clearTool();
    ImageMeasurementRectTool* rectTool = new ImageMeasurementRectTool(uiHelper->canvas);
    updateRectToolParam(rectTool);
    uiHelper->currentTool = rectTool;
    addMeasurementTool(rectTool);
    uiHelper->currentTool->update();
}

void ImageOperationController::updateRectToolParam(
    ImageMeasurementRectTool* rectTool) {
    rectTool->setWeight(2, false);
    rectTool->setColor(QColor(255, 255, 255, 125), false);
    rectTool->setMoveAsPixel(true);
    rectTool->setBorderStyle(MTBS_SOLID_CORNER_DASH_LINE, false);
    rectTool->setOverLine(MTBOP_INNER);

    if (rectCursorStatus == image_operation::RECT_CURSOR_64) {
        rectTool->setSize("64", "64", false);
    } else if (rectCursorStatus == image_operation::RECT_CURSOR_32_192) {
        rectTool->setSize("128", "128", false);
    } else if (rectCursorStatus == image_operation::RECT_CURSOR_192x80) {
        rectTool->setSize("192", "80", false);
    } else if (rectCursorStatus == image_operation::RECT_CURSOR_80x192) {
        rectTool->setSize("80", "192", false);
    } else if (rectCursorStatus == image_operation::RECT_CURSOR_40x96) {
        rectTool->setSize("40", "96", false);
    } else if (rectCursorStatus == image_operation::RECT_CURSOR_96x40) {
        rectTool->setSize("96", "40", false);
    } else {
        rectTool->toNormalSize(false);
    }
    if (rectCursorStatus == image_operation::RECT_CURSOR_32_192) {
        rectTool->disableBorderResize(false);
        rectTool->disableCornerResize(false);
        rectTool->setBorderStyle(MTBS_SOLID_CORNER_DASH_LINE, false);
        rectTool->setMinSize({"32", "32"});
        rectTool->setMaxSize({"192", "192"});
    } else {
        rectTool->disableBorderResize(true);
        rectTool->disableCornerResize(true);
        rectTool->setBorderStyle(MTBS_SOLID, false);
        rectTool->setMinSize({});
        rectTool->setMaxSize({MeasurementNumber(), MeasurementNumber()});
    }
    rectTool->move(calcCenterPos(rectTool->getSize(), uiHelper->canvas), false);
}

void ImageOperationController::createCrossTool() {
    clearTool();
    ImageMeasurementCrossTool* crossTool = new ImageMeasurementCrossTool(uiHelper->canvas);
    // crossTool->setCrossSize(MeasurementSize{120, 120}, false);
    // crossTool->lockImageCenter(true);
    crossTool->setWeight(2, false);
    crossTool->setScaleProportionally(true, false);
    crossTool->setCrossSize({30, 30}, false);
    crossTool->setColor(QColor(255, 255, 255, 125), false);
    // crossTool->setMouseTransparent(true);
    crossTool->toScreenMode(true, false);
    crossTool->toCurrImgCenter();
    uiHelper->currentTool = crossTool;
    addMeasurementTool(crossTool);
    uiHelper->currentTool->update();
}

void ImageOperationController::clearTool() {
    if (uiHelper->currentTool) {
        removeMeasurementTool(uiHelper->currentTool);
        uiHelper->currentTool->update();
        delete uiHelper->currentTool;
        uiHelper->currentTool = nullptr;
    }
    ui->resultLabel->setText(dataHelper->resultText);
    if (resetRectCursorMode && rectCursorStatus != image_operation::RECT_CURSOR_BEGIN) {
        rectCursorStatus = image_operation::RECT_CURSOR_BEGIN;
    }
    if (!resetRectCursorMode) resetRectCursorMode = true;
}

ImageMeasurementRectTool* ImageOperationController::createMeasurementRectTool() {
    return new ImageMeasurementRectTool(uiHelper->canvas);
}
ImageMeasurementRectTextTool* ImageOperationController::createMeasurementRectTextTool() {
    return new ImageMeasurementRectTextTool(uiHelper->canvas);
}
ImageMeasurementAnnulusTool* ImageOperationController::createMeasurementAnnulusTool() {
    return new ImageMeasurementAnnulusTool(uiHelper->canvas);
}
ImageMeasurementCircleTool* ImageOperationController::createMeasurementCircleTool() {
    return new ImageMeasurementCircleTool(uiHelper->canvas);
}
ImageMeasurementCrossTool* ImageOperationController::createMeasurementCrossTool() {
    return new ImageMeasurementCrossTool(uiHelper->canvas);
}
ImageMeasurementDoubleRectTool* ImageOperationController::createMeasurementDoubleRectTool() {
    return new ImageMeasurementDoubleRectTool(uiHelper->canvas);
}

ImageMeasurementSectorTool* ImageOperationController::createMeasurementSectorTool() {
    return new ImageMeasurementSectorTool(uiHelper->canvas);
}

ImageMeasurementLinesTool* ImageOperationController::createMeasurementLinesTool() {
    return new ImageMeasurementLinesTool(uiHelper->canvas);
}

void ImageOperationController::addMeasurementTool(ImageMeasurementTool* t) {
    uiHelper->canvas->addMeasurementTool(t);
}
void ImageOperationController::removeMeasurementTool(ImageMeasurementTool* t) {
    uiHelper->canvas->removeMeasurementTool(t);
}
void ImageOperationController::measurementToolToPrevFloor(ImageMeasurementTool* t, ImageMeasurementTool* target) {
    uiHelper->canvas->measurementToolToPrevFloor(t, target);
}
void ImageOperationController::measurementToolToBehindFloor(ImageMeasurementTool* t, ImageMeasurementTool* target) {
    uiHelper->canvas->measurementToolToBehindFloor(t, target);
}
void ImageOperationController::measurementToolToTopFloor(ImageMeasurementTool* t) {
    uiHelper->canvas->measurementToolToTopFloor(t);
}
void ImageOperationController::measurementToolToBottomFloor(ImageMeasurementTool* t) {
    uiHelper->canvas->measurementToolToBottomFloor(t);
}

void ImageOperationController::createOpticsController() {
    MsgReceiveStatus<RelationOpticsEditController*> status = image_optics::openSingleOpticsController(ACaller, this);
    if (!status.ok) {
        simpleMessageBox(this, "Error", "Error to open Optics Window.");
    }
}

void ImageOperationController::localCrossCursorMode() {
    if (localShiftMode != 1)
        ui->locateCrossBtn->click();
}

void ImageOperationController::rectCursorMode(const MeasurementRect& pos, image_operation::RectCursorType type) {
    rectCursorStatus = type;
    ui->rectBtn->click();
    ImageMeasurementRectTool* rectTool = dynamic_cast<ImageMeasurementRectTool*>(uiHelper->currentTool);
    if (!rectTool) return;
    rectTool->move(pos.offset);
    if (type == image_operation::RECT_CURSOR_32_192) {
        rectTool->setSize(pos.size);
    }
}

void ImageOperationController::rectCursorMode(image_operation::RectCursorType type) {
    rectCursorStatus = type;
    ui->rectBtn->click();
}
void ImageOperationController::blankCursorMode() {
    ui->blankBtn->toggle();
}

QPoint ImageOperationController::getCrossCursorCenter() {
    if (!uiHelper->currentTool) {
        return QPoint();
    }
    ImageMeasurementCrossTool* crossTool = dynamic_cast<ImageMeasurementCrossTool*>(uiHelper->currentTool);
    if (!crossTool) {
        return QPoint();
    }
    const MeasurementPoint& p = crossTool->getCenter();
    return QPoint(p.x, p.y);
}

QRect ImageOperationController::getRectCursorRange() {
    if (!uiHelper->currentTool) {
        return QRect();
    }
    ImageMeasurementRectTool* rectTool = dynamic_cast<ImageMeasurementRectTool*>(uiHelper->currentTool);
    if (!rectTool) {
        return QRect();
    }
    return QRect(qint32(rectTool->getPos().x),
                 qint32(rectTool->getPos().y),
                 qint32(rectTool->getSize().width),
                 qint32(rectTool->getSize().height));
}


StreamImageSupplier* ImageOperationController::imageStream() const {
    if (!uiHelper || !uiHelper->canvas) {
        qCritical() << "ImageOperationController: canvas is null!";
        return nullptr;
    }
    return uiHelper->canvas->imageStream();
}

void ImageOperationController::refreshScanParamLabel() {
    int ot = IOPT->getImageOperationType();
    cdsem::PhotoImageType it = IOPT->getPhotoImageType();
    if (ot == IMG_OPT_TYPE_SEM || (ot == IMG_OPT_TYPE_IM && it == cdsem::PIT_SEM)) {
        int mag = IOPT->getSemMag();
        renderMag(mag);
        dataHelper->refreshRot();
        dataHelper->refreshRot();
        ui->resultLabel->clear();
    }
}

void ImageOperationController::reloadData() {
    dataHelper->reloadData();
}

void ImageOperationController::amsToFirstStep() {
    dataHelper->stopAutoMeasure();
    dataHelper->amsNextStep();
}

void ImageOperationController::setMeasureResult(const ImageResultData& rData) {
    dataHelper->amsBtnToGreen();
    cdsem::CDSEM_GlobalDataHelper::get()->setAmsStatusIgnoralData(cdsem::AMS_SMEASURE);
    dataHelper->resultText = QString::number(rData.result, 'f', 2) + "nm";
    if (!ui->vLineBtn->isChecked() && !ui->hLineBtn->isChecked()) {
        ui->resultLabel->setText(dataHelper->resultText);
    }
    if (rData.measureResults.size() > 0) {
        int size = 0;
        for (const MeasureResultData& data : rData.measureResults) {
            size += data.baseResults.size();
        }
        if (size > 0) {
            dataHelper->ampCursors.results.reserve(size * 2);
            QList<ImageMeasurementCrossTool*> list;
            drawResultCross(uiHelper->canvas, rData.measureResults, &list);
            if (list.size() > 0) {
                for (ImageMeasurementCrossTool*& c : list) {
                    dataHelper->ampCursors.results.append(c);
                }
            }
        }
    }
    if (rData.profile.size() > 0) {
        uiHelper->canvas->updateProfileLines(rData.profile, QColor(255, 255, 255, 125));
    } else {
        uiHelper->canvas->clearProfileLines();
    }
}

void ImageOperationController::stopAutoMeasure() {
    dataHelper->stopAutoMeasure();
}

bool ImageOperationController::collectCursorCenterToTmp() {
    return dataHelper->collectCenters();
}

void ImageOperationController::reloadImageFromGlobal() {
    cdsem::CDSEM_GlobalDataHelper::get()->invokeImage([this]() {
        dataHelper->rewriteImage();
    });
}

void ImageOperationController::renderMag(int mag) {
    if (IOPT->getImageOperationType() == IMG_OPT_TYPE_OM) return;
    int n = 0;
    if (mag < 10'000) {
        n = 2;
    } else if (mag < 100'000) {
        n = 1;
    }
}


void ImageOperationController::changeIconLabel() {
    cdsem::MeasureObject mObj = cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpMeasureObject();
    if (mObj == cdsem::MO_LINE) {
        ui->iconLabel->setPixmap(ResourceManager::getResourceManager().getM_LineIcon());
    } else if (mObj == cdsem::MO_LEFT_LINE || mObj == cdsem::MO_SLOPE_L) {
        ui->iconLabel->setPixmap(ResourceManager::getResourceManager().getM_LeftSlopeOrLineIcon());
    } else if (mObj == cdsem::MO_RIGHT_LINE || mObj == cdsem::MO_SLOPE_R) {
        ui->iconLabel->setPixmap(ResourceManager::getResourceManager().getM_RightSlopeOrLineIcon());
    } else if (mObj == cdsem::MO_PITCH_L) {
        ui->iconLabel->setPixmap(ResourceManager::getResourceManager().getM_LeftPitchIcon());
    } else if (mObj == cdsem::MO_PITCH_R) {
        ui->iconLabel->setPixmap(ResourceManager::getResourceManager().getM_RightPitchIcon());
    } else if (mObj == cdsem::MO_GAP) {
        ui->iconLabel->setPixmap(ResourceManager::getResourceManager().getM_GapLineIcon());
    } else if (mObj == cdsem::MO_INNER_DIAMETER) {
        ui->iconLabel->setPixmap(ResourceManager::getResourceManager().getM_InnerDiameterIcon());
    } else if (mObj == cdsem::MO_CORE_GAP) {
        ui->iconLabel->setPixmap(ResourceManager::getResourceManager().getM_CoreGapIcon());
    } else if (mObj == cdsem::MO_SPACE || mObj == cdsem::MO_SPACER_GAP) {
        ui->iconLabel->setPixmap(ResourceManager::getResourceManager().getM_SpaceOrCoreSpaceIcon());
    } else if (mObj == cdsem::MO_TOP) {
        ui->iconLabel->setPixmap(ResourceManager::getResourceManager().getM_TopIcon());
    } else if (mObj == cdsem::MO_CORNER_LT) {
        ui->iconLabel->setPixmap(
            ResourceManager::getResourceManager().getCornerLeftTop().pixmap(QSize(30, 30)));
    } else if (mObj == cdsem::MO_CORNER_LB) {
        ui->iconLabel->setPixmap(
            ResourceManager::getResourceManager().getCornerLeftBottom().pixmap(QSize(30, 30)));
    } else if (mObj == cdsem::MO_CORNER_RT) {
        ui->iconLabel->setPixmap(
            ResourceManager::getResourceManager().getCornerRightTop().pixmap(QSize(30, 30)));
    } else if (mObj == cdsem::MO_CORNER_RB) {
        ui->iconLabel->setPixmap(
            ResourceManager::getResourceManager().getCornerRightBottom().pixmap(QSize(30, 30)));
    } else {
        ui->iconLabel->clear();
    }
}

void ImageOperationController::refreshCanvasCursorToolContent() {
    uiHelper->alignBtn->setVisible(dataHelper->canvasAcdPaneStatus && cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpMeasureType() == cdsem::MT_GAP);
    uiHelper->detectBtn->setVisible(dataHelper->canvasAcdPaneStatus && cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpMeasureType() == cdsem::MT_DIAMETER_HOLE);
    uiHelper->sortBtn->setVisible(dataHelper->canvasAcdPaneStatus);
    uiHelper->lockCursorMoveBtn->setVisible(dataHelper->canvasAcdPaneStatus);
    uiHelper->gatherToCenterBtn->setVisible(dataHelper->canvasAcdPaneStatus);
}

void ImageOperationController::create() {
    uiHelper = new ImageOperationControllerUiHelper;
    uiHelper->d = this;
    uiHelper->normalColor = QColor(179, 179, 179);
    uiHelper->lower10k = QColor(0, 253, 253);
    uiHelper->higherEq10k = QColor(255, 0, 255);

    QWidget* content = new QWidget;
    setContent(content);
    CDE90LayoutHelper layoutHelper(content);

    layoutHelper.registUnknowType("canvasContent", [this](QObject* parent, const QHash<QString, QObject*>&, QHash<QString, QList<QStringList>>&) -> QObject*{
        QWidget* widget = dynamic_cast<QWidget*>(parent);
        if (!widget) {
            return nullptr;
        }
        return uiHelper->initImageCanvas(widget);
    });

    ui = new cdsem::UiImageOperationController;
    ui->init(layoutHelper);

    uiHelper->viewSetter = new ImageOperationViewSetterController;
    uiHelper->viewSetter->setCloseCall([this](CDE90WindowCloseStatus){
        return true;
    });
    uiHelper->viewSetter->setWindowTitle("View Setup");
    uiHelper->viewSetter->setResizable(false);

    ui->centerWidget->setStyleSheet("QLabel {"
                               "border-top: 2px solid rgb(46, 46, 46);"
                               "border-left: 2px solid rgb(46, 46, 46);"
                               "border-bottom: 2px solid rgb(181, 181, 181);"
                               "border-right: 2px solid rgb(181, 181, 181);"
                               "background-color: rgb(100, 100, 100);"
                               "color: #FFF;"
                               "padding: 3px 0;"
                               "}");
    ui->centerTitleWidget->setStyleSheet("QWidget {"
                                    "border-top: 2px solid rgb(46, 46, 46);"
                                    "border-left: 2px solid rgb(46, 46, 46);"
                                    "border-bottom: 2px solid rgb(181, 181, 181);"
                                    "border-right: 2px solid rgb(181, 181, 181);"
                                    "background-color: rgb(100, 100, 100);"
                                    "color: #FFF;"
                                    "}"
                                    "QLabel {"
                                     "background-color: transparent;"
                                     "border: none;"
                                    "}");
    ui->toolsWidget->setStyleSheet("#vLineBtn {image: url(':/icon/v_line.png');}"
                                   "#vLineBtn:disabled {image: url(':/icon/v_line_disabled.png');}"
                                   "#hLineBtn {image: url(':/icon/h_line.png');}"
                                   "#hLineBtn:disabled {image: url(':/icon/h_line_disabled.png');}"
                                   "#crossBtn {image: url(':/icon/cross.png');}"
                                   "#crossBtn:disabled {image: url(':/icon/cross_disabled.png');}"
                                   "#rectBtn {image: url(':/icon/rect.png');}"
                                   "#rectBtn:disabled {image: url(':/icon/rect_disabled.png');}"
                                   "#locateCrossBtn {image: url(':/icon/beam_shift.png');}"
                                   "#locateCrossBtn:disabled {image: url(':/icon/beam_shift_disabled.png');}"
                                   "#poundBtn {image: url(':/icon/pound.png');}"
                                   "#poundBtn:disabled {image: url(':/icon/pound_disabled.png');}");

    SquareSizeHandler* sizeHandler = new SquareSizeHandler(uiHelper->canvasContent);
    sizeHandler->setFPolicy(square_size::SSP_FOLLOW_HEIGHT);
    sizeHandler->setUPolicy(square_size::SSP_USE_MIN_SIZE);
    sizeHandler->setWidget(uiHelper->canvasContent);
}

void ImageOperationController::createAction() {

    QButtonGroup* toolBtnGroup = new QButtonGroup(this);
    toolBtnGroup->addButton(ui->vLineBtn);
    toolBtnGroup->addButton(ui->hLineBtn);
    toolBtnGroup->addButton(ui->crossBtn);
    toolBtnGroup->addButton(ui->rectBtn);
    toolBtnGroup->addButton(ui->locateCrossBtn);
    toolBtnGroup->addButton(ui->poundBtn);
    toolBtnGroup->addButton(ui->blankBtn);

    ui->blankBtn->setChecked(true);
    uiHelper->canvasContent->setColor(uiHelper->normalColor);

    {
        CDE90Menu* menu = new CDE90Menu(ui->functionBtn->getQPushButton());
        ui->functionBtn->getQPushButton()->setMenu(menu);
        QAction* action;
        action = menu->addAction("Image Save... (在293页,9380可能是右下角的按钮))");

        //TODO 临时功能
        action = menu->addAction("Image Export");
        connect(action, &QAction::triggered, this, [this](){
            QStringList list;
            while (true) {
                if (list.isEmpty()) {
                    simpleMessageBox(this, "错误", "请指定要保存的文件名");
                    continue;
                }
                break;
            }
            QString filePath = list.first();
            QImage* image = uiHelper->canvas->imageStream()->beginVisitImage();
            if (image == nullptr || image->isNull()) {
                simpleMessageBox(this, "错误", "图像不存在");
                return;
            }
            if (!image->save(filePath, "jpg")) {
                simpleMessageBox(this, "错误", "图像保存失败");
                return;
            }
            simpleMessageBox(this, "消息", "图像保存成功");
        });
    }


    //connect(uiHelper->scanParamViewBtn, &CDE90Button::toggled, this, [this](bool checked){
     //   uiHelper->scanParamLabel->setVisible(checked);
    //});

    connect(ui->closeBtn->getQPushButton(), &CDE90Button::clicked, this, [this]() {
        this->close();
        // QGuiApplication::exit();
    });

    connect(ui->ampBtn->getQPushButton(), &CDE90Button::clicked, this, [this]() {
        amp_editor::openSingleAMPEditor();
    });
    connect(ui->amsBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        dataHelper->amsNextStep();
    });
    connect(ui->vLineBtn, &CDE90Button::toggled, this, [this](bool checked) {
        if (checked) {
            createDistenceTool(Qt::Horizontal);
        }
    });
    connect(ui->hLineBtn, &CDE90Button::toggled, this, [this](bool checked) {
        if (checked) {
            createDistenceTool(Qt::Vertical);
        }
    });
    connect(ui->rectBtn, &CDE90Button::clicked, this, [this]() {
        qint32 i = qint32(rectCursorStatus);
        if (i < image_operation::RECT_CURSOR_BEGIN || i >= image_operation::RECT_CURSOR_END) i = image_operation::RECT_CURSOR_BEGIN;
        rectCursorStatus = image_operation::RectCursorType(i);
        createRectAreaTool();
        rectCursorStatus = image_operation::RectCursorType(i + 1);
    });
    connect(ui->crossBtn, &CDE90Button::toggled, this, [this](bool checked) {
        if (checked) {
            createCrossTool();
        }
    });

    connect(ui->locateCrossBtn, &QPushButton::toggled, this, [this](bool checked) {
        if (!checked) {
            if (IOPT->getImageOperationType() == IMG_OPT_TYPE_SEM) {
                dataHelper->switchShiftFieldBtnView(0);
            } else if (IOPT->getImageOperationType() == IMG_OPT_TYPE_OM) {
                dataHelper->switchShiftFieldBtnView(2);
            }
            dataHelper->switchShiftFieldModel(0);
        }
    });

    connect(ui->locateCrossBtn, &QPushButton::clicked, this, [this](bool checked) {
        if (checked) {
            if (IOPT->getImageOperationType() == IMG_OPT_TYPE_SEM) {
                if (localShiftMode == 1)
                    dataHelper->switchShiftFieldModel(2);
                else
                    dataHelper->switchShiftFieldModel(1);
            } else if (IOPT->getImageOperationType() == IMG_OPT_TYPE_OM) {
                dataHelper->switchShiftFieldModel(2);
            }
        }
    });

    connect(ui->poundBtn, &QPushButton::toggled, this, [this](bool checked) {
        if (checked) {
            clearTool();
            ImageMeasurementPoundLinesTool* poundLines = new ImageMeasurementPoundLinesTool(uiHelper->canvas);
            uiHelper->currentTool = poundLines;
            addMeasurementTool(uiHelper->currentTool);
            uiHelper->currentTool->update();
        }
    });

    connect(ui->blankBtn, &QPushButton::toggled, this, [this](bool checked) {
        if (checked) {
            clearTool();
        }
    });

    //viewSetter的apply按钮点击后收到这个信号

    connect(cdsem::CDSEM_GlobalDataHelper::get(), &cdsem::CDSEM_GlobalData::tmpAmpChanged, this, [this](QPointer<QObject> obj, void* pPtr){
        dataHelper->notifyAmpChanged(obj, pPtr);
    });

    connect(cdsem::CDSEM_GlobalDataHelper::get(), &cdsem::CDSEM_GlobalData::ampWindowStatusChanged, this, [this](){
        if (!cdsem::CDSEM_GlobalDataHelper::get()->getAmpWindowStatus()) {
            dataHelper->stopAutoMeasure();
            dataHelper->btnRemoveGreen(ui->ampBtn->getQPushButton());
        } else {
            dataHelper->btnToGreen(ui->ampBtn->getQPushButton());
        }
    });
    connect(uiHelper->lockCursorMoveBtn, &QPushButton::toggled, this, [this](bool checked){
        if (checked)
            uiHelper->lockCursorMoveBtn->setIcon(QIcon(":/icon/free_move_selected.png"));
        else
            uiHelper->lockCursorMoveBtn->setIcon(QIcon(":/icon/free_move.png"));
    });
    connect(uiHelper->cursorToolViewBtn, &QPushButton::clicked, this, [this](){
        dataHelper->canvasAcdPaneStatus = !dataHelper->canvasAcdPaneStatus;
        refreshCanvasCursorToolContent();
    });

    connect(uiHelper->sortBtn, &QPushButton::clicked, this, [this](){
        dataHelper->sortAmpCursor();
    });
    connect(uiHelper->gatherToCenterBtn, &QPushButton::clicked, this, [this](){
        dataHelper->allAmpCursorToCenter();
    });
    connect(uiHelper->lockCursorMoveBtn, &QPushButton::clicked, this, [this](bool checked){
        dataHelper->lockAcdCursorsMouse(checked);
    });

    CSender sender(this);
    connect(CDSEM_ControlHelper::instance()->getControl(), &CDSEM_Control::magPreparedToSend, this, [this, sender](CSender oSender, int mag){
        Q_UNUSED(mag)
        dataHelper->stopAutoMeasure();
        if (!ui->blankBtn->isChecked()) {
            ui->blankBtn->setChecked(true);
        }
        if (IOPT->imageIsFreezen()) {
            CDSEM_ControlHelper::instance()->grabImage(sender);
        }
    }, Qt::QueuedConnection);

    connect(CDSEM_ControlHelper::instance()->getControl(), &CDSEM_Control::beamRotPreparedToSend, this, [this, sender](CSender oSender, float){

        dataHelper->stopAutoMeasure();
        if (!ui->blankBtn->isChecked()) {
            ui->blankBtn->setChecked(true);
        }
        if (IOPT->imageIsFreezen()) {
            CDSEM_ControlHelper::instance()->grabImage(sender);
        }
    }, Qt::QueuedConnection);

    connect(CDSEM_ControlHelper::instance()->getControl(), &CDSEM_Control::beamOffsetXPreparedToSend, this, [this, sender](CSender oSender, float){
        if (oSender != ui->locateCrossBtn) return;
        if (IOPT->getImageFreezeStatus() != IFS_LIVE) {
            CDSEM_ControlHelper::instance()->grabImage(sender);
        }
    }, Qt::QueuedConnection);

    connect(&STAGE_ENV, &CDSEM_GlobalStagePyParamModel::stageBusyStatusChanged, this, [this, sender](){
        if (!STAGE_ENV.isStageBusyStatus() && dataHelper->needGrabImageForStageOver) {
            CDSEM_ControlHelper::instance()->grabImage(sender);
        }
    }, Qt::QueuedConnection);

    connect(CDSEM_ControlHelper::instance()->getControl(), &CDSEM_Control::mixStageMovePreparedToSend, this, [this, sender](CSender oSender){
        if (oSender != ui->locateCrossBtn || IOPT->getImageOperationType() != IMG_OPT_TYPE_SEM) return;
        if (IOPT->getImageFreezeStatus() != IFS_LIVE) {
            dataHelper->needGrabImageForStageOver = true;
        }
    }, Qt::QueuedConnection);

    connect(IOPT, &ImageOperationDataModel::beamRotChanged, this, [this](){
        dataHelper->refreshRot();
    });
    connect(IOPT, &ImageOperationDataModel::omRotChanged, this, [this](){
        dataHelper->refreshRot();
    });

    connect(IOPT, &ImageOperationDataModel::semMagChanged, this, [this](){
        renderMag(IOPT->getSemMag());
    });

    cdsem::CDSEM_GlobalDataHelper::get()->registImageArrivedCallback(IMG_OPT_TYPE_IM, IMG_OPT_BUSINESS_TYPE_VIEW_WINDOW_, [this]() {
        dataHelper->rewriteImage(IMG_OPT_TYPE_IM);
    });
    cdsem::CDSEM_GlobalDataHelper::get()->registImageArrivedCallback(IMG_OPT_TYPE_SEM, IMG_OPT_BUSINESS_TYPE_VIEW_WINDOW_, [this]() {
        dataHelper->writeSemImage();
    });
    cdsem::CDSEM_GlobalDataHelper::get()->registImageArrivedCallback(IMG_OPT_TYPE_OM, IMG_OPT_BUSINESS_TYPE_VIEW_WINDOW_, [this]() {
        dataHelper->writeOmImage();
    });

    QObject::connect(imageStream(), &StreamImageSupplier::rewriteImage, this, [this](){
        cdsem::CDSEM_GlobalDataHelper::get()->invokeImage([this]() {
            dataHelper->rewriteImage();
        });
        if (dataHelper->needGrabImageForStageOver) dataHelper->needGrabImageForStageOver = false;
    }, Qt::QueuedConnection);

    connect(ui->nextBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        CDSEM_ControlHelper::instance()->cmdStageInit(this);
    });

    connect(SignalHubSingleObjectManager::instance()->getSignalObject(), &SignalHubObjectSMObject::objectChanged, this, [this](const QString& id, bool opened){
        if (image_optics::isSignalOpticsWindow(id)) {
            if (opened)
                dataHelper->btnToGreen(ui->opticsBtn->getQPushButton());
            else
                dataHelper->btnRemoveGreen(ui->opticsBtn->getQPushButton());
        }
    });

    connect(cdsem::CDSEM_GlobalDataHelper::get(), &cdsem::CDSEM_GlobalData::procedureRunningStatusChanged, this, [this]() {
        dataHelper->setProcedureRunningStatus();
    });
}

void ImageOperationController::triggerAction() {

    if (uiHelper && uiHelper->canvas) {
        QObject::connect(imageStream(), &StreamImageSupplier::rewriteImage, this, [this](){

        }, Qt::QueuedConnection);
    }
    emit IOPT->imageOperationTypeChanged();
    if (cdsem::CDSEM_GlobalDataHelper::get()->getAmpWindowStatus()) {
        dataHelper->btnToGreen(ui->ampBtn->getQPushButton());
    }
    dataHelper->setProcedureRunningStatus();
}
