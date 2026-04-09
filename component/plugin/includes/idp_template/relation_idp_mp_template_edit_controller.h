#ifndef RELATION_IDP_MP_TEMPLATE_EDIT_CONTROLLER_H
#define RELATION_IDP_MP_TEMPLATE_EDIT_CONTROLLER_H
#include "plugin_signal_hub_hook.h"
#include "cde90_frame.h"
#include "float_number.h"
class AsyncCaller;
struct MpTemplateParam;
class CDE90CheckBox;
class CDE90TextFieldBox;
class RelationIDPMPTemplateEditController : public CDE90Frame, public PluginSignalHubHook {
    Q_OBJECT
public:
    explicit RelationIDPMPTemplateEditController(QWidget* parent = nullptr): CDE90Frame(parent) {}
    virtual ~RelationIDPMPTemplateEditController() noexcept {}
    virtual void initAsyncCaller(AsyncCaller* caller) = 0;
    virtual void initParam(MpTemplateParam* param) = 0;
    virtual void setDefaultDNumber(int) = 0;

    virtual void resetToComponent() = 0;
    virtual QString collectData() = 0;

    virtual void setRotTempContentVisible(bool) = 0;
    virtual CDE90CheckBox* rotateTemplateCheckBox() const = 0;
    virtual CDE90TextFieldBox* rotateTemplateTextField() const = 0;

    virtual void setGroup(const QString&) = 0;

    virtual void hideTemplateMenu(bool b = true) = 0;

    virtual void toDynamicRefStyle(bool = true) = 0;

    virtual void setHandleType(int type) = 0;

    virtual void hidePrevBtn(bool = true) = 0;
    virtual void openFirstEditor() = 0;

    //禁用一切与机台互动相关的操作
    virtual void disablePyHandlers(bool b) = 0;

    virtual void setAPos(const IDW_PointF& aPos) = 0;

signals:
    void openDynamicLoader();
    void openStaticLoader();
    void reloadTemplate();
    void saveTemplate();
    void shouldHideParent(bool);
    void toPrevEditor();
    //MP的测量点注册流程成功时触发该信号。
    //外部界面可通过该信号更新对应的组件，通过获取自己的param来得到对应的值
    void posChanged(const IDW_PointF&);

};
#endif //RELATION_IDP_MP_TEMPLATE_EDIT_CONTROLLER_H
