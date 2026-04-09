#include "diameter_hole_cursor_handler.h"
#include "image_measurement_double_rect_tool.h"
#include "image_measurement_annulus_tool.h"
#include "image_operation_widget_canvas.h"
#include "image_operation_controller_data_helper.h"
#include "image_operation_controller.h"
#include "cdsem_global_data.h"
#include "amp_data_model.h"
#include "cdsem_data_common.h"
#include "image_file_temp_handler.h"
#include "tmp_image_data_converter.h"
#include "sem_common_data_model.h"
#include "mat_measure.h"
#include "sem_global.h"
#include "image_save_model.h"
#include "image_operation_data_model.h"
#include "image_view_measure_tool_draw_helper.h"
#include "image_operation_controller_ui_helper.h"
#include "cde90_button.h"
#include <QPointer>

#define uiHelper dataHelper->d->uiHelper
bool cursorKindChanged(int lastKind, int currKind) {
    return lastKind / 2 != currKind / 2;
}

DiameterHoleCursorHandler::DiameterHoleCursorHandler(ImageOperationControllerDataHelper* dataHelper):dataHelper(dataHelper){}
DiameterHoleCursorHandler::~DiameterHoleCursorHandler() noexcept {}

void DiameterHoleCursorHandler::doAutoMeasure(const sem::ONE_PARAM_ARG_FUNC<const OperationResult&>& callback, const cdsem::TmpImageData& imageData, const ImageAddtionDataModel& addition) {
    if (imageData.data.isEmpty()) {
        MainThreadInvoker::getInvoker().runLater([callback](){
            OperationResult result;
            result.errCode = NULL_POINTER_ERR_CODE;
            result.errMsg = "Empty Image.";
            callback(result);
        });
        return;
    }
    QImage image;
    tmpToQImage(imageData, image);
    float pixelSize = addition.pixelSize;
    MainThreadInvoker::getInvoker().runLater([image, pixelSize, this, callback]() mutable{
        OperationResult result;
        sem::AfterGc gc([&callback, &result](){
            callback(result);
        });
        AmpDiameterHoleParam& diameterHoleParam = cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpDiameterHoleParam();
        collectCenters();
        if (diameterHoleParam.acdParam.centers.size() > 0) {
            ImageResultData data = HRMat::measureHoleImage(image, pixelSize, diameterHoleParam, {});
            qDebug() << "测量完毕";
            dataHelper->d->setMeasureResult(data);
            markDiameterHoleLineResultTools();
            dataHelper->removeAllAmpCursor();
        } else {
            dataHelper->stopAutoMeasure();
        }
    });
}

void DiameterHoleCursorHandler::collectCenters() {
    AmpDiameterHoleParam& diameterHoleParam = cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpDiameterHoleParam();
    QList<QPoint> centers;
    QPoint center;
    if (diameterHoleParam.kind == 0 || diameterHoleParam.kind == 1) {
        if (dataHelper->ampCursors.doubleBoxTools.isEmpty()) return;
        ImageMeasurementDoubleRectTool* d;
        centers.reserve(dataHelper->ampCursors.doubleBoxTools.size());
        for (ImageMeasurementTool* & t : dataHelper->ampCursors.doubleBoxTools) {
            d = static_cast<ImageMeasurementDoubleRectTool*>(t);
            const MeasurementPoint& leftPos = d->getLeftTopPos();
            const MeasurementPoint& rightPos = d->getRightBottomPos();
            const MeasurementSize& rightSize = d->getRightBottomSize();
            if (d->getOrientation() == Qt::Horizontal) {
                center.setX(leftPos.x + (rightPos.x + rightSize.width - leftPos.x) / 2);
                center.setY(rightPos.y + rightSize.height / 2);
            } else {
                center.setY(leftPos.y + (rightPos.y + rightSize.height - leftPos.y) / 2);
                center.setX(rightPos.x + rightSize.width / 2);
            }
            centers.append(center);
        }

    } else if (diameterHoleParam.kind == 2){
        if (dataHelper->ampCursors.annulusTools.isEmpty()) return;
        ImageMeasurementAnnulusTool* d;
        centers.reserve(dataHelper->ampCursors.annulusTools.size());
        for (ImageMeasurementTool* & t : dataHelper->ampCursors.annulusTools) {
            d = static_cast<ImageMeasurementAnnulusTool*>(t);
            const MeasurementPoint& p = d->getCenterPos();
            centers.append(QPoint(p.x.justRemoveDecimalPointTo32(), p.y.justRemoveDecimalPointTo32()));
        }
    } else {
        diameterHoleParam.acdParam.centers.clear();
        return;
    }
    diameterHoleParam.acdParam.centers = centers;
}

void DiameterHoleCursorHandler::markDiameterHoleCursors() {
    AmpDiameterHoleParam& param = cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpDiameterHoleParam();
    lastKind = param.kind;
    cdsem::AMS_Status amsStatus = cdsem::CDSEM_GlobalDataHelper::get()->getAmsStatus();
    if (amsStatus == cdsem::AMS_SOFF && param.measurePosition == 1) {
        markDiameterHoleLineTools();
        cdsem::CDSEM_GlobalDataHelper::get()->setAmsStatus(cdsem::AMS_SASSIST);
        return;
    }
    dataHelper->recordAssistCenterPos();
    int num = param.acdParam.acdType == cdsem::ACDT_MANUAL ? param.acdParam.target : 1;
    uiHelper->canvasToolWidget->setVisible(param.acdParam.acdType == cdsem::ACDT_MANUAL && num > 1);
    //双盒光标
    if (param.kind == 0 || param.kind == 1) {
        bool needExchange = dataHelper->ampCursors.doubleBoxTools.isEmpty();
        markDoubleBoxTools(param);
        if (needExchange) {
            dataHelper->exchangeCurrentAmpCursor(&dataHelper->ampCursors.doubleBoxTools);
        }
    } else if (param.kind == 2) {
        //环形光标
        bool needExchange = dataHelper->ampCursors.annulusTools.isEmpty();
        markAnnulusBoxTools(param);
        if (needExchange) {
            dataHelper->exchangeCurrentAmpCursor(&dataHelper->ampCursors.annulusTools);
        }
    }
    if (dataHelper->ampCursors.distanceTools.size() > 0) {
        dataHelper->removeAllAmpCursor(dataHelper->ampCursors.distanceTools);
    }
    cdsem::CDSEM_GlobalDataHelper::get()->setAmsStatus(cdsem::AMS_SCURSOR);
    dataHelper->d->update();
}

void DiameterHoleCursorHandler::markDoubleBoxTools(AmpDiameterHoleParam& diameterHoleParam) {
    int num = diameterHoleParam.acdParam.acdType == cdsem::ACDT_MANUAL ? diameterHoleParam.acdParam.target : 1;
    if (dataHelper->ampCursors.doubleBoxTools.size() < num) {
        num -= dataHelper->ampCursors.doubleBoxTools.size();
        MeasurementToolTextOption option;
        option.fontSize = 16;
        option.alignment = MTTA_LEFT_OR_TOP;
        option.padlt = 5;
        for (int i = 0 ; i < num; ++ i) {
            ImageMeasurementDoubleRectTool* measurementTool = dataHelper->d->createMeasurementDoubleRectTool();
            if (dataHelper->ampCursors.doubleBoxTools.isEmpty()) {
                QObject::connect(measurementTool, &ImageMeasurementDoubleRectTool::mouseMoveBegin, dataHelper->d, [this](){
                    dataHelper->d->dataHelper->moving = true;
                    dataHelper->initDoubleBoxBeginPos();
                });
                QObject::connect(measurementTool, &ImageMeasurementDoubleRectTool::mouseResizeBegin, dataHelper->d, [this](){
                    dataHelper->d->dataHelper->moving = false;
                    dataHelper->initDoubleBoxBeginPos();
                });
                QObject::connect(measurementTool, &ImageMeasurementDoubleRectTool::sizeChanged, dataHelper->d, [this, measurementTool](){
                    cdsem::MeasureType mType = cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpMeasureType();
                    if (mType != cdsem::MT_DIAMETER_HOLE) {
                        return;
                    }
                    AmpDiameterHoleParam& param = cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpDiameterHoleParam();
                    MeasurementNumber sumLine = measurementTool->getSumLine();
                    MeasurementNumber searchArea = measurementTool->getSearchArea();
                    param.sumLineOrInspectArea = sumLine;
                    emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(dataHelper->d), &param.sumLineOrInspectArea);
                    param.searchArea = searchArea;
                    ImageMeasurementDoubleRectTool* r;
                    for (ImageMeasurementTool* & t : dataHelper->ampCursors.doubleBoxTools) {
                        if (t == measurementTool) continue;
                        r = static_cast<ImageMeasurementDoubleRectTool*>(t);
                        r->blockSignals(true);
                        r->setSearchArea(searchArea, false);
                        r->setSumLine(sumLine, false);
                        r->blockSignals(false);
                    }
                    dataHelper->moveDoubleBoxTool(param.direction, dataHelper->d->dataHelper->moving);
                    dataHelper->d->update();
                    emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(dataHelper->d), &param.searchArea);
                });
                QObject::connect(measurementTool, &ImageMeasurementDoubleRectTool::posChanged, dataHelper->d, [this, measurementTool](){
                    cdsem::MeasureType mType = cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpMeasureType();
                    if (mType != cdsem::MT_DIAMETER_HOLE) {
                        return;
                    }
                    AmpDiameterHoleParam& param = cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpDiameterHoleParam();
                    MeasurementNumber designValue = measurementTool->getDesignValue();
                    param.designValue = cdsem::PixelDistanceConvert::instance()->getNMSizeFromPixel(designValue, IOPT->getPixelSize());
                    if (param.designValue < cdsem::AMP_WIDTH_MIN_DESIGN_VALUE) {
                        param.designValue = cdsem::AMP_WIDTH_MIN_DESIGN_VALUE;
                    }
                    if (!uiHelper->lockCursorMoveBtn->isChecked()) {
                        ImageMeasurementDoubleRectTool* r;
                        for (ImageMeasurementTool* & t : dataHelper->ampCursors.doubleBoxTools) {
                            if (t == measurementTool) continue;
                            r = static_cast<ImageMeasurementDoubleRectTool*>(t);
                            r->blockSignals(true);
                            r->setDesignValue(designValue, false);
                            r->blockSignals(false);
                        }
                    }
                    dataHelper->moveDoubleBoxTool(param.direction, !uiHelper->lockCursorMoveBtn->isChecked() && dataHelper->moving);
                    dataHelper->d->update();
                    emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(dataHelper->d), &param.designValue);
                });
                measurementTool->setLockDistance(false);
                measurementTool->setBorderStyle(MTBS_SOLID_CORNER_DASH_LINE, false);
                measurementTool->setMinSearchArea(cdsem::AMP_WIDTH_MIN_SEARCH_AREA);
                measurementTool->setMaxSearchArea(cdsem::AMP_WIDTH_MAX_SEARCH_AREA);
                measurementTool->setMinSumLine(cdsem::AMP_WIDTH_MIN_SUM_LINE);
                measurementTool->setMaxSumLine(cdsem::AMP_WIDTH_MAX_SUM_LINE);
            } else {
                measurementTool->setLockDistance(true);
                measurementTool->setBorderStyle(MTBS_DASH, false);
                measurementTool->disableBorderResize();
                measurementTool->disableCornerResize();
            }


            measurementTool->setCrossSize(QSize(20, 20));
            measurementTool->setWeight(2, false);
            measurementTool->setColor(QColor(255, 255, 255, 125), false);
            measurementTool->setOverLine(MTBOP_INNER, false);
            measurementTool->setCrossStyle(MTCS_SOLID, false);
            measurementTool->setMarginBottom(cdsem::AMP_CURSOR_MARGIN);
            measurementTool->setMarginLeft(cdsem::AMP_CURSOR_MARGIN);
            measurementTool->setMarginTop(cdsem::AMP_CURSOR_MARGIN);
            measurementTool->setMarginRight(cdsem::AMP_CURSOR_MARGIN);


            measurementTool->setInnerTopTextOption(option, MDTLR_LEFT_TOP, false);
            measurementTool->setInnerTopTextOption(option, MDTLR_RIGHT_BOTTOM, false);
            measurementTool->setInnerTopText(QString::number(dataHelper->ampCursors.doubleBoxTools.size() + 1), MDTLR_LEFT_TOP, false);
            measurementTool->setInnerTopText(QString::number(dataHelper->ampCursors.doubleBoxTools.size() + 1), MDTLR_RIGHT_BOTTOM, false);
            dataHelper->ampCursors.doubleBoxTools.append(measurementTool);
            dataHelper->d->addMeasurementTool(measurementTool);
        }
        if (dataHelper->ampCursors.doubleBoxTools.size() > 1) {
            dataHelper->d->measurementToolToBehindFloor(dataHelper->ampCursors.doubleBoxTools.first(), dataHelper->ampCursors.doubleBoxTools.last());
        }
    } else if (dataHelper->ampCursors.doubleBoxTools.size() > num) {
        ImageMeasurementTool* r;
        while (dataHelper->ampCursors.doubleBoxTools.size() > num) {
            r = dataHelper->ampCursors.doubleBoxTools.takeLast();
            dataHelper->d->removeMeasurementTool(r);
            delete r;
        }
    }

    num = dataHelper->ampCursors.doubleBoxTools.size();
    for (int i = 0; i < num; ++ i) {
        ImageMeasurementDoubleRectTool* measurementTool = static_cast<ImageMeasurementDoubleRectTool*>(dataHelper->ampCursors.doubleBoxTools[i]);
        measurementTool->setMinDesignValue(cdsem::PixelDistanceConvert::instance()->getPixelSizeFromDistanceNM(cdsem::AMP_WIDTH_MIN_DESIGN_VALUE, IOPT->getPixelSize()));
        measurementTool->setMaxDesignValue(cdsem::PixelDistanceConvert::instance()->getPixelSizeFromDistanceNM(cdsem::AMP_WIDTH_MAX_DESIGN_VALUE, IOPT->getPixelSize()));
        if (diameterHoleParam.direction == 0) {
            measurementTool->setOrientation(Qt::Horizontal);
        } else {
            measurementTool->setOrientation(Qt::Vertical);
        }
        measurementTool->blockSignals(true);
        if (diameterHoleParam.designValue != nullptr)
            measurementTool->setDesignValue(cdsem::PixelDistanceConvert::instance()->getPixelSizeFromDistanceNM(diameterHoleParam.designValue, IOPT->getPixelSize()));
        measurementTool->setSumLine(diameterHoleParam.sumLineOrInspectArea);
        measurementTool->setSearchArea(diameterHoleParam.searchArea);
        if (dataHelper->assistRange.offset.x != nullptr) {
            measurementTool->enableResizeOnCenter(true);
            measurementTool->disableSumLineDirectionMove();
            MeasurementPoint point = calcCenterPos(getDoubleBoxSize(measurementTool), dataHelper->assistRange.size);
            measurementTool->move({point.x + dataHelper->assistRange.offset.x, point.y + dataHelper->assistRange.offset.y});
        } else if (diameterHoleParam.acdParam.centers.size() > 0 && i < diameterHoleParam.acdParam.centers.size()){
            moveMeasuremntToolByCenter(measurementTool, diameterHoleParam.acdParam.centers[i], false);
        } else {
            moveDoubleBoxToCenter(measurementTool, uiHelper->canvas, false);
        }
        measurementTool->blockSignals(false);
    }
}

void DiameterHoleCursorHandler::markAnnulusBoxTools(AmpDiameterHoleParam& diameterHoleParam) {
    int num = diameterHoleParam.acdParam.acdType == cdsem::ACDT_MANUAL ? diameterHoleParam.acdParam.target : 1;
    if (dataHelper->ampCursors.annulusTools.size() < num) {
        num -= dataHelper->ampCursors.annulusTools.size();
        MeasurementToolCrossTextOption option;
        option.fontSize = 16;
        option.padb = 5;
        for (int i = 0 ; i < num; ++ i) {
            ImageMeasurementAnnulusTool* measurementTool = dataHelper->d->createMeasurementAnnulusTool();
            if (dataHelper->ampCursors.annulusTools.isEmpty()) {
                QObject::connect(measurementTool, &ImageMeasurementAnnulusTool::mouseMoveBegin, dataHelper->d, [this](){
                    dataHelper->d->dataHelper->moving = true;
                    dataHelper->initAnnualBoxBeginPos();
                });
                QObject::connect(measurementTool, &ImageMeasurementAnnulusTool::designChanged, dataHelper->d, [this, measurementTool](){
                    cdsem::MeasureType mType = cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpMeasureType();
                    if (mType != cdsem::MT_DIAMETER_HOLE) {
                        return;
                    }
                    AmpDiameterHoleParam& param = cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpDiameterHoleParam();
                    MeasurementNumber designValue = measurementTool->getHDesignValue();
                    param.designValue = cdsem::PixelDistanceConvert::instance()->getNMSizeFromPixel(designValue, IOPT->getPixelSize());
                    emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(dataHelper->d), &param.designValue);
                    ImageMeasurementAnnulusTool* r;
                    for (ImageMeasurementTool* & t : dataHelper->ampCursors.annulusTools) {
                        if (t == measurementTool) continue;
                        r = static_cast<ImageMeasurementAnnulusTool*>(t);
                        r->blockSignals(true);
                        r->setDesignValue(designValue, false);
                        r->blockSignals(false);
                    }
                    dataHelper->moveDoubleBoxTool(param.direction, dataHelper->d->dataHelper->moving);
                    dataHelper->d->update();
                    emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(dataHelper->d), &param.searchArea);
                });
                QObject::connect(measurementTool, &ImageMeasurementAnnulusTool::searchAreaChanged, dataHelper->d, [this, measurementTool](){
                    cdsem::MeasureType mType = cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpMeasureType();
                    if (mType != cdsem::MT_DIAMETER_HOLE) {
                        return;
                    }
                    AmpDiameterHoleParam& param = cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpDiameterHoleParam();
                    MeasurementNumber searchArea = measurementTool->getSearchArea();
                    param.searchArea = searchArea;
                    emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(dataHelper->d), &param.designValue);
                    emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(dataHelper->d), &param.searchArea);
                    ImageMeasurementAnnulusTool* r;
                    for (ImageMeasurementTool* & t : dataHelper->ampCursors.annulusTools) {
                        if (t == measurementTool) continue;
                        r = static_cast<ImageMeasurementAnnulusTool*>(t);
                        r->blockSignals(true);
                        r->setSearchArea(searchArea, false);
                        r->blockSignals(false);
                    }
                    dataHelper->moveDoubleBoxTool(param.direction, dataHelper->d->dataHelper->moving);
                    dataHelper->d->update();
                    emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(dataHelper->d), &param.searchArea);
                });
                QObject::connect(measurementTool, &ImageMeasurementAnnulusTool::posChanged, dataHelper->d, [this](){
                    if (uiHelper->lockCursorMoveBtn->isChecked()) {
                        dataHelper->moveAnnualTool();
                        dataHelper->d->update();
                    }
                });
                // measurementTool->setBorderStyle(MTBS_SOLID, false);
            } else {
                measurementTool->setInnerBorder(MTBS_SOLID_CORNER_DASH_LINE, false);
                measurementTool->setOuterBorder(MTBS_SOLID_CORNER_DASH_LINE, false);
                measurementTool->disableBorderResize();
                measurementTool->disableCornerResize();
            }
            measurementTool->setCrossSize(QSize(20, 20));
            measurementTool->setWeight(2, false);
            measurementTool->setColor(QColor(255, 255, 255, 125), false);
            measurementTool->setCrossStyle(MTCS_SOLID, false);
            measurementTool->setMargin({cdsem::AMP_CURSOR_MARGIN, cdsem::AMP_CURSOR_MARGIN, cdsem::AMP_CURSOR_MARGIN, cdsem::AMP_CURSOR_MARGIN});
            measurementTool->ignoreInnerCircle();
            measurementTool->setMinSearchArea(cdsem::AMP_WIDTH_MIN_SEARCH_AREA);
            measurementTool->setMaxSearchArea(cdsem::AMP_WIDTH_MAX_SEARCH_AREA);
            measurementTool->setCrossRightBottomTextOption(option, false);
            measurementTool->setCrossRightBottomText(QString::number(dataHelper->ampCursors.annulusTools.size() + 1), false);

            dataHelper->ampCursors.annulusTools.append(measurementTool);
            dataHelper->d->addMeasurementTool(measurementTool);
        }
        if (dataHelper->ampCursors.annulusTools.size() > 1) {
            dataHelper->d->measurementToolToBehindFloor(dataHelper->ampCursors.annulusTools.first(), dataHelper->ampCursors.annulusTools.last());
        }
    } else if (dataHelper->ampCursors.annulusTools.size() > num) {
        ImageMeasurementTool* r;
        while (dataHelper->ampCursors.annulusTools.size() > num) {
            r = dataHelper->ampCursors.annulusTools.takeLast();
            dataHelper->d->removeMeasurementTool(r);
            delete r;
        }
    }

    num = dataHelper->ampCursors.annulusTools.size();
    for (int i = 0; i < num; ++ i) {
        ImageMeasurementAnnulusTool* measurementTool = static_cast<ImageMeasurementAnnulusTool*>(dataHelper->ampCursors.annulusTools[i]);
        measurementTool->blockSignals(true);

        int minDesignValue = cdsem::PixelDistanceConvert::instance()->getPixelSizeFromDistanceNM(cdsem::AMP_WIDTH_MIN_DESIGN_VALUE, IOPT->getPixelSize());
        int maxDesignValue = cdsem::PixelDistanceConvert::instance()->getPixelSizeFromDistanceNM(cdsem::AMP_WIDTH_MAX_DESIGN_VALUE, IOPT->getPixelSize());
        measurementTool->setMinHDesignValue(minDesignValue);
        measurementTool->setMinVDesignValue(minDesignValue);
        measurementTool->setMaxHDesignValue(maxDesignValue);
        measurementTool->setMaxVDesignValue(maxDesignValue);

        if (diameterHoleParam.designValue != nullptr)
            measurementTool->setDesignValue(cdsem::PixelDistanceConvert::instance()->getPixelSizeFromDistanceNM(diameterHoleParam.designValue, IOPT->getPixelSize()));

        measurementTool->setSearchArea(diameterHoleParam.searchArea);
        if (dataHelper->assistRange.offset.x != nullptr) {
            MeasurementPoint point = calcCenterPos({0, 0}, dataHelper->assistRange.size);
            measurementTool->moveCenter({point.x + dataHelper->assistRange.offset.x, point.y + dataHelper->assistRange.offset.y});
            measurementTool->disableMove();
        } else if (diameterHoleParam.acdParam.centers.size() > 0 && i < diameterHoleParam.acdParam.centers.size()){
            moveMeasuremntToolByCenter(measurementTool, diameterHoleParam.acdParam.centers[i], false);
        } else {
            measurementTool->moveCenter(calcCenterPos({0, 0}, uiHelper->canvas), false);
        }
        measurementTool->toolIsCircle(true);
        measurementTool->blockSignals(false);
    }
}

void DiameterHoleCursorHandler::markDiameterHoleLineResultTools() {}
void DiameterHoleCursorHandler::markDiameterHoleLineTools() {
    dataHelper->markWidthLineTools();
}
void DiameterHoleCursorHandler::notifyAmpChanged(void* pPtr) {
    AmpDiameterHoleParam& param = cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpDiameterHoleParam();
    if (pPtr == &param.measurePosition) {
        dataHelper->stopAutoMeasure();
        return;
    }
    cdsem::AMS_Status amsStatus = cdsem::CDSEM_GlobalDataHelper::get()->getAmsStatus();
    if (amsStatus != cdsem::AMS_SCURSOR) {
        return;
    }
    bool cursorTypeChanged = cursorKindChanged(lastKind, param.kind);
    uiHelper->canvasToolWidget->setVisible(param.acdParam.acdType == cdsem::ACDT_MANUAL && param.acdParam.target > 1);
    if (param.kind == 0 || param.kind == 1) {
        if (pPtr != &param.direction && pPtr != &param.designValue &&
            pPtr != &param.kind && pPtr != &param.searchArea && pPtr != &param.sumLineOrInspectArea
            && pPtr != &param.acdParam.acdType && pPtr != &param.acdParam.target) {
            return;
        }
        dataHelper->markMeasureCursor();
        ImageMeasurementDoubleRectTool* d;
        for (int i = 0; i < dataHelper->ampCursors.doubleBoxTools.size();  ++ i) {
            d = static_cast<ImageMeasurementDoubleRectTool*>(dataHelper->ampCursors.doubleBoxTools[i]);
            if (pPtr == &param.direction || param.direction != int(d->getOrientation()) - 1) {
                if (param.direction == 0) {
                    d->setOrientation(Qt::Horizontal, false);
                } else {
                    d->setOrientation(Qt::Vertical, false);
                }
                if (dataHelper->assistRange.offset.x != nullptr) {
                    MeasurementPoint point = calcCenterPos(getDoubleBoxSize(d), dataHelper->assistRange.size);
                    d->move({point.x + dataHelper->assistRange.offset.x, point.y + dataHelper->assistRange.offset.y}, false);
                }
            }
            if (pPtr == &param.designValue) {
                d->blockSignals(true);
                if (param.designValue != nullptr)
                    d->setDesignValueKeepCenter(cdsem::PixelDistanceConvert::instance()->getPixelSizeFromDistanceNM(param.designValue, IOPT->getPixelSize()), false);
                d->blockSignals(false);
            }
            if (pPtr == &param.searchArea) {
                d->setSearchArea(param.searchArea, false);
            }
            if (pPtr == &param.sumLineOrInspectArea) {
                d->setSumLineKeepCenter(param.sumLineOrInspectArea, false);
            }

            if (pPtr == &param.kind && cursorTypeChanged) {
                if (i < dataHelper->ampCursors.annulusTools.size()) {
                    MeasurementSize size = getDoubleBoxSize(d);
                    MeasurementPoint pos = static_cast<ImageMeasurementAnnulusTool*>(dataHelper->ampCursors.annulusTools[i])->getCenterPos();
                    d->move({pos.x - size.width / 2, pos.y - size.height / 2}, false);
                }
            }
        }
        if (pPtr == &param.kind && cursorTypeChanged) {
            dataHelper->exchangeCurrentAmpCursor(&dataHelper->ampCursors.doubleBoxTools);
        }
        dataHelper->d->update();
    } else if (param.kind == 2) {
        dataHelper->markMeasureCursor();
        if (pPtr != &param.designValue &&
            pPtr != &param.kind && pPtr != &param.searchArea
            && pPtr != &param.acdParam.acdType && pPtr != &param.acdParam.target) {
            return;
        }
        ImageMeasurementAnnulusTool* d;
        for (int i = 0; i < dataHelper->ampCursors.annulusTools.size(); ++ i) {
            d = static_cast<ImageMeasurementAnnulusTool*>(dataHelper->ampCursors.annulusTools[i]);
            d->blockSignals(true);
            d->setDesignValue(cdsem::PixelDistanceConvert::instance()->getPixelSizeFromDistanceNM(param.designValue, IOPT->getPixelSize()), false);
            d->setSearchArea(param.searchArea, false);

            if (dataHelper->assistRange.offset.x != nullptr) {
                MeasurementPoint point = calcCenterPos({0, 0}, dataHelper->assistRange.size);
                d->moveCenter({point.x + dataHelper->assistRange.offset.x, point.y + dataHelper->assistRange.offset.y}, false);
            }
            d->blockSignals(false);
            if (pPtr == &param.kind && cursorTypeChanged) {
                if (i < dataHelper->ampCursors.doubleBoxTools.size()) {
                    ImageMeasurementDoubleRectTool* doubleTool = static_cast<ImageMeasurementDoubleRectTool*>(dataHelper->ampCursors.doubleBoxTools[i]);
                    MeasurementSize doubleSize = getDoubleBoxSize(doubleTool);
                    const MeasurementPoint& pos = doubleTool->getLeftTopPos();
                    d->moveCenter({doubleSize.width / 2 + pos.x, doubleSize.height / 2 + pos.y}, false);
                }
            }
        }
        if (pPtr == &param.kind && cursorTypeChanged) {
            dataHelper->exchangeCurrentAmpCursor(&dataHelper->ampCursors.annulusTools);
        }
        dataHelper->d->update();
    }
    if (pPtr == &param.kind && cursorTypeChanged) {
        dataHelper->lockAcdCursorsMouse(uiHelper->lockCursorMoveBtn->isChecked());
    }
    if (pPtr == &param.kind)
        lastKind = param.kind;
}

void DiameterHoleCursorHandler::sortCursors() {
    AmpDiameterHoleParam& param = cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpDiameterHoleParam();
    if (param.acdParam.target <= 1 || param.acdParam.acdType != cdsem::ACDT_MANUAL) {
        return;
    }
    if (dataHelper->ampCursors.currentTools == nullptr || dataHelper->ampCursors.currentTools->size() < 1) {
        return;
    }
    QList<MeasurementPoint> list;
    list.reserve(dataHelper->ampCursors.currentTools->size());
    for (ImageMeasurementTool* &t : *dataHelper->ampCursors.currentTools) {
        if (param.kind == 0 || param.kind == 1) {
            ImageMeasurementDoubleRectTool* d = static_cast<ImageMeasurementDoubleRectTool*>(t);
            list.append(d->getLeftTopPos());
        } else if (param.kind == 2){
            ImageMeasurementAnnulusTool* d = static_cast<ImageMeasurementAnnulusTool*>(t);
            list.append(d->getCenterPos());
        }
    }
    std::sort(list.begin(), list.end(), &ImageOperationControllerDataHelper::ampCursorSortFunc);
    for (int i = 0; i < dataHelper->ampCursors.currentTools->size(); ++ i) {
        ImageMeasurementTool* t = (*dataHelper->ampCursors.currentTools)[i];
        if (param.kind == 0 || param.kind == 1) {
            ImageMeasurementDoubleRectTool* d = static_cast<ImageMeasurementDoubleRectTool*>(t);
            d->move(list[i]);
        } else if (param.kind == 2){
            ImageMeasurementAnnulusTool* d = static_cast<ImageMeasurementAnnulusTool*>(t);
            d->moveCenter(list[i]);
        }
    }
}
void DiameterHoleCursorHandler::allAmpCursorToCenter() {
    AmpDiameterHoleParam& param = cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpDiameterHoleParam();
    if (param.acdParam.target <= 1 || param.acdParam.acdType != cdsem::ACDT_MANUAL) {
        return;
    }
    if (dataHelper->ampCursors.currentTools == nullptr || dataHelper->ampCursors.currentTools->size() < 1) {
        return;
    }
    MeasurementPoint pos;
    if (param.kind == 0 || param.kind == 1) {
        ImageMeasurementDoubleRectTool* d = static_cast<ImageMeasurementDoubleRectTool*>(dataHelper->ampCursors.currentTools->first());
        moveDoubleBoxToCenter(d, uiHelper->canvas, false);
        pos = d->getLeftTopPos();
    } else if (param.kind == 2) {
        ImageMeasurementAnnulusTool* d = static_cast<ImageMeasurementAnnulusTool*>(dataHelper->ampCursors.currentTools->first());
        d->moveCenter(calcCenterPos({0, 0}, uiHelper->canvas), false);
        pos = d->getCenterPos();
    } else {
        return;
    }
    for (int i = 1; i < dataHelper->ampCursors.currentTools->size(); ++ i) {
        if (param.kind == 0 || param.kind == 1) {
            ImageMeasurementDoubleRectTool* d = static_cast<ImageMeasurementDoubleRectTool*>((*dataHelper->ampCursors.currentTools)[i]);
            d->move(pos, false);
        } else if (param.kind == 2){
            ImageMeasurementAnnulusTool* d = static_cast<ImageMeasurementAnnulusTool*>((*dataHelper->ampCursors.currentTools)[i]);
            d->moveCenter(pos, false);
        }
    }
    dataHelper->d->update();
}
void DiameterHoleCursorHandler::lockAcdCursorsMouse(bool lock) {
    AmpDiameterHoleParam& param = cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpDiameterHoleParam();
    if (param.acdParam.acdType != cdsem::ACDT_MANUAL || param.acdParam.target <= 1 || dataHelper->ampCursors.currentTools == nullptr || dataHelper->ampCursors.currentTools->size() < 1) {
        return;
    }
    if (param.kind == 0 || param.kind == 1) {
        ImageMeasurementDoubleRectTool* d = static_cast<ImageMeasurementDoubleRectTool*>(dataHelper->ampCursors.currentTools->first());
        d->setLockDistance(lock);
        for (int i = 1; i < dataHelper->ampCursors.currentTools->size(); ++ i) {
            d = static_cast<ImageMeasurementDoubleRectTool*>((*dataHelper->ampCursors.currentTools)[i]);
            d->setMouseTransparent(lock);
        }
    } else if (param.kind == 2) {
        ImageMeasurementAnnulusTool* d;
        for (int i = 1; i < dataHelper->ampCursors.currentTools->size(); ++ i) {
            d = static_cast<ImageMeasurementAnnulusTool*>((*dataHelper->ampCursors.currentTools)[i]);
            d->setMouseTransparent(lock);
        }
    }
}
