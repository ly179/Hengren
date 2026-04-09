#ifndef RELATION_MAIN_CONTROLLER_H
#define RELATION_MAIN_CONTROLLER_H
#include "cde90_window.h"
#include "plugin_signal_hub_hook.h"

class RelationMainController : public CDE90Window, public PluginSignalHubHook {
    Q_OBJECT
public:
    explicit RelationMainController(QWidget* parent = nullptr): CDE90Window(parent) {}
    virtual ~RelationMainController() noexcept {}
signals:
    void retryBtnClick();

};
#endif // RELATION_MAIN_CONTROLLER_H
