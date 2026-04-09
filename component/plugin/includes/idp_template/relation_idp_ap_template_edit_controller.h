#ifndef RELATION_IDP_AP_TEMPLATE_EDIT_CONTROLLER_H
#define RELATION_IDP_AP_TEMPLATE_EDIT_CONTROLLER_H
#include "plugin_signal_hub_hook.h"
#include "cde90_frame.h"
#include "float_number.h"
class AsyncCaller;
struct ApTemplateParam;
class RelationIDPAPTemplateEditController : public CDE90Frame, public PluginSignalHubHook {
    Q_OBJECT
public:
    explicit RelationIDPAPTemplateEditController(QWidget* parent = nullptr): CDE90Frame(parent) {}
    virtual ~RelationIDPAPTemplateEditController() override = default;
    virtual void initAsyncCaller(AsyncCaller* caller) = 0;
    virtual void initParam(ApTemplateParam* param) = 0;
    virtual void resetToComponent() = 0;
    virtual QString collectData() = 0;
    virtual void setGroup(const QString&) = 0;
    virtual void hideTemplateMenu(bool b = true) = 0;
    virtual void toDynamicRefStyle(bool = true) = 0;
    //禁用一切与机台互动相关的操作
    virtual void disablePyHandlers(bool b) = 0;
    virtual void setAPos(const IDW_PointF& aPos) = 0;

signals:
    void openDynamicLoader();
    void openStaticLoader();
    void reloadTemplate();
    void saveTemplate();
    //true start false over
    void shouldHideParent(bool);
    void posChanged(const IDW_PointF&);
};
#endif //RELATION_IDP_AP_TEMPLATE_EDIT_CONTROLLER_H
