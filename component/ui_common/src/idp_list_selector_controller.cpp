#include "idp_list_selector_controller.h"

#include "cde90_message_box.h"
#include "cde90_process_box.h"
#include "idp_data_service.h"
#include "list_view_data_query_helper.h"
#include "sem_global.h"
#include "service_manager.h"
#include "list_view_main_data_model.h"

IdpListSelectorController::IdpListSelectorController(QWidget* parent)
    : DataSelectController(parent) {}
IdpListSelectorController::~IdpListSelectorController() noexcept {}

void IdpListSelectorController::localSearchIdByName(
    ListViewMainData& data, int* errCode) {
    DataSelectController::localSearchIdByName(data, errCode);
    IdpDataService* service = ServiceManager::instance()->getDefaultService<IdpDataService>();
    IdpDataEntity entity = service->getSimpleIdpByName(classId, data.getText(), errCode);
    if (entity.getId() > 0) {
        data.setId(entity.getId());
        data.setTopId(entity.getTopId());
    }
}

void IdpListSelectorController::initIdpList(
    quint64 classId, quint64 idwId) {
    this->classId = classId;
    getSelectModel().clearSelection();
    bool succ = getAsyncCaller()->push(this, [this, classId, idwId](TSC) {
        if (classId == 0) {
            getDataModel().clear();
            return;
        }
        IdpSearchDTO param;
        param.setInvalid(0);
        param.setClassId(classId);
        if (idwId > 0) {
            param.setIdwId(idwId);
        }
        int errCode = localLoadIdpList(param, getDataModel());
        if (errCode) {
            simpleMessageBox(this, "Error", "Load IDP list error.");
            getDataModel().clear();
            return;
        }
    });
    if (succ) {
        loadingBox(this, "Working", "Loading...");
    }
}

