#include "idw_list_selector_controller.h"
#include "cde90_message_box.h"
#include "cde90_process_box.h"
#include "idw_wafer_service.h"
#include "list_view_data_query_helper.h"
#include "sem_global.h"
#include "service_manager.h"
#include "list_view_main_data_model.h"

IdwListSelectorController::IdwListSelectorController(QWidget* parent)
    : DataSelectController(parent) {}
IdwListSelectorController::~IdwListSelectorController() noexcept {}

void IdwListSelectorController::localSearchIdByName(
    ListViewMainData& data, int* errCode) {
    DataSelectController::localSearchIdByName(data, errCode);
    IDW_WaferService* service = ServiceManager::instance()->getDefaultService<IDW_WaferService>();
    IDW_WaferInfoEntity entity = service->getSimpleWaferByName(data.getText(), classId, errCode);
    if (entity.getId() != 0) {
        data.setId(entity.getId());
        data.setTopId(entity.getTopId());
    }
}

void IdwListSelectorController::initIdwList(
    quint64 classId) {
    this->classId = classId;
    getSelectModel().clearSelection();
    bool succ = getAsyncCaller()->push(this, [this, classId](TSC) {
        if (classId == 0) {
            getDataModel().clear();
            return;
        }
        int errCode = localLoadIdwList(classId, getDataModel());
        if (errCode) {
            simpleMessageBox(this, "Error", "Load IDW list error.");
            getDataModel().clear();
            return;
        }
    });
    if (succ) {
        loadingBox(this, "Working", "Loading...");
    }
}

