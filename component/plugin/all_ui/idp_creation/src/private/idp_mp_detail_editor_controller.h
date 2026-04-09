#ifndef IDP_MP_DETAIL_EDITOR_CONTROLLER_H
#define IDP_MP_DETAIL_EDITOR_CONTROLLER_H

#include "cde90_window.h"
#include "common_act_param_helper.h"
#include "idp_data_model.h"

namespace cdsem {
class UiIdpMpDetailEditorController;
};
class ListViewMainData;

struct IdpMpIpEditModel {
    IdpMpParamListData param;
    //seq序列下的当前测量点的前一个测量点的coordinate参数坐标
    IDW_PointF prevCoordinate;
    IDW_PointF oPoint;
    quint64 id = 0;
    Integer32 chipX;
    Integer32 chipY;
    qint32 chipNumber = -1;
    qint32 index = -1;
};

struct MPIPTemplateInfoModel;
struct IdpMpDetailEditorDataHelper;
class IdpMpDetailEditorController : public CDE90Window {
    friend struct IdpMpDetailEditorDataHelper;
    SEM_SET_INIT_PARAM(IdpMpIpEditModel)
    SEM_OK_CALL(IdpMpIpEditModel)

private:
    IdpMpDetailEditorDataHelper* dataHelper = nullptr;
    cdsem::UiIdpMpDetailEditorController* ui = nullptr;

    void create();
    void createAction();

    void resetToComponent();
    void resetMsToComponent();
    void resetMpToComponent();
    QString collectData();

    void createCoordinateSettingController();

    void createTemplateSelector();
    void openTemplateSelector(char type);
    void createSaveBox();
    void createMpipSelector();

    void localOverloadMSTemplateData(quint64 templateId, char refType, bool overview = false);
    void overloadMSTemplateData(bool overview = false);
    void localOverloadMPTemplateData(quint64 templateId, char refType, bool overview = false);
    void overloadMPTemplateData(bool overview = false);
    void openErrMsgBox(const QString&);
    bool titleSelectCall(const ListViewMainData& data);

    void loadTitleTemplate(const ListViewMainData& data);
    void saveTitleTemplate(const ListViewMainData& data);

    void collectTemplateTitleData(MPIPTemplateInfoModel& model);

    void closeEvent(QCloseEvent *event) override;

public:
    explicit IdpMpDetailEditorController(QWidget* parent = nullptr);
    ~IdpMpDetailEditorController() noexcept;

    //禁用一切与机台互动相关的操作
    void disablePyHandlers(bool b);

    //在调用collectData后调用
    //return true表示动态模板被改动过但每保存
    bool checkDynamicTemplateStatus();
    //返回动态模板原始数据，如果checkDynamicTemplateStatus()返回true，则应该使用该函数的内容
    void recoverToDynamicTemplateData();
};
#endif
