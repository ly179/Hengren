#include "list_view_data_query_helper.h"
#include "class_service.h"
#include "idp_data_service.h"
#include "idw_wafer_service.h"
#include "recipe_data_service.h"
#include "sem_global.h"
#include "service_manager.h"
#include "list_view_main_data_model.h"

int localLoadClassList(
    ListViewMainModel& listModel) {
    ClassService* service = ServiceManager::instance()->getDefaultService<ClassService>();
    ClassSearchDTO param;
    int errCode;
    QList<ClassEntity> list = service->searchList(param, &errCode);
    if (errCode) {
        return errCode;
    }
    if (list.isEmpty()) {
        return 0;
    }
    listModel.resizeDataList(list.size());
    QList<ListViewMainData>& viewList = listModel.getDataList();
    for (int i = 0; i < list.size(); ++i) {
        ClassEntity& e = list[i];
        ListViewMainData& data = viewList[i];
        data.setId(e.getId());
        data.setTopId(0);
        data.setText(e.getClassName());
    }
    listModel.notifyDataChanged(0, list.size() - 1);
    return 0;
}

int localLoadIdwList(
    quint64 classId, ListViewMainModel& listModel) {
    IDW_WaferService* service = ServiceManager::instance()
                                    ->getDefaultService<IDW_WaferService>();
    IDW_WaferSearchDTO param;
    param.setClassId(classId);
    param.setInvalid(0);
    int errCode;
    QList<IDW_WaferInfoEntity> list = service->searchList(param, &errCode);
    if (errCode) {
        return errCode;
    }
    int dataLen = list.size();
    listModel.resizeDataList(dataLen);
    QList<ListViewMainData>& modeDataList = listModel.getDataList();
    for (int i = 0; i < dataLen; ++i) {
        IDW_WaferInfoEntity& entity = list[i];
        ListViewMainData& view = modeDataList[i];
        view.setText(entity.getWaferName());
        view.setId(entity.getId());
        view.setTopId(entity.getTopId());
    }
    MainThreadInvoker::getInvoker().runLater(
        [&listModel, dataLen]() { listModel.notifyDataChanged(0, dataLen - 1); });
    return 0;
}

int localLoadIdpList(
    const IdpSearchDTO& param, ListViewMainModel& listModel) {
    IdpDataService* service = ServiceManager::instance()->getDefaultService<IdpDataService>();
    int errCode;
    QList<IdpDataEntity> list = service->searchList(param, &errCode);
    if (errCode) {
        return errCode;
    }
    int dataLen = list.size();
    listModel.resizeDataList(dataLen);
    QList<ListViewMainData>& modeDataList = listModel.getDataList();
    for (int i = 0; i < dataLen; ++i) {
        IdpDataEntity& entity = list[i];
        ListViewMainData& view = modeDataList[i];
        view.setText(entity.getIdpName());
        view.setId(entity.getId());
        view.setTopId(entity.getTopId());
    }
    MainThreadInvoker::getInvoker().runLater(
        [&listModel, dataLen]() { listModel.notifyDataChanged(0, dataLen - 1); });
    return 0;
}

int localLoadRecipeList(
    quint64 classId, ListViewMainModel& listModel, quint64 idwId) {
    RecipeDataService* service = ServiceManager::instance()
                                     ->getDefaultService<RecipeDataService>();
    RecipeSearchDTO param;
    param.setClassId(classId);
    param.setInvalid(0);
    if (idwId > 0) {
        param.setIdwId(idwId);
    }
    int errCode;
    QList<RecipeDataEntity> list = service->searchList(param, &errCode);
    if (errCode) {
        return errCode;
    }
    int dataLen = list.size();
    listModel.resizeDataList(dataLen);
    QList<ListViewMainData>& modeDataList = listModel.getDataList();
    for (int i = 0; i < dataLen; ++i) {
        RecipeDataEntity& entity = list[i];
        ListViewMainData& view = modeDataList[i];
        view.setText(entity.getRecipeName());
        view.setId(entity.getId());
        view.setTopId(entity.getTopId());
    }
    MainThreadInvoker::getInvoker().runLater(
        [&listModel, dataLen]() { listModel.notifyDataChanged(0, dataLen - 1); });
    return 0;
}
