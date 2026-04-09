#include "recipe_ref_check_controller.h"
#include "cde90_process_box.h"
#include "idp_data_service.h"
#include "idw_wafer_service.h"
#include "recipe_data_service.h"
#include "sem_global.h"
#include "service_manager.h"
#include "ui_recipe_ref_check_controller.h"

RecipeRefCheckController::RecipeRefCheckController(
    QWidget* parent)
    : CDE90Window(parent) {
    //如果你没有异步任务需求，请将connect清理掉，保留create和createAction函数
    connect(this, &RecipeRefCheckController::asyncCallerChanged, this, [this](){
        create();
        createAction();
    }, Qt::SingleShotConnection);
}

RecipeRefCheckController::~RecipeRefCheckController() noexcept {
    if (ui) {
        delete ui;
    }
}

void RecipeRefCheckController::create() {
    ui = new cdsem::UiRecipeRefCheckController;
    setContent(new QWidget);
    ui->init(getContent());
}

void RecipeRefCheckController::createAction() {
    bindCloseBtn(ui->closeBtn->getQPushButton());
}

void RecipeRefCheckController::checkIdwRef(
    quint64 classId) {
    ui->textContent->clear();
    bool succ = ACaller->push(this, [this, classId](TSC) {
        QString checkResult = "IDW Cross Reference\nCross Reference List";
        RecipeDataService* recipeService = ServiceManager::instance()
                                               ->getDefaultService<RecipeDataService>();
        RecipeSearchDTO recipeParam;
        recipeParam.setClassId(classId);
        recipeParam.setInvalid(0);
        int errCode;
        QList<RecipeDataEntity> list = recipeService->searchList(recipeParam, &errCode);
        if (errCode) {
            checkResult.append("\nReference Check Error.");
            MainThreadInvoker::getInvoker().blockRun(
                [this, &checkResult]() { ui->textContent->setText(checkResult); });
            return;
        }
        if (list.isEmpty()) {
            checkResult.append("\nNo Recipe found.");
            MainThreadInvoker::getInvoker().blockRun(
                [this, &checkResult]() { ui->textContent->setText(checkResult); });
            return;
        }
        QList<quint64> idwIds;
        idwIds.reserve(list.size());
        for (RecipeDataEntity& entity : list) {
            if (entity.getIdwId() > 0) {
                idwIds.append(entity.getIdwId());
            }
        }
        if (idwIds.isEmpty()) {
            MainThreadInvoker::getInvoker().blockRun(
                [this, &checkResult]() { ui->textContent->setText(checkResult); });
            return;
        }
        // 如果需求变为Recipe关联的IDP更新后动态关联更新，则这里需要动态查询最新的IDP
        IDW_WaferService* idwService = ServiceManager::instance()
                                           ->getDefaultService<IDW_WaferService>();
        IDW_WaferSearchDTO idwParam;
        idwParam.setIds(idwIds);
        QList<IDW_WaferInfoEntity> idwList = idwService->searchList(idwParam, &errCode);
        if (errCode) {
            checkResult.append("\nReference Check Error.");
            MainThreadInvoker::getInvoker().blockRun(
                [this, &checkResult]() { ui->textContent->setText(checkResult); });
            return;
        }
        if (idwList.isEmpty()) {
            MainThreadInvoker::getInvoker().blockRun(
                [this, &checkResult]() { ui->textContent->setText(checkResult); });
            return;
        }
        QHash<quint64, QString> idwNameMap;
        idwNameMap.reserve(idwList.size());
        for (IDW_WaferInfoEntity& entity : idwList) {
            idwNameMap.insert(entity.getId(), entity.getWaferName());
        }
        for (RecipeDataEntity& entity : list) {
            auto it = idwNameMap.find(entity.getIdwId());
            if (it == idwNameMap.end()) {
                continue;
            }
            checkResult.append(
                QString("\nRecipe=[%1]\n          IDW=[%2]").arg(entity.getRecipeName()).arg(*it));
        }
        MainThreadInvoker::getInvoker().blockRun(
            [this, &checkResult]() { ui->textContent->setText(checkResult); });
    });
    if (succ) {
        loadingBox(this, "Working", "Checking");
    }
}

void RecipeRefCheckController::checkIdpRef(
    quint64 classId) {
    ui->textContent->clear();
    bool succ = ACaller->push(this, [this, classId](TSC) {
        QString checkResult = "IDP Cross Reference\nCross Reference List";
        RecipeDataService* recipeService = ServiceManager::instance()
                                               ->getDefaultService<RecipeDataService>();
        RecipeSearchDTO recipeParam;
        recipeParam.setClassId(classId);
        recipeParam.setInvalid(0);
        int errCode;
        QList<RecipeDataEntity> list = recipeService->searchList(recipeParam, &errCode);
        if (errCode) {
            checkResult.append("\nReference Check Error.");
            MainThreadInvoker::getInvoker().blockRun(
                [this, &checkResult]() { ui->textContent->setText(checkResult); });
            return;
        }
        if (list.isEmpty()) {
            checkResult.append("\nNo Recipe found.");
            MainThreadInvoker::getInvoker().blockRun(
                [this, &checkResult]() { ui->textContent->setText(checkResult); });
            return;
        }
        QList<quint64> idpIds;
        idpIds.reserve(list.size());
        for (RecipeDataEntity& entity : list) {
            if (entity.getIdpId() > 0) {
                idpIds.append(entity.getIdpId());
            }
        }
        if (idpIds.isEmpty()) {
            MainThreadInvoker::getInvoker().blockRun(
                [this, &checkResult]() { ui->textContent->setText(checkResult); });
            return;
        }
        // 如果需求变为Recipe关联的IDP更新后动态关联更新，则这里需要动态查询最新的IDP
        IdpDataService* idpService = ServiceManager::instance()->getDefaultService<IdpDataService>();
        IdpSearchDTO idpParam;
        idpParam.setIds(idpIds);
        QList<IdpDataEntity> idpList = idpService->searchList(idpParam, &errCode);
        if (errCode) {
            checkResult.append("\nReference Check Error.");
            MainThreadInvoker::getInvoker().blockRun(
                [this, &checkResult]() { ui->textContent->setText(checkResult); });
            return;
        }
        if (idpList.isEmpty()) {
            MainThreadInvoker::getInvoker().blockRun(
                [this, &checkResult]() { ui->textContent->setText(checkResult); });
            return;
        }
        QHash<quint64, QString> idpNameMap;
        idpNameMap.reserve(idpList.size());
        for (IdpDataEntity& entity : idpList) {
            idpNameMap.insert(entity.getId(), entity.getIdpName());
        }
        for (RecipeDataEntity& entity : list) {
            auto it = idpNameMap.find(entity.getIdpId());
            if (it == idpNameMap.end()) {
                continue;
            }
            checkResult.append(
                QString("\nRecipe=[%1]\n          IDP=[%2]").arg(entity.getRecipeName()).arg(*it));
        }
        MainThreadInvoker::getInvoker().blockRun(
            [this, &checkResult]() { ui->textContent->setText(checkResult); });
    });
    if (succ) {
        loadingBox(this, "Working", "Checking");
    }
}
