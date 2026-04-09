#include "pattern_recognition_controller.h"
#include "cbox_global_keys.h"
#include "cde90_message_box.h"
#include "cde90_process_box.h"
#include "cdsem_data_common.h"
#include "cdsem_number_validator_helper.h"
#include "exception_code.h"
#include "service_manager.h"
#include "system_default_config_keys.h"
#include "system_default_config_service.h"
#include "template_data_model.h"
#include "template_data_model_convert.h"
#include "ui_pattern_recognition_controller.h"
#include "pattern_alg_itembox_helper.h"

PatternRecognitionController::PatternRecognitionController(
    cdsem::PatternRegParamOpenMode mode, QWidget* parent)
    : CDE90Dialog(parent)
    , mode(mode) {
    connect(this, &PatternRecognitionController::asyncCallerChanged, this, [this](){
        create();
        createAction();
    }, Qt::SingleShotConnection);
}

PatternRecognitionController::~PatternRecognitionController() noexcept {
    if (ui) {
        delete ui;
    }
}

void PatternRecognitionController::create() {
    ui = new cdsem::UiPatternRecognitionController;
    setContent(new QWidget);
    ui->init(getContent());
    if (mode != cdsem::PRPOM_AP_OM && mode != cdsem::PRPOM_AP_SEM) {
        ui->execBeforeRegContent->hide();
        if (mode == cdsem::PRPOM_MP_ADD) {
            ui->line1->hide();
        } else if (mode == cdsem::PRPOM_MP_M) {
            ui->loadRegisteredContent->show();
            ui->macroAlignLabel->show();
            ui->macroAlignItemBox->show();
            ui->lineSpaceDetectionLabel->show();
            ui->lineSpaceDetectionItemBox->show();
        }
    }
}

void PatternRecognitionController::createAction() {
    {
        CDSemNumberValidatorHelper helper = CDSemNumberValidatorHelper::range(0, 1000);
        helper.put(ui->lowLimitTextField->getQLineEdit()).autoFixMode();
    }
    itsChildAndBeManaged(ui->kindBtn, this);
    itsChildAndBeManaged(ui->algorithmBtn, this);
    itsChildAndBeManaged(ui->processorBtn, this);
    ui->kindBtn->setAsyncCaller(getAsyncCaller());
    ui->algorithmBtn->setAsyncCaller(getAsyncCaller());
    ui->processorBtn->setAsyncCaller(getAsyncCaller());

    SEM_OK_ACTION(ui->okBtn->getQPushButton())
    bindCloseBtn(ui->cancelBtn->getQPushButton());

    connect(ui->defaultBtn->getQPushButton(), &CDE90Button::clicked, this, [this]() {
        CDE90ProcessBox* box = loadingMessageBox(this, "Working", "Loading", true);
        getAsyncCaller()->push(this, [this](TSC){
            SystemDefaultConfigService* service = ServiceManager::instance()->getDefaultService<SystemDefaultConfigService>();
            SystemDefaultConfigEntity entity;
            int errCode;
            if (mode == cdsem::PRPOM_AP_OM || mode == cdsem::PRPOM_AP_SEM) {
                entity = service->getByKey(SDC_DEF_CONFIG_PATTERN_RECOGNITION_AP, &errCode);
            } else if (mode == cdsem::PRPOM_MP_ADD) {
                entity = service->getByKey(SDC_DEF_CONFIG_PATTERN_RECOGNITION_ADDRESS,
                                           &errCode);
            } else {
                entity = service->getByKey(SDC_DEF_CONFIG_PATTERN_RECOGNITION_MP, &errCode);
            }
            if (errCode) {
                if (errCode == SQL_QUERY_DATA_NOT_EXISTS) {
                    simpleMessageBox(this, "Error", "No default configuration available.");
                    return;
                } else {
                    simpleMessageBox(this, "Error", "Load default configuration error.");
                    return;
                }
            }
            PatternRecognitionAlgorithmSettingParam param;
            errCode = patternRegAlgJsonToStruct(entity.getSValue(), param, mode);
            if (errCode) {
                simpleMessageBox(this, "Error", "Load default config error, The configuration has some problem.");
                return;
            }
            MainThreadInvoker::getInvoker().blockRun([this, &param](){
                this->param = param;
                resetToComponent();
            });
        });
        autoReleaseLoadingBoxLater(box);
    });

    connect(ui->saveDefaultBtn->getQPushButton(), &CDE90Button::clicked, this, [this]() {
        QString errMsg = collectData();
        if (errMsg.length() > 0) {
            simpleMessageBox(this, "Error", errMsg);
            return;
        }
        CDE90ProcessBox* box = loadingMessageBox(this, "Working", "Saving...", true);
        ACaller->push(this, [this](TSC){
            QString json;
            patternRegAlgStructToJson(json, param);
            SystemDefaultConfigEntity entity;
            entity.setSValue(json);
            if (mode == cdsem::PRPOM_AP_OM || mode == cdsem::PRPOM_AP_SEM) {
                entity.setSKey(SDC_DEF_CONFIG_PATTERN_RECOGNITION_AP);
            } else if (mode == cdsem::PRPOM_MP_ADD) {
                entity.setSKey(SDC_DEF_CONFIG_PATTERN_RECOGNITION_ADDRESS);
            } else {
                entity.setSKey(SDC_DEF_CONFIG_PATTERN_RECOGNITION_MP);
            }
            SystemDefaultConfigService* service = ServiceManager::instance()->getDefaultService<SystemDefaultConfigService>();
            int errCode = service->saveWithTransaction(entity);
            if (errCode) {
                simpleMessageBox(this, "Error", "Save default configuration failed.");
                return;
            }
            simpleMessageBox(this, "Info", "Default configuration has updated.");
        });
        autoReleaseLoadingBoxLater(box);
    });

    connect(ui->kindBtn->getQComboBox(), &UComboBox::currentIndexChanged, this, [this](int) {
        kindChanged();
    });

    connect(ui->algorithmBtn->getQComboBox(), &UComboBox::currentIndexChanged, this, [this](int) {
        int alg = ui->algorithmBtn->currentData().toInt();
        if (mode == cdsem::PRPOM_AP_OM || mode == cdsem::PRPOM_AP_SEM) {
            if (alg == cdsem::MODEL_PATTERN_ALGORITHM_VERSATILE) {
                ui->abcCbox->setDisabled(true);
                ui->autoFocusCBox->setDisabled(true);
            } else if (alg == cdsem::MODEL_PATTERN_ALGORITHM_EDGE) {
                ui->abcCbox->setDisabled(false);
                ui->autoFocusCBox->setDisabled(false);
            }
        }
    });

    if (mode == cdsem::PRPOM_AP_OM || mode == cdsem::PRPOM_AP_SEM) {
        ui->kindBtn->loadItemFromDatabase(CBOX_TYPE_AP_MODEL_KIND, true, [this]() {
            kindChanged();
        });
    } else if (mode == cdsem::PRPOM_MP_ADD) {
        ui->kindBtn->loadItemFromDatabase(CBOX_TYPE_ADDRESS_MODEL_KIND,
                                          true,
                                          [this]() { kindChanged(); });
    } else {
        ui->kindBtn->loadItemFromDatabase(CBOX_TYPE_MP_MODEL_KIND, true, [this]() {
            kindChanged();
        });
        ui->lineSpaceDetectionItemBox->setAsyncCaller(getAsyncCaller());
        itsChildAndBeManaged(ui->lineSpaceDetectionItemBox, this);
        ui->lineSpaceDetectionItemBox->loadItemFromDatabase(
            CBOX_LINE_SPACE_DETECTION);
        ui->lineSpaceDetectionItemBox->safeSetCurrentData(cdsem::LINE_SPACE_DETECTION_OFF);
    }
    ui->processorBtn->loadItemFromDatabase(CBOX_PR_ALG_PROCESSOR);
}

void PatternRecognitionController::resetToComponent() {
    ui->kindBtn->safeSetCurrentData(QString::number(param.kind), true);
    // ui->algorithmBtn->safeSetCurrentData(QString::number(param.algorithm));
    kindChanged(param.algorithm);
    ui->processorBtn->safeSetCurrentData(QString::number(param.processor));
    ui->lowLimitTextField->setText(QString::number(param.lowLimit));
    if (mode == cdsem::PRPOM_AP_OM || mode == cdsem::PRPOM_AP_SEM) {
        ui->abcCbox->setChecked(param.enableABC);
        ui->autoFocusCBox->setChecked(param.enableAutoFocus);
    } else if (mode == cdsem::PRPOM_MP_M) {
        ui->axbCbox->setChecked(param.loadAPlusB);
        ui->ampCursorCbox->setChecked(param.loadAMPCursor);
        ui->lineSpaceDetectionItemBox->safeSetCurrentData(param.lineSpaceDetection);
        ui->macroAlignItemBox->setCurrentData(param.macroAlignment);
    }
}

QString PatternRecognitionController::collectData() {
    QString val = ui->kindBtn->currentData();
    if (val.isEmpty()) {
        return "Kind is not selected.";
    }
    param.kind = val.toInt();

    val = ui->processorBtn->currentData();
    if (val.isEmpty()) {
        return "Processor is not selected.";
    }
    param.processor = val.toInt();

    val = ui->algorithmBtn->currentData();
    if (val.isEmpty()) {
        return "Algorithm is not selected.";
    }
    param.algorithm = val.toInt();
    param.lowLimit = ui->lowLimitTextField->text().toInt();
    param.algMode = mode;
    if (mode == cdsem::PRPOM_AP_OM || mode == cdsem::PRPOM_AP_SEM) {
        param.enableABC = ui->abcCbox->isChecked();
        param.enableAutoFocus = ui->autoFocusCBox->isChecked();
    } else if (mode == cdsem::PRPOM_MP_M) {
        param.loadAPlusB = ui->axbCbox->isChecked();
        param.loadAMPCursor = ui->ampCursorCbox->isChecked();
        QString val = ui->lineSpaceDetectionItemBox->currentData();
        if (val.isEmpty()) {
            return "Line & space dection is not specified.";
        }
        param.lineSpaceDetection = val.toInt();
        param.macroAlignment = ui->macroAlignItemBox->currentData().toInt();
    }
    return "";
}

void PatternRecognitionController::kindChanged(int cData) {
    algKindChanged(&mode, cData, ui->kindBtn, ui->algorithmBtn);
}

void PatternRecognitionController::hideDefault(
    bool b) {
    ui->defaultBtn->setVisible(!b);
    ui->saveDefaultBtn->setVisible(!b);
}
