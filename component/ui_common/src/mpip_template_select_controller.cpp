#include "mpip_template_select_controller.h"
#include "cde90_message_box.h"
#include "cde90_process_box.h"
#include "sem_global.h"
#include "service_manager.h"
#include "template_info_service.h"
#include "list_view_main_data_model.h"

MPIPTemplateSelectController::MPIPTemplateSelectController(QWidget* parent)
    : DataSelectController(parent) {}
MPIPTemplateSelectController::~MPIPTemplateSelectController() noexcept {}

void MPIPTemplateSelectController::localSearchIdByName(
    ListViewMainData& data, int* errCode) {
    DataSelectController::localSearchIdByName(data, errCode);
    TemplateInfoService* service = ServiceManager::instance()
                                       ->getDefaultService<TemplateInfoService>();
    TemplateInfoEntity entity = service->getSimpleInfoByName(data.getText(), type, errCode);
    if (entity.getId() > 0) {
        data.setId(entity.getId());
        data.setTopId(entity.getTopId());
    }
}

void MPIPTemplateSelectController::initMpIpList(int type) {
    this->type = type;
    getSelectModel().clearSelection();
    bool succ = getAsyncCaller()->push(this, [this, type](TSC) {
        TemplateInfoService* service = ServiceManager::instance()
                                           ->getDefaultService<TemplateInfoService>();
        TemplateInfoSearchDTO param;
        param.setTemplateType(type);
        param.setInvalid(0);
        int errCode;
        QList<TemplateInfoEntity> list = service->searchList(param, &errCode);
        if (errCode) {
            simpleMessageBox(this, "Error", "Load template list error.");
            getDataModel().clear();
            return;
        }
        int dataLen = list.size();
        getDataModel().resizeDataList(dataLen);
        QList<ListViewMainData>& modeDataList = getDataModel().getDataList();
        for (int i = 0; i < dataLen; ++i) {
            TemplateInfoEntity& entity = list[i];
            ListViewMainData& view = modeDataList[i];
            view.setText(entity.getTemplateName());
            view.setId(entity.getId());
            view.setTopId(entity.getTopId());
        }
        MainThreadInvoker::getInvoker().runLater(
            [this, dataLen]() { getDataModel().notifyDataChanged(0, dataLen - 1); });
    });
    if (succ) {
        loadingBox(this, "Working", "Loading...");
    }
}
