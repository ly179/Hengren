#ifndef IDP_AP_DETAIL_EDITOR_CONTROLLER_H
#define IDP_AP_DETAIL_EDITOR_CONTROLLER_H
#include "cde90_window.h"
#include "common_act_param_helper.h"
#include "idp_data_model.h"

namespace cdsem {
class UiIdpApDetailEditorController;
};
class IdpApTemplateController;
class TemplateSelectorController;
class CDE90MessageBox;
class TemplateSaveBoxController;
class ApSaveBoxCaller;

struct IdpApEditModel {
    IdpApParamListData param;
    IDW_PointF oPoint;
    IDW_PointF offsetOPoint;
    quint64 id = 0;
    Integer32 chipX;
    Integer32 chipY;
    qint32 chipNumber = -1;
    qint32 index = -1;
};

struct IdpApDetailEditorDataHelper;
class IdpApDetailEditorController : public CDE90Window {
    friend struct IdpApDetailEditorDataHelper;
    SEM_OK_CALL(IdpApEditModel)
    SEM_SET_INIT_PARAM(IdpApEditModel)

private:
    cdsem::UiIdpApDetailEditorController* ui = nullptr;
    IdpApDetailEditorDataHelper* dataHelper = nullptr;
    void closeEvent(QCloseEvent* event) override;
    void create();
    void createAction();
    void resetToComponent();
    QString collectData();

public:
    explicit IdpApDetailEditorController(QWidget* parent = nullptr);
    ~IdpApDetailEditorController() noexcept;

    //禁用一切与机台互动相关的操作
    void disablePyHandlers(bool b);
    //在调用collectData后调用
    //return true表示动态模板被改动过但每保存
    bool checkDynamicTemplateStatus();
    //返回动态模板原始数据，如果checkDynamicTemplateStatus()返回true，则应该使用该函数的内容
    void recoverToDynamicTemplateData();
};
#endif
