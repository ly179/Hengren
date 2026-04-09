#include "recipe_idw_data_select_controller.h"
#include "cde90_message_box.h"
#include "cde90_process_box.h"
#include "class_service.h"
#include "list_view_data_query_helper.h"
#include "service_manager.h"
#include "list_view_main_data_model.h"

RecipeIdwDataSelectController::RecipeIdwDataSelectController(
    QWidget* parent)
    : RecipeRefDataSelectController(parent) {}

RecipeIdwDataSelectController::~RecipeIdwDataSelectController() noexcept {};

void RecipeIdwDataSelectController::initIdwList(
    quint64 classId) {
    getSelectModel().clearSelection();
    getAsyncCaller()->push(this, [this, classId](TSC) {
        simpleLoadingBox(this, "Working", "Loading...", true);
        ClassService* service = ServiceManager::instance()->getDefaultService<ClassService>();
        int errCode;
        ClassEntity entity = service->getById(classId, &errCode);
        if (errCode) {
            simpleMessageBox(this, "Error", "Load class info error.");
            getDataModel().clear();
            return;
        }
        MainThreadInvoker::getInvoker().blockRun(
            [this, &entity]() { setClassName(entity.getClassName()); });
        errCode = localLoadIdwList(classId, getDataModel());
        if (errCode) {
            simpleMessageBox(this, "Error", "Load wafer list error.");
            getDataModel().clear();
            return;
        }
    });
}
