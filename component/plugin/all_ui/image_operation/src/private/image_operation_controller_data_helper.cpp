#include "image_operation_controller_data_helper.h"
#include "image_operation_controller.h"
#include "image_measurement_rect_tool.h"
#include "image_measurement_double_rect_tool.h"
#include "image_measurement_distance_tool.h"
#include "image_measurement_annulus_tool.h"
#include "sem_view_common.h"
#include "cde90_button.h"
#include "cde90_label.h"
#include "cdsem_global_data.h"
#include "image_operation_widget_canvas.h"
#include "width_cursor_handler.h"
#include "width_roughness_cursor_handler.h"
#include "edge_roughness_cursor_handler.h"
#include "diameter_hole_cursor_handler.h"
#include "ellipse_cursor_handler.h"
#include "stream_image_supplier.h"
#include "double_background_color_animation.h"
#include "sem_common_data_model.h"
#include "cde90_message_box.h"
#include "sem_global.h"
#include "param_horizontal_slider.h"
#include "cde90_text_field.h"
#include "cdsem_control_helper.h"
#include "image_operation_data_model.h"
#include "image_measurement_cross_tool.h"
#include "image_operation_controller_ui_helper.h"
#include "ui_image_operation_controller.h"
#include "image_type_defition.h"
#include <QStyle>
#include "image_operation_custom_setter_controller.h"
#include "image_operation_view_setter_controller.h"

#define uiHelper d->uiHelper
#define ui d->ui

class ImageMeasurementLocateCrossTool : public ImageMeasurementCrossTool {
private:
    std::function<void()> mouseReleaseCallback;
public:
    explicit ImageMeasurementLocateCrossTool(
        ImageOperationCanvas* canvas)
        : ImageMeasurementCrossTool(canvas) {
        stayImageCenter(true, false);
    }
    ~ImageMeasurementLocateCrossTool() noexcept {}

    void mouseEvent(
        ImageMouseEvent& event) override {
        if (event.btnStatus == IMBS_PRESSED && event.btn == Qt::LeftButton) {
            setCenter(event.imgPxPos);
        }
        ImageMeasurementCrossTool::mouseEvent(event);
        event.cursor = Qt::ArrowCursor;
        if (event.btn == Qt::LeftButton && event.btnStatus == IMBS_RELEASED && mouseReleaseCallback) {
            mouseReleaseCallback();
        }
    }

    void setMouseReleaseCallback(const std::function<void()>& callback) {
        mouseReleaseCallback = callback;
    }
};

CursorHandler* ImageOperationControllerDataHelper::getCursorHandler() {
    cdsem::MeasureType mType = cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpMeasureType();
    if (mType == cdsem::MT_WIDTH) {
        return widthCursorHandler;
    } else if (mType == cdsem::MT_WIDTH_ROUGHNESS) {
        return widthRoughnessCursorHandler;
    } else if (mType == cdsem::MT_EDGE_ROUGHNESS) {
        return edgeRoughnessCursorHandler;
    } else if (mType == cdsem::MT_DIAMETER_HOLE) {
        return diameterHoleCursorHandler;
    } else if (mType == cdsem::MT_ELLIPSE) {
        return ellipseCursorHandler;
    }
    return nullptr;
}

void ImageOperationControllerDataHelper::amsBtnToYellow() {
    addCssClass(ui->amsBtn->getQPushButton(), "YellowBg");
    removeCssClass(ui->amsBtn->getQPushButton(), "GreenBg");
    ui->amsBtn->getQPushButton()->style()->unpolish(ui->amsBtn->getQPushButton());
    ui->amsBtn->getQPushButton()->style()->polish(ui->amsBtn->getQPushButton());
}
void ImageOperationControllerDataHelper::amsBtnToGreen() {
    addCssClass(ui->amsBtn->getQPushButton(), "GreenBg");
    removeCssClass(ui->amsBtn->getQPushButton(), "YellowBg");
    ui->amsBtn->getQPushButton()->style()->unpolish(ui->amsBtn->getQPushButton());
    ui->amsBtn->getQPushButton()->style()->polish(ui->amsBtn->getQPushButton());
}
void ImageOperationControllerDataHelper::amsBtnToNormal() {
    removeCssClass(ui->amsBtn->getQPushButton(), "YellowBg");
    removeCssClass(ui->amsBtn->getQPushButton(), "GreenBg");
    ui->amsBtn->getQPushButton()->style()->unpolish(ui->amsBtn->getQPushButton());
    ui->amsBtn->getQPushButton()->style()->polish(ui->amsBtn->getQPushButton());
}

void ImageOperationControllerDataHelper::btnToGreen(QPushButton* btn) {
    addCssClass(btn, "GreenBg");
    btn->style()->unpolish(btn);
    btn->style()->polish(btn);
}

void ImageOperationControllerDataHelper::btnRemoveGreen(QPushButton* btn) {
    removeCssClass(btn, "GreenBg");
    btn->style()->unpolish(btn);
    btn->style()->polish(btn);
}

void ImageOperationControllerDataHelper::initDoubleBoxBeginPos() {
    d->dataHelper->beginPoints.clear();
    d->dataHelper->beginPoints.reserve(ampCursors.doubleBoxTools.size());
    ImageMeasurementDoubleRectTool* r;
    for (ImageMeasurementTool* & t : ampCursors.doubleBoxTools) {
        r = static_cast<ImageMeasurementDoubleRectTool*>(t);
        d->dataHelper->beginPoints.append(r->getLeftTopPos());
    }
}

void ImageOperationControllerDataHelper::initOneBoxBeginPos() {
    d->dataHelper->beginPoints.clear();
    d->dataHelper->beginPoints.reserve(ampCursors.oneBoxTools.size());
    ImageMeasurementRectTool* r;
    for (ImageMeasurementTool* & t : ampCursors.oneBoxTools) {
        r = static_cast<ImageMeasurementRectTool*>(t);
        d->dataHelper->beginPoints.append(r->getPos());
    }
}

void ImageOperationControllerDataHelper::initAnnualBoxBeginPos() {
    d->dataHelper->beginPoints.clear();
    d->dataHelper->beginPoints.reserve(ampCursors.annulusTools.size());
    ImageMeasurementAnnulusTool* r;
    for (ImageMeasurementTool* & t : ampCursors.annulusTools) {
        r = static_cast<ImageMeasurementAnnulusTool*>(t);
        d->dataHelper->beginPoints.append(r->getCenterPos());
    }
}

void ImageOperationControllerDataHelper::removeAllAmpCursor(QList<ImageMeasurementTool*>& list) {
    if (list.isEmpty()) return;
    for (ImageMeasurementTool* & t : list) {
        d->removeMeasurementTool(t);
        delete t;
    }
    list.clear();
}

void ImageOperationControllerDataHelper::removeAllAmpCursor() {
    removeAllAmpCursor(ampCursors.doubleBoxTools);
    removeAllAmpCursor(ampCursors.oneBoxTools);
    removeAllAmpCursor(ampCursors.distanceTools);
    removeAllAmpCursor(ampCursors.annulusTools);
    ampCursors.currentTools = nullptr;
}

void ImageOperationControllerDataHelper::exchangeCurrentAmpCursor(QList<ImageMeasurementTool*>* targetTools) {
    if (ampCursors.currentTools == nullptr || ampCursors.currentTools != targetTools) {
        if (ampCursors.currentTools != nullptr) {
            for (ImageMeasurementTool* & t : *ampCursors.currentTools) {
                d->removeMeasurementTool(t);
            }
        }
        for (ImageMeasurementTool* & t : *targetTools) {
            //remove的目的是重新洗牌，因为此时并不知晓target中哪些元素存在于canvas的toolsList中。
            d->removeMeasurementTool(t);
            d->addMeasurementTool(t);
        }
        if (targetTools->size() > 1) {
            d->measurementToolToBehindFloor(targetTools->first(), targetTools->last());
        }
        ampCursors.currentTools = targetTools;
    }
}

void ImageOperationControllerDataHelper::moveDoubleBoxTool(int cursorDirection, bool moving) {
    if (ampCursors.doubleBoxTools.size() <= 1 || beginPoints.size() <= 1) {
        return;
    }
    ImageMeasurementDoubleRectTool* firstTool = static_cast<ImageMeasurementDoubleRectTool*>(ampCursors.doubleBoxTools.first());
    const MeasurementPoint& pos = beginPoints.first();
    const MeasurementPoint& firstCurrPos = firstTool->getLeftTopPos();
    if (pos.x == firstCurrPos.x && pos.y == firstCurrPos.y) return;
    if (moving) {
        MeasurementNumber diff;
        if (cursorDirection == 0) {
            diff = firstCurrPos.x - pos.x;
        } else {
            diff = firstCurrPos.y - pos.y;
        }
        ImageMeasurementDoubleRectTool* r;
        for (int i = 1; i < ampCursors.doubleBoxTools.size(); ++ i) {
            if (i >= beginPoints.size()) {
                break;
            }
            const MeasurementPoint& oldPos = beginPoints[i];
            r = static_cast<ImageMeasurementDoubleRectTool*>(ampCursors.doubleBoxTools[i]);
            r->blockSignals(true);
            if (cursorDirection == 0) {
                r->move({oldPos.x + diff, oldPos.y}, false);
            } else {
                r->move({oldPos.x, oldPos.y + diff}, false);
            }
            r->blockSignals(false);
        }
    } else {
        MeasurementNumber diffX = firstCurrPos.x - pos.x;
        MeasurementNumber diffY = firstCurrPos.y - pos.y;
        ImageMeasurementDoubleRectTool* r;
        for (int i = 1; i < ampCursors.doubleBoxTools.size(); ++ i) {
            if (i >= d->dataHelper->beginPoints.size()) {
                break;
            }
            const MeasurementPoint& oldPos = beginPoints[i];
            r = static_cast<ImageMeasurementDoubleRectTool*>(ampCursors.doubleBoxTools[i]);
            r->blockSignals(true);
            r->move({oldPos.x + diffX, oldPos.y + diffY}, false);
            r->blockSignals(false);
        }
    }
}

void ImageOperationControllerDataHelper::moveOneBoxTool() {
    if (ampCursors.oneBoxTools.size() <= 1 || beginPoints.size() <= 1) {
        return;
    }
    ImageMeasurementRectTool* firstTool = static_cast<ImageMeasurementRectTool*>(ampCursors.oneBoxTools.first());
    const MeasurementPoint& pos = beginPoints.first();
    const MeasurementPoint& firstCurrPos = firstTool->getPos();
    MeasurementNumber diffX = firstCurrPos.x - pos.x;
    MeasurementNumber diffY = firstCurrPos.y - pos.y;
    ImageMeasurementRectTool* r;
    for (int i = 1; i < ampCursors.oneBoxTools.size(); ++ i) {
        if (i >= beginPoints.size()) {
            break;
        }
        const MeasurementPoint& oldPos = beginPoints[i];
        r = static_cast<ImageMeasurementRectTool*>(ampCursors.oneBoxTools[i]);
        r->blockSignals(true);
        r->move({oldPos.x + diffX, oldPos.y + diffY}, false);
        r->blockSignals(false);
    }
}

void ImageOperationControllerDataHelper::moveAnnualTool() {
    if (ampCursors.annulusTools.size() <= 1 || beginPoints.size() <= 1) {
        return;
    }
    ImageMeasurementAnnulusTool* firstTool = static_cast<ImageMeasurementAnnulusTool*>(ampCursors.annulusTools.first());
    const MeasurementPoint& pos = beginPoints.first();
    const MeasurementPoint& firstCurrPos = firstTool->getCenterPos();
    MeasurementNumber diffX = firstCurrPos.x - pos.x;
    MeasurementNumber diffY = firstCurrPos.y - pos.y;
    ImageMeasurementAnnulusTool* r;
    for (int i = 1; i < ampCursors.annulusTools.size(); ++ i) {
        if (i >= beginPoints.size()) {
            break;
        }
        const MeasurementPoint& oldPos = beginPoints[i];
        r = static_cast<ImageMeasurementAnnulusTool*>(ampCursors.annulusTools[i]);
        r->blockSignals(true);
        r->moveCenter({oldPos.x + diffX, oldPos.y + diffY}, false);
        r->blockSignals(false);
    }
}

void ImageOperationControllerDataHelper::recordAssistCenterPos() {
    // 安全检查：确保有足够的工具（需要水平和垂直两个方向）
    if (ampCursors.distanceTools.size() < 2) {
        return;
    }

    // 获取水平距离工具（第一个）
    ImageMeasurementDistanceTool* dToolH = dynamic_cast<ImageMeasurementDistanceTool*>(
        ampCursors.distanceTools.first());

    // 获取垂直距离工具（最后一个）
    ImageMeasurementDistanceTool* dToolV = dynamic_cast<ImageMeasurementDistanceTool*>(
        ampCursors.distanceTools.last());

    // 检查指针有效性
    if (!dToolH || !dToolV) {
        return;
    }

    // 获取垂直方向的两个线位置，取较小的作为起始位置
    const MeasurementNumber *lv, *lh;
    if (dToolV->getLine1() < dToolV->getLine2()) {
        lv = &dToolV->getLine1();
    } else {
        lv = &dToolV->getLine2();
    }

    // 获取水平方向的两个线位置，取较小的作为起始位置
    if (dToolH->getLine1() < dToolH->getLine2()) {
        lh = &dToolH->getLine1();
    } else {
        lh = &dToolH->getLine2();
    }

    // 记录辅助范围：起始点(水平起始, 垂直起始) 和 尺寸(水平距离, 垂直距离)
    assistRange = {{*lh, *lv}, {dToolH->getDistance(), dToolV->getDistance()}};
}

void ImageOperationControllerDataHelper::clearAssistCenterPos() {
    assistRange = {};
}

void ImageOperationControllerDataHelper::amsNextStep() {
    cdsem::AMS_Status amsStatus = cdsem::CDSEM_GlobalDataHelper::get()->getAmsStatus();
    if (amsStatus == cdsem::AMS_SOFF || amsStatus == cdsem::AMS_SASSIST) {
        markMeasureCursor();
        lockAcdCursorsMouse(uiHelper->lockCursorMoveBtn->isChecked());
        amsBtnToYellow();
    } else if (amsStatus == cdsem::AMS_SCURSOR) {
        ui->amsBtn->setDisabled(true);
        startAutoMeasure([this](const OperationResult& result){
            if (result.errCode) {
                simpleMessageBox(d, "Error", result.errMsg);
            }
            ui->amsBtn->setDisabled(false);
        });
        amsBtnToGreen();
    } else if (amsStatus == cdsem::AMS_SMEASURE) {
        overAutoMeasure();
        amsBtnToNormal();
    }
}

void ImageOperationControllerDataHelper::markWidthLineTools() {
    if (ampCursors.distanceTools.size() > 0) {
        return;
    }
    QSize oSize = uiHelper->canvas->imageStream()->currentImageSize();
    int w, h;
    if (oSize.isNull()) {
        w = uiHelper->canvas->canvasWidth();
        h = uiHelper->canvas->canvasHeight();
    } else {
        w = oSize.width();
        h = oSize.height();
    }
    ImageMeasurementDistanceTool* dTool;
    for (int i = 0; i < 2; ++ i) {
        dTool = new ImageMeasurementDistanceTool(uiHelper->canvas);
        dTool->setLineWeight(2, false);
        dTool->setColor(QColor(255, 255, 255, 125), false);
        dTool->setLineStyle(MTBS_SOLID, false);
        dTool->setOverLine(MTBOP_CENTER, false);
        if (i == 0) {
            dTool->setLine1(w / 3, false);
            dTool->setLine2(w - w / 3);
            dTool->setOrientation(Qt::Horizontal);
        } else {
            dTool->setLine1(h / 3, false);
            dTool->setLine2(h - h / 3);
            dTool->setOrientation(Qt::Vertical);
        }
        dTool->setLeftTopMargin(cdsem::AMP_CURSOR_MARGIN);
        dTool->setRightBottomMargin(cdsem::AMP_CURSOR_MARGIN);
        d->addMeasurementTool(dTool);
        ampCursors.distanceTools.append(dTool);
    }
    d->update();
}

bool ImageOperationControllerDataHelper::ampCursorSortFunc(const MeasurementPoint& o1, const MeasurementPoint& o2) {
    if (o1.y < o2.y) {
        return true;
    } else if (o1.y > o2.y) {
        return false;
    }
    return o1.x < o2.x;
}

//TODO 修改点2
void ImageOperationControllerDataHelper::markMeasureCursor() {
    d->changeIconLabel();
    cdsem::MeasureType mType = cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpMeasureType();
    if (mType == cdsem::MT_WIDTH) {
        widthCursorHandler->markWidthCursors();
    } else if (mType == cdsem::MT_WIDTH_ROUGHNESS) {
        widthRoughnessCursorHandler->markWidthCursors();
    } else if (mType == cdsem::MT_EDGE_ROUGHNESS) {
        edgeRoughnessCursorHandler->markEdgeCursors();
    } else if (mType == cdsem::MT_DIAMETER_HOLE) {
        diameterHoleCursorHandler->markDiameterHoleCursors();
    } else if (mType == cdsem::MT_ELLIPSE) {
        ellipseCursorHandler->markEdgeCursors();
    }
}

//TODO 修改点3
void ImageOperationControllerDataHelper::startAutoMeasure(const sem::ONE_PARAM_ARG_FUNC<const OperationResult&>& callback) {
    cdsem::CDSEM_GlobalDataHelper::get()->setAmsStatus(cdsem::AMS_SMEASURE);
    CursorHandler* cursorHandler = getCursorHandler();
    if (cursorHandler == nullptr) return;
    cursorHandler->startAutoMeasure(callback);
}

void ImageOperationControllerDataHelper::overAutoMeasure() {
    if (ampCursors.results.size() > 0) {
        for (auto it = ampCursors.results.begin(); it != ampCursors.results.end(); ++ it) {
            d->removeMeasurementTool(*it);
        }
        qDeleteAll(ampCursors.results);
        ampCursors.results.clear();
    }
    uiHelper->canvas->clearProfileLines();
    uiHelper->canvasToolWidget->setVisible(false);
    clearAssistCenterPos();
    cdsem::CDSEM_GlobalDataHelper::get()->setAmsStatus(cdsem::AMS_SOFF);
    if (!ui->vLineBtn->isChecked() && !ui->hLineBtn->isChecked()) {
        ui->resultLabel->clear();
    }
    resultText = "";
}

void ImageOperationControllerDataHelper::stopAutoMeasure() {
    amsBtnToNormal();
    removeAllAmpCursor();
    overAutoMeasure();
}

bool ImageOperationControllerDataHelper::collectCenters() {
    CursorHandler* cursorHandler = getCursorHandler();
    if (cursorHandler == nullptr) return false;
    cursorHandler->collectCenters();
    return true;
}

//修改点5
void ImageOperationControllerDataHelper::lockAcdCursorsMouse(bool lock) {
    CursorHandler* cursorHandler = getCursorHandler();
    if (cursorHandler == nullptr) return;
    cursorHandler->lockAcdCursorsMouse(lock);
}

//修改点6
void ImageOperationControllerDataHelper::sortAmpCursor() {
    CursorHandler* cursorHandler = getCursorHandler();
    if (cursorHandler == nullptr) return;
    cursorHandler->sortCursors();
}

//修改点4
void ImageOperationControllerDataHelper::allAmpCursorToCenter() {
    CursorHandler* cursorHandler = getCursorHandler();
    if (cursorHandler == nullptr) return;
    cursorHandler->allAmpCursorToCenter();
}


//TODO 修改点1
void ImageOperationControllerDataHelper::notifyAmpChanged(QPointer<QObject> obj, void* pPtr) {
    if ((cdsem::CDSEM_GlobalDataHelper::get()->getAmsStatus() != cdsem::AMS_SCURSOR && cdsem::CDSEM_GlobalDataHelper::get()->getAmsStatus() != cdsem::AMS_SASSIST) || obj == d) return;
    if (pPtr == nullptr || pPtr == &cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpMeasureType()) {
        stopAutoMeasure();
        return;
    }

    if (pPtr == &cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpMeasureObject())
        d->changeIconLabel();
    cdsem::AMS_Status amsStatus = cdsem::CDSEM_GlobalDataHelper::get()->getAmsStatus();
    if (amsStatus == cdsem::AMS_SCURSOR || amsStatus == cdsem::AMS_SASSIST) {
        CursorHandler* cursorHandler = getCursorHandler();
        if (cursorHandler == nullptr) return;
        cursorHandler->notifyAmpChanged(pPtr);
    }
}

void ImageOperationControllerDataHelper::setWorkDistanceFromSlider(int v) {
    CDSEM_ControlHelper::instance()->workDistance(d, float(v) / 10000 / 1000);
}

void ImageOperationControllerDataHelper::setWorkDistanceFromTextField(const QString& text) {
    float v = text.toFloat();
    CDSEM_ControlHelper::instance()->workDistance(d, v / 1000);
    uiHelper->focusSlider->setValue(qRound(v * 10000));
}

void ImageOperationControllerDataHelper::refreshRot() {
    int ot = IOPT->getImageOperationType();
    cdsem::PhotoImageType it = IOPT->getPhotoImageType();
    FloatNumber1 n;
    if (ot == IMG_OPT_TYPE_SEM || (ot == IMG_OPT_TYPE_IM && it == cdsem::PIT_SEM)) {
        n = IOPT->getBeamRot();
    } else {
        n = IOPT->getOmRot();
    }
}

void ImageOperationControllerDataHelper::reloadData() {
    d->refreshScanParamLabel();
}

void ImageOperationControllerDataHelper::switchShiftFieldModel(int s) {
    if (s == 0) {
        d->localShiftMode = s;
        d->clearTool();
        return;
    }
    if (s != 1 && s != 2) return;
    ImageMeasurementLocateCrossTool* crossTool = nullptr;
    if (uiHelper->currentTool) {
        crossTool = dynamic_cast<ImageMeasurementLocateCrossTool*>(uiHelper->currentTool);
        if (!crossTool) {
            d->clearTool();
        }
    }
    d->localShiftMode = s;
    if (!crossTool) {
        crossTool = new ImageMeasurementLocateCrossTool(uiHelper->canvas);
        crossTool->setMouseReleaseCallback([crossTool, this](){
            MeasurementPoint pos = crossTool->getCenterOffset();
            qDebug() << "pos offset: " << pos.x.toQStringAndKeepFullDecimal() << ", " << pos.y.toQStringAndKeepFullDecimal();
            if (d->localShiftMode == 1) {
                QPointF beamPos = SEM_PY_ENV.getBeamPos();
                CDSEM_ControlHelper::instance()->beamOffsetX(ui->locateCrossBtn, beamPos.x() - cdsem::PixelDistanceConvert::instance()->getMSizeFromPixel(pos.x.justRemoveDecimalPointTo32()).toDouble());
                CDSEM_ControlHelper::instance()->beamOffsetY(ui->locateCrossBtn, beamPos.y() - cdsem::PixelDistanceConvert::instance()->getMSizeFromPixel(pos.y.justRemoveDecimalPointTo32()).toDouble());
            } else if (d->localShiftMode == 2){
                if (IOPT->getImageOperationType() == IMG_OPT_TYPE_OM) {
                    QPointF target = STAGE_ENV.getMovingStagePos();
                    QPointF viewPos(pos.x.justRemoveDecimalPointTo32(),
                                    pos.y.justRemoveDecimalPointTo32());
                    viewPos = cdsem::CDSEM_GlobalDataHelper::get()->viewShiftOMToStage(viewPos);
                    target.setX(target.x() - viewPos.x());
                    target.setY(target.y() - viewPos.y());
                    MoveStageParam param = cdsem::CDSEM_GlobalDataHelper::get()
                                               ->getMoveStageParam();
                    param.x = target.x();
                    param.y = target.y();
                    CDSEM_ControlHelper::instance()->moveStage(ui->locateCrossBtn, param);
                } else if (IOPT->getImageOperationType() == IMG_OPT_TYPE_SEM) {
                    QPointF target = STAGE_ENV.getMovingStagePos();
                    QPointF viewPos(cdsem::PixelDistanceConvert::instance()
                                        ->getMSizeFromPixel(pos.x.justRemoveDecimalPointTo32())
                                        .toDouble(),
                                    cdsem::PixelDistanceConvert::instance()
                                        ->getMSizeFromPixel(pos.y.justRemoveDecimalPointTo32())
                                        .toDouble());
                    viewPos = cdsem::CDSEM_GlobalDataHelper::get()->viewShiftToStage(viewPos);
                    target.setX(target.x() - viewPos.x());
                    target.setY(target.y() - viewPos.y());
                    MoveStageParam param = cdsem::CDSEM_GlobalDataHelper::get()
                                               ->getMoveStageParam();
                    param.x = target.x();
                    param.y = target.y();
                    CDSEM_ControlHelper::instance()->moveStage(ui->locateCrossBtn, param);
                }
            }

            crossTool->blockSignals(true);
            crossTool->toCurrImgCenter();
            crossTool->blockSignals(false);
        });
        crossTool->setWeight(2, false);
        crossTool->setColor(QColor(255, 255, 255, 125), false);
        crossTool->toScreenMode(true, false);
        crossTool->toCurrImgCenter();
        uiHelper->currentTool = crossTool;
        d->addMeasurementTool(crossTool);
        uiHelper->currentTool->update();
    }
    switchShiftFieldBtnView(s);
}
void ImageOperationControllerDataHelper::switchShiftFieldBtnView(int s) {
    if (s == 2) {
        ui->locateCrossBtn->setStyleSheet(
            "QPushButton {image: url(':/icon/move_stage.png');}"
            "QPushButton:disabled {image: url(':/icon/move_stage_disabled.png');}"
            );
    } else {
        ui->locateCrossBtn->setToolTip("Stage Move Tool");

        ui->locateCrossBtn->setStyleSheet(
            "QPushButton {image: url(':/icon/beam_shift.png');}"
            "QPushButton:disabled {image: url(':/icon/beam_shift_disabled.png');}"
            );
        ui->locateCrossBtn->setToolTip("Beam Shift Tool");
    }
}

void ImageOperationControllerDataHelper::setProcedureRunningStatus() {
    bool b = cdsem::CDSEM_GlobalDataHelper::get()->procedureRunningStatus() == cdsem::PRS_RUNNING;
}

void ImageOperationControllerDataHelper::setAutoFocusBtnStatus(bool b) {
    ui->autoFocusBtn->getQPushButton()->setDisabled(b);

    ui->focusTextField->getQLineEdit()->setDisabled(b);
    ui->focusUpBtn->getQPushButton()->setDisabled(b);
    ui->focusDownBtn->getQPushButton()->setDisabled(b);
    uiHelper->focusSlider->setDisabled(b);
    if (b) {
        addCssClass(ui->autoFocusBtn->getQPushButton(), "YellowBg");
        ui->autoFocusBtn->getQPushButton()->style()->unpolish(ui->autoFocusBtn->getQPushButton());
        ui->autoFocusBtn->getQPushButton()->style()->polish(ui->autoFocusBtn->getQPushButton());
    } else {
        removeCssClass(ui->autoFocusBtn->getQPushButton(), "YellowBg");
        ui->autoFocusBtn->getQPushButton()->style()->unpolish(ui->autoFocusBtn->getQPushButton());
        ui->autoFocusBtn->getQPushButton()->style()->polish(ui->autoFocusBtn->getQPushButton());
    }
}

void ImageOperationControllerDataHelper::writeSemImage() {
    const ImageAddtionDataModel* addition = cdsem::CDSEM_GlobalDataHelper::get()->getImageAddition(IMG_OPT_TYPE_SEM);
    if (addition->imageType == cdsem::SIT_PHOTO) {
        MainThreadInvoker::getInvoker().runLater([this](){
            if (d->dataHelper->needGrabImageForStageOver) d->dataHelper->needGrabImageForStageOver = false;
            d->isTakingPhoto = false;
        });
    }
    if (IOPT->getImageOperationType() != IMG_OPT_TYPE_SEM) return;
    MainThreadInvoker::getInvoker().runLater([](){
        IOPT->setAptNum(SEM_PY_ENV.getAptNum());
        IOPT->setIsProbe(SEM_PY_ENV.isOptIProbe());
        IOPT->setIProbe(SEM_PY_ENV.getIProbe());
        IOPT->setEHT(SEM_PY_ENV.getEHT());
        IOPT->setScanRate(SEM_PY_ENV.getScanRate());
        IOPT->setFrame(SEM_PY_ENV.getFrame());
        IOPT->setPixelSize(SEM_PY_ENV.getPixelSize());
    });
    const cdsem::TmpImageData* data;
    cdsem::CDSEM_GlobalDataHelper::get()->getImage(IMG_OPT_TYPE_SEM, data);
    //writeImageFromSupplier设计上就允许在单一子线程上调用，而imageOperationController就是为TCP的图像流提供服务的
    d->imageStream()->writeImageFromSupplier(*data, addition->imageType);
}

void ImageOperationControllerDataHelper::writeOmImage() {
    if (IOPT->getImageOperationType() != IMG_OPT_TYPE_OM) return;
    const cdsem::TmpImageData* data;
    cdsem::CDSEM_GlobalDataHelper::get()->getImage(IMG_OPT_TYPE_OM, data);
    const ImageAddtionDataModel* addition = cdsem::CDSEM_GlobalDataHelper::get()->getImageAddition(IMG_OPT_TYPE_OM);
    //writeImageFromSupplier设计上就允许在单一子线程上调用，而imageOperationController就是为TCP的图像流提供服务的
    d->imageStream()->writeImageFromSupplier(*data, addition->imageType);
    if (addition->imageType == cdsem::SIT_PHOTO) {
        MainThreadInvoker::getInvoker().runLater([this](){
            d->isTakingPhoto = false;
            if (d->dataHelper->needGrabImageForStageOver) d->dataHelper->needGrabImageForStageOver = false;
        });
    }
}

void ImageOperationControllerDataHelper::rewriteImage(int imageType) {
    int oType = imageType < 0 ? IOPT->getImageOperationType() : imageType;
    if (oType == IMG_OPT_TYPE_SEM) {
        const cdsem::TmpImageData* data = nullptr;
        cdsem::CDSEM_GlobalDataHelper::get()->getImage(IMG_OPT_TYPE_SEM, data);
        if (!data) return;
        const ImageAddtionDataModel* addition = cdsem::CDSEM_GlobalDataHelper::get()->getImageAddition(IMG_OPT_TYPE_SEM);
        cdsem::ScanImageType type = addition->imageType;
        d->imageStream()->writeImageFromSupplier(*data, type);

        MainThreadInvoker::getInvoker().runLater([addition](){
            IOPT->setEHT(addition->eht);
            IOPT->setPixelSize(addition->pixelSize);
            IOPT->setScanRate(addition->scanRate);
            IOPT->setAptNum(addition->aptNum);
            IOPT->setIsProbe(addition->optiProbe);
            IOPT->setIProbe(addition->iProbe);
            IOPT->setFrame(addition->frame);

            IOPT->setImageFreezeStatus(SEM_PY_ENV.getFreezeStatus());
            IOPT->setMinWorkDistance(SEM_PY_ENV.getMinWorkDistance());
            IOPT->setMaxWorkDistance(SEM_PY_ENV.getMaxWorkDistance());
            IOPT->setWorkDistance(SEM_PY_ENV.getWorkDistance());
            IOPT->setBeamRot(SEM_PY_ENV.getBeamRot());
            IOPT->setSemMag(SEM_PY_ENV.getMag());
        });
    } else if (oType == IMG_OPT_TYPE_OM) {
        const cdsem::TmpImageData* data = nullptr;
        cdsem::CDSEM_GlobalDataHelper::get()->getImage(IMG_OPT_TYPE_OM, data);
        if (!data) return;
        const ImageAddtionDataModel* addition = cdsem::CDSEM_GlobalDataHelper::get()->getImageAddition(IMG_OPT_TYPE_OM);
        cdsem::ScanImageType type = addition->imageType;
        d->imageStream()->writeImageFromSupplier(*data, type);
    } else if (oType == IMG_OPT_TYPE_IM) {
        const cdsem::TmpImageData* data = nullptr;
        cdsem::CDSEM_GlobalDataHelper::get()->getImage(IMG_OPT_TYPE_IM, data);
        if (!data) return;
        const ImageAddtionDataModel* addition = cdsem::CDSEM_GlobalDataHelper::get()->getImageAddition(IMG_OPT_TYPE_IM);
        cdsem::PhotoImageType type = addition->photoImagetype;
        d->imageStream()->writeImageFromSupplier(*data, addition->imageType);
        if (type == cdsem::PIT_SEM) {
            MainThreadInvoker::getInvoker().runLater([addition](){
                IOPT->setAptNum(addition->aptNum);
                IOPT->setIsProbe(addition->optiProbe);
                IOPT->setIProbe(addition->iProbe);
                IOPT->setEHT(addition->eht);
                IOPT->setScanRate(addition->scanRate);
                IOPT->setFrame(addition->frame);
                IOPT->setPixelSize(addition->pixelSize);
                IOPT->setMinWorkDistance(addition->minWorkDistance);
                IOPT->setMaxWorkDistance(addition->maxWorkDistance);
                IOPT->setWorkDistance(addition->workDistance);
                IOPT->setBeamRot(addition->imageRot);
                IOPT->setSemMag(addition->mag);
            });
        }
    }
}

void ImageOperationControllerDataHelper::imageOperationTypeChanged() {
    if (needGrabImageForStageOver) needGrabImageForStageOver = false;
    int type = IOPT->getImageOperationType();
    bool IS_OM = type == IMG_OPT_TYPE_OM || (type == IMG_OPT_TYPE_IM && IOPT->getPhotoImageType() == cdsem::PIT_OM);
    bool IS_SEM = type == IMG_OPT_TYPE_SEM || (type == IMG_OPT_TYPE_IM && IOPT->getPhotoImageType() == cdsem::PIT_SEM);
    bool IS_IM = type ==IMG_OPT_TYPE_IM;

    //必须保证设置顺序，否则窗口会被撑到不理想大小
    if (IS_OM) {
        ui->beamViewContent->setVisible(false);
    }
    if (IS_OM) {
        if (d->localShiftMode == 0) {
            switchShiftFieldBtnView(2);
        } else if (d->localShiftMode == 1) {
            switchShiftFieldModel(2);
        }
    }

    bool notSemDisable = type != IMG_OPT_TYPE_SEM;
    bool isImDisable = type == IMG_OPT_TYPE_IM;
    bool notSemImage = !IS_SEM;

    ui->enclosureBtn->setDisabled(notSemDisable);
    ui->focusBtn->setDisabled(notSemDisable);
    ui->nextBtn->setDisabled(notSemDisable);

    ui->autoFocusBtn->setDisabled(isImDisable);
    uiHelper->focusSlider->setDisabled(isImDisable);
    ui->focusTextField->setDisabled(isImDisable);
    ui->locateCrossBtn->setDisabled(isImDisable);

    ui->vLineBtn->setDisabled(notSemImage);
    ui->hLineBtn->setDisabled(notSemImage);
    ui->poundBtn->setDisabled(notSemImage);
    ui->ampBtn->setDisabled(notSemImage);
    ui->amsBtn->setDisabled(notSemImage);

    d->refreshScanParamLabel();

    if (uiHelper->viewSetter && uiHelper->viewSetter->isVisible()) uiHelper->viewSetter->close();
    if (uiHelper->customSetter && uiHelper->customSetter->isVisible()) uiHelper->customSetter->close();
    stopAutoMeasure();
    emit ui->blankBtn->toggled(ui->blankBtn->isChecked());
    if (!SEM_PY_ENV.isFreezen()) {
        CDSEM_ControlHelper::instance()->imageOffline(d);
    }

    cdsem::CDSEM_GlobalDataHelper::get()->invokeImage([this]() {
        rewriteImage();
    });
}