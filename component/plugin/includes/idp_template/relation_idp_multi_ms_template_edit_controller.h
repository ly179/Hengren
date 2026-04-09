//
// Created by Shisan on 2026/1/15.
//

#ifndef HENGREN_CD_SEM_RELATION_IDP_MULTI_MS_TEMPLATE_EDIT_CONTROLLER_H
#define HENGREN_CD_SEM_RELATION_IDP_MULTI_MS_TEMPLATE_EDIT_CONTROLLER_H
#include "plugin_signal_hub_hook.h"
#include "cde90_frame.h"
#include "float_number.h"

class AsyncCaller;
class MultiAddressTemplateParam;
class CDE90CheckBox;
class CDE90TextFieldBox;
class RelationIDPMultiMSTemplateEditController : public CDE90Frame, public PluginSignalHubHook {
    Q_OBJECT
public:
    explicit RelationIDPMultiMSTemplateEditController(QWidget* parent = nullptr): CDE90Frame(parent) {}
    virtual ~RelationIDPMultiMSTemplateEditController() noexcept override {}

    virtual void initAsyncCaller(AsyncCaller* caller) = 0;
    virtual void initParam(MultiAddressTemplateParam* param) = 0;

    virtual void setRotTempContentVisible(bool) = 0;
    virtual CDE90CheckBox* rotateTemplateCheckBox() const = 0;
    virtual CDE90TextFieldBox* rotateTemplateTextField() const = 0;

    virtual void setGroup(const QString&) = 0;

    virtual void hideTemplateMenu(bool b = true) = 0;

    virtual void toDynamicRefStyle(bool = true) = 0;

    virtual void hidePrevBtn(bool = true) = 0;
    virtual void hideNextBtn(bool = true) = 0;

    virtual void openEndEditor() = 0;
    virtual void openFirstEditor() = 0;

    //禁用一切与机台互动相关的操作
    virtual void disablePyHandlers(bool b) = 0;

    virtual void setAPos(const IDW_PointF& aPos) = 0;

    virtual void resetToComponent() = 0;
    virtual QString collectData() = 0;
signals:
    void openDynamicLoader();
    void openStaticLoader();
    void reloadTemplate();
    void saveTemplate();
    void shouldHideParent(bool);

    void toPrevEditor();
    void toNextEditor();
};

#endif //HENGREN_CD_SEM_RELATION_IDP_MULTI_MS_TEMPLATE_EDIT_CONTROLLER_H