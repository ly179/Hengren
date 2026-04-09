#include "image_manager_signal_bridge.h"
#ifdef __IMAGE_MANAGER_TARGET
#include "plugin_signal_hub.h"
#include "signal_hub_plugin_ids.h"
#include "signal_hub_single_object_manager.h"
#include "image_file_temp_handler.h"
#include "image_list_controller.h"
#include "image_view_controller.h"
#include "sem_global.h"

#include "exception_code.h"
#include "plugin_version_util.h"
#include "sem_common_data_model.h"

ImageListController* openImageListViewer() {
    ImageListController* imageListViewer = SignalHubSingleObjectManager::instance()->getObject<ImageListController*>(OPEN_SINGLE_IMAGE_LIST_CONTROLLER);
    if (imageListViewer) {
        if (!imageListViewer->isActiveWindow()) imageListViewer->activateWindow();
        return imageListViewer;
    }
    imageListViewer = new ImageListController;
    imageListViewer->setWindowTitle("Image List");
    imageListViewer->setAttribute(Qt::WA_DeleteOnClose);
    imageListViewer->initAsyncCaller([](){
        SignalHubSingleObjectManager::instance()->removeObject(OPEN_SINGLE_IMAGE_LIST_CONTROLLER);
    }, true);
    imageListViewer->show();
    MainThreadInvoker::getInvoker().moveToScreenCenter(imageListViewer);
    SignalHubSingleObjectManager::instance()->setObject(OPEN_SINGLE_IMAGE_LIST_CONTROLLER, imageListViewer);
    return imageListViewer;
}

ImageListController* openImageListViewer(const QList<cdsem::ImageViewListData>& list) {
    ImageListController* imageListViewer = openImageListViewer();
    imageListViewer->setImageList(list);
    return imageListViewer;
}

ImageViewController* openImageViewer() {
    ImageViewController* imageViewer = SignalHubSingleObjectManager::instance()->getObject<ImageViewController*>(OPEN_SINGLE_IMAGE_VIEW_CONTROLLER);
    if (imageViewer) {
        if (!imageViewer->isActiveWindow()) imageViewer->activateWindow();
        return imageViewer;
    }
    imageViewer = new ImageViewController;
    imageViewer->setWindowTitle("Saved image display");
    imageViewer->setAttribute(Qt::WA_DeleteOnClose);
    imageViewer->initAsyncCaller([](){
        SignalHubSingleObjectManager::instance()->removeObject(OPEN_SINGLE_IMAGE_VIEW_CONTROLLER);
    }, true);
    imageViewer->show();
    MainThreadInvoker::getInvoker().moveToScreenCenter(imageViewer);
    SignalHubSingleObjectManager::instance()->setObject(OPEN_SINGLE_IMAGE_VIEW_CONTROLLER, imageViewer);
    return imageViewer;
}

ImageViewController* openImageViewer(quint64 id, int selectedData){
    ImageViewController* imageViewer = openImageViewer();
    imageViewer->clearList();
    imageViewer->loadImage(id, selectedData);
    return imageViewer;
}

ImageViewController* openImageViewer(const QString& imagePath, int selectedData){
    ImageViewController* imageViewer = openImageViewer();
    imageViewer->clearList();
    imageViewer->loadImage(imagePath, selectedData);
    return imageViewer;
}

ImageViewController* openImageViewer(const QString& imagePath, const ImageAddtionDataModel& addition, int selectedData){
    ImageViewController* imageViewer = openImageViewer();
    imageViewer->clearList();
    imageViewer->loadImage(imagePath, addition, selectedData);
    return imageViewer;
}

ImageViewController* openImageViewer(const cdsem::TmpImageData& imageData, const QString& name, int selectedData){
    ImageViewController* imageViewer = openImageViewer();
    imageViewer->clearList();
    imageViewer->loadImage(imageData, name, selectedData);
    return imageViewer;
}

ImageViewController* openImageViewer(const cdsem::TmpImageData& imageData, const ImageAddtionDataModel& addition, const QString& name, int selectedData){
    ImageViewController* imageViewer = openImageViewer();
    imageViewer->clearList();
    imageViewer->loadImage(imageData, addition, name, selectedData);
    return imageViewer;
}

ImageViewController* openImageViewer(const QList<cdsem::ImageViewListData>& list, int index, int selectedData, quint64* version){
    ImageViewController* imageViewer = openImageViewer();
    quint64 v = imageViewer->setImageList(list);
    //设置列表时，强制刷新
    imageViewer->setCurrentNo(index, selectedData, v);
    imageViewer->refreshCurrentImage();
    if (version) *version = v;
    return imageViewer;
}

ImageViewController* openImageViewer(int index, int selectedData, quint64 v, bool* succ){
    ImageViewController* imageViewer = openImageViewer();
    char c = imageViewer->setCurrentNo(index, selectedData, v);
    if (c == 1)
        imageViewer->refreshCurrentImage();
    if (succ) *succ = c != 0;
    return imageViewer;
}

SIGNAL_CHECK(__IMAGE_MANAGER_TARGET) {
    Q_UNUSED(sysVersion)
#ifdef _IMAGE_MANAGER_VERSION_
    bool succ = readVersion(_IMAGE_MANAGER_VERSION_, *libVersion);
    if (succ) {
        result->errCode = 0;
        result->errMsg.clear();
        //TODO 未来可添加版本号处理机制
    } else {
        result->errCode = LIBRARY_VERSION_READ_ERROR_CODE;
        result->errMsg = "Image manager version is not defined.";
    }
#else
#warning "Image Manager component version is not defined."
    result->errCode = 0;
    result->errMsg.clear();
#endif
}

SIGNAL_REG(__IMAGE_MANAGER_TARGET) {
    Q_UNUSED(sysVersion)
    PluginSignalHub::instance()->registSlot(OPEN_SINGLE_IMAGE_LIST_CONTROLLER, PluginHubAnyFunc<RelationImageListController*>([](){
        return openImageListViewer();
    }));
    PluginSignalHub::instance()->registSlot(OPEN_SINGLE_IMAGE_LIST_CONTROLLER, PluginHubAnyFunc<RelationImageListController*, const QList<cdsem::ImageViewListData>&>([](const QList<cdsem::ImageViewListData>& list){
        return openImageListViewer(list);
    }));
    PluginSignalHub::instance()->registSlot(OPEN_SINGLE_IMAGE_VIEW_CONTROLLER, PluginHubAnyFunc<RelationImageViewController*>([](){
        return openImageViewer();
    }));
    PluginSignalHub::instance()->registSlot(OPEN_SINGLE_IMAGE_VIEW_CONTROLLER, PluginHubAnyFunc<RelationImageViewController*, quint64, int>([](quint64 id, int selectedData){
        return openImageViewer(id, selectedData);
    }));
    PluginSignalHub::instance()->registSlot(OPEN_SINGLE_IMAGE_VIEW_CONTROLLER, PluginHubAnyFunc<RelationImageViewController*, const QString&, int>([](const QString& imagePath, int selectedData){
        return openImageViewer(imagePath, selectedData);
    }));
    PluginSignalHub::instance()->registSlot(OPEN_SINGLE_IMAGE_VIEW_CONTROLLER, PluginHubAnyFunc<RelationImageViewController*, const QString&, const ImageAddtionDataModel&, int>([](const QString& imagePath, const ImageAddtionDataModel& addition, int selectedData){
        return openImageViewer(imagePath, addition, selectedData);
    }));
    PluginSignalHub::instance()->registSlot(OPEN_SINGLE_IMAGE_VIEW_CONTROLLER, PluginHubAnyFunc<RelationImageViewController*, const cdsem::TmpImageData&, const QString&, int>([](const cdsem::TmpImageData& imageData, const QString& name, int selectedData){
        return openImageViewer(imageData, name, selectedData);
    }));
    PluginSignalHub::instance()->registSlot(OPEN_SINGLE_IMAGE_VIEW_CONTROLLER, PluginHubAnyFunc<RelationImageViewController*, const cdsem::TmpImageData&, const ImageAddtionDataModel&, const QString&, int>([](const cdsem::TmpImageData& imageData, const ImageAddtionDataModel& addition, const QString& name, int selectedData){
        return openImageViewer(imageData, addition, name, selectedData);
    }));
    PluginSignalHub::instance()->registSlot(OPEN_SINGLE_IMAGE_VIEW_CONTROLLER, PluginHubAnyFunc<RelationImageViewController*, const QList<cdsem::ImageViewListData>&, int, int, quint64*>([](const QList<cdsem::ImageViewListData>& list, int index, int selectedData, quint64* version){
        return openImageViewer(list, index, selectedData, version);
    }));
    PluginSignalHub::instance()->registSlot(OPEN_SINGLE_IMAGE_VIEW_CONTROLLER, PluginHubAnyFunc<RelationImageViewController*, int, int, quint64, bool*>([](int index, int selectedData, quint64 v, bool* succ){
        return openImageViewer(index, selectedData, v, succ);
    }));

    if (result) {
        result->errCode = 0;
        result->errMsg.clear();
    }
}

SIGNAL_UNREG(__IMAGE_MANAGER_TARGET) {
    QList<QString> list;
    list.reserve(2);
    list << OPEN_SINGLE_IMAGE_LIST_CONTROLLER << OPEN_SINGLE_IMAGE_VIEW_CONTROLLER;
    QString pid = QString::fromLatin1(pluginId);
    PluginSignalHub::instance()->unRegistAll(list, [pid, callback](){
        callback(pid.toLatin1());
    });
}

IMAGE_MANAGER_SIGNAL_BRIDGE_H
#endif //
