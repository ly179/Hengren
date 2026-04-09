#include "recipe_eraser_controller.h"
#include <QDateTime>
#include "cde90_message_box.h"
#include "cde90_process_box.h"
#include "class_service.h"
#include "idp_data_service.h"
#include "idw_wafer_service.h"
#include "recipe_data_service.h"
#include "recipe_ref_check_controller.h"
#include "service_manager.h"
#include "ui_recipe_eraser_controller.h"
#include "view_data_mode.h"

struct RecipeEraserViewData {
    QDateTime lastExecDate;
    QDateTime lastEditTime;
    QString recipeName;
    QString idwName;
    QString idpName;
    quint64 id;
};

class RecipeEraseViewModel : public SimpleRowTableModel<RecipeEraserViewData> {
private:
private:
    QVariant getItemData(
        qint32 row, qint32 column) const {
        const RecipeEraserViewData& data = getDataList()[row];

        switch (column) {
        case 0:
            return data.recipeName;
        case 1:
            return data.lastExecDate.toString("yyyy/MM/dd/HH:mm");
        case 2:
            return data.lastEditTime.toString("yyyy/MM/dd/HH:mm");
        case 3:
            return data.idwName;
        case 4:
            return data.idpName;
        }
        return "";
    }

public:
    RecipeEraseViewModel(
        QObject* parent)
        : SimpleRowTableModel(parent) {}
};

RecipeEraserController::RecipeEraserController(QWidget* parent) : CDE90Window(parent) {
    //如果你没有异步任务需求，请将connect清理掉，保留create和createAction函数
    connect(this, &RecipeEraserController::asyncCallerChanged, this, [this](){
        create();
        createAction();
    }, Qt::SingleShotConnection);
}

RecipeEraserController::~RecipeEraserController() noexcept {
    if (ui) {
        delete ui;
    }
}

void RecipeEraserController::create() {
    ui = new cdsem::UiRecipeEraserController;
    setContent(new QWidget);
    ui->init(getContent());
}

void RecipeEraserController::createAction() {
    tableViewModel = new RecipeEraseViewModel(ui->tableContent);
    tableViewModel->insertHead("Recipe Name");
    tableViewModel->insertHead("Last Exec Time(Ascent)");
    tableViewModel->insertHead("Last Edit Time");
    tableViewModel->insertHead("IDW Name");
    tableViewModel->insertHead("IDP Name");
    ui->tableContent->replaceDataModel(tableViewModel);

    QList<qint32> columnWidths;
    columnWidths.reserve(5);
    columnWidths << 115 << 165 << 165 << 115 << 115;
    ui->tableContent->setMinColumnsSize(columnWidths);
    ui->tableContent->recalcColumnSize();
    ui->tableContent->setSelectionMode(QAbstractItemView::SingleSelection);

    bindCloseBtn(ui->closeBtn->getQPushButton());

    connect(ui->refCheckBtn1->getQPushButton(), &QPushButton::clicked, this, [this]() {
        createRefCheckController();
        refCheckController->setWindowTitle("IDP Cross Reference");
        refCheckController->show();
        MainThreadInvoker::getInvoker().moveToScreenCenter(refCheckController);
        refCheckController->checkIdpRef(classId);
    });
    connect(ui->refCheckBtn2->getQPushButton(), &QPushButton::clicked, this, [this]() {
        createRefCheckController();
        refCheckController->setWindowTitle("IDW Cross Reference");
        refCheckController->show();
        MainThreadInvoker::getInvoker().moveToScreenCenter(refCheckController);
        refCheckController->checkIdwRef(classId);
    });
    connect(ui->deleteBtn->getQPushButton(), &QPushButton::clicked, this, [this]() {
        int index = ui->tableContent->getLimitSelectedRow();
        if (index < 0) {
            return;
        }
        quint64 id = tableViewModel->getDataList()[index].id;
        int r = showYesNoBox(this, "Question", "Will you delete the selected data?", true);
        if (!r) {
            deleteRecipe(id);
        }
    });
}

void RecipeEraserController::deleteRecipe(
    quint64 id) {
    bool refIdp = ui->whileDelIdpCBox->isChecked();
    bool refIdw = ui->whileDelIdwCBox->isChecked();
    bool succ = ACaller->push(this, [this, id, refIdp, refIdw](TSC) {
        RecipeDataService* service = ServiceManager::instance()
                                         ->getDefaultService<RecipeDataService>();
        OperationResult result;
        service->eraseRecipe(id, refIdw, refIdp, &result);
        if (result.errCode) {
            simpleMessageBox(this, "Error", result.errMsg);
            return;
        }
        MainThreadInvoker::getInvoker().blockRun([this]() {
            simpleMessageBox(this, "Info", "Delete Recipe successfully.");
            refreshDataList(classId);
        });
    });
    if (succ) {
        loadingBox(this, "Workding", "Processing...");
    }
}

void RecipeEraserController::createRefCheckController() {
    if (refCheckController) {
        return;
    }
    refCheckController = new RecipeRefCheckController(this);
    refCheckController->setWindowModality(Qt::WindowModal);
    refCheckController->setResizable(false);
    refCheckController->resize(600, 400);
    refCheckController->initAsyncCaller(ACaller);
}

void RecipeEraserController::refreshDataList(
    quint64 classId) {
    this->classId = classId;
    tableViewModel->clear();
    ui->classTextField->getQLineEdit()->clear();
    bool succ = ACaller->push(this, [this, classId](TSC) {
        int errCode;
        ClassService* classService = ServiceManager::instance()->getDefaultService<ClassService>();
        ClassEntity classEntity = classService->getById(classId, &errCode);
        if (errCode) {
            simpleMessageBox(this, "Error", "Error to load Class data.");
        } else if (classEntity.getId() > 0) {
            MainThreadInvoker::getInvoker().blockRun([this, &classEntity]() {
                ui->classTextField->setText(classEntity.getClassName());
            });
        }

        RecipeDataService* service = ServiceManager::instance()
                                         ->getDefaultService<RecipeDataService>();
        RecipeSearchDTO param;
        param.setClassId(classId);
        param.setInvalid(0);

        QList<RecipeDataEntity> list = service->searchList(param, &errCode);
        if (errCode) {
            simpleMessageBox(this, "Error", "Error to load Recipe data.");
            return;
        }
        if (list.isEmpty()) {
            return;
        }
        QList<quint64> idwIds;
        QList<quint64> idpIds;
        idwIds.reserve(list.size());
        idpIds.reserve(list.size());
        quint64 tmpId;
        for (RecipeDataEntity& entity : list) {
            tmpId = entity.getIdwId();
            if (tmpId > 0) {
                idwIds.append(tmpId);
            }
            tmpId = entity.getIdpId();
            if (tmpId > 0) {
                idpIds.append(tmpId);
            }
        }
        QHash<quint64, QString> idwNameMap;
        QHash<quint64, QString> idpNameMap;
        idwNameMap.reserve(list.size());
        idpNameMap.reserve(list.size());
        if (idwIds.size() > 0) {
            IDW_WaferService* idwService = ServiceManager::instance()
                                               ->getDefaultService<IDW_WaferService>();
            IDW_WaferSearchDTO param;
            param.setIds(idwIds);
            int errCode;
            QList<IDW_WaferInfoEntity> list = idwService->searchList(param, &errCode);
            if (errCode) {
                simpleMessageBox(this,
                                 "Error",
                                 "Load IDW file name error, the loading will continue.");
            } else {
                for (IDW_WaferInfoEntity& e : list) {
                    idwNameMap.insert(e.getId(), e.getWaferName());
                }
            }
        }
        if (idpIds.size() > 0) {
            IdpDataService* idpService = ServiceManager::instance()
                                             ->getDefaultService<IdpDataService>();
            IdpSearchDTO param;
            param.setIds(idpIds);
            int errCode;
            QList<IdpDataEntity> list = idpService->searchList(param, &errCode);
            if (errCode) {
                simpleMessageBox(this,
                                 "Error",
                                 "Load IDP file name error, the loading will continue.");
            } else {
                for (IdpDataEntity& e : list) {
                    idpNameMap.insert(e.getId(), e.getIdpName());
                }
            }
        }

        int dataLen = list.size();
        tableViewModel->resizeDataList(dataLen);
        QList<RecipeEraserViewData>& modeDataList = tableViewModel->getDataList();
        for (int i = 0; i < dataLen; ++i) {
            RecipeDataEntity& entity = list[i];
            RecipeEraserViewData& view = modeDataList[i];
            view.id = entity.getId();
            view.recipeName = entity.getRecipeName();
            view.lastEditTime = entity.getCreateTime();
            view.idwName = idwNameMap.value(entity.getIdwId(), "");
            view.idpName = idpNameMap.value(entity.getIdpId(), "");
        }
        MainThreadInvoker::getInvoker().blockRun(
            [this, &list]() { tableViewModel->notifyDataChanged(0, list.size() - 1); });
    });
    if (succ) {
        loadingBox(this, "Workding", "Loading...");
    }
}
