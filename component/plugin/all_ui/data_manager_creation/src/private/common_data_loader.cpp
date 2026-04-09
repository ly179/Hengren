#include "common_data_loader.h"
#include "class_service.h"
#include "msr_service.h"
#include "service_manager.h"
#include "class_entity.h"
#include "list_view_main_data_model.h"

FileCommonResult refreshClass(const ClassSearchDTO& searchParam, quint64 selectedId) {
    ClassService* service = ServiceManager::instance()->getDefaultService<ClassService>();
    FileCommonResult result;
    QList<ClassEntity> list = service->searchList(searchParam, &result.errCode);
    if (result.errCode) {
        result.errMsg = "Query Class List Error!";
        return result;
    }
    result.list.resize(list.size());
    for (int i = 0; i < list.size(); ++i) {
        result.list[i].setId(list[i].getId());
        result.list[i].setText(list[i].getClassName());
        if (result.selectedIndex == -1 && selectedId > 0 && selectedId == list[i].getId()) {
            result.selectedIndex = i;
        }
    }
    return result;
}

FileCommonResult refreshMSR(const MsrSearchDTO& searchParam, quint64 selectedTopId) {
    MsrService* service = ServiceManager::instance()->getDefaultService<MsrService>();
    FileCommonResult result;
    QList<MsrEntity> list = service->searchSimple(searchParam, &result.errCode);
    if (result.errCode) {
        result.errMsg = "Query MSR List Error!";
        return result;
    }
    result.list.resize(list.size());
    for (int i = 0; i < list.size(); ++i) {
        result.list[i].setId(list[i].getId());
        result.list[i].setTopId(list[i].getTopId());
        result.list[i].setText(list[i].getMsrName());
        if (result.selectedIndex == -1 && selectedTopId > 0 && selectedTopId == list[i].getTopId()) {
            result.selectedIndex = i;
        }
    }
    return result;
}
