#include "idp_ap_add_reg_helper.h"
#include <QRect>
#include "cde90_message_box.h"
#include "image_info_service.h"
#include "image_measurement_rect_tool.h"
#include "image_measurement_cross_tool.h"
#include "image_operation_image_canvas.h"
#include "exception_code.h"
#include "sem_global.h"
#include "sem_thumbnail_widget.h"
#include "service_manager.h"
#include "cdsem_global_data.h"
#include "tmp_image_data_converter.h"
#include "image_file_temp_handler.h"
#include "cde90_process_box.h"
#include "image_save_model_convert.h"
#include "image_measurement.h"
#include "image_operation_relation.h"
#include "image_type_defition.h"

//这里截获Image是否已经处理
struct ImageHandleResult {
    bool over = false;
};

void regImageStepToGetImage(QPointer<QWidget> _this, const sem::ONE_PARAM_ARG_FUNC<NormalImageRegResult&>& workCallback, const sem::TWO_PARAM_ARG_FUNC<QImage&, const ImageAddtionDataModel&>& callback, const QSharedPointer<ImageHandleResult>& hasReadStatus = nullptr, const QSharedPointer<ImageHandleResult>& cancelStatus = nullptr, const QPointer<CDE90ProcessBox>& box = nullptr);

void drawRegImage(
    QWidget *_this,
    ImageOperationImageCanvas *&canvasPainter,
    SemThumbnailWidget *thumbnialCanvas,
    QImage &&image,
    const QRect &range);

int openSimpleMessage(
    QWidget *parent, const QString &text) {
    CDE90MessageBox d(parent);
    d.setWindowTitle("Waiting input");
    d.setText(text);
    d.addLeftButton("OK", true);
    d.addRightButton("Cancel", false);
    d.setResizable(false);
    d.getContent()->setStyleSheet(
        "QLabel {color: rgb(79, 73, 246);} QPushButton {color: rgb(79, 73, 246);}");
    return d.exec();
};

void idpToCreateAndDrawThumbnailCanvas(
    QWidget *_this,
    ImageOperationImageCanvas *&canvasPainter,
    SemThumbnailWidget *thumbnialCanvas,
    const QImage &image,
    const QRect &range) {
    if (!canvasPainter) {
        canvasPainter = new ImageOperationImageCanvas(_this);
    }
    int p = (image.width() - 1) / thumbnialCanvas->canvasWidth() + 1;
    p = std::min((image.height() - 1) / thumbnialCanvas->canvasHeight() + 1, p);
    canvasPainter->clearAndDeleteMeasurementTools();
    canvasPainter->setImage(image);
    if (range.isValid()) {
        ImageMeasurementRectTool *tool = new ImageMeasurementRectTool(canvasPainter);
        tool->setWeight(std::max(2, p * 2), false);
        tool->setOverLine(MTBOP_INNER, false);
        tool->move(MeasurementPoint{range.x(), range.y()}, false);
        tool->setSize(MeasurementSize{range.width(), range.height()});
        tool->setMouseTransparent(true);
        canvasPainter->addMeasurementTool(tool);
    } else {
        ImageMeasurementCrossTool* tool = new ImageMeasurementCrossTool(canvasPainter);
        tool->setWeight(std::max(2, p * 2), false);
        tool->setCenter(MeasurementPoint{image.width() / 2, image.height() / 2}, false);
        tool->toScreenMode(true, false);
        tool->setMouseTransparent(true);
        canvasPainter->addMeasurementTool(tool);
    }
    QImage result(image.width(), image.height(), image.format());
    canvasPainter->paint(result);
    thumbnialCanvas->setImage(result);
    canvasPainter->clearAndDeleteMeasurementTools();
}

void toRegImageStep1(NormalImageRegResult& result, SemRegModel mode);
void toRegImageStep2(NormalImageRegResult& result);
void toRegImageStep3(QWidget *_this,
                              ImageOperationImageCanvas *&canvasPainter,
                              SemThumbnailWidget *thumbnialCanvas, QImage&& image, NormalImageRegResult& result, const sem::ONE_PARAM_ARG_FUNC<NormalImageRegResult&>& callback, int skipStep);

void idpRegImage(SemRegModel mode, bool autoMode, QWidget *_this,
    ImageOperationImageCanvas *&canvasPainter,
    SemThumbnailWidget *thumbnialCanvas, const sem::ONE_PARAM_ARG_FUNC<NormalImageRegResult&>& callback, int skipStep) {
    NormalImageRegResult result;
    if (skipStep <= 0) {
        toRegImageStep1(result, mode);
        if (result.error != 0) {
            callback(result);
            return;
        }
    }

    if (!autoMode && skipStep <= 1) {
        toRegImageStep2(result);
        if (result.error != 0) {
            callback(result);
            return;
        }
        if (!result.area.isValid()) {
            result.error = INVALID_AREA_ERROR_CODE;
            callback(result);
            return;
        }
    }
    regImageStepToGetImage(_this, callback, [_this, &canvasPainter, thumbnialCanvas, callback, result, skipStep](QImage& image, const ImageAddtionDataModel& otherData) mutable{
        result.additionParam = otherData;
        toRegImageStep3(_this, canvasPainter, thumbnialCanvas, std::move(image), result, callback, skipStep);
    });
}

//第一步弹框与偏转电子束定位光标
void toRegImageStep1(NormalImageRegResult& result, SemRegModel mode) {
    bool succ = false;
    MainThreadInvoker::getInvoker().blockRun(
        [&succ]() {
            MsgReceiveStatus<RelationImageOperationController*> status = image_operation::openSingleImageOperation(true);
            if (!status.result) {
                simpleMessageBox(nullptr, "Error", "Error to open Image Operation Widnow.");
                return;
            }
            status.result->localCrossCursorMode();
            succ = true;
        });
    if (!succ) {
        result.error = LIBRARY_IS_NOT_ENALBED_ERROR_CODE;
        return;
    }
    QString name = "alignment";
    if (mode == SRM_ADDRESS) {
        name = "address";
    } else if (mode == SRM_MP) {
        name = "measurement";
    }
    int r = openSimpleMessage(
        nullptr, QString("Match the cross cursor with the %1 point, and click the [OK] button.").arg(name));
    if (r) {
        MainThreadInvoker::getInvoker().runLater(
            []() {
                MsgReceiveStatus<RelationImageOperationController*> status = image_operation::openSingleImageOperation(true);
                if (!status.result) {
                    return;
                }
                status.result->blankCursorMode();
            });
        result.error = CANCELED_ERROR_CODE;
        return;
    }
    //TODO 需要获取图像所在的坐标，分为stage和beam
}
//第二步弹框确认注册的图像区块
void toRegImageStep2(NormalImageRegResult& result) {
    bool succ = false;
    MainThreadInvoker::getInvoker().blockRun(
        [&succ]() {
            MsgReceiveStatus<RelationImageOperationController*> status = image_operation::openSingleImageOperation(true);
            if (!status.result) {
                simpleMessageBox(nullptr, "Error", "Error to open Image Operation Widnow.");
                return;
            }
            status.result->rectCursorMode();
            succ = true;
        });
    if (!succ) {
        result.error = LIBRARY_IS_NOT_ENALBED_ERROR_CODE;
        return;
    }
    int r = openSimpleMessage(nullptr,
                          "Bring the area cursor to the position for model registration, and "
                          "click the [OK] button.");
    if (r) {
        MainThreadInvoker::getInvoker().runLater(
            []() {
                MsgReceiveStatus<RelationImageOperationController*> status = image_operation::openSingleImageOperation(true);
                if (!status.result) {
                    return;
                }
                status.result->blankCursorMode();
            });
        result.error = CANCELED_ERROR_CODE;
        return;
    }
    MainThreadInvoker::getInvoker().blockRun(
        [&succ, &result]() {
            MsgReceiveStatus<RelationImageOperationController*> status = image_operation::openSingleImageOperation(true);
            if (!status.result) {
                simpleMessageBox(nullptr, "Error", "Error to open Image Operation Widnow.");
                succ = false;
                return;
            }
            result.area = status.result->getRectCursorRange();
            status.result->blankCursorMode();
            succ = true;
        });
    if (!succ) {
        result.error = LIBRARY_IS_NOT_ENALBED_ERROR_CODE;
    }
}

void regImageStepToGetImage(QPointer<QWidget> _this, const sem::ONE_PARAM_ARG_FUNC<NormalImageRegResult&>& workCallback, const sem::TWO_PARAM_ARG_FUNC<QImage&, const ImageAddtionDataModel&>& callback, const QSharedPointer<ImageHandleResult>& hasReadStatus, const QSharedPointer<ImageHandleResult>& cancelStatus, const QPointer<CDE90ProcessBox>& box) {
    int type = cdsem::CDSEM_GlobalDataHelper::get()->getImageOperationType();
    if (type == IMG_OPT_TYPE_OM) {
        cdsem::CDSEM_GlobalDataHelper::get()->getImage(IMG_OPT_TYPE_OM, [callback, _this, hasReadStatus, workCallback, cancelStatus, box](const cdsem::TmpImageData &imageData, const ImageAddtionDataModel& otherData) {
            QImage image;
            tmpToQImage(imageData, image);
            MainThreadInvoker::getInvoker().runLater([image, cancelStatus, callback, box, otherData]() mutable {
                if (box) box->overLoading();
                if (cancelStatus && cancelStatus->over) return;
                callback(image, otherData);
            });
            if (hasReadStatus) {
                hasReadStatus->over = true;
            }
        });
    } else if (type == IMG_OPT_TYPE_SEM) {
        cdsem::CDSEM_GlobalDataHelper::get()->getImage(IMG_OPT_TYPE_SEM, [callback, _this, hasReadStatus, workCallback, cancelStatus, box](const cdsem::TmpImageData &imageData, const ImageAddtionDataModel& otherData) {
            QImage image;
            tmpToQImage(imageData, image);
            MainThreadInvoker::getInvoker().runLater([image, cancelStatus, callback, box, otherData]() mutable {
                if (box) box->overLoading();
                if (cancelStatus && cancelStatus->over) return;
                callback(image, otherData);
            });
            if (hasReadStatus) {
                hasReadStatus->over = true;
            }
        });
    } else {
        if (box) box->overLoading();
        if (cancelStatus && cancelStatus->over) return;
        QImage image;
        callback(image, ImageAddtionDataModel {});
        if (hasReadStatus) {
            hasReadStatus->over = true;
        }
    }

}
//第四步评测图像
void toRegImageStep3(QWidget *_this,
    ImageOperationImageCanvas *&canvasPainter,
    SemThumbnailWidget *thumbnialCanvas, QImage&& image, NormalImageRegResult& result, const sem::ONE_PARAM_ARG_FUNC<NormalImageRegResult&>& callback, int skipStep) {
    if (image.isNull()) {
        result.error = NULL_POINTER_ERR_CODE;
        callback(result);
        return;
    }
    //TODO 代表图像中心所在晶圆的坐标
    // result.basePoint = center;
    //TODO 目前坎仅unique算法需要检查

    drawRegImage(_this, canvasPainter, thumbnialCanvas, std::move(image), result.area);
    if (skipStep <= 2) {
        float quality = algo::getPatternQuality(image, result.area);
        result.score = 0;
        result.quality = int(quality);
        result.error = 0;
        result.additionParam.markInfo.patternArea = result.area;
    }
    callback(result);
}

void drawRegImage(
    QWidget *_this,
    ImageOperationImageCanvas *&canvasPainter,
    SemThumbnailWidget *thumbnialCanvas,
    QImage &&image,
    const QRect &range) {
    if (!canvasPainter) {
        canvasPainter = new ImageOperationImageCanvas(_this);
    }
    int p = (image.width() - 1) / thumbnialCanvas->canvasWidth() + 1;
    p = std::min((image.height() - 1) / thumbnialCanvas->canvasHeight() + 1, p);
    canvasPainter->clearAndDeleteMeasurementTools();
    canvasPainter->setImage(image);
    if (range.isValid()) {
        ImageMeasurementRectTool *tool = new ImageMeasurementRectTool(canvasPainter);
        tool->setWeight(std::max(2, p * 2), false);
        tool->setOverLine(MTBOP_INNER, false);
        tool->move(MeasurementPoint{range.x(), range.y()}, false);
        tool->setSize(MeasurementSize{range.width(), range.height()});
        tool->setMouseTransparent(true);
        canvasPainter->addMeasurementTool(tool);
    } else {
        ImageMeasurementCrossTool* tool = new ImageMeasurementCrossTool(canvasPainter);
        tool->setWeight(std::max(2, p * 2), false);
        tool->setCenter(MeasurementPoint{image.width() / 2, image.height() / 2}, false);
        tool->toScreenMode(true, false);
        tool->setMouseTransparent(true);
        canvasPainter->addMeasurementTool(tool);
    }
    canvasPainter->paint(image);
    thumbnialCanvas->setImage(std::move(image));
    canvasPainter->clearAndDeleteMeasurementTools();
}

void resetImageFromDatabase(
    QWidget *_this,
    AsyncCaller *caller,
    quint64 *imageId,
    ImageAddtionDataModel& imageDetail,
    ImageOperationImageCanvas *&canvasPainter,
    SemThumbnailWidget *thumbnialCanvas,
    const QRect &rect,
    const std::function<void(bool)> &callback) {
    quint64 cImageId = *imageId;
    thumbnialCanvas->setText("Loading...", false);
    bool succ = caller->push(
        _this, [callback, rect, _this, thumbnialCanvas, imageId, cImageId, &canvasPainter, &imageDetail](TSC) {
            sem::AfterGc gc([thumbnialCanvas, imageId, cImageId]() {
                MainThreadInvoker::getInvoker().runLater([imageId, cImageId, thumbnialCanvas]() {
                    if (*imageId == cImageId) {
                        thumbnialCanvas->setText("");
                    }
                });
            });
            ImageInfoService *service = ServiceManager::instance()
                                            ->getDefaultService<ImageInfoService>();
            int errCode;
            ImageInfoEntity entity = service->getById(cImageId, &errCode);
            if (errCode) {
                MainThreadInvoker::getInvoker().blockRun([_this, &callback]() {
                    simpleMessageBox(_this, "Error", "Load image data error.");
                    if (callback) {
                        callback(false);
                    }
                });
                return;
            }
            if (entity.getId() == 0) {
                MainThreadInvoker::getInvoker().blockRun([_this, &callback]() {
                    simpleMessageBox(_this, "Error", "Lost image data.");
                    if (callback) {
                        callback(false);
                    }
                });
                return;
            }
            QImage image(entity.getWidth(),entity.getHeight(),
                        QImage::Format(entity.getImageFormat()));
            if (entity.getImageData().length() != image.sizeInBytes()) {
                MainThreadInvoker::getInvoker().blockRun([_this, &callback]() {
                    simpleMessageBox(_this, "Error", "Load image data error, data format error.");
                    if (callback) {
                        callback(false);
                    }
                });
                return;
            }
            std::memcpy(image.bits(),
                        entity.getImageData().constData(),
                        entity.getImageData().length());
            MainThreadInvoker::getInvoker().blockRun([&rect,
                                                     &callback,
                                                     &_this,
                                                     &image,
                                                     imageId,
                                                     &cImageId,
                                                     &canvasPainter,
                                                     &entity,
                                                     &imageDetail,
                                                     thumbnialCanvas]() {
                if (*imageId == cImageId) {
                    int errCode = fromImageDeitalDataModelJson(entity.getImageInfo(), imageDetail);
                    if (errCode) {
                        simpleMessageBox(_this, "Error", "Error to load image \"Scan\" data.");
                        if (callback) {
                            callback(false);
                        }
                        return;
                    }
                    idpToCreateAndDrawThumbnailCanvas(_this,
                                                      canvasPainter,
                                                      thumbnialCanvas,
                                                      image,
                                                      rect);
                    if (callback) {
                        callback(true);
                    }
                } else if (callback) {
                    callback(false);
                }
            });
        });
    if (!succ) {
        thumbnialCanvas->setText("");
        if (callback) {
            callback(false);
        }
    }
}
