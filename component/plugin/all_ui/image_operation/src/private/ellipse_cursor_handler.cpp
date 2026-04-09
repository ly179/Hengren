#include "ellipse_cursor_handler.h"
#include "amp_data_model.h"
#include "cdsem_data_common.h"
#include "cdsem_global_data.h"
#include "image_operation_controller_data_helper.h"
#include "image_operation_controller.h"
#include "image_measurement_rect_text_tool.h"
#include "cde90_button.h"
#include "sem_global.h"
#include "image_file_temp_handler.h"
#include "tmp_image_data_converter.h"
#include "sem_common_data_model.h"
#include "mat_measure.h"
#include "image_operation_widget_canvas.h"
#include "image_save_model.h"
#include "image_view_measure_tool_draw_helper.h"
#include "image_operation_controller_ui_helper.h"
#include <QPointer>

//为了方便修改，未来如果想对该功能添加direction，即X/Y方向切换支持时，搜索关键字：modify direction即可
#define uiHelper dataHelper->d->uiHelper

EllipseCursorHandler::EllipseCursorHandler(ImageOperationControllerDataHelper* dataHelper) : dataHelper(dataHelper) {}

EllipseCursorHandler::~EllipseCursorHandler() noexcept {}

void EllipseCursorHandler::doAutoMeasure(const sem::ONE_PARAM_ARG_FUNC<const OperationResult&>& callback, const cdsem::TmpImageData& imageData, const ImageAddtionDataModel& addition) {
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
        AmpEllipseParam& ellipseParam = cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpEllipseParam();
        collectCenters();
        if (ellipseParam.acdParam.centers.size() > 0) {
            ImageResultData data = HRMat::measureEllipseImage(image, pixelSize, ellipseParam, {});
            qDebug() << "测量完毕";
            dataHelper->d->setMeasureResult(data);
            dataHelper->removeAllAmpCursor();
        } else {
            dataHelper->stopAutoMeasure();
        }
    });
}

void EllipseCursorHandler::collectCenters() {
    AmpEllipseParam& ellipseParam = cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpEllipseParam();
    QList<QPoint> centers;
    QPoint center;
    if (dataHelper->ampCursors.oneBoxTools.isEmpty()) {
        ellipseParam.acdParam.centers.clear();
        return;
    }
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
    ellipseParam.acdParam.centers = centers;
}

void EllipseCursorHandler::markEdgeCursors() {
    AmpEllipseParam& ellipseParam = cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpEllipseParam();
    dataHelper->recordAssistCenterPos();
    int num = ellipseParam.acdParam.acdType == cdsem::ACDT_MANUAL ? ellipseParam.acdParam.target : 1;
    uiHelper->canvasToolWidget->setVisible(ellipseParam.acdParam.acdType == cdsem::ACDT_MANUAL && num > 1);
    bool needExchange = dataHelper->ampCursors.oneBoxTools.isEmpty();
    markEdgeOneBoxTools(ellipseParam);
    if (needExchange) {
        dataHelper->exchangeCurrentAmpCursor(&dataHelper->ampCursors.oneBoxTools);
    }
    if (dataHelper->ampCursors.distanceTools.size() > 0) {
        dataHelper->removeAllAmpCursor(dataHelper->ampCursors.distanceTools);
    }
    cdsem::CDSEM_GlobalDataHelper::get()->setAmsStatus(cdsem::AMS_SCURSOR);
    dataHelper->d->update();
}
void EllipseCursorHandler::markEdgeOneBoxTools(AmpEllipseParam& ellipseParam) {
    int num = ellipseParam.acdParam.acdType == cdsem::ACDT_MANUAL ? ellipseParam.acdParam.target : 1;
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
                    if (mType != cdsem::MT_ELLIPSE) {
                        return;
                    }
                    AmpEllipseParam& param = cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpEllipseParam();
                    const MeasurementSize& size = measurementTool->getSize();
                    param.cursorSizeX = size.width;
                    param.cursorSizeY = size.height;
                    emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(dataHelper->d), &param.cursorSizeX);
                    emit cdsem::CDSEM_GlobalDataHelper::get()->tmpAmpChanged(QPointer<QObject>(dataHelper->d), &param.cursorSizeY);
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
            measurementTool->setResizePolicy(MMD_TOP, MMD_TOP_BOTTOM);
            measurementTool->setResizePolicy(MMD_BOTTOM, MMD_TOP_BOTTOM);
            measurementTool->setResizePolicy(MMD_LEFT, MMD_LEFT_RIGHT);
            measurementTool->setResizePolicy(MMD_RIGHT, MMD_LEFT_RIGHT);

            measurementTool->setInnerTopTextOption(option, false);
            measurementTool->setInnerTopText(QString::number(dataHelper->ampCursors.oneBoxTools.size() + 1), false);
            measurementTool->setMarginBottom(cdsem::AMP_CURSOR_MARGIN);
            measurementTool->setMarginLeft(cdsem::AMP_CURSOR_MARGIN);
            measurementTool->setMarginTop(cdsem::AMP_CURSOR_MARGIN);
            measurementTool->setMarginRight(cdsem::AMP_CURSOR_MARGIN);

            measurementTool->setMaxSize({cdsem::AMP_ELLIPSE_CURSOR_MAX, cdsem::AMP_ELLIPSE_CURSOR_MAX});
            measurementTool->setMinSize({cdsem::AMP_ELLIPSE_CURSOR_MIN, cdsem::AMP_ELLIPSE_CURSOR_MIN});

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
        ImageMeasurementRectTextTool* measurementTool = static_cast<ImageMeasurementRectTextTool*>(dataHelper->ampCursors.oneBoxTools[i]);
        measurementTool->blockSignals(true);
        FloatNumber2 width = ellipseParam.cursorSizeX;
        FloatNumber2 height = ellipseParam.cursorSizeY;

        measurementTool->setSize({width, height});

        if (dataHelper->assistRange.offset.x != nullptr) {
            //modify direction
            // if (ellipseParam.direction == 0) {

            measurementTool->disableMove(MMMD_Vertical);
            // } else {
            //     measurementTool->setResizePolicy(MMD_LEFT, MMD_LEFT_RIGHT);
            //     measurementTool->setResizePolicy(MMD_RIGHT, MMD_LEFT_RIGHT);
            //     measurementTool->disableMove(MMMD_Horizontal);
            // }
            MeasurementPoint point = calcCenterPos(measurementTool->getSize(), dataHelper->assistRange.size);
            measurementTool->move({point.x + dataHelper->assistRange.offset.x, point.y + dataHelper->assistRange.offset.y});
        } else if (ellipseParam.acdParam.centers.size() > 0 && i < ellipseParam.acdParam.centers.size()){
            moveMeasuremntToolByCenter(measurementTool, ellipseParam.acdParam.centers[i], false);
        } else {
            measurementTool->move(calcCenterPos(measurementTool->getSize(), uiHelper->canvas));
        }

        measurementTool->blockSignals(false);
    }
}


void EllipseCursorHandler::notifyAmpChanged(void* pPtr) {
    AmpEllipseParam& ellipseParam = cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpEllipseParam();
    cdsem::AMS_Status amsStatus = cdsem::CDSEM_GlobalDataHelper::get()->getAmsStatus();
    if (amsStatus != cdsem::AMS_SCURSOR) {
        return;
    }
    uiHelper->canvasToolWidget->setVisible(ellipseParam.acdParam.acdType == cdsem::ACDT_MANUAL && ellipseParam.acdParam.target > 1);
    if (pPtr != &ellipseParam.cursorSizeX &&
        pPtr != &ellipseParam.cursorSizeY && pPtr != &ellipseParam.acdParam.acdType
        && pPtr != &ellipseParam.acdParam.target) {
        return;
    }
    dataHelper->markMeasureCursor();
    ImageMeasurementRectTool* d;
    for (int i = 0; i < dataHelper->ampCursors.oneBoxTools.size(); ++ i) {
        d = static_cast<ImageMeasurementRectTool*>(dataHelper->ampCursors.oneBoxTools[i]);
        d->blockSignals(true);
        const MeasurementPoint& pos = d->getPos();
        const MeasurementSize& oldSize = d->getSize();
        MeasurementPoint centerPos = {pos.x + oldSize.width / 2, pos.y + oldSize.height / 2};
        d->setSize({ellipseParam.cursorSizeX, ellipseParam.cursorSizeY}, false);
        const MeasurementSize& newSize = d->getSize();
        d->move({centerPos.x - newSize.width / 2, centerPos.y - newSize.height / 2}, false);
        d->blockSignals(false);
    }
    dataHelper->d->update();
}

void EllipseCursorHandler::sortCursors() {
    AmpEllipseParam& param = cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpEllipseParam();
    if (param.acdParam.target <= 1 || param.acdParam.acdType != cdsem::ACDT_MANUAL) {
        return;
    }
    if (dataHelper->ampCursors.currentTools == nullptr || dataHelper->ampCursors.currentTools->size() < 1) {
        return;
    }
    QList<MeasurementPoint> list;
    list.reserve(dataHelper->ampCursors.currentTools->size());
    for (ImageMeasurementTool* &t : *dataHelper->ampCursors.currentTools) {
        ImageMeasurementRectTool* d = static_cast<ImageMeasurementRectTool*>(t);
        list.append(d->getPos());
    }
    std::sort(list.begin(), list.end(), &ImageOperationControllerDataHelper::ampCursorSortFunc);
    for (int i = 0; i < dataHelper->ampCursors.currentTools->size(); ++ i) {
        ImageMeasurementTool* t = (*dataHelper->ampCursors.currentTools)[i];
        ImageMeasurementRectTool* d = static_cast<ImageMeasurementRectTool*>(t);
        d->move(list[i]);
    }
}
void EllipseCursorHandler::allAmpCursorToCenter() {
    AmpEllipseParam& param = cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpEllipseParam();
    if (param.acdParam.target <= 1 || param.acdParam.acdType != cdsem::ACDT_MANUAL) {
        return;
    }
    if (dataHelper->ampCursors.currentTools == nullptr || dataHelper->ampCursors.currentTools->size() < 1) {
        return;
    }
    MeasurementPoint pos;
    ImageMeasurementRectTool* d = static_cast<ImageMeasurementRectTool*>(dataHelper->ampCursors.currentTools->first());
    d->move(pos = calcCenterPos(d->getSize(), uiHelper->canvas), false);
    for (int i = 1; i < dataHelper->ampCursors.currentTools->size(); ++ i) {
        ImageMeasurementRectTool* d = static_cast<ImageMeasurementRectTool*>((*dataHelper->ampCursors.currentTools)[i]);
        d->move(pos, false);
    }
    dataHelper->d->update();
}
void EllipseCursorHandler::lockAcdCursorsMouse(bool lock) {
    AmpEllipseParam& param = cdsem::CDSEM_GlobalDataHelper::get()->getTmpAmpEllipseParam();
    if (param.acdParam.acdType != cdsem::ACDT_MANUAL || param.acdParam.target <= 1 || dataHelper->ampCursors.currentTools == nullptr || dataHelper->ampCursors.currentTools->size() < 1) {
        return;
    }
    ImageMeasurementRectTool* d;
    for (int i = 1; i < dataHelper->ampCursors.currentTools->size(); ++ i) {
        d = static_cast<ImageMeasurementRectTool*>((*dataHelper->ampCursors.currentTools)[i]);
        d->setMouseTransparent(lock);
    }
}
