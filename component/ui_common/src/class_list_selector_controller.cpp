#include "class_list_selector_controller.h"
#include "list_view_data_query_helper.h"
#include "service_manager.h"
#include "class_service.h"
#include "cde90_process_box.h"
#include "cde90_message_box.h"
#include <QItemSelectionModel>
#include "sem_global.h"
#include "list_view_main_data_model.h"

ClassListSelectorController::ClassListSelectorController(QWidget* parent) :  DataSelectController(parent){}
ClassListSelectorController::~ClassListSelectorController() noexcept {}

void ClassListSelectorController::localSearchIdByName(ListViewMainData& data, int* errCode) {
    ClassService* service = ServiceManager::defaultService<ClassService>();
    data.setTopId(0);
    data.setId(service->getByName(data.getText(), errCode));
}

void ClassListSelectorController::initClassList() {
    getSelectModel().clearSelection();
    getAsyncCaller()->push(this, [this](TSC) {
        simpleLoadingBox(this, "Working", "Loading...", true);
        int errCode = localLoadClassList(getDataModel());
        if (errCode) {
            simpleMessageBox(this, "Error", "Load class list error.");
            getDataModel().clear();
            return;
        }
    });
}
