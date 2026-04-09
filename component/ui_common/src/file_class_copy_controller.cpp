#include "file_class_copy_controller.h"
#include "service_manager.h"
#include "class_service.h"

FileClassCopyController::FileClassCopyController(QWidget* parent) : FileSimpleCopyController(parent) {
}

FileClassCopyController::~FileClassCopyController() noexcept {}

QString FileClassCopyController::initData(const SimpleDataCopyParam& param) {
    ClassService* service = ServiceManager::defaultService<ClassService>();
    int errCode;
    ClassEntity entity = service->getById(param.fromId, &errCode);
    if (errCode) {
        return "Error to Load Class Info.";
    }
    if (entity.getId() == 0) {
        return "Class data is not exists.";
    }
    setFromName(entity.getClassName());
    setToName(QString("Copy of %1").arg(entity.getClassName()));
    return "";
}

QString FileClassCopyController::beginCopy(const SimpleDataCopyParam& param) {
    if (param.toName.isEmpty()) {
        return "Please input new Class Name.";
    }
    ClassService* service = ServiceManager::defaultService<ClassService>();
    OperationResult result = service->copyNew(param.fromId, param.toName);
    if (result.errCode) {
        return result.errMsg;
    }
    return "";
}
