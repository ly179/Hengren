#include "image_operation_window_helper.h"
#include "sem_global.h"
#include "signal_hub.h"
#include "signal_hub_ids.h"
#include "scan_mode_data_model.h"
#include "image_scan_live_mode_helper.h"
#include "cdsem_control_helper.h"
#include "cdsem_global_data.h"
#include "cdsem_global_data.h"
#include "result_data_model.h"
#include "cdsem_global_env_data_model.h"
#include "image_operation_data_model.h"
#include "om_camera_manager.h"

struct ImageOperationWindowDataHelper {
    ScanModeDataModel currentScanModel;
    bool windowVisible = false;
    bool connected = false;
    bool scanParamReady = false;
};

void startOmDevice() {
    OmCameraManager::getDefaultCamera()->invoke([](){
        OmCameraManager::getDefaultCamera()->openDevice();
        OmCameraManager::getDefaultCamera()->startDevice();
    });
}

void stopOmDevice() {
    OmCameraManager::getDefaultCamera()->invoke([](){
        OmCameraManager::getDefaultCamera()->stopDevice();
        OmCameraManager::getDefaultCamera()->closeDevice();
    });
}

void startLiveImage(CPSender sender) {
    CDSEM_ControlHelper::instance()->imageOnline(sender);
    if (MainThreadInvoker::getInvoker().isUiThread())
        ImageOperationWindowHelper::get()->refreshScanParamLabel();
    else
        MainThreadInvoker::getInvoker().runLater([](){
            ImageOperationWindowHelper::get()->refreshScanParamLabel();
        });
}

void stopLiveImage(CPSender sender) {
    CDSEM_ControlHelper::instance()->imageOffline(sender);
}


void freezenImage(CPSender sender) {
    CDSEM_ControlHelper::instance()->grabImage(sender);
}

void cleanupImageBuffer(void* data) {
    delete static_cast<QByteArray*>(data);
}

ImageOperationWindowHelper::ImageOperationWindowHelper() {
    dataHelper = new ImageOperationWindowDataHelper;
    imageOperationController = new ImageOperationController;
    // imageOperationController->hideTitle();
    // imageOperationController->setResizable(false);
    CSender sender(imageOperationController);
    QObject::connect(imageOperationController, &ImageOperationController::visibleSignal, imageOperationController, [this, sender](bool visible){
        if (sender) {
            dataHelper->windowVisible = visible;
        }
        if (visible) {
            if (IOPT->getImageOperationType() == IMG_OPT_TYPE_OM) {
                startOmDevice();
            } else if (IOPT->getImageOperationType() == IMG_OPT_TYPE_SEM) {
                if (SEM_PY_ENV.isFreezen()) {
                    return;
                }
                checkAndStartLiveImage(sender);
            }
        } else {
            stopOmDevice();
            if (dataHelper->connected && dataHelper->scanParamReady) {
                if (SEM_PY_ENV.isFreezen()) {
                    return;
                }
                stopLiveImage(sender);
            }
        }
    });
    imageOperationController->setWindowTitle("Image Operation");
    //callback 在UI线程上回调的。
    imageOperationController->initAsyncCaller(sem::NO_PARAM_ARG_FUNC([]() {
        SignalHub::instance()->unRegistAll(SIGNAL_HUB_OPEN_IMG_OPT);
        SignalHub::instance()->unRegistAll(SIGNAL_HUB_IMG_OPT_LCROSS_CURSOR_MODE);
        SignalHub::instance()->unRegistAll(SIGNAL_HUB_IMG_OPT_RECT_CURSOR_MODE);
        SignalHub::instance()->unRegistAll(SIGNAL_HUB_IMG_OPT_NO_CURSOR_MODE);
        SignalHub::instance()->unRegistAll(SIGNAL_HUB_IMG_OPT_CROSS_CURSOR_CENTER);
        SignalHub::instance()->unRegistAll(SIGNAL_HUB_IMG_OPT_RECT_CURSOR_RANGE);
        SignalHub::instance()->unRegistAll(SIGNAL_HUB_IMG_OPT_AMS_FIRST_STEP);
        SignalHub::instance()->unRegistAll(SIGNAL_HUB_SET_IMG_OPT_RESULT);
        SignalHub::instance()->unRegistAll(SIGNAL_HUB_STOP_IMAGE_OPT_AMP);
        SignalHub::instance()->unRegistAll(SIGNAL_HUB_COLLECT_CENTERS_TO_TMP_AMP);
    }));

    ImageOperationController::connect(imageOperationController,
                                      &ImageOperationController::signalOpenAMP,
                                      imageOperationController,
                                      []() {
                                          SignalHub::instance()->sendMsg<int, const QString&>(SIGNAL_HUB_OPEN_AMP, -1, "");
                                      });

    ImageOperationController::connect(ImageScanLiveModeHelper::get(),
                                      &ImageScanLiveMode::cmdToScan,
                                      imageOperationController,
                                      [this, sender](ImageScanMode) {
                                          if (!sender) return;
                                          if (!dataHelper->scanParamReady) {
                                              dataHelper->scanParamReady = true;
                                          }
                                          CDSEM_ControlHelper::instance()->sendScanParam(sender);
                                          ScanModeDataModel model = ImageScanLiveModeHelper::get()->getGlobal(ISM_CURRENT);
                                          if (SEM_PY_ENV.getFreezeStatus() != IFS_LIVE || model.getDataMode() == SDM_INTEG) {
                                              freezenImage(sender);
                                          }
                                      }, Qt::QueuedConnection);

    SignalHub::instance()->registSlot<>(SIGNAL_HUB_OPEN_IMG_OPT,
                                        HubAnyFunc<>([this, sender]() {
                                            if (!sender) return;
                                            show();
                                        }));
    SignalHub::instance()->registSlot(SIGNAL_HUB_IMG_OPT_LCROSS_CURSOR_MODE, HubAnyFunc<>([this, sender]() {
        if (!sender) return;
        imageOperationController->localCrossCursorMode();
    }));
    SignalHub::instance()->registSlot(SIGNAL_HUB_IMG_OPT_RECT_CURSOR_MODE, HubAnyFunc<>([this, sender]() {
        if (!sender) return;
        imageOperationController->rectCursorMode();
    }));
    SignalHub::instance()->registSlot(SIGNAL_HUB_IMG_OPT_NO_CURSOR_MODE, HubAnyFunc<>([this, sender]() {
        if (!sender) return;
        imageOperationController->blankCursorMode();
    }));
    SignalHub::instance()->registSlot(SIGNAL_HUB_IMG_OPT_CROSS_CURSOR_CENTER,
                                      HubAnyFunc<QPoint &>([this, sender](QPoint &point) {
                                          if (!sender) return;
                                          point = imageOperationController->getCrossCursorCenter();
                                      }));
    SignalHub::instance()->registSlot(SIGNAL_HUB_IMG_OPT_RECT_CURSOR_RANGE,
                                      HubAnyFunc<QRect &>([this, sender](QRect &rect) {
                                          if (!sender) return;
                                          rect = imageOperationController->getRectCursorRange();
                                      }));
    SignalHub::instance()->registSlot(SIGNAL_HUB_IMG_OPT_AMS_FIRST_STEP, HubAnyFunc<>([this, sender]() {
                                          if (!sender) return;
                                          imageOperationController->amsToFirstStep();
                                      }));

    SignalHub::instance()->registSlot(SIGNAL_HUB_SET_IMG_OPT_RESULT, HubAnyFunc<const ImageResultData&>([this, sender](const ImageResultData& rData) {
                                          if (MainThreadInvoker::instance()->isUiThread()) {
                                              if (!sender) return;
                                              imageOperationController->setMeasureResult(rData);
                                          } else {
                                              MainThreadInvoker::getInvoker().runLater([this, rData, sender](){
                                                  if (!sender) return;
                                                  imageOperationController->setMeasureResult(rData);
                                              });
                                          }
                                      }));
    SignalHub::instance()->registSlot(SIGNAL_HUB_STOP_IMAGE_OPT_AMP, HubAnyFunc<>([this, sender]() {
                                          if (MainThreadInvoker::instance()->isUiThread()) {
                                              if (!sender) return;
                                              imageOperationController->stopAutoMeasure();
                                          } else {
                                              MainThreadInvoker::getInvoker().runLater([this, sender](){
                                                  if (!sender) return;
                                                  imageOperationController->stopAutoMeasure();
                                              });
                                          }
                                      }));
    SignalHub::instance()->registSlot(SIGNAL_HUB_COLLECT_CENTERS_TO_TMP_AMP, HubAnyFunc<const sem::ONE_PARAM_ARG_FUNC<bool>&>([this, sender](const sem::ONE_PARAM_ARG_FUNC<bool>& callback) {
                                          if (MainThreadInvoker::instance()->isUiThread()) {
                                              if (!sender) return;
                                              bool succ = imageOperationController->collectCursorCenterToTmp();
                                              if (callback) callback(succ);
                                          } else {
                                              MainThreadInvoker::getInvoker().blockRun([this, sender, &callback](){
                                                  if (!sender) return;
                                                  bool succ = imageOperationController->collectCursorCenterToTmp();
                                                  if (callback) callback(succ);
                                              });
                                          }
                                      }));


    CDSEM_ControlHelper::instance()->isConnected([this, sender](bool connected){
        MainThreadInvoker::getInvoker().runLater([connected, this, sender](){
            if (!sender) return;
            imageOperationController->reloadData();
            dataHelper->connected = connected;
            checkAndStartLiveImage(sender);
        });
    });

    QObject::connect(CDSEM_ControlHelper::instance()->getControl(),
         &CDSEM_Control::connected, CDSEM_ControlHelper::instance()->getControl(), [this, sender](){
            CDSEM_ControlHelper::instance()->sendReadCondition(sender);
             MainThreadInvoker::getInvoker().runLater([this, sender](){
                 if (!sender) return;
                 imageOperationController->reloadData();
                 dataHelper->connected = true;
                 checkAndStartLiveImage(sender);
             });
         }, Qt::DirectConnection);
    QObject::connect(CDSEM_ControlHelper::instance()->getControl(),
         &CDSEM_Control::disconnected, CDSEM_ControlHelper::instance()->getControl(), [this, sender](){
             MainThreadInvoker::getInvoker().runLater([this, sender](){
                 if (!sender) return;
                 dataHelper->connected = false;
             });
         }, Qt::DirectConnection);
}

ImageOperationWindowHelper::~ImageOperationWindowHelper() noexcept {
    //init asynccaller时，已指定自动delete imageOperationController
    delete dataHelper;
}

void ImageOperationWindowHelper::checkAndStartLiveImage(CPSender sender) {
    if (dataHelper->connected && dataHelper->windowVisible && dataHelper->scanParamReady && !SEM_PY_ENV.isFreezen() && IOPT->getImageOperationType() == IMG_OPT_TYPE_SEM) {
        startLiveImage(sender);
    }
}

void ImageOperationWindowHelper::show() {
    if (imageOperationController->isVisible()) {
        imageOperationController->activateWindow();
        return;
    }
    imageOperationController->show();
    MainThreadInvoker::getInvoker().moveToScreenCenter(imageOperationController);
}


void ImageOperationWindowHelper::close() {
    imageOperationController->close();
}
