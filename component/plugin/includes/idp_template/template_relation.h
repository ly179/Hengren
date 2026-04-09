#ifndef TEMPLATE_RELATION_H
#define TEMPLATE_RELATION_H
#include "plugin_signal_hub.h"
#include "signal_hub_plugin_ids.h"
#include "signal_hub_single_object_manager.h"
#include "relation_ap_template_edit_controller.h"
#include "relation_mp_template_edit_controller.h"
#include "relation_mpip_template_edit_controller.h"
#include "relation_idp_ap_template_edit_controller.h"
#include "relation_idp_mp_template_edit_controller.h"
#include "relation_idp_multi_ms_template_edit_controller.h"
#include "relation_ms_multi_template_edit_controller.h"

namespace idp_template {

inline bool apTemplateWindowAvailable() {
    return PluginSignalHub::instance()->testSlot<RelationAPTemplateEditController*, const sem::ONE_PARAM_ARG_FUNC<quint64>&, quint64, const QString&>(OPEN_SINGLE_AP_TEMPLATE_WINDOW);
}

inline bool multiMsTemplateWindowAvailable() {
    return PluginSignalHub::instance()->testSlot<RelationMSMultiTemplateEditController*, const sem::ONE_PARAM_ARG_FUNC<quint64>&, quint64, const QString&>(OPEN_SINGLE_MULTI_MS_TEMPLATE_WINDOW);
}
inline bool mpTemplateWindowAvailable() {
    return PluginSignalHub::instance()->testSlot<RelationMPTemplateEditController*, const sem::ONE_PARAM_ARG_FUNC<quint64>&, quint64, const QString&>(OPEN_SINGLE_MP_TEMPLATE_WINDOW);
}
inline bool mpipTemplateWindowAvailable() {
    return PluginSignalHub::instance()->testSlot<RelationMpIpTemplateEditController*, const sem::ONE_PARAM_ARG_FUNC<quint64>&, quint64, const QString&>(OPEN_SINGLE_MPIP_TEMPLATE_WINDOW);
}
inline bool apTemplateComponentAvailable() {
    return PluginSignalHub::instance()->testSlot<RelationIDPAPTemplateEditController*, QWidget*>(CREATE_AP_TEMPLATE_COMPONENT);
}

inline bool mpTemplateComponentAvailable() {
    return PluginSignalHub::instance()->testSlot<RelationIDPMPTemplateEditController*, QWidget*>(CREATE_MP_TEMPLATE_COMPONENT);
}

inline MsgReceiveStatus<RelationAPTemplateEditController*> openSingleApTemplateWindow(const sem::ONE_PARAM_ARG_FUNC<quint64>& callback, quint64 id, const QString& title) {
    return PluginSignalHub::instance()->sendCreateObjMsg<RelationAPTemplateEditController*, const sem::ONE_PARAM_ARG_FUNC<quint64>&, quint64, const QString&>(OPEN_SINGLE_AP_TEMPLATE_WINDOW, callback, id, title);
}

inline MsgReceiveStatus<RelationMSMultiTemplateEditController*> openSingleMultiMsTemplateWindow(const sem::ONE_PARAM_ARG_FUNC<quint64>& callback, quint64 id, const QString& title) {
    return PluginSignalHub::instance()->sendCreateObjMsg<RelationMSMultiTemplateEditController*, const sem::ONE_PARAM_ARG_FUNC<quint64>&, quint64, const QString&>(OPEN_SINGLE_MULTI_MS_TEMPLATE_WINDOW, callback, id, title);
}

inline MsgReceiveStatus<RelationMPTemplateEditController*> openSingleMpTemplateWindow(const sem::ONE_PARAM_ARG_FUNC<quint64>& callback, quint64 id, const QString& title) {
    return PluginSignalHub::instance()->sendCreateObjMsg<RelationMPTemplateEditController*, const sem::ONE_PARAM_ARG_FUNC<quint64>&, quint64, const QString&>(OPEN_SINGLE_MP_TEMPLATE_WINDOW, callback, id, title);
}

inline MsgReceiveStatus<RelationMpIpTemplateEditController*> openSingleMpIpTemplateWindow(const sem::ONE_PARAM_ARG_FUNC<quint64>& callback, quint64 id, const QString& title) {
    return PluginSignalHub::instance()->sendCreateObjMsg<RelationMpIpTemplateEditController*, const sem::ONE_PARAM_ARG_FUNC<quint64>&, quint64, const QString&>(OPEN_SINGLE_MPIP_TEMPLATE_WINDOW, callback, id, title);
}

inline MsgReceiveStatus<RelationIDPAPTemplateEditController*> createApTemplateComponent(QWidget* parent) {
    return PluginSignalHub::instance()->sendCreateObjMsg<RelationIDPAPTemplateEditController*, QWidget*>(CREATE_AP_TEMPLATE_COMPONENT, parent);
}


inline MsgReceiveStatus<RelationIDPMultiMSTemplateEditController*> createMultiMsTemplateComponent(QWidget* parent) {
    return PluginSignalHub::instance()->sendCreateObjMsg<RelationIDPMultiMSTemplateEditController*, QWidget*>(CREATE_MULTI_MS_TEMPLATE_COMPONENT, parent);
}

inline MsgReceiveStatus<RelationIDPMPTemplateEditController*> createMpTemplateComponent(QWidget* parent) {
    return PluginSignalHub::instance()->sendCreateObjMsg<RelationIDPMPTemplateEditController*, QWidget*>(CREATE_MP_TEMPLATE_COMPONENT, parent);
}

inline bool singleApWindowOpened() {
    return SignalHubSingleObjectManager::instance()->getObject<RelationAPTemplateEditController*>(OPEN_SINGLE_AP_TEMPLATE_WINDOW);
}

inline bool singleMultiMsWindowOpened() {
    return SignalHubSingleObjectManager::instance()->getObject<RelationMSMultiTemplateEditController*>(OPEN_SINGLE_MULTI_MS_TEMPLATE_WINDOW);
}

inline bool singleMpWindowOpened() {
    return SignalHubSingleObjectManager::instance()->getObject<RelationMPTemplateEditController*>(OPEN_SINGLE_MP_TEMPLATE_WINDOW);
}
inline bool singleMpIpWindowOpened() {
    return SignalHubSingleObjectManager::instance()->getObject<RelationMpIpTemplateEditController*>(OPEN_SINGLE_MPIP_TEMPLATE_WINDOW);
}

}
#endif // TEMPLATE_RELATION_H
