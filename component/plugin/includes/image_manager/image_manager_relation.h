#ifndef IMAGE_MANAGER_RELATION_H
#define IMAGE_MANAGER_RELATION_H
#include "relation_image_list_controller.h"
#include "relation_image_view_controller.h"
#include "plugin_signal_hub.h"
#include "signal_hub_plugin_ids.h"
#include "image_save_model.h"
#include "image_file_temp_handler.h"
namespace image_manager {
inline bool imageViewAvaliable() {
    return PluginSignalHub::instance()->testSlot<RelationImageViewController*>(OPEN_SINGLE_IMAGE_VIEW_CONTROLLER);
}
inline bool imageListViewAvaliable() {
    return PluginSignalHub::instance()->testSlot<RelationImageViewController*>(OPEN_SINGLE_IMAGE_LIST_CONTROLLER);
}

inline bool isSingleImageListController(const QString& id) {
    return OPEN_SINGLE_IMAGE_LIST_CONTROLLER == id;
}

inline bool isSingleImageViewController(const QString& id) {
    return OPEN_SINGLE_IMAGE_VIEW_CONTROLLER == id;
}

inline MsgReceiveStatus<RelationImageListController*> openImageListController() {
    return PluginSignalHub::instance()->sendCreateObjMsg<RelationImageListController*>(OPEN_SINGLE_IMAGE_LIST_CONTROLLER);
}

inline MsgReceiveStatus<RelationImageListController*> openImageListController(const QList<cdsem::ImageViewListData>& list) {
    return PluginSignalHub::instance()->sendCreateObjMsg<RelationImageListController*, const QList<cdsem::ImageViewListData>&>(OPEN_SINGLE_IMAGE_LIST_CONTROLLER, list);
}

inline MsgReceiveStatus<RelationImageViewController*> openImageViewController() {
    return PluginSignalHub::instance()->sendCreateObjMsg<RelationImageViewController*>(OPEN_SINGLE_IMAGE_VIEW_CONTROLLER);
}

inline MsgReceiveStatus<RelationImageViewController*> openImageViewController(quint64 id, int selectedData) {
    return PluginSignalHub::instance()->sendCreateObjMsg<RelationImageViewController*, quint64, int>(OPEN_SINGLE_IMAGE_VIEW_CONTROLLER, id, selectedData);
}

inline MsgReceiveStatus<RelationImageViewController*> openImageViewController(const QString& imagePath, int selectedData) {
    return PluginSignalHub::instance()->sendCreateObjMsg<RelationImageViewController*, const QString&, int>(OPEN_SINGLE_IMAGE_VIEW_CONTROLLER, imagePath, selectedData);
}

inline MsgReceiveStatus<RelationImageViewController*> openImageViewController(const QString& imagePath, const ImageAddtionDataModel& addition, int selectedData) {
    return PluginSignalHub::instance()->sendCreateObjMsg<RelationImageViewController*, const QString&, const ImageAddtionDataModel&, int>(OPEN_SINGLE_IMAGE_VIEW_CONTROLLER, imagePath, addition, selectedData);
}

inline MsgReceiveStatus<RelationImageViewController*> openImageViewController(const cdsem::TmpImageData& imageData, const QString& name, int selectedData) {
    return PluginSignalHub::instance()->sendCreateObjMsg<RelationImageViewController*, const cdsem::TmpImageData&, const QString&, int>(OPEN_SINGLE_IMAGE_VIEW_CONTROLLER, imageData, name, selectedData);
}

inline MsgReceiveStatus<RelationImageViewController*> openImageViewController(const cdsem::TmpImageData& imageData, const ImageAddtionDataModel& addition, const QString& name, int selectedData) {
    return PluginSignalHub::instance()->sendCreateObjMsg<RelationImageViewController*, const cdsem::TmpImageData&, const ImageAddtionDataModel&, const QString&, int>(OPEN_SINGLE_IMAGE_VIEW_CONTROLLER, imageData, addition, name, selectedData);
}

inline MsgReceiveStatus<RelationImageViewController*> openImageViewController(const QList<cdsem::ImageViewListData>& list, int index, int selectedData, quint64* version) {
    return PluginSignalHub::instance()->sendCreateObjMsg<RelationImageViewController*, const QList<cdsem::ImageViewListData>&, int, int, quint64*>(OPEN_SINGLE_IMAGE_VIEW_CONTROLLER, list, index, selectedData, version);
}

inline MsgReceiveStatus<RelationImageViewController*> openImageViewController(int index, int selectedData, quint64 v, bool* succ) {
    return PluginSignalHub::instance()->sendCreateObjMsg<RelationImageViewController*, int, int, quint64, bool*>(OPEN_SINGLE_IMAGE_VIEW_CONTROLLER, index, selectedData, v, succ);
}

}

#endif // IMAGE_MANAGER_RELATION_H
