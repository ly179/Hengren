#include "recipe_environment_controller.h"
#include "cde90_message_box.h"
#include "cde90_process_box.h"
#include "cdsem_number_validator_helper.h"
#include "recipe_data_convert.h"
#include "recipe_data_model.h"
#include "sem_global.h"
#include "service_manager.h"
#include "system_default_config_global_keys.h"
#include "system_default_config_service.h"
#include "ui_recipe_environment_controller.h"

RecipeEnvironmentController::RecipeEnvironmentController(QWidget* parent) : CDE90Window(parent) {
    connect(
        this,
        &RecipeEnvironmentController::asyncCallerChanged,
        this,
        [this]() {
            create();
            createAction();
        },
        Qt::SingleShotConnection);
}

RecipeEnvironmentController::~RecipeEnvironmentController() noexcept {
    if (ui) {
        delete ui;
    }
}

void RecipeEnvironmentController::create() {
    ui = new cdsem::UiRecipeEnvironmentController;
    setContent(new QWidget);
    ui->init(getContent());
}

void RecipeEnvironmentController::createAction() {
    CDSemNumberValidatorHelper::range(0, 100).put(ui->confirmSuccRateTextField).autoFixMode();
    CDSemNumberValidatorHelper::range(0, 900).put(ui->ionizerTextField).autoFixMode();
    bindCloseBtn(ui->cancelBtn->getQPushButton());
    CDSEM_OK_ACTION2(ui->okBtn->getQPushButton(), RecipeEnvironmentModel)
    connect(ui->confirmItemBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this]() {
        int val = ui->confirmItemBox->currentValue();
        ui->succRateContent->setEnabled(val == 1 || val == 2);
    });
    connect(ui->saveDefaultBtn->getQPushButton(), &QPushButton::clicked, this, [this]() {
        saveDefault();
    });
}

QString RecipeEnvironmentController::collectData(
    RecipeEnvironmentModel& param) {
    RecipeMeasureResultModel& resultModel = param.measureResult;
    resultModel.diskSave = ui->diskSaveCBox->isChecked();
    resultModel.sendToHost = ui->sendToHostCBox->isChecked();
    resultModel.printOut = ui->printOutCBox->isChecked();
    resultModel.networkTransfer = ui->networkTransferCBox->isChecked();
    resultModel.confirm = ui->confirmItemBox->currentValue();
    if (resultModel.confirm < 0) {
        return "Measurement Result Confirm is not specified.";
    }
    if (ui->succRateContent->isEnabled()) {
        QString text = ui->confirmSuccRateTextField->text();
        if (text.length() > 0) {
            resultModel.succRate = text.toInt();
        } else {
            resultModel.succRate = 100;
        }
    }
    resultModel.method = ui->methodItemBox->currentValue();
    if (resultModel.method < 0) {
        return "Measurement Result Method is not specified.";
    }
    resultModel.limitCheck = ui->limitCheckItemBox->currentValue();
    if (resultModel.limitCheck < 0) {
        return "Measurement Result Limit Check is not specified.";
    }
    RecipeProcessModel& waferAlign = param.waferAlign;
    waferAlign.executeMode = ui->waferExeItemBox->currentValue();
    if (waferAlign.executeMode < 0) {
        return "Wafer Alignment Execute Mode is not specified.";
    }
    waferAlign.manualAssist = ui->waferExeManualAssistItemBox->currentValue();
    if (waferAlign.manualAssist < 0) {
        return "Wafer Alignment Manual Assist is not specified.";
    }
    waferAlign.imageSave = ui->waferExeImageSaveItemBox->currentValue();
    if (waferAlign.imageSave < 0) {
        return "Wafer Alignment Image Save is not specified.";
    }

    RecipeProcessModel& masureAddress = param.measureAdderss;
    masureAddress.executeMode = ui->measureAddressItemBox->currentValue();
    if (masureAddress.executeMode < 0) {
        return "Measurement Addressing Execute Mode is not specified.";
    }
    masureAddress.manualAssist = ui->measureAddressManualAssistItemBox->currentValue();
    if (masureAddress.manualAssist < 0) {
        return "Measurement Addressing Manual Assist is not specified.";
    }
    masureAddress.imageSave = ui->measureAddressImageSaveItemBox->currentValue();
    if (masureAddress.imageSave < 0) {
        return "Measurement Addressing Image Save is not specified.";
    }

    RecipeProcessModel& measureExe = param.measureExe;
    measureExe.executeMode = ui->measureExeItemBox->currentValue();
    if (measureExe.executeMode < 0) {
        return "Measurement Execution Execute Mode is not specified.";
    }
    measureExe.manualAssist = ui->measureExeManualAssistItemBox->currentValue();
    if (measureExe.manualAssist < 0) {
        return "Measurement Execution Manual Assist is not specified.";
    }
    measureExe.imageSave = ui->measureExeImageSaveItemBox->currentValue();
    if (measureExe.imageSave < 0) {
        return "Measurement Execution Image Save is not specified.";
    }

    RecipeEnvAdditionSettingModel& env = param.additionSetting;
    env.editIdpAfterWaferAlign = ui->editIdpCBox->isChecked();
    env.presetBC = ui->presetBcCBox->isChecked();
    env.retrySemAlignWithAFCC = ui->retryAlignCBox->isChecked();
    env.retryAddressWithAFCC = ui->retryAddressCBox->isChecked();
    env.retryPatternRegWithAFCC = ui->retryPatternRegCBox->isChecked();
    env.reMeasureErrorSite = ui->reMeasureErrCBox->isChecked();
    env.periodicWaferAlign = ui->periodicWaferAlignCBox->isChecked();
    QString text = ui->ionizerTextField->text();
    if (text.length() > 0) {
        env.ionizerIrradition = text.toInt();
    }

    return "";
}

void RecipeEnvironmentController::resetToComponent(
    const RecipeEnvironmentModel& param) {
    ui->waferExeItemBox->setCurrentData(param.waferAlign.executeMode);
    ui->waferExeManualAssistItemBox->setCurrentData(param.waferAlign.manualAssist);
    ui->waferExeImageSaveItemBox->setCurrentData(param.waferAlign.imageSave);
    ui->measureAddressItemBox->setCurrentData(param.measureAdderss.executeMode);
    ui->measureAddressManualAssistItemBox->setCurrentData(param.measureAdderss.manualAssist);
    ui->measureAddressImageSaveItemBox->setCurrentData(param.measureAdderss.imageSave);
    ui->measureExeItemBox->setCurrentData(param.measureExe.executeMode);
    ui->measureExeManualAssistItemBox->setCurrentData(param.measureExe.manualAssist);
    ui->measureExeImageSaveItemBox->setCurrentData(param.measureExe.imageSave);
    ui->diskSaveCBox->setChecked(param.measureResult.diskSave);
    ui->sendToHostCBox->setChecked(param.measureResult.sendToHost);
    ui->printOutCBox->setChecked(param.measureResult.printOut);
    ui->networkTransferCBox->setChecked(param.measureResult.networkTransfer);
    ui->confirmItemBox->setCurrentData(param.measureResult.confirm);
    ui->confirmSuccRateTextField->setText(param.measureResult.succRate.toString());
    ui->methodItemBox->setCurrentData(param.measureResult.method);
    ui->limitCheckItemBox->setCurrentData(param.measureResult.limitCheck);
    ui->editIdpCBox->setChecked(param.additionSetting.editIdpAfterWaferAlign);
    ui->presetBcCBox->setChecked(param.additionSetting.presetBC);
    ui->retryAlignCBox->setChecked(param.additionSetting.retrySemAlignWithAFCC);
    ui->retryAddressCBox->setChecked(param.additionSetting.retryAddressWithAFCC);
    ui->retryPatternRegCBox->setChecked(param.additionSetting.retryPatternRegWithAFCC);
    ui->reMeasureErrCBox->setChecked(param.additionSetting.reMeasureErrorSite);
    ui->periodicWaferAlignCBox->setChecked(param.additionSetting.periodicWaferAlign);
    ui->ionizerTextField->setText(param.additionSetting.ionizerIrradition.toString());
}

void RecipeEnvironmentController::saveDefault() {
    bool succ = ACaller->push(this, [this](TSC) {
        bool collectSucc = true;
        RecipeEnvironmentModel model;
        MainThreadInvoker::getInvoker().blockRun([this, &model, &collectSucc]() {
            QString errMsg = collectData(model);
            if (errMsg.length() > 0) {
                simpleMessageBox(this, "Error", errMsg);
                collectSucc = false;
            }
        });
        if (!collectSucc) {
            return;
        }
        QString json;
        recipeEnvironmentToJson(json, model);
        SystemDefaultConfigEntity entity;
        entity.setSKey(sysConf::DEF_CONFIG_MEASURE_ENV);
        entity.setSValue(json);
        SystemDefaultConfigService* service
            = ServiceManager::instance()->getDefaultService<SystemDefaultConfigService>();
        int errCode = service->saveWithTransaction(entity);
        if (errCode) {
            simpleMessageBox(this, "Error", "Save default data error.");
            return;
        }
        simpleMessageBox(this, "Info", "Save default data successfully.");
    });
    if (succ) {
        loadingBox(this, "Working", "Saving...");
    }
}
