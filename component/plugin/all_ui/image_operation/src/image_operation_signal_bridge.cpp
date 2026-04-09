#include "image_operation_signal_bridge.h"
#ifdef _IMAGE_OPERATION_TARGET
#include "plugin_signal_hub.h"
#include "signal_hub_plugin_ids.h"
#include "signal_hub_single_object_manager.h"
#include "sem_global.h"
#include "image_operation_controller.h"

#include "plugin_version_util.h"
#include "sem_common_data_model.h"
#include "exception_code.h"

#include "cdsem_control_helper.h"
#include "image_operation_data_model.h"
#include "cdsem_global_data.h"
#include "image_type_defition.h"

inline void stopLiveImage(CPSender sender) {
    CDSEM_ControlHelper::instance()->imageOffline(sender);
}

void cleanupImageBuffer(void* data) {
    delete static_cast<QByteArray*>(data);
}

RelationImageOperationController* openSignalImageOperationWindow(bool notExistsCreate = false, bool activeWindow = true) {
    ImageOperationController* imageOperationController = SignalHubSingleObjectManager::instance()->getObject<ImageOperationController*>(OPEN_SINGLE_IMAGE_OPERATION_WINDOW);
    if (imageOperationController) {
        if (activeWindow) MainThreadInvoker::getInvoker().activeWindow(imageOperationController);
        imageOperationController->reloadImageFromGlobal();
        return imageOperationController;
    } if (!notExistsCreate) return nullptr;
    imageOperationController = new ImageOperationController;
    imageOperationController->setAttribute(Qt::WA_DeleteOnClose);
    imageOperationController->setWindowTitle("Image Operation");
    imageOperationController->initAsyncCaller([](){
        SignalHubSingleObjectManager::instance()->removeObject(OPEN_SINGLE_IMAGE_OPERATION_WINDOW);
        if (cdsem::CDSEM_GlobalDataHelper::get()->procedureRunningStatus()) {
            return;
        } else if (IOPT->getImageOperationType() == IMG_OPT_TYPE_SEM) {
            if (SEM_PY_ENV.isFreezen()) {
                return;
            }
            stopLiveImage(nullptr);
        }
    }, true);
    MainThreadInvoker::getInvoker().moveToScreenCenter(imageOperationController);
    imageOperationController->show();
    imageOperationController->reloadData();
    SignalHubSingleObjectManager::instance()->setObject(OPEN_SINGLE_IMAGE_OPERATION_WINDOW, imageOperationController);
    return imageOperationController;
}

SIGNAL_CHECK(_IMAGE_OPERATION_TARGET) {
    Q_UNUSED(sysVersion)
#ifdef _IMG_OPERATION_VERSION_
    bool succ = readVersion(_IMG_OPERATION_VERSION_, *libVersion);
    if (succ) {
        result->errCode = 0;
        result->errMsg.clear();
        //TODO 未来可添加版本号处理机制
    } else {
        result->errCode = LIBRARY_VERSION_READ_ERROR_CODE;
        result->errMsg = "IDW wafer version is not defined.";
    }
#else
#warning "Image Operation Window component version is not defined."
    result->errCode = 0;
    result->errMsg.clear();
#endif
}

SIGNAL_REG(_IMAGE_OPERATION_TARGET) {
    Q_UNUSED(sysVersion)
    PluginSignalHub::instance()->registSlot(OPEN_SINGLE_IMAGE_OPERATION_WINDOW, PluginHubAnyFunc<RelationImageOperationController*, bool, bool>([](bool notExistsCreate, bool activeWindow){
        return openSignalImageOperationWindow(notExistsCreate, activeWindow);
    }));
    if (result) {
        result->errCode = 0;
        result->errMsg.clear();
    }
}

SIGNAL_UNREG(_IMAGE_OPERATION_TARGET){
    QString pid = QString::fromLatin1(pluginId);
    PluginSignalHub::instance()->unRegistAll(OPEN_SINGLE_IMAGE_OPERATION_WINDOW, [pid, callback](){
        callback(pid.toLatin1());
    });
}

IMAGE_OPERATION_SIGNAL_BRIDGE_H
#endif //_IMAGE_OPERATION_TARGET
