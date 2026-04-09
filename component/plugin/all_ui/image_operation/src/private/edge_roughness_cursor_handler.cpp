#include "edge_roughness_cursor_handler.h"
#include "cdsem_global_data.h"
#include "image_operation_controller_data_helper.h"
#include "image_operation_controller.h"
#include "image_measurement_rect_text_tool.h"
#include "image_measurement_double_rect_tool.h"
#include "cde90_button.h"
#include "image_measurement_distance_tool.h"
#include "image_operation_widget_canvas.h"
#include "tmp_image_data_converter.h"
#include "sem_global.h"
#include "image_file_temp_handler.h"
#include "sem_common_data_model.h"
#include "mat_measure.h"
#include "image_view_measure_tool_draw_helper.h"
#include "image_save_model.h"
#include "image_operation_data_model.h"
#include "image_operation_controller_ui_helper.h"
#include <QPointer>

//为了方便修改，未来如果想对该功能添加direction，即X/Y方向切换支持时，搜索关键字：modify direction即可
#define uiHelper dataHelper->d->uiHelper

EdgeRoughnessCursorHandler::EdgeRoughnessCursorHandler(ImageOperationControllerDataHelper* dataHelper) : dataHelper(dataHelper){}
EdgeRoughnessCursorHandler::~EdgeRoughnessCursorHandler() noexcept {}

void EdgeRoughnessCursorHandler::doAutoMeasure(const sem::ONE_PARAM_ARG_FUNC<const OperationResult&>& callback, const cdsem::TmpImageData& imageData, const ImageAddtionDataModel& addition) {
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
        AmpEdgeRoughnessParam& edgeRoughnessParam = cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpEdgeRoughnessParam();
        collectCenters();
        if (edgeRoughnessParam.acdParam.centers.size() > 0) {
            ImageResultData data = HRMat::measureRoughnessImage(image, pixelSize, edgeRoughnessParam, {});
            qDebug() << "测量完毕";
            dataHelper->d->setMeasureResult(data);
            markEdgeLineResultTools();
            dataHelper->removeAllAmpCursor();
        } else {
            dataHelper->stopAutoMeasure();
        }
    });
}

void EdgeRoughnessCursorHandler::collectCenters() {
    AmpEdgeRoughnessParam& edgeRoughnessParam = cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpEdgeRoughnessParam();
    QList<QPoint> centers;
    QPoint center;
    if (edgeRoughnessParam.cursorType == 0) {
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

    } else if (edgeRoughnessParam.cursorType == 1){
        if (dataHelper->ampCursors.oneBoxTools.isEmpty()) return;
        ImageMeasurementRectTool* d;
        centers.reserve(dataHelper->ampCursors.oneBoxTools.size());
        for (ImageMeasurementTool* & t : dataHelper->ampCursors.oneBoxTools) {
            d = static_cast<ImageMeasurementRectTool*>(t);
            const MeasurementSize& size = d->getSize();
            const MeasurementPoint& pos = d->getPos();
            center.setX(pos.x + size.width / 2);
            center.setY(pos.y + size.height / 2);
            centers.append(center);
        }
    } else {
        edgeRoughnessParam.acdParam.centers.clear();
        return;
    }
    edgeRoughnessParam.acdParam.centers = centers;
}

void EdgeRoughnessCursorHandler::markEdgeCursors() {
    AmpEdgeRoughnessParam& edgeRoughnessParam = cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpEdgeRoughnessParam();
    cdsem::AMS_Status amsStatus = cdsem::CDSEM_GlobalDataHelper::get()->getAmsStatus();
    if (amsStatus == cdsem::AMS_SOFF && edgeRoughnessParam.measurePosition == 1) {
        markEdgeLineTools();
        cdsem::CDSEM_GlobalDataHelper::get()->setAmsStatus(cdsem::AMS_SASSIST);
        return;
    }
    dataHelper->recordAssistCenterPos();
    int num = edgeRoughnessParam.acdParam.acdType == cdsem::ACDT_MANUAL ? edgeRoughnessParam.acdParam.target : 1;
    uiHelper->canvasToolWidget->setVisible(edgeRoughnessParam.acdParam.acdType == cdsem::ACDT_MANUAL && num > 1);
    if (edgeRoughnessParam.cursorType == cdsem::ABCT_TWO_BOX) {
        bool needExchange = dataHelper->ampCursors.doubleBoxTools.isEmpty();
        markEdgeDoubleBoxTools(edgeRoughnessParam);
        if (needExchange) {
            dataHelper->exchangeCurrentAmpCursor(&dataHelper->ampCursors.doubleBoxTools);
        }
    } else if (edgeRoughnessParam.cursorType == cdsem::ABCT_ONE_BOX) {
        bool needExchange = dataHelper->ampCursors.oneBoxTools.isEmpty();
        markEdgeOneBoxTools(edgeRoughnessParam);
        if (needExchange) {
            dataHelper->exchangeCurrentAmpCursor(&dataHelper->ampCursors.oneBoxTools);
        }
    }
    if (dataHelper->ampCursors.distanceTools.size() > 0) {
        dataHelper->removeAllAmpCursor(dataHelper->ampCursors.distanceTools);
    }
    cdsem::CDSEM_GlobalDataHelper::get()->setAmsStatus(cdsem::AMS_SCURSOR);
    dataHelper->d->update();
}

void EdgeRoughnessCursorHandler::markEdgeDoubleBoxTools(AmpEdgeRoughnessParam& edgeRoughnessParam) {
    int num = edgeRoughnessParam.acdParam.acdType == cdsem::ACDT_MANUAL ? edgeRoughnessParam.acdParam.target : 1;
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
                    if (mType != cdsem::MT_EDGE_ROUGHNESS) {
                        return;
                    }
                    AmpEdgeRoughnessParam& param = cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpEdgeRoughnessParam();
                    MeasurementNumber sumLine = measurementTool->getSumLine();
                    MeasurementNumber searchArea = measurementTool->getSearchArea();
                    param.inspectArea = sumLine;
                    emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(dataHelper->d), &param.inspectArea);
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
                    //modify direction
                    dataHelper->moveDoubleBoxTool(cdsem::ABD_X, dataHelper->d->dataHelper->moving);
                    dataHelper->d->update();
                    emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(dataHelper->d), &param.searchArea);
                });
                QObject::connect(measurementTool, &ImageMeasurementDoubleRectTool::posChanged, dataHelper->d, [this, measurementTool](){
                    cdsem::MeasureType mType = cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpMeasureType();
                    if (mType != cdsem::MT_EDGE_ROUGHNESS) {
                        return;
                    }
                    AmpEdgeRoughnessParam& param = cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpEdgeRoughnessParam();
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
                    //modify direction
                    dataHelper->moveDoubleBoxTool(cdsem::ABD_X, !uiHelper->lockCursorMoveBtn->isChecked() && dataHelper->moving);
                    dataHelper->d->update();
                    emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(dataHelper->d), &param.designValue);
                });
                measurementTool->setLockDistance(false);
                measurementTool->setBorderStyle(MTBS_SOLID_CORNER_DASH_LINE, false);
                measurementTool->setMinSearchArea(cdsem::AMP_WIDTH_MIN_SEARCH_AREA);
                measurementTool->setMaxSearchArea(cdsem::AMP_WIDTH_MAX_SEARCH_AREA);
                measurementTool->setMinSumLine(cdsem::AMP_WIDTH_MIN_SUM_LINE);
                measurementTool->setMaxSumLine(cdsem::AMP_WIDTH_MAX_SUM_LINE);
                measurementTool->setMinDesignValue(cdsem::PixelDistanceConvert::instance()->getPixelSizeFromDistanceNM(cdsem::AMP_WIDTH_MIN_DESIGN_VALUE, IOPT->getPixelSize()));
                measurementTool->setMaxDesignValue(cdsem::PixelDistanceConvert::instance()->getPixelSizeFromDistanceNM(cdsem::AMP_WIDTH_MAX_DESIGN_VALUE, IOPT->getPixelSize()));
            } else {
                measurementTool->setLockDistance(true);
                measurementTool->setBorderStyle(MTBS_DASH, false);
                measurementTool->disableBorderResize();
                measurementTool->disableCornerResize();
            }

            //modify direction
            // if (edgeRoughnessParam.direction == 0) {
            measurementTool->setOrientation(Qt::Horizontal);
            // } else {
            // measurementTool->setOrientation(Qt::Vertical);
            // }
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
        measurementTool->blockSignals(true);
        if (edgeRoughnessParam.designValue != nullptr)
            measurementTool->setDesignValue(cdsem::PixelDistanceConvert::instance()->getPixelSizeFromDistanceNM(edgeRoughnessParam.designValue, IOPT->getPixelSize()));
        measurementTool->setSumLine(edgeRoughnessParam.inspectArea);
        measurementTool->setSearchArea(edgeRoughnessParam.searchArea);
        if (dataHelper->assistRange.offset.x != nullptr) {
            measurementTool->enableResizeOnCenter(true);
            measurementTool->disableSumLineDirectionMove();
            MeasurementPoint point = calcCenterPos(getDoubleBoxSize(measurementTool), dataHelper->assistRange.size);
            measurementTool->move({point.x + dataHelper->assistRange.offset.x, point.y + dataHelper->assistRange.offset.y});
        } else if (edgeRoughnessParam.acdParam.centers.size() > 0 && i < edgeRoughnessParam.acdParam.centers.size()){
            moveMeasuremntToolByCenter(measurementTool, edgeRoughnessParam.acdParam.centers[i], false);
        } else {
            moveDoubleBoxToCenter(measurementTool, uiHelper->canvas, false);
        }
        measurementTool->blockSignals(false);
    }
}

void EdgeRoughnessCursorHandler::markEdgeOneBoxTools(AmpEdgeRoughnessParam& edgeRoughnessParam) {
    int num = edgeRoughnessParam.acdParam.acdType == cdsem::ACDT_MANUAL ? edgeRoughnessParam.acdParam.target : 1;
    if (dataHelper->ampCursors.oneBoxTools.size() < num) {
        num -= dataHelper->ampCursors.oneBoxTools.size();
        MeasurementToolTextOption option;
        option.fontSize = 16;
        option.alignment = MTTA_LEFT_OR_TOP;
        option.padlt = 5;
        for (int i = 0; i < num; ++ i) {
            ImageMeasurementRectTextTool* measurementTool = dataHelper->d->createMeasurementRectTextTool();
            if (dataHelper->ampCursors.oneBoxTools.isEmpty()) {
                QObject::connect(measurementTool, &ImageMeasurementRectTool::mouseResizeBegin, dataHelper->d, [this](){
                    dataHelper->initOneBoxBeginPos();
                });
                QObject::connect(measurementTool, &ImageMeasurementRectTool::mouseMoveBegin, dataHelper->d, [this](){
                    if (uiHelper->lockCursorMoveBtn->isChecked())
                        dataHelper->initOneBoxBeginPos();
                });
                QObject::connect(measurementTool, &ImageMeasurementRectTool::posChanged, dataHelper->d, [this](){
                    if (uiHelper->lockCursorMoveBtn->isChecked()) {
                        dataHelper->moveOneBoxTool();
                        dataHelper->d->update();
                    }
                });
                QObject::connect(measurementTool, &ImageMeasurementRectTool::sizeChanged, dataHelper->d, [this, measurementTool](){
                    cdsem::MeasureType mType = cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpMeasureType();
                    if (mType != cdsem::MT_EDGE_ROUGHNESS) {
                        return;
                    }
                    AmpEdgeRoughnessParam& param = cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpEdgeRoughnessParam();
                    const MeasurementSize& size = measurementTool->getSize();
                    if (cdsem::checkAmpWidthRoughnessObjectIsDense(param.object)) {
                        param.denseParam.cursorSizeX = size.width;
                        param.denseParam.cursorSizeY = size.height;
                        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(dataHelper->d), &param.denseParam.cursorSizeX);
                        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(dataHelper->d), &param.denseParam.cursorSizeY);
                        measurementTool->setMaxSize({cdsem::AMP_WIDTH_DENES_MAX_CURSOR_X, cdsem::AMP_WIDTH_DENES_MAX_CURSOR_Y});
                        measurementTool->setMinSize({cdsem::AMP_WIDTH_DENES_MIN_CURSOR_X, cdsem::AMP_WIDTH_DENES_MIN_CURSOR_Y});
                    } else {
                        int distance = cdsem::PixelDistanceConvert::instance()->getPixelSizeFromDistanceNM(param.designValue, IOPT->getPixelSize());
                        //modify direction
                        // if (param.direction == 0) {
                        param.inspectArea = size.height;
                        param.searchArea = size.width - distance;
                        measurementTool->setMaxSize({distance + cdsem::AMP_WIDTH_MAX_SEARCH_AREA, cdsem::AMP_WIDTH_MAX_SUM_LINE});
                        measurementTool->setMinSize({distance + cdsem::AMP_WIDTH_MIN_SEARCH_AREA, cdsem::AMP_WIDTH_MIN_SUM_LINE});
                        // } else {
                        //     param.inspectArea = size.width;
                        //     param.searchArea = size.height - cdsem::PixelDistanceConvert::instance()->getPixelSizeFromDistanceNM(param.designValue, IOPT->getPixelSize());
                        //     measurementTool->setMaxSize({cdsem::AMP_WIDTH_MAX_SUM_LINE, distance + cdsem::AMP_WIDTH_MAX_SEARCH_AREA});
                        //     measurementTool->setMinSize({cdsem::AMP_WIDTH_MIN_SUM_LINE, distance + cdsem::AMP_WIDTH_MIN_SEARCH_AREA});
                        // }
                        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(dataHelper->d), &param.searchArea);
                        emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(dataHelper->d), &param.inspectArea);
                    }
                    ImageMeasurementRectTool* r;
                    for (ImageMeasurementTool* & t : dataHelper->ampCursors.oneBoxTools) {
                        if (t == measurementTool) continue;
                        r = static_cast<ImageMeasurementRectTool*>(t);
                        r->blockSignals(true);
                        r->setSize(size, false);
                        r->blockSignals(false);
                    }
                    dataHelper->moveOneBoxTool();
                    dataHelper->d->update();
                });
                measurementTool->setBorderStyle(MTBS_SOLID_CORNER_DASH_LINE, false);
            } else {
                measurementTool->setBorderStyle(MTBS_DASH, false);
                measurementTool->disableBorderResize();
                measurementTool->disableCornerResize();
            }

            measurementTool->setCrossSize(QSize(20, 20));
            measurementTool->setWeight(2, false);
            measurementTool->setColor(QColor(255, 255, 255, 125), false);

            measurementTool->setOverLine(MTBOP_INNER, false);
            measurementTool->setCrossStyle(MTCS_SOLID, false);

            measurementTool->setInnerTopTextOption(option, false);
            measurementTool->setInnerTopText(QString::number(dataHelper->ampCursors.oneBoxTools.size() + 1), false);
            measurementTool->setMarginBottom(cdsem::AMP_CURSOR_MARGIN);
            measurementTool->setMarginLeft(cdsem::AMP_CURSOR_MARGIN);
            measurementTool->setMarginTop(cdsem::AMP_CURSOR_MARGIN);
            measurementTool->setMarginRight(cdsem::AMP_CURSOR_MARGIN);
            dataHelper->d->addMeasurementTool(measurementTool);
            dataHelper->ampCursors.oneBoxTools.append(measurementTool);
        }
        if (dataHelper->ampCursors.oneBoxTools.size() > 1) {
            dataHelper->d->measurementToolToBehindFloor(dataHelper->ampCursors.oneBoxTools.first(), dataHelper->ampCursors.oneBoxTools.last());
        }
    }  else if (dataHelper->ampCursors.oneBoxTools.size() > num) {
        ImageMeasurementTool* r;
        while (dataHelper->ampCursors.oneBoxTools.size() > num) {
            r = dataHelper->ampCursors.oneBoxTools.takeLast();
            dataHelper->d->removeMeasurementTool(r);
            delete r;
        }
    }

    num = dataHelper->ampCursors.oneBoxTools.size();
    for (int i = 0; i < num; ++ i) {
        ImageMeasurementRectTextTool* measurementTool = static_cast<ImageMeasurementRectTextTool*>(dataHelper->ampCursors.doubleBoxTools[i]);
        if (cdsem::checkAmpWidthRoughnessObjectIsDense(edgeRoughnessParam.object)) {
            measurementTool->setMaxSize({cdsem::AMP_WIDTH_DENES_MAX_CURSOR_X, cdsem::AMP_WIDTH_DENES_MAX_CURSOR_Y});
            measurementTool->setMinSize({cdsem::AMP_WIDTH_DENES_MIN_CURSOR_X, cdsem::AMP_WIDTH_DENES_MIN_CURSOR_Y});
        } else {
            int distance = cdsem::PixelDistanceConvert::instance()->getPixelSizeFromDistanceNM(edgeRoughnessParam.designValue, IOPT->getPixelSize());
            //modify direction
            // if (edgeRoughnessParam.direction == 0) {
            measurementTool->setMaxSize({distance + cdsem::AMP_WIDTH_MAX_SEARCH_AREA, cdsem::AMP_WIDTH_MAX_SUM_LINE});
            measurementTool->setMinSize({distance + cdsem::AMP_WIDTH_MIN_SEARCH_AREA, cdsem::AMP_WIDTH_MIN_SUM_LINE});
            // } else {
            //     measurementTool->setMaxSize({cdsem::AMP_WIDTH_MAX_SUM_LINE, distance + cdsem::AMP_WIDTH_MAX_SEARCH_AREA});
            //     measurementTool->setMinSize({cdsem::AMP_WIDTH_MIN_SUM_LINE, distance + cdsem::AMP_WIDTH_MIN_SEARCH_AREA});
            // }
        }
        measurementTool->blockSignals(true);
        FloatNumber2 width = 0;
        FloatNumber2 height = 0;
        if (cdsem::checkAmpWidthRoughnessObjectIsDense(edgeRoughnessParam.object)) {
            width = edgeRoughnessParam.denseParam.cursorSizeX;
            height = edgeRoughnessParam.denseParam.cursorSizeY;
        } else {
            //modify direction
            // if (edgeRoughnessParam.direction == 0) {
            height = edgeRoughnessParam.inspectArea;
            width = edgeRoughnessParam.searchArea + cdsem::PixelDistanceConvert::instance()->getPixelSizeFromDistanceNM(edgeRoughnessParam.designValue, IOPT->getPixelSize());
            // } else {
            //     width = edgeRoughnessParam.inspectArea;
            //     height = edgeRoughnessParam.searchArea + cdsem::PixelDistanceConvert::instance()->getPixelSizeFromDistanceNM(edgeRoughnessParam.designValue, IOPT->getPixelSize());
            // }
        }

        measurementTool->setSize({width, height});
        if (dataHelper->assistRange.offset.x != nullptr) {
            //modify direction
            // if (edgeRoughnessParam.direction == 0) {
            measurementTool->setResizePolicy(MMD_TOP, MMD_TOP_BOTTOM);
            measurementTool->setResizePolicy(MMD_BOTTOM, MMD_TOP_BOTTOM);
            measurementTool->disableMove(MMMD_Vertical);
            // } else {
            //     measurementTool->setResizePolicy(MMD_LEFT, MMD_LEFT_RIGHT);
            //     measurementTool->setResizePolicy(MMD_RIGHT, MMD_LEFT_RIGHT);
            //     measurementTool->disableMove(MMMD_Horizontal);
            // }
            MeasurementPoint point = calcCenterPos(measurementTool->getSize(), dataHelper->assistRange.size);
            measurementTool->move({point.x + dataHelper->assistRange.offset.x, point.y + dataHelper->assistRange.offset.y});
        } else if (edgeRoughnessParam.acdParam.centers.size() > 0 && i < edgeRoughnessParam.acdParam.centers.size()){
            moveMeasuremntToolByCenter(measurementTool, edgeRoughnessParam.acdParam.centers[i], false);
        } else {
            measurementTool->move(calcCenterPos(measurementTool->getSize(), uiHelper->canvas));
        }
        measurementTool->blockSignals(false);
    }
}

void EdgeRoughnessCursorHandler::markEdgeLineResultTools() {
    AmpEdgeRoughnessParam& edgeRoughnessParam = cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpEdgeRoughnessParam();
    // if (edgeRoughnessParam.measurePosition != 1) return;
    MeasurementNumber l1, l2, l3, l4;
    if (edgeRoughnessParam.cursorType == cdsem::ABCT_TWO_BOX) {
        if (dataHelper->ampCursors.doubleBoxTools.isEmpty()) return;
        ImageMeasurementDoubleRectTool* tool = static_cast<ImageMeasurementDoubleRectTool*>(dataHelper->ampCursors.doubleBoxTools.first());
        // if (edgeRoughnessParam.direction == 0) {
        l1 = tool->getDesignValue1();
        l2 = tool->getDesignValue2();
        l3 = tool->getLeftTopPos().y;
        l4 = tool->getRightBottomPos().y + tool->getRightBottomSize().height;
        // } else {
        //     l3 = tool->getDesignValue1();
        //     l4 = tool->getDesignValue2();
        //     l1 = tool->getLeftTopPos().x;
        //     l2 = tool->getRightBottomPos().x + tool->getRightBottomSize().width;
        // }
    } else if (edgeRoughnessParam.cursorType == cdsem::ABCT_ONE_BOX) {
        if (dataHelper->ampCursors.oneBoxTools.isEmpty()) return;
        ImageMeasurementRectTool* tool = static_cast<ImageMeasurementRectTool*>(dataHelper->ampCursors.oneBoxTools.first());
        const MeasurementSize& size = tool->getSize();
        int d = cdsem::PixelDistanceConvert::instance()->getPixelSizeFromDistanceNM(edgeRoughnessParam.designValue, IOPT->getPixelSize());
        //modify direction
        // if (ampWidth.direction == 0) {
        MeasurementNumber p = (size.width - d) / 2;
        l1 = tool->getPos().x + p;
        l2 = tool->getPos().x + size.width - p;
        l3 = tool->getPos().y;
        l4 = tool->getPos().y + size.height;
        // } else {
        //     MeasurementNumber p = (size.height - d) / 2;
        //     l3 = tool->getPos().y + p;
        //     l4 = tool->getPos().y + size.height - p;
        //     l1 = tool->getPos().x;
        //     l2 = tool->getPos().x + size.width;
        // }
    } else {
        return;
    }
    ImageMeasurementDistanceTool* dTool;
    for (int i = 0; i < 2; ++ i) {
        dTool = new ImageMeasurementDistanceTool(uiHelper->canvas);
        dTool->setLineWeight(2, false);
        dTool->setColor(QColor(255, 255, 255, 125), false);
        dTool->setLineStyle(MTBS_DASH, false);
        dTool->setOverLine(MTBOP_CENTER, false);
        dTool->setMouseTransparent(true);

        if (i == 0) {
            dTool->setOrientation(Qt::Horizontal);
            dTool->setLine1(l1);
            dTool->setLine2(l2);
        } else {
            dTool->setOrientation(Qt::Vertical);
            dTool->setLine1(l3);
            dTool->setLine2(l4);
        }
        dataHelper->d->addMeasurementTool(dTool);
        dataHelper->ampCursors.results.append(dTool);
    }
    dataHelper->d->update();
}

void EdgeRoughnessCursorHandler::markEdgeLineTools() {
    dataHelper->markWidthLineTools();
}

void EdgeRoughnessCursorHandler::notifyAmpChanged(void* pPtr) {
    AmpEdgeRoughnessParam& edgeRoughnessParam = cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpEdgeRoughnessParam();
    if (pPtr == &edgeRoughnessParam.measurePosition) {
        dataHelper->stopAutoMeasure();
        return;
    }
    cdsem::AMS_Status amsStatus = cdsem::CDSEM_GlobalDataHelper::get()->getAmsStatus();
    if (amsStatus != cdsem::AMS_SCURSOR) {
        return;
    }
    uiHelper->canvasToolWidget->setVisible(edgeRoughnessParam.acdParam.acdType == cdsem::ACDT_MANUAL && edgeRoughnessParam.acdParam.target > 1);
    //modify direction
    if (/*pPtr != &edgeRoughnessParam.direction && */pPtr != &edgeRoughnessParam.designValue &&
        pPtr != &edgeRoughnessParam.cursorType && pPtr != &edgeRoughnessParam.searchArea &&
        pPtr != &edgeRoughnessParam.inspectArea && pPtr != &edgeRoughnessParam.object && pPtr != &edgeRoughnessParam.acdParam.acdType && pPtr != &edgeRoughnessParam.acdParam.target
        && pPtr != &edgeRoughnessParam.denseParam.cursorSizeX && pPtr != &edgeRoughnessParam.denseParam.cursorSizeY) {
        return;
    }
    if (edgeRoughnessParam.cursorType == cdsem::ABCT_TWO_BOX) {
        dataHelper->markMeasureCursor();
        ImageMeasurementDoubleRectTool* d;
        for (int i = 0; i < dataHelper->ampCursors.doubleBoxTools.size();  ++ i) {
            d = static_cast<ImageMeasurementDoubleRectTool*>(dataHelper->ampCursors.doubleBoxTools[i]);
            //modify direction
            // if (pPtr == &edgeRoughnessParam.direction || edgeRoughnessParam.direction != int(d->getOrientation()) - 1) {
            //     if (edgeRoughnessParam.direction == 0) {
            //         d->setOrientation(Qt::Horizontal, false);
            //     } else {
            //         d->setOrientation(Qt::Vertical, false);
            //     }
            //     if (dataHelper->assistRange.offset.x != nullptr) {
            //         MeasurementPoint point = dataHelper->calcCenterPos(dataHelper->getDoubleBoxSize(d), dataHelper->assistRange.size);
            //         d->move({point.x + dataHelper->assistRange.offset.x, point.y + dataHelper->assistRange.offset.y}, false);
            //     }
            // }
            if (pPtr == &edgeRoughnessParam.designValue || pPtr == &edgeRoughnessParam.cursorType) {
                d->blockSignals(true);
                if (edgeRoughnessParam.designValue != nullptr)
                    d->setDesignValueKeepCenter(cdsem::PixelDistanceConvert::instance()->getPixelSizeFromDistanceNM(edgeRoughnessParam.designValue, IOPT->getPixelSize()), false);
                d->blockSignals(false);
            }
            if (pPtr == &edgeRoughnessParam.searchArea || pPtr == &edgeRoughnessParam.cursorType) {
                d->setSearchArea(edgeRoughnessParam.searchArea, false);
            }
            if ((pPtr == &edgeRoughnessParam.inspectArea || pPtr == &edgeRoughnessParam.cursorType)) {
                d->setSumLine(edgeRoughnessParam.inspectArea, false);
            }
            if (pPtr == &edgeRoughnessParam.cursorType) {
                if (i < dataHelper->ampCursors.oneBoxTools.size()) {
                    d->move(static_cast<ImageMeasurementRectTool*>(dataHelper->ampCursors.oneBoxTools[i])->getPos(), false);
                }
            }
        }
        if (pPtr == &edgeRoughnessParam.cursorType) {
            dataHelper->exchangeCurrentAmpCursor(&dataHelper->ampCursors.doubleBoxTools);
        }
        dataHelper->d->update();
    } else if (edgeRoughnessParam.cursorType == cdsem::ABCT_ONE_BOX) {
        dataHelper->markMeasureCursor();
        //modify direction
        if (pPtr == &edgeRoughnessParam.denseParam.cursorSizeX || pPtr == &edgeRoughnessParam.denseParam.cursorSizeY || pPtr == &edgeRoughnessParam.object/* || pPtr == &edgeRoughnessParam.direction*/ || pPtr == &edgeRoughnessParam.searchArea || pPtr == &edgeRoughnessParam.inspectArea || pPtr == &edgeRoughnessParam.cursorType || pPtr == &edgeRoughnessParam.designValue) {
            ImageMeasurementRectTool* d;
            for (int i = 0; i < dataHelper->ampCursors.oneBoxTools.size(); ++ i) {
                d = static_cast<ImageMeasurementRectTool*>(dataHelper->ampCursors.oneBoxTools[i]);
                d->blockSignals(true);
                const MeasurementPoint& pos = d->getPos();
                const MeasurementSize& oldSize = d->getSize();
                MeasurementPoint centerPos = {pos.x + oldSize.width / 2, pos.y + oldSize.height / 2};
                if (i == 0) {
                    bool normalChange = false;
                    if (pPtr == &edgeRoughnessParam.object) {
                        if (cdsem::checkAmpWidthRoughnessObjectIsDense(edgeRoughnessParam.object)) {
                            d->setMaxSize({cdsem::AMP_WIDTH_DENES_MAX_CURSOR_X, cdsem::AMP_WIDTH_DENES_MAX_CURSOR_Y});
                            d->setMinSize({cdsem::AMP_WIDTH_DENES_MIN_CURSOR_X, cdsem::AMP_WIDTH_DENES_MIN_CURSOR_Y});
                        } else {
                            normalChange = true;
                        }
                        //modify direction
                    } else if (pPtr == &edgeRoughnessParam.cursorType/* || pPtr == &edgeRoughnessParam.direction*/ || pPtr == &edgeRoughnessParam.designValue) {
                        normalChange = true;
                    }
                    if (normalChange) {
                        int dv = cdsem::PixelDistanceConvert::instance()->getPixelSizeFromDistanceNM(edgeRoughnessParam.designValue, IOPT->getPixelSize());
                        //modify direction
                        // if (edgeRoughnessParam.direction == 0) {
                        d->setMaxSize({dv + cdsem::AMP_WIDTH_MAX_SEARCH_AREA, cdsem::AMP_WIDTH_MAX_SUM_LINE});
                        d->setMinSize({dv + cdsem::AMP_WIDTH_MIN_SEARCH_AREA, cdsem::AMP_WIDTH_MIN_SUM_LINE});
                        // } else {
                        //     d->setMaxSize({cdsem::AMP_WIDTH_MAX_SUM_LINE, dv + cdsem::AMP_WIDTH_MAX_SEARCH_AREA});
                        //     d->setMinSize({cdsem::AMP_WIDTH_MIN_SUM_LINE, dv + cdsem::AMP_WIDTH_MIN_SEARCH_AREA});
                        // }
                    }
                }

                if (cdsem::checkAmpWidthRoughnessObjectIsDense(edgeRoughnessParam.object)) {
                    d->setSize({edgeRoughnessParam.denseParam.cursorSizeX, edgeRoughnessParam.denseParam.cursorSizeY}, false);
                } else {
                    //modify direction
                    // if (edgeRoughnessParam.direction == 0) {
                    d->setSize({edgeRoughnessParam.searchArea + cdsem::PixelDistanceConvert::instance()->getPixelSizeFromDistanceNM(edgeRoughnessParam.designValue, IOPT->getPixelSize()), edgeRoughnessParam.inspectArea}, false);
                    // } else {
                    // d->setSize({edgeRoughnessParam.inspectArea, edgeRoughnessParam.searchArea + cdsem::PixelDistanceConvert::instance()->getPixelSizeFromDistanceNM(edgeRoughnessParam.designValue, IOPT->getPixelSize())}, false);
                    // }
                }

                const MeasurementSize& newSize = d->getSize();
                // modify direction
                if (/*pPtr == &edgeRoughnessParam.direction && */dataHelper->assistRange.offset.x != nullptr) {
                    // if (edgeRoughnessParam.direction == 0) {
                    d->setResizePolicy(MMD_TOP, MMD_TOP_BOTTOM);
                    d->setResizePolicy(MMD_BOTTOM, MMD_TOP_BOTTOM);
                    d->setResizePolicy(MMD_LEFT, MMD_LEFT);
                    d->setResizePolicy(MMD_RIGHT, MMD_RIGHT);
                    d->disableMove(MMMD_Vertical);
                    // } else {
                    //     d->setResizePolicy(MMD_TOP, MMD_TOP);
                    //     d->setResizePolicy(MMD_BOTTOM, MMD_BOTTOM);
                    //     d->setResizePolicy(MMD_LEFT, MMD_LEFT_RIGHT);
                    //     d->setResizePolicy(MMD_RIGHT, MMD_LEFT_RIGHT);
                    //     d->disableMove(MMMD_Horizontal);
                    // }
                    MeasurementPoint point = calcCenterPos(d->getSize(), dataHelper->assistRange.size);
                    d->move({point.x + dataHelper->assistRange.offset.x, point.y + dataHelper->assistRange.offset.y}, false);
                } else {
                    d->move({centerPos.x - newSize.width / 2, centerPos.y - newSize.height / 2}, false);
                }
                d->blockSignals(false);
                if (pPtr == &edgeRoughnessParam.cursorType) {
                    if (i < dataHelper->ampCursors.doubleBoxTools.size())
                        d->move(static_cast<ImageMeasurementDoubleRectTool*>(dataHelper->ampCursors.doubleBoxTools[i])->getLeftTopPos(), false);
                }
            }
            if (pPtr == &edgeRoughnessParam.cursorType) {
                dataHelper->exchangeCurrentAmpCursor(&dataHelper->ampCursors.oneBoxTools);
            }
            dataHelper->d->update();
        }
    }
    if (pPtr == &edgeRoughnessParam.cursorType) {
        dataHelper->lockAcdCursorsMouse(uiHelper->lockCursorMoveBtn->isChecked());
    }
}

void EdgeRoughnessCursorHandler::sortCursors() {
    AmpEdgeRoughnessParam& param = cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpEdgeRoughnessParam();
    if (param.acdParam.target <= 1 || param.acdParam.acdType != cdsem::ACDT_MANUAL) {
        return;
    }
    if (dataHelper->ampCursors.currentTools == nullptr || dataHelper->ampCursors.currentTools->size() < 1) {
        return;
    }
    QList<MeasurementPoint> list;
    list.reserve(dataHelper->ampCursors.currentTools->size());
    for (ImageMeasurementTool* &t : *dataHelper->ampCursors.currentTools) {
        if (param.cursorType == cdsem::ABCT_ONE_BOX) {
            ImageMeasurementRectTool* d = static_cast<ImageMeasurementRectTool*>(t);
            list.append(d->getPos());
        } else {
            ImageMeasurementDoubleRectTool* d = static_cast<ImageMeasurementDoubleRectTool*>(t);
            list.append(d->getLeftTopPos());
        }
    }
    std::sort(list.begin(), list.end(), &ImageOperationControllerDataHelper::ampCursorSortFunc);
    for (int i = 0; i < dataHelper->ampCursors.currentTools->size(); ++ i) {
        ImageMeasurementTool* t = (*dataHelper->ampCursors.currentTools)[i];
        if (param.cursorType == cdsem::ABCT_ONE_BOX) {
            ImageMeasurementRectTool* d = static_cast<ImageMeasurementRectTool*>(t);
            d->move(list[i]);
        } else {
            ImageMeasurementDoubleRectTool* d = static_cast<ImageMeasurementDoubleRectTool*>(t);
            d->move(list[i]);
        }
    }
}

void EdgeRoughnessCursorHandler::allAmpCursorToCenter() {
    AmpEdgeRoughnessParam& param = cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpEdgeRoughnessParam();
    if (param.acdParam.target <= 1 || param.acdParam.acdType != cdsem::ACDT_MANUAL) {
        return;
    }
    if (dataHelper->ampCursors.currentTools == nullptr || dataHelper->ampCursors.currentTools->size() < 1) {
        return;
    }
    MeasurementPoint pos;
    if (param.cursorType == cdsem::ABCT_ONE_BOX) {
        ImageMeasurementRectTool* d = static_cast<ImageMeasurementRectTool*>(dataHelper->ampCursors.currentTools->first());
        d->move(pos = calcCenterPos(d->getSize(), uiHelper->canvas), false);
    } else if (param.cursorType == cdsem::ABCT_TWO_BOX) {
        ImageMeasurementDoubleRectTool* d = static_cast<ImageMeasurementDoubleRectTool*>(dataHelper->ampCursors.currentTools->first());
        moveDoubleBoxToCenter(d, uiHelper->canvas, false);
        pos = d->getLeftTopPos();
    } else {
        return;
    }
    for (int i = 1; i < dataHelper->ampCursors.currentTools->size(); ++ i) {
        if (param.cursorType == cdsem::ABCT_ONE_BOX) {
            ImageMeasurementRectTool* d = static_cast<ImageMeasurementRectTool*>((*dataHelper->ampCursors.currentTools)[i]);
            d->move(pos, false);
        } else {
            ImageMeasurementDoubleRectTool* d = static_cast<ImageMeasurementDoubleRectTool*>((*dataHelper->ampCursors.currentTools)[i]);
            d->move(pos, false);
        }
    }
    dataHelper->d->update();
}

void EdgeRoughnessCursorHandler::lockAcdCursorsMouse(bool lock) {
    AmpEdgeRoughnessParam& param = cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpEdgeRoughnessParam();
    if (param.acdParam.acdType != cdsem::ACDT_MANUAL || param.acdParam.target <= 1 || dataHelper->ampCursors.currentTools == nullptr || dataHelper->ampCursors.currentTools->size() < 1) {
        return;
    }
    if (param.cursorType == cdsem::ABCT_ONE_BOX) {
        ImageMeasurementRectTool* d;
        for (int i = 1; i < dataHelper->ampCursors.currentTools->size(); ++ i) {
            d = static_cast<ImageMeasurementRectTool*>((*dataHelper->ampCursors.currentTools)[i]);
            d->setMouseTransparent(lock);
        }
    } else if (param.cursorType == cdsem::ABCT_TWO_BOX) {
        ImageMeasurementDoubleRectTool* d = static_cast<ImageMeasurementDoubleRectTool*>(dataHelper->ampCursors.currentTools->first());
        d->setLockDistance(lock);
        for (int i = 1; i < dataHelper->ampCursors.currentTools->size(); ++ i) {
            d = static_cast<ImageMeasurementDoubleRectTool*>((*dataHelper->ampCursors.currentTools)[i]);
            d->setMouseTransparent(lock);
        }
    }
}
