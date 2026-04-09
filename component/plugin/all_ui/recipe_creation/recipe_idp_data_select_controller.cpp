#include "recipe_idp_data_select_controller.h"

#include "cde90_message_box.h"
#include "cde90_process_box.h"
#include "class_service.h"
#include "list_view_data_query_helper.h"
#include "service_manager.h"
#include "list_view_main_data_model.h"
#include "idp_data_entity.h"

RecipeIdpDataSelectController::RecipeIdpDataSelectController(
    QWidget* parent)
    : RecipeRefDataSelectController(parent) {}

RecipeIdpDataSelectController::~RecipeIdpDataSelectController() noexcept {};

void RecipeIdpDataSelectController::initIdpList(
    quint64 classId, quint64 idwId) {
    getSelectModel().clearSelection();
    getAsyncCaller()->push(this, [this, classId, idwId](TSC) {
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
        IdpSearchDTO param;
        param.setClassId(classId);
        if (idwId > 0) {
            param.setIdwId(idwId);
        }
        errCode = localLoadIdpList(param, getDataModel());
        if (errCode) {
            simpleMessageBox(this, "Error", "Load IDP list error.");
            getDataModel().clear();
            return;
        }
    });
}
