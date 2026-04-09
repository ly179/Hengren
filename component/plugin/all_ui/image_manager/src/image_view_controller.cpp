#include "image_view_controller.h"
#include "ui_image_view_controller.h"
#include "service_manager.h"
#include "image_info_service.h"
#include "image_file_temp_handler.h"
#include "image_save_model_convert.h"
#include "tmp_image_data_converter.h"
#include "image_save_model.h"
#include "cde90_message_box.h"
#include "cde90_process_box.h"
#include "image_operation_widget_canvas.h"
#include "stream_image_supplier.h"
#include "sem_global.h"
#include "private/image_info_controller.h"
#include "image_view_measure_tool_draw_helper.h"
#include "image_measurement_rect_tool.h"
#include "image_measurement_double_rect_tool.h"
#include "image_measurement_annulus_tool.h"
#include "image_measurement_distance_tool.h"
#include "image_measurement_cross_tool.h"
#include "image_measurement_result_tool.h"
#include "private/image_overlay_controller.h"
#include "float_number_formatter.h"
#include "loading_helper.h"
#include "cdsem_global_data.h"
#include "image_operation_data_model.h"
#include "image_type_defition.h"
#include <QFileInfo>
#include <QStyle>
#include <QLayout>
#include <QPainter>

#define LOAD_IMG_GID 1

class ImageCanvas : public ImageOperationWidgetCanvas {
private:
    LoadingHelper* loadingHelper;
    qreal deviceRatio;

    void updateLoadingView() {
        loadingHelper->setRadius(30 * deviceRatio);
        loadingHelper->setPRadius(3 * deviceRatio);
        loadingHelper->setPBorder(1 * deviceRatio, QColor(180, 180, 180));
        loadingHelper->setCanvasSizeFunc([this](){
            return QSize(canvasWidth(), canvasHeight());
        });
    }
public:
    explicit ImageCanvas(QWidget* parent = nullptr) : ImageOperationWidgetCanvas(parent) {
        loadingHelper = new LoadingHelper(this);
        deviceRatio = devicePixelRatio();
        updateLoadingView();
        connect(loadingHelper, &LoadingHelper::timerUpdate, this, [this](const QRect& rect){
            qreal radio = this->getDevicePixelRatioCache();
            if (radio == 0) radio = devicePixelRatio();
            this->update(QRect(rect.x() / radio,
                               rect.y() / radio,
                               rect.width() / radio,
                               rect.height() / radio
            ));
        });
    }
    ~ImageCanvas() noexcept {}

    void toLoadingStatus(bool b = true) {
        if (b) loadingHelper->exec();
        else loadingHelper->block();
    }

protected:
    void paintEvent(QPaintEvent* event) override {
        if (loadingHelper->isActive()) {
            refreshMapFromRoot(true);
            painterBegin();
            painter->fillRect(QRect(0, 0, canvasWidthInPainting(), canvasHeightInPainting()), Qt::black);
            loadingHelper->paintLoadIcon(*painter);
            painter->end();
        } else {
            ImageOperationWidgetCanvas::paintEvent(event);
        }
    }
};

struct OverlayControlToolModel : public OverlayControlModel {
    QList<ImageMeasurementTool*> edgeTools;
    QList<ImageMeasurementTool*> designTools;
    QList<ImageMeasurementTool*> measureCursorTools;
    ImageMeasurementResultTool* resultTool = nullptr;
};

struct ImageViewControllerDataHelper {
    //如果write image fail,则会暂存这里，否则清除。
    cdsem::TmpImageData tmpData;
    QHash<OverlaySelectedType, bool> defaultOverlayConfig;
    QString imageName;
    QList<cdsem::ImageViewListData> imageList;
    QList <OverlayControlToolModel> overlayControls;
    ImageMeasurementTool* patternOverlayTool = nullptr;
    ImageCanvas* canvas;
    ImageAddtionDataModel addition;
    ImageViewController* d;
    ImageInfoController* infoController = nullptr;
    ImageOverlayController* overlayController = nullptr;
    //设定version，用于检测是否需要重新推送image list
    quint64 version = 0;
    int selectedRow = 0;
    int currentNo = 0;

    //当path/data和id联合加载时，防止加载顺序问题错误
    int loadVersion = 0;

    void drawPatternMatch() {
        if (d->ui->patternAreaCBox->isChecked() && !d->ui->eraseOvelayCBox->isChecked()) {
            if (patternOverlayTool) {
                canvas->addMeasurementTool(patternOverlayTool);
            } else {
                ImageMarkData& markData = addition.markInfo;
                if (markData.patternArea.isValid()) {
                    markSimpleBoxCursor(canvas, markData.patternArea, reinterpret_cast<ImageMeasurementRectTool**>(&patternOverlayTool));
                }
            }
        } else if (patternOverlayTool) {
            canvas->removeMeasurementTool(patternOverlayTool);
        }

    }

    void drawDesign(const ImageMeasureCenters& c, OverlayControlToolModel& toolModel) {
        if (c.l1 == nullptr || c.l2 == nullptr || c.l3 == nullptr || c.l4 == nullptr) return;
        ImageMeasurementDistanceTool* dTool;
        for (int i = 0; i < 2; ++ i) {
            dTool = new ImageMeasurementDistanceTool(canvas);
            dTool->setLineWeight(2, false);
            dTool->setColor(QColor(255, 255, 255, 125), false);
            dTool->setLineStyle(MTBS_DASH, false);
            dTool->setOverLine(MTBOP_CENTER, false);
            dTool->setMouseTransparent(true);
            if (i == 0) {
                dTool->setOrientation(Qt::Horizontal);
                dTool->setLine1(c.l1);
                dTool->setLine2(c.l2);
            } else {
                dTool->setOrientation(Qt::Vertical);
                dTool->setLine1(c.l3);
                dTool->setLine2(c.l4);
            }
            toolModel.designTools.append(dTool);
            canvas->addMeasurementTool(dTool);
        }
    }

    void drawMeasureCursor(ImageMeasureMarkData& m, OverlayControlToolModel& overlayModel) {
        if (overlayModel.cursorMark == OVS_CHECKED && !d->ui->eraseOvelayCBox->isChecked()) {
            if (overlayModel.measureCursorTools.isEmpty()) {
                if (m.cursorSize.cursorType == IMMCT_DOUBLE_BOX) {
                    QList<ImageMeasurementDoubleRectTool*> list;
                    markDoubleBoxCursor(canvas, m, &list);
                    for (ImageMeasurementDoubleRectTool*& t : list) {
                        overlayModel.measureCursorTools.append(t);
                    }
                } else if (m.cursorSize.cursorType == IMMCT_SINGLE_BOX) {
                    QList<ImageMeasurementRectTool*> list;
                    markSimpleBoxCursor(canvas, m, &list);
                    for (ImageMeasurementRectTool*& t : list) {
                        overlayModel.measureCursorTools.append(t);
                    }
                } else if (m.cursorSize.cursorType == IMMCT_ANNULAR) {
                    QList<ImageMeasurementAnnulusTool*> list;
                    markAnnulayBoxCursor(canvas, m, &list);
                    for (ImageMeasurementAnnulusTool*& t : list) {
                        overlayModel.measureCursorTools.append(t);
                    }
                }
            } else {
                for (ImageMeasurementTool*& t : overlayModel.measureCursorTools) {
                    canvas->addMeasurementTool(t);
                }
            }
        } else if (overlayModel.measureCursorTools.size() > 0) {
            for (ImageMeasurementTool*& t : overlayModel.measureCursorTools) {
                canvas->removeMeasurementTool(t);
            }
        }
    }

    void drawEdgeCrossCursors(ImageMeasureMarkData& m, OverlayControlToolModel& overlayModel) {
        if (overlayModel.edgeCrossMark == OVS_CHECKED && !d->ui->eraseOvelayCBox->isChecked()) {
            if (overlayModel.edgeTools.isEmpty()) {
                QList<ImageMeasurementCrossTool*> list;
                drawResultCross(canvas, m.resultData.measureResults, &list);
                if (list.size() > 0) {
                    for (ImageMeasurementCrossTool*& c : list) {
                        overlayModel.edgeTools.append(c);
                    }
                }
            } else {
                for (ImageMeasurementTool*& t : overlayModel.edgeTools) {
                    canvas->addMeasurementTool(t);
                }
            }
        } else if (overlayModel.edgeTools.size() > 0) {
            for (ImageMeasurementTool*& t : overlayModel.edgeTools) {
                canvas->removeMeasurementTool(t);
            }
        }
    }

    void drawDesignCursors(ImageMeasureMarkData& m, OverlayControlToolModel& overlayModel) {
        if (overlayModel.designMark == OVS_CHECKED && !d->ui->eraseOvelayCBox->isChecked()) {
            if (overlayModel.designTools.isEmpty()) {
                for (ImageMeasureCenters& c : m.centers) {
                    drawDesign(c, overlayModel);
                }
            } else {
                for (ImageMeasurementTool*& t : overlayModel.designTools) {
                    canvas->addMeasurementTool(t);
                }
            }
        } else if (overlayModel.designTools.size() > 0) {
            for (ImageMeasurementTool*& t : overlayModel.designTools) {
                canvas->removeMeasurementTool(t);
            }
        }
    }

    void drawProfiles() {
        ImageMarkData& markData = addition.markInfo;
        if (markData.measureCursors.size() > 0) {
            QList<QList<QPointF>> profiles;
            profiles.reserve(markData.measureCursors.size());
            for (int i = 0; i < markData.measureCursors.size(); ++ i) {
                if (i != selectedRow) continue;
                OverlayControlToolModel& overlayModel = overlayControls[i];
                if (overlayModel.profileLine != OVS_CHECKED || d->ui->eraseOvelayCBox->isChecked()) continue;
                ImageMeasureMarkData& m = markData.measureCursors[i];
                if (m.resultData.profile.size() > 0) {
                    profiles.append(m.resultData.profile);
                }
            }
            canvas->updateProfileLines(profiles, QColor(255, 255, 255, 125), false);
        }
    }

    void drawResult(ImageMeasureMarkData& m, OverlayControlToolModel& overlayModel) {
        if (overlayModel.viewResult == OVS_CHECKED && !d->ui->eraseOvelayCBox->isChecked()) {
            if (overlayModel.resultTool == nullptr) {

                QString text;
                qreal result = m.resultData.result;
                Qt::GlobalColor color;
                if (result < 0) {
                    text = "Error";
                    color = Qt::red;
                } else {
                    text = toMinQString(result, 3).append("nm");
                    color = Qt::white;
                }

                overlayModel.resultTool = new ImageMeasurementResultTool(canvas);
                overlayModel.resultTool->setMargin(MeasurementMargin {10, 10, 10, 10}, false);
                overlayModel.resultTool->setFontSize(16, false);
                overlayModel.resultTool->setColor(color, false);
                overlayModel.resultTool->setTextAlignment(MRTO_RIGHT_BOTTOM, false);
                overlayModel.resultTool->setText(text, false);
                canvas->addMeasurementTool(overlayModel.resultTool);
            } else {
                canvas->addMeasurementTool(overlayModel.resultTool);
            }
        } else if (overlayModel.resultTool) {
            canvas->removeMeasurementTool(overlayModel.resultTool);
        }

    }

    void drawMeasureOverlayCursors() {
        ImageMarkData& markData = addition.markInfo;
        if (markData.measureCursors.size() > 0) {
            for (int i = 0; i < markData.measureCursors.size(); ++ i) {
                if (i != selectedRow) continue;
                ImageMeasureMarkData& m = markData.measureCursors[i];
                OverlayControlToolModel& overlayModel = overlayControls[i];
                drawMeasureCursor(m, overlayModel);
                drawEdgeCrossCursors(m, overlayModel);
                drawDesignCursors(m, overlayModel);
                drawResult(m, overlayModel);
            }
            drawProfiles();
        }
    }

    void drawImage(const cdsem::TmpImageData& data) {
        bool succ;
        if (data.originSize.isNull()) {
            succ = canvas->imageStream()->writeImageFromSupplier(data, addition.originSize, cdsem::SIT_OTHER);
        } else {
            succ = canvas->imageStream()->writeImageFromSupplier(data, cdsem::SIT_OTHER);
        }
        //这里必须保留一份数据副本，用于sendImage。
        if (&data != &tmpData)
            tmpData = data;
        if (succ) {
            canvas->update();
        }
        if (!addition.realSize.isValid()) {
            addition.realSize = data.size;
        }
        if (!addition.originSize.isValid()) {
            addition.originSize = data.originSize;
        }
        if (d->ui->eraseOvelayCBox->isChecked()) {
            return;
        }
        drawOverlay();
    }

    void drawOverlay() {
        drawPatternMatch();
        drawMeasureOverlayCursors();
        canvas->update();
    }

    void clear() {
        if (patternOverlayTool) {
            canvas->removeMeasurementTool(patternOverlayTool);
            delete patternOverlayTool;
            patternOverlayTool = nullptr;
        }
        if (overlayControls.size() > 0) {
            for (OverlayControlToolModel& d : overlayControls) {
                if (d.edgeTools.size() > 0) {
                    for (ImageMeasurementTool*& t : d.edgeTools) {
                        canvas->removeMeasurementTool(t);
                    }
                    qDeleteAll(d.edgeTools.begin(), d.edgeTools.end());
                }
                if (d.designTools.size() > 0) {
                    for (ImageMeasurementTool*& t : d.designTools) {
                        canvas->removeMeasurementTool(t);
                    }
                    qDeleteAll(d.designTools.begin(), d.designTools.end());
                }
                if (d.measureCursorTools.size() > 0) {
                    for (ImageMeasurementTool*& t : d.measureCursorTools) {
                        canvas->removeMeasurementTool(t);
                    }
                    qDeleteAll(d.measureCursorTools.begin(), d.measureCursorTools.end());
                }
                if (d.resultTool) {
                    canvas->removeMeasurementTool(d.resultTool);
                    delete d.resultTool;
                    d.resultTool = nullptr;
                }
            }
            overlayControls.clear();
        }
        canvas->clearProfileLines();
    }

    void openInfoController() {
        if (!infoController) {
            infoController = new ImageInfoController;
            infoController->setWindowTitle("Information");
            infoController->initAsyncCaller(d->getAsyncCaller(), d);
            infoController->setCloseCall([this](CDE90WindowCloseStatus){
                removeCssClass(d->ui->infoBtn->getQPushButton(), "GreenBg");
                d->ui->infoBtn->getQPushButton()->style()->unpolish(d->ui->infoBtn->getQPushButton());
                d->ui->infoBtn->getQPushButton()->style()->polish(d->ui->infoBtn->getQPushButton());
                return true;
            });
        }
        if (infoController->isHidden()) {
            infoController->show();
            addCssClass(d->ui->infoBtn->getQPushButton(), "GreenBg");
            d->ui->infoBtn->getQPushButton()->style()->unpolish(d->ui->infoBtn->getQPushButton());
            d->ui->infoBtn->getQPushButton()->style()->polish(d->ui->infoBtn->getQPushButton());
        } else if (!infoController->isActiveWindow()) {
            infoController->activateWindow();
        }
    }

    void hideEntireRowCursors(int i) {
        if (i < 0 || i >= overlayControls.size()) return;
        OverlayControlToolModel& overlay = overlayControls[i];
        if (overlay.edgeTools.size() > 0) {
            for (ImageMeasurementTool*& t : overlay.edgeTools) {
                canvas->removeMeasurementTool(t);
            }
        }
        if (overlay.designTools.size() > 0) {
            for (ImageMeasurementTool*& t : overlay.designTools) {
                canvas->removeMeasurementTool(t);
            }
        }
        if (overlay.measureCursorTools.size() > 0) {
            for (ImageMeasurementTool*& t : overlay.measureCursorTools) {
                canvas->removeMeasurementTool(t);
            }
        }
        if (overlay.resultTool) {
            canvas->removeMeasurementTool(overlay.resultTool);
        }
    }

    void doSelectData(int i) {
        if (i < 0 || i >= overlayControls.size()) return;
        ImageMarkData& markData = addition.markInfo;
        if (i >= markData.measureCursors.size()) return;
        if (i != selectedRow) {
            OverlayControlToolModel& overlay = overlayControls[i];
            ImageMeasureMarkData& m = markData.measureCursors[i];
            hideEntireRowCursors(selectedRow);
            selectedRow = i;
            drawProfiles();
            drawMeasureCursor(m, overlay);
            drawDesignCursors(m, overlay);
            drawEdgeCrossCursors(m, overlay);
            drawResult(m, overlay);
            canvas->update();
        }

    }

    void openOverlayController() {
        if (!overlayController) {
            overlayController = new ImageOverlayController;
            overlayController->setWindowTitle("Overlay Control");
            overlayController->initAsyncCaller(d->getAsyncCaller(), d);
            overlayController->setCloseCall([this](CDE90WindowCloseStatus){
                removeCssClass(d->ui->overlayControlBtn->getQPushButton(), "GreenBg");
                d->ui->overlayControlBtn->getQPushButton()->style()->unpolish(d->ui->overlayControlBtn->getQPushButton());
                d->ui->overlayControlBtn->getQPushButton()->style()->polish(d->ui->overlayControlBtn->getQPushButton());
                return true;
            });

            QObject::connect(overlayController, &ImageOverlayController::overlaySettingChanged, d, [this](OverlaySelectedType t, bool checked){
                defaultOverlayConfig[t] = checked;
            });

            QObject::connect(overlayController, &ImageOverlayController::overlaySelectedRow, d, [this](int i){
                doSelectData(i);
            });

            QObject::connect(overlayController, &ImageOverlayController::overlaySelectedType, d, [this](int i, OverlaySelectedType type, bool checked){
                if (i < 0 || i >= overlayControls.size()) return;
                ImageMarkData& markData = addition.markInfo;
                if (i >= markData.measureCursors.size()) return;
                OverlayControlToolModel& overlay = overlayControls[i];
                ImageMeasureMarkData& m = markData.measureCursors[i];
                if (i != selectedRow) {
                    hideEntireRowCursors(selectedRow);
                    selectedRow = i;

                    switch (type) {
                    case OST_PROFILE_LINE:
                        overlay.profileLine = checked ? OVS_CHECKED : OVS_EXISTS;
                        break;
                    case OST_CURSOR_MARK:
                        overlay.cursorMark = checked ? OVS_CHECKED : OVS_EXISTS;
                        break;
                    case OST_DESIGN_MARK:
                        overlay.designMark = checked ? OVS_CHECKED : OVS_EXISTS;
                        break;
                    case OST_EDGE_CROSS_MARK:
                        overlay.edgeCrossMark = checked ? OVS_CHECKED : OVS_EXISTS;
                        break;
                    case OST_VIEW_RESULT:
                        overlay.viewResult = checked ? OVS_CHECKED : OVS_EXISTS;
                        break;
                    default:
                        break;
                    }


                    drawProfiles();
                    drawMeasureCursor(m, overlay);
                    drawDesignCursors(m, overlay);
                    drawEdgeCrossCursors(m, overlay);
                    drawResult(m, overlay);
                    canvas->update();
                    return;
                }
                switch (type) {
                case OST_PROFILE_LINE:
                    if (overlay.profileLine == OVS_CHECKED) {
                        if (!checked) {
                            overlay.profileLine = OVS_EXISTS;
                            drawProfiles();
                        }
                    } else if (overlay.profileLine == OVS_EXISTS) {
                        if (checked) {
                            overlay.profileLine = OVS_CHECKED;
                            drawProfiles();
                        }
                    }
                    break;
                case OST_CURSOR_MARK:
                    if (overlay.cursorMark == OVS_CHECKED) {
                        if (!checked) {
                            overlay.cursorMark = OVS_EXISTS;
                            drawMeasureCursor(m, overlay);
                        }
                    } else if (overlay.cursorMark == OVS_EXISTS) {
                        if (checked) {
                            overlay.cursorMark = OVS_CHECKED;
                            drawMeasureCursor(m, overlay);
                        }
                    }
                    break;
                case OST_DESIGN_MARK:
                    if (overlay.designMark == OVS_CHECKED) {
                        if (!checked) {
                            overlay.designMark = OVS_EXISTS;
                            drawDesignCursors(m, overlay);
                        }
                    } else if (overlay.designMark == OVS_EXISTS) {
                        if (checked) {
                            overlay.designMark = OVS_CHECKED;
                            drawDesignCursors(m, overlay);
                        }
                    }
                    break;
                case OST_EDGE_CROSS_MARK:
                    if (overlay.edgeCrossMark == OVS_CHECKED) {
                        if (!checked) {
                            overlay.edgeCrossMark = OVS_EXISTS;
                            drawEdgeCrossCursors(m, overlay);
                        }
                    } else if (overlay.edgeCrossMark == OVS_EXISTS) {
                        if (checked) {
                            overlay.edgeCrossMark = OVS_CHECKED;
                            drawEdgeCrossCursors(m, overlay);
                        }
                    }
                    break;
                case OST_VIEW_RESULT:
                    if (overlay.viewResult == OVS_CHECKED) {
                        if (!checked) {
                            overlay.viewResult = OVS_EXISTS;
                            drawResult(m, overlay);
                        }
                    } else if (overlay.viewResult == OVS_EXISTS) {
                        if (checked) {
                            overlay.viewResult = OVS_CHECKED;
                            drawResult(m, overlay);
                        }
                    }
                    break;
                default:
                    return;
                }
                canvas->update();
            });
        }
        if (overlayController->isHidden()) {
            overlayController->show();
            addCssClass(d->ui->overlayControlBtn->getQPushButton(), "GreenBg");
            d->ui->overlayControlBtn->getQPushButton()->style()->unpolish(d->ui->overlayControlBtn->getQPushButton());
            d->ui->overlayControlBtn->getQPushButton()->style()->polish(d->ui->overlayControlBtn->getQPushButton());
        } else if (!overlayController->isActiveWindow()) {
            overlayController->activateWindow();
        }
    }

    void setOverlayControlSetting(OverlayStatus& status, OverlaySelectedType type) {
        if (status == OVS_EXISTS) {
            if (defaultOverlayConfig[type]) {
                status = OVS_CHECKED;
            }
        } else if (status == OVS_CHECKED) {
            if (!defaultOverlayConfig[type]) {
                status = OVS_EXISTS;
            }
        }
    }

    void setOverlayControlData(bool blockSignal = false) {
        QList<ImageOverlayDataModel> list;
        list.resize(overlayControls.size());
        for (int i = 0; i < list.size(); ++ i) {
            ImageOverlayDataModel& m = list[i];
            m = overlayControls[i];
            m.data = &addition.markInfo.measureCursors[i];
        }
        overlayController->setDataPtr(list, selectedRow, blockSignal);
        overlayController->setDefaultSetting(OST_PROFILE_LINE, defaultOverlayConfig[OST_PROFILE_LINE]);
        overlayController->setDefaultSetting(OST_EDGE_CROSS_MARK, defaultOverlayConfig[OST_EDGE_CROSS_MARK]);
        overlayController->setDefaultSetting(OST_DESIGN_MARK, defaultOverlayConfig[OST_DESIGN_MARK]);
        overlayController->setDefaultSetting(OST_CURSOR_MARK, defaultOverlayConfig[OST_CURSOR_MARK]);
        overlayController->setDefaultSetting(OST_VIEW_RESULT, defaultOverlayConfig[OST_VIEW_RESULT]);
    }

    void afterLoadImage() {
        if (imageList.isEmpty()) {
            d->ui->imageTotalLabel->setText("1");
            d->ui->imageNoTextField->setText(1);
            currentNo = 0;
        }
        if (infoController && infoController->isVisible()) {
            infoController->setName(imageName);
            infoController->setImageInfo(addition);
        }
        d->ui->patternAreaCBox->setVisible(addition.markInfo.patternArea.isValid());

        overlayControls.clear();
        if (addition.markInfo.measureCursors.size() > 0) {
            d->ui->overlayControlBtn->setVisible(true);
            overlayControls.reserve(addition.markInfo.measureCursors.size());
            OverlayControlToolModel overlayData;
            for (ImageMeasureMarkData& data : addition.markInfo.measureCursors) {
                if (data.centers.isEmpty()) {
                    overlayData.cursorMark = OVS_NOT_EXISTS;
                } else {
                    overlayData.cursorMark = OVS_CHECKED;
                    if (data.centers[0].l1 == nullptr) {
                        overlayData.designMark = OVS_NOT_EXISTS;
                    } else {
                        overlayData.designMark = OVS_CHECKED;
                    }
                }
                if (data.resultData.profile.isEmpty()) {
                    overlayData.profileLine = OVS_NOT_EXISTS;
                } else {
                    overlayData.profileLine = OVS_CHECKED;
                }

                bool hasEdgeCross = false;
                if (data.resultData.measureResults.size() > 0) {
                    for (MeasureResultData& r : data.resultData.measureResults) {
                        for (BaseResultData& b : r.baseResults) {
                            if (b.listL.size() > 0 || b.listR.size() > 0) {
                                hasEdgeCross = true;
                                break;
                            }
                        }
                        if (hasEdgeCross) break;
                    }
                }
                if (hasEdgeCross) {
                    overlayData.edgeCrossMark = OVS_CHECKED;
                } else {
                    overlayData.edgeCrossMark = OVS_NOT_EXISTS;
                }
                overlayData.viewResult = OVS_CHECKED;

                setOverlayControlSetting(overlayData.profileLine, OST_PROFILE_LINE);
                setOverlayControlSetting(overlayData.edgeCrossMark, OST_EDGE_CROSS_MARK);
                setOverlayControlSetting(overlayData.designMark, OST_DESIGN_MARK);
                setOverlayControlSetting(overlayData.cursorMark, OST_CURSOR_MARK);
                setOverlayControlSetting(overlayData.viewResult, OST_VIEW_RESULT);
                overlayControls.append(overlayData);
            }
        } else {
            d->ui->overlayControlBtn->setVisible(false);
            if (overlayController && overlayController->isVisible()) {
                overlayController->clear();
            }
        }
        if (overlayController && overlayController->isVisible()) {
            setOverlayControlData(true);
        }
    }

    bool checkAndFixToNextImage() {
        if (imageList.size() < 1) {
            if (currentNo != 0) {
                currentNo = 0;
                d->ui->imageNoTextField->setText(1);
            }
            return false;
        }
        return true;
    }

    void reloadImageFromList() {
        if (currentNo < 0 || currentNo >= imageList.size()) return;
        cdsem::ImageViewListData& data = imageList[currentNo];
        if (data.path.length() > 0) {
            d->loadImage(data.path, selectedRow);
        } else if (data.imageId > 0) {
            d->loadImage(data.imageId, selectedRow);
        }
    }
};

ImageViewController::ImageViewController(QWidget* parent) : RelationImageViewController(parent) {
    connect(this, &ImageViewController::asyncCallerChanged, this, [this](){
        create();
        createAction();
    }, Qt::SingleShotConnection);
}

ImageViewController::~ImageViewController() noexcept {
    if (dataHelper) {
        clearImage();
        clearList();
        if (dataHelper->infoController) delete dataHelper->infoController;
        if (dataHelper->overlayController) delete dataHelper->overlayController;
        delete dataHelper;
    }
    if (ui) {
        delete ui;
    }
}

void ImageViewController::create() {
    QWidget* content = new QWidget;
    setContent(content);
    ui = new cdsem::UiImageViewController;
    ui->init(content);

    ui->leftBtn->getQPushButton()->setIcon(QIcon(":/icon/idw_shift_left.png"));
    ui->rightBtn->getQPushButton()->setIcon(QIcon(":/icon/idw_shift_right.png"));

    dataHelper = new ImageViewControllerDataHelper;
    dataHelper->d = this;
    dataHelper->canvas = new ImageCanvas(ui->canvasContent);
    dataHelper->canvas->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    dataHelper->canvas->setMinimumHeight(386);
    dataHelper->defaultOverlayConfig.insert(OST_PROFILE_LINE, true);
    dataHelper->defaultOverlayConfig.insert(OST_EDGE_CROSS_MARK, true);
    dataHelper->defaultOverlayConfig.insert(OST_DESIGN_MARK, true);
    dataHelper->defaultOverlayConfig.insert(OST_CURSOR_MARK, true);
    dataHelper->defaultOverlayConfig.insert(OST_VIEW_RESULT, true);
    ui->canvasContent->layout()->addWidget(dataHelper->canvas);
}

void ImageViewController::createAction() {
    bindCloseBtn(ui->closeBtn->getQPushButton());
    connect(dataHelper->canvas->imageStream(), &StreamImageSupplier::rewriteImage, this, [this](){
        if (dataHelper->tmpData.data.size() > 0)
            dataHelper->drawImage(dataHelper->tmpData);
    });

    connect(ui->infoBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        dataHelper->openInfoController();
        dataHelper->infoController->setName(dataHelper->imageName);
        dataHelper->infoController->setImageInfo(dataHelper->addition);
    });
    connect(ui->overlayControlBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        dataHelper->openOverlayController();
        dataHelper->setOverlayControlData();
    });

    connect(ui->eraseOvelayCBox, &QCheckBox::toggled, this, [this](bool){
        dataHelper->drawOverlay();
    });

    connect(ui->patternAreaCBox, &QCheckBox::toggled, this, [this](bool){
        dataHelper->drawPatternMatch();
        dataHelper->canvas->update();
    });

    connect(ui->leftBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        if (!dataHelper->checkAndFixToNextImage()) {
            return;
        }
        if (dataHelper->currentNo >= dataHelper->imageList.size() || dataHelper->currentNo <= 0) {
            dataHelper->currentNo = dataHelper->imageList.size() - 1;
        } else {
            -- dataHelper->currentNo;
        }
        ui->imageNoTextField->setText(dataHelper->currentNo + 1);
        dataHelper->reloadImageFromList();
    });
    connect(ui->rightBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        if (!dataHelper->checkAndFixToNextImage()) {
            return;
        }
        if (dataHelper->currentNo >= dataHelper->imageList.size() - 1 || dataHelper->currentNo < 0) {
            dataHelper->currentNo = 0;
        } else {
            ++ dataHelper->currentNo;
        }
        ui->imageNoTextField->setText(dataHelper->currentNo + 1);
        dataHelper->reloadImageFromList();
    });

    connect(ui->sendImageBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        cdsem::TmpImageData& imageData = dataHelper->tmpData;
        ImageAddtionDataModel& addition = dataHelper->addition;
        cdsem::CDSEM_GlobalDataHelper::get()->invokeImage([imageData, addition, this](){  // 捕获 this
            if (imageData.data.isEmpty()) return;
            cdsem::CDSEM_GlobalDataHelper::get()->setImage(IMG_OPT_TYPE_IM, imageData.data, imageData.size.width(), imageData.size.height(), imageData.format, addition);
            MainThreadInvoker::getInvoker().runLater([addition, this](){  // 捕获 this
                IOPT->setPhotoImageType(addition.photoImagetype);
                cdsem::CDSEM_GlobalDataHelper::get()->setImageOperationType(IMG_OPT_TYPE_IM);
                IOPT->setSemMag(addition.mag);
            });
        });
    });
}

void ImageViewController::closeEvent(QCloseEvent* event) {
    CDE90Window::closeEvent(event);
    if (event->isAccepted()) {
        clearImage();
        clearList();
        if (dataHelper->overlayController && dataHelper->overlayController->isVisible()) {
            dataHelper->overlayController->close();
        }
        if (dataHelper->infoController && dataHelper->infoController->isVisible()) {
            dataHelper->infoController->close();
        }
    }
}

void ImageViewController::loadImage(quint64 imageId, int selectedData) {
    clearImage();
    int version = dataHelper->loadVersion;
    ACaller->stop(LOAD_IMG_GID);
    ACaller->push(this, LOAD_IMG_GID, [this, imageId, selectedData, version](TSC cmd){
        if (cmd.isStop()) return;
        MainThreadInvoker::getInvoker().runLater([this](){
            dataHelper->canvas->toLoadingStatus();
        });
        bool succ = false;
        cdsem::TmpImageData data;
        QString name;
        sem::AfterGc gc([&succ, this, &selectedData, &data, &name, &version](){
            MainThreadInvoker::getInvoker().runLater([this, succ, selectedData, data, name, version](){
                if (version != dataHelper->loadVersion) return;
                if (succ) loadImage(std::move(data), name, selectedData);
            });
        });
        ImageInfoService* imageService = ServiceManager::defaultService<ImageInfoService>();
        int errCode;
        ImageInfoEntity entity = imageService->getById(imageId, &errCode);
        if (errCode) {
            simpleMessageBox(this, "Error", "Error to load image data.");
            return;
        }
        if (cmd.isStop()) return;
        if (entity.getId() == 0) return;
        data.data = std::move(entity.getImageData());
        data.addition = entity.getImageInfo();
        data.format = entity.getImageFormat();
        data.size = QSize(entity.getWidth(), entity.getHeight());
        name = entity.getImageName();
        succ = true;
    });
}
void ImageViewController::loadImage(const QString& imagePath, int selectedData) {
    clearImage();
    cdsem::TmpImageData imageData;
    bool succ = readImage(imageData, imagePath);
    if (!succ) {
        simpleMessageBox(this, "Error", "Error to load image data.");
        return;
    }
    QFileInfo info(imagePath);
    loadImage(std::move(imageData), info.baseName(), selectedData);
}
void ImageViewController::loadImage(const QString& imagePath, const ImageAddtionDataModel& addition, int selectedData) {
    clearImage();
    cdsem::TmpImageData imageData;
    int errCode = readImage(imageData, imagePath);
    if (errCode) {
        simpleMessageBox(this, "Error", "Error to load image data.");
        return;
    }
    QFileInfo info(imagePath);
    loadImage(std::move(imageData), addition, info.baseName(), selectedData);
}
void ImageViewController::loadImage(const cdsem::TmpImageData& imageData, const QString& name, int selectedData) {
    dataHelper->canvas->toLoadingStatus(false);
    clearImage();
    ++ dataHelper->loadVersion;
    if (imageData.addition.length() > 0) {
        int errCode = fromImageDeitalDataModelJson(imageData.addition, dataHelper->addition);
        if (errCode) {
            simpleMessageBox(this, "Error", "Error to load image information.");
            return;
        }
    }
    dataHelper->imageName = name;
    ui->fileNameTextField->setText(name);
    dataHelper->selectedRow = selectedData;
    dataHelper->afterLoadImage();
    dataHelper->drawImage(imageData);
}

void ImageViewController::loadImage(const cdsem::TmpImageData& imageData, const ImageAddtionDataModel& addition, const QString& name, int selectedData) {
    dataHelper->canvas->toLoadingStatus(false);
    clearImage();
    ++ dataHelper->loadVersion;
    dataHelper->addition = addition;
    dataHelper->imageName = name;
    ui->fileNameTextField->setText(name);
    dataHelper->selectedRow = selectedData;
    dataHelper->afterLoadImage();
    dataHelper->drawImage(imageData);
}


void ImageViewController::clearImage() {
    dataHelper->addition = {};
    dataHelper->imageName.clear();
    if (dataHelper->infoController && dataHelper->infoController->isVisible()) {
        dataHelper->infoController->setName("");
        dataHelper->infoController->setImageInfo("");
    }
    if (dataHelper->overlayController) {
        dataHelper->overlayController->clear();
    }
    dataHelper->clear();
    dataHelper->canvas->imageStream()->clearImage();
    dataHelper->tmpData.deepClear();
    ++ dataHelper->version;
}

void ImageViewController::clearList() {
    dataHelper->imageList.clear();
    dataHelper->currentNo = 0;
    ui->imageNoTextField->getQLineEdit()->clear();
    ui->imageTotalLabel->setText("0");
    ++ dataHelper->version;
}

quint64 ImageViewController::setImageList(const QList<cdsem::ImageViewListData>& list) {
    dataHelper->imageList = list;
    ui->imageTotalLabel->setText(QString::number(list.size()));
    return ++ dataHelper->version;
}

char ImageViewController::setCurrentNo(int n, int selectedData, quint64 version) {
    if (version != dataHelper->version) return 0;
    if (dataHelper->currentNo == n) {
        if (dataHelper->selectedRow != selectedData) {
            dataHelper->doSelectData(selectedData);
        }
        return 2;
    }
    dataHelper->currentNo = n;
    dataHelper->selectedRow = selectedData;
    ui->imageNoTextField->setText(n + 1);
    return 1;
}

void ImageViewController::refreshCurrentImage() {
    dataHelper->reloadImageFromList();
}
