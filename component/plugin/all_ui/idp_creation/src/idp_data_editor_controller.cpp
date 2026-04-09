#include "idp_data_editor_controller.h"
#include <QHash>
#include <QSettings>
#include <QSpacerItem>
#include "private/axis_alignment_wafer_controller.h"
#include "cde90_message_box.h"
#include "cde90_process_box.h"
#include "cdsem_data_common.h"
#include "class_service.h"
#include "private/idp_ap_detail_editor_controller.h"
#include "private/idp_ap_wafer_selector_controller.h"
#include "private/idp_chip_selector_controller.h"
#include "private/idp_d_no_measurement_controller.h"
#include "private/idp_d_no_p_controller.h"
#include "idp_data_model_convert.h"
#include "idp_data_service.h"
#include "private/idp_measurement_arrange_setting_controller.h"
#include "private/idp_mp_detail_editor_controller.h"
#include "private/idp_mp_wafer_selector_controller.h"
#include "private/idp_p_measure_times_controller.h"
#include "private/idp_p_no_controller.h"
#include "private/idp_shift_coordinate_setting_controller.h"
#include "idw_wafer_service.h"
#include "private/image_environment_controller.h"
#include "image_info_service.h"
#include "exception_code.h"
#include "service_manager.h"
#include "template_info_model_convert.h"
#include "template_info_service.h"
#include "ui_idp_data_editor_controller.h"
#include "view_data_mode.h"
#include "z_sensor_setting_controller.h"
#include "service_manager.h"
#include "cdsem_global_data.h"
#include "private/idp_init_stage_different_controller.h"
#include "idw_model_data_convert.h"
#include "private/idp_opened_stage_different_controller.h"
#include "image_save_model_convert.h"
#include "recipe_procedure.h"
#include "image_relation_record_service.h"
#include "transaction_manager_service.h"
#include "sem_common.h"
#include "private/idp_common_util.h"
#include "idw_wafer_data_model.h"
#include "idp_wizard_window.h"
#include "ap/ap_wizard_components.h"
#include "image_file_temp_handler.h"

#include "relation_idp_ap_template_edit_controller.h"
#include "relation_idp_multi_ms_template_edit_controller.h"
#include "relation_idp_mp_template_edit_controller.h"
#include "template_relation.h"

struct IdpApTableModeData {
    QSharedPointer<IdpApPosListData> posData;
    QSharedPointer<IdpApParamListData> detailParamData;
};

struct IdpApSaveTmpData {
    ImageAddtionDataModel apImageDetail;
    QWeakPointer<IdpApParamListData> data;
    QString path;
    quint64 id = 0;
};

struct AddressSaveTmpData {
    ImageAddtionDataModel detail;
    QString path;
    quint64 imageId = 0;
    int index = -1;
};

struct IdpMpSaveTmpData {
    ImageAddtionDataModel mpImageDetail;
    ImageAddtionDataModel exeImageDetail;
    QWeakPointer<IdpMpParamListData> data;
    QList<AddressSaveTmpData> addressList;
    QString mpPath;
    QString exePath;
    quint64 mpId = 0;
    quint64 exeId = 0;
};

struct IdpAxisSaveTmpData {
    ImageAddtionDataModel alignImageDetail;
    ImageAddtionDataModel stigmaImageDetail;
    QString axisAlignPath;
    QString axisStigmaPath;
    quint64 axisAlignId = 0;
    quint64 axisStigmaId = 0;
    bool available = false;
};

struct TotalSaveImgTmpData {
    QList<IdpMpSaveTmpData> mpData;
    IdpAxisSaveTmpData axisData;
    QList<IdpApSaveTmpData> apData;
};

struct IDP_SaveDataParam {
    QString name;
    quint64 id;
};

struct D_By_P_Data {
    QString name;
    QList<int> list;
};

template<class T>
int helpToCutData(SimpleRowTableModel<T>* tableMode,
                  CDE90TableView* table,
                  QList<T>& copied,
                  bool cut = true) {
    copied.clear();
    QList<qint32> indices = table->getOrderedSelectedRows();
    if (indices.isEmpty()) {
        return 0;
    }
    QList<T>& list = tableMode->getDataList();
    copied.reserve(indices.size());
    for (qint32& i : indices) {
        copied.append(list[i]);
    }
    if (cut) {
        tableMode->removeDatas(indices);
    }
    return indices.size();
}

template<class T>
void helpToPaste(SimpleRowTableModel<T>* tableMode,
                 CDE90TableView* table,
                 QList<T>& copied,
                 bool before) {
    if (copied.isEmpty()) {
        return;
    }
    int index = table->getLimitSelectedRow();
    if (index == -1) {
        if (before) {
            index = 0;
        }
        // index = before ? 0 : list.size();
    } else if (!before) {
        ++index;
    }
    tableMode->insertDatas(copied, index);
    copied.clear();
}

class IDP_ApDataListMode : public SimpleRowTableModel<IdpApTableModeData> {
private:
    QVariant getItemData(qint32 row, qint32 column) const {
        const IdpApTableModeData& data = getDataList()[row];
        IdpApParamListData* detailParam = nullptr;
        if (!data.detailParamData.isNull()) {
            detailParam = data.detailParamData.data();
        }

        switch (column) {
        case 0:
            return row + 1;
        case 1:
            return data.posData->chipX == nullptr ? "-" : data.posData->chipX.toString();
        case 2:
            return ",";
        case 3:
            return data.posData->chipY == nullptr ? "-" : data.posData->chipY.toString();
        case 4:
            if (detailParam && detailParam->dPoint.x != nullptr && data.posData->oPoint.x != nullptr) {
                return (detailParam->dPoint.x + data.posData->oPoint.x).toQStringAndKeepFullDecimal();
            } else if (detailParam && detailParam->dPoint.x != nullptr) {
                return detailParam->dPoint.x.toQStringAndKeepFullDecimal();
            } else if (data.posData->oPoint.x != nullptr) {
                data.posData->oPoint.x.toQStringAndKeepFullDecimal();
            }
            return QVariant();
        case 5:
            return ",";
        case 6:
            if (detailParam && detailParam->dPoint.y != nullptr && data.posData->oPoint.y != nullptr) {
                return (detailParam->dPoint.y + data.posData->oPoint.y).toQStringAndKeepFullDecimal();
            } else if (detailParam && detailParam->dPoint.y != nullptr) {
                return detailParam->dPoint.y.toQStringAndKeepFullDecimal();
            } else if (data.posData->oPoint.y != nullptr) {
                data.posData->oPoint.y.toQStringAndKeepFullDecimal();
            }
            return QVariant();

        case 7:
            if (detailParam) {
                return detailParam->pNumber;
            } else {
                return "0";
            }
        case 8:
            if (detailParam) {
                if (detailParam->apTemplate.templateParam.patternRegParam.imgType == cdsem::PIT_OM) {
                    return "OM";
                } else if (detailParam->apTemplate.templateParam.patternRegParam.imgType
                           == cdsem::PIT_SEM) {
                    return "SEM";
                }
            }
            return "Unknow";
        }
        return "";
    }

public:
    IDP_ApDataListMode(QObject* parent)
        : SimpleRowTableModel(parent) {}
};

struct IdpDataEditorDataHelper {
    IDP_BaseData baseData;

    QRect windowGeometry;

    QList<IdpApSelectedChip> apWaitAddChips;
    QList<IdpMpSelectedChip> mpWaitAddChips;
    QList<IdpApTableModeData> apCutDatas;
    QList<IdpMpTableModeData> mpCutDatas;

    IdpApWaferSelectorController* apWaferSelector = nullptr;
    IdpApDetailEditorController* apDetailEditor = nullptr;
    IdpChipSelectorController* chipSelectorController = nullptr;
    IdpMpWaferSelectorController* mpWaferSelector = nullptr;
    IdpMpDetailEditorController* mpDetailEditor = nullptr;
    IdpMeasurementArrangeSettingController* measurementDuplicateController = nullptr;
    IdpShiftCoordinateSettingController* shiftCoordinateController = nullptr;
    IdpDNoByPController* dNoByPController = nullptr;
    IdpDNoMeasurementController* dNoByMeasurementController = nullptr;
    IdpPNoController* pNoController = nullptr;
    IdpPMeasureTimesController* timesController = nullptr;
    ImageEnvironmentController* imageEnvController = nullptr;
    ZSensorSettingController* zSensorController = nullptr;
    AxisAlignmentWaferController* axisAlignmentController = nullptr;
    IdpInitStageDifferentController* initStageDiffController = nullptr;
    IdpOpenedStageDifferentController* openedStageDiffController = nullptr;

    IDP_ApDataListMode* apTableMode;
    IdpMpDataTableModel* mpTableMode;

    cdsem::UiIdpDataEditorController* ui = nullptr;
    CDE90InputBox* saveAsBox = nullptr;
    IdpDataEditorController* d;

    IdpWizardWindow* apWizardWindow = nullptr;
    ApWizardComponents* apWizardComponents = nullptr;

    //modify -> chip no选项卡时使用该值
    QSharedPointer<IdpApParamListData> selectedApParam;
    QSharedPointer<IdpMpParamListData> selectedMpParam;

    int maxAP_P_Number = 1;
    int maxMP_P_Number = 1;

    bool pyHandlerDisabled = false;
    bool pyRealDisabled = false;

    QString localLoadGlobalIDW(quint64 idwId) {
        int errCode;
        if (idwId != 0) {
            IDW_WaferService* service = ServiceManager::defaultService<IDW_WaferService>();
            IDW_WaferInfoEntity entity = service->getWaferInfoById(idwId, &errCode);
            if (errCode) {
                return "Error to load IDW Data";
            }
            if (entity.getId() == 0) {
                return "IDW data is not exists.";
            }
            if (entity.getId() != 0) {
                IDW_WaferData waferData;
                IDW_ShotChipData shotData;
                IDW_ShotChipData chipData;
                errCode = waferDataFromEntity(entity, waferData, shotData, chipData);
                if (errCode) {
                    return "Error to load IDW Data";
                }
                cdsem::CDSEM_GlobalDataHelper::get()->setGlobalIDW(entity.getId(), entity.getTopId(), waferData, shotData, chipData);
            }
        }
        return "";
    }

    void createOpenedDIffStageController() {
        if (initStageDiffController || openedStageDiffController) return;
        openedStageDiffController = new IdpOpenedStageDifferentController(d);
        openedStageDiffController->setWindowTitle("Warning");
        openedStageDiffController->setWindowModality(Qt::WindowModal);
        openedStageDiffController->setResizable(false);
        openedStageDiffController->setAttribute(Qt::WA_DeleteOnClose);
        openedStageDiffController->disableClose();
        openedStageDiffController->setCloseCall([this](CDE90WindowCloseStatus){
            openedStageDiffController = nullptr;
            return true;
        });
        openedStageDiffController->setOkCallback([this](){
            IdpOpenedStageDiffMode mode = openedStageDiffController->getMode();
            if (mode == IOSDM_RELOAD) {
                quint64 idwId = this->baseData.idwId;
                bool succ = d->getAsyncCaller()->push(d, [this, idwId](TSC){
                    QString errMsg = localLoadGlobalIDW(idwId);
                    if (errMsg.length() > 0) {
                        simpleMessageBox(d, "Error", errMsg);
                    } else {
                        MainThreadInvoker::getInvoker().runLater([this](){
                            if (openedStageDiffController) {
                                openedStageDiffController->changeCloseStatusOnce(HW_OK);
                                openedStageDiffController->disableClose(false);
                                openedStageDiffController->close();
                            }
                            disablePyHandlers(false);
                        });
                    }
                });
                if (succ) {
                    loadingBoxHelper(d, "Working", "Loading...");
                }
                return false;
            } else if (mode == IOSDM_SAVE_EXIT) {
                d->saveData({d->param.idpName, baseData.idpId}, [this](){
                    d->close();
                });
                return false;
            } else if (mode == IOSDM_SAVE_AS_EXIT) {
                QString text = openedStageDiffController->saveAsFileName();
                if (text.isEmpty()) {
                    simpleMessageBox(d, "Error", "Please input file name.");
                    return false;
                }
                d->saveData({text, 0}, [this](){d->close();}, true);
                return false;
            } else if (mode == IOSDM_IGNORE) {
                d->changeWindowColor(qRgb(227, 171, 98), true);
                openedStageDiffController->disableClose(false);
                disablePyHandlers();
                return true;
            }
            return false;
        });
    }

    void createInitDiffStageController() {
        if (initStageDiffController) return;
        initStageDiffController = new IdpInitStageDifferentController(d);
        initStageDiffController->setWindowTitle("Warning");
        initStageDiffController->setWindowModality(Qt::WindowModal);
        initStageDiffController->setResizable(false);
        initStageDiffController->setAttribute(Qt::WA_DeleteOnClose);
        initStageDiffController->setCloseCall([this](CDE90WindowCloseStatus status){
            if (status == HW_CLOSE) {
                initStageDiffController = nullptr;
                d->close();
                return true;
            } else if (status == HW_USER) {
                initStageDiffController = nullptr;
                return true;
            }
            IdpInitStageDiffMode mode = initStageDiffController->getMode();
            if (mode == IISDM_RELOAD) {
                quint64 idwId = this->baseData.idwId;
                bool succ = d->getAsyncCaller()->push(d, [this, idwId](TSC){
                    QString errMsg = localLoadGlobalIDW(idwId);
                    if (errMsg.length() > 0) {
                        simpleMessageBox(d, "Error", errMsg);
                    } else {
                        MainThreadInvoker::getInvoker().runLater([this](){
                            if (!initStageDiffController) return;
                            initStageDiffController->changeCloseStatusOnce(HW_USER);
                            initStageDiffController->close();
                        });
                    }
                });
                if (succ) {
                    loadingBoxHelper(d, "Working", "Loading...");
                }
            } else if (mode == IISDM_IGNORE) {
                d->changeWindowColor(qRgb(227, 171, 98), true);
                disablePyHandlers();
                initStageDiffController = nullptr;
                return true;
            }
            return false;
        });
    }

    //禁用一切与机台相关的操作按钮，相当于离线编辑一样
    void disablePyHandlers(bool b = true) {
        pyHandlerDisabled = b;
        updatePyDisabledStatus();
    }

    void updatePyDisabledStatus() {
        bool b = pyHandlerDisabled;
        if (!b && cdsem::CDSEM_GlobalDataHelper::get()->procedureRunningStatus() != cdsem::PRS_STOPPED) {
            b = true;
        }
        pyRealDisabled = b;
        if (apDetailEditor) {
            apDetailEditor->disablePyHandlers(b);
        }
        if (mpDetailEditor) {
            mpDetailEditor->disablePyHandlers(b);
        }

        if (b && apWizardWindow && apWizardWindow->isVisible()) {
            apWizardWindow->disableClose(false);
            apWizardWindow->close();
        }
        ui->apWizardBtn->setDisabled(b);
        ui->mpWizardBtn->setDisabled(b);
        ui->apAlignmentBtn->setDisabled(b);
        ui->measurementBtn->setDisabled(b);
    }

    bool apNewPDataWithIDW(const IdpApEditModel& data) {
        for (int i = 0; i < apWaitAddChips.size();) {
            IdpApSelectedChip& c = apWaitAddChips[i];
            if (i == 0) {
                c.number = data.chipNumber;
                c.x = data.chipX;
                c.y = data.chipY;
                c.oPoint = data.oPoint;
            } else if (c.number == data.chipNumber) {
                int r = showYesNoBox(d, "Warning", "This Pattern Group contains duplicate chips. Identical entries will be automatically removed if you proceed.");
                if (r) {
                    return false;
                }
                //只有重复一个的可能，不可能重复多个，因此不需要考虑多次弹出对话框的问题
                apWaitAddChips.remove(i);
                continue;
            }
            ++ i;
        }

        int oldSize = apTableMode->getDataList().size();
        ++maxAP_P_Number;
        int start = oldSize;
        apTableMode->resizeDataList(apWaitAddChips.size() + oldSize);
        QSharedPointer<IdpApParamListData> sharedData(new IdpApParamListData);
        bool dataSetted = false;
        for (IdpApSelectedChip& c : apWaitAddChips) {
            IdpApTableModeData& d = apTableMode->getDataList()[start];
            if (d.posData.isNull()) {
                d.posData.reset(new IdpApPosListData);
            }
            d.posData->oPoint = c.oPoint;
            d.posData->chipX = c.x;
            d.posData->chipY = c.y;
            d.posData->chipNumber = c.number;
            d.detailParamData = sharedData;
            ++start;
            if (dataSetted) {
                continue;
            }
            *sharedData = data.param;
            dataSetted = true;
        }
        apTableMode->notifyDataChanged(oldSize, apTableMode->getDataList().size() - 1);
        ui->apTable->recalcColumnSize();
        apWaitAddChips.clear();
        return true;
    }

    bool apNewPDataWithoutIDW(const IdpApEditModel& data) {
        int oldSize = apTableMode->getDataList().size();
        ++maxAP_P_Number;
        apTableMode->resizeDataList(oldSize + 1);
        QSharedPointer<IdpApParamListData> sharedData(new IdpApParamListData);
        *sharedData = data.param;
        IdpApTableModeData& d = apTableMode->getDataList()[oldSize];
        if (d.posData.isNull()) {
            d.posData.reset(new IdpApPosListData);
        }
        d.posData->chipNumber = -1;
        d.detailParamData = sharedData;
        // d.detailParamData->chipCount = 0;
        apTableMode->notifyDataChanged(oldSize, apTableMode->getDataList().size() - 1);
        ui->apTable->recalcColumnSize();
        return true;
    }

    bool apEditPData(const IdpApEditModel& data) {
        int repeatIndex = -1;
        QList<IdpApTableModeData>& list = apTableMode->getDataList();
        //再怎么改一次最多也只可能有两个之间重复。
        for (int i = 0; i < list.size(); ++ i) {
            if (i == data.index) continue;
            IdpApTableModeData& c = list[i];
            if (c.posData->chipNumber == data.chipNumber && c.detailParamData->pNumber == data.param.pNumber) {
                int r = showYesNoBox(d, "Warning", "This Pattern Group contains duplicate chips. Identical entries will be automatically removed if you proceed.");
                if (r) {
                    return false;
                }
                repeatIndex = i;
                break;
            }
        }
        IdpApTableModeData& d = apTableMode->getDataList()[data.index];
        *d.detailParamData = data.param;
        apTableMode->notifyDataChanged(data.index, data.index);
        if (repeatIndex >= 0) {
            apTableMode->removeData(repeatIndex);
        }
        return true;
    }

    IDW_PointF getPrevPNoCoordinate(int pNo) const {
        QList<IdpMpTableModeData>& list = mpTableMode->getDataList();
        int rightPNo = -1;
        int index = -1;
        for (int i = 0; i < list.size(); ++ i) {
            IdpMpTableModeData& data = list[i];
            if (data.detailParamData->pNumber >= pNo) {
                continue;
            }
            if (data.detailParamData->pNumber > rightPNo) {
                rightPNo = data.detailParamData->pNumber;
                index = i;
            }
        }
        if (index == -1) {
            return IDW_PointF{};
        }
        return list[index].detailParamData->dPoint;
    }

    void collectDString(const QList<int>& dNums, QString& str) {
        str = "";
        for (int i = 0; i < dNums.size(); ++ i) {
            if (i > 0) str.append(",");
            if (i > 1) {
                str.append("...");
                break;
            }
            str.append(QString::number(dNums[i]));
        }
    }

    void appendApDataFromWizard(const ApWizardDataResult& param) {
        int oldSize = apTableMode->getDataList().size();
        int newSize = oldSize;
        if (param.omParam.valid) ++ newSize;
        if (param.semParam.valid) ++ newSize;
        if (newSize == oldSize) return;
        apTableMode->resizeDataList(newSize);
        int begin = oldSize;
        if (param.omParam.valid) {
            QSharedPointer<IdpApParamListData> sharedData(new IdpApParamListData);
            sharedData->apTemplate.templateParam.patternRegParam = *param.omParam.param;
            IdpApTableModeData& d = apTableMode->getDataList()[begin];
            if (d.posData.isNull()) {
                d.posData.reset(new IdpApPosListData);
            }
            d.posData->oPoint = IDW_PointF {param.chipCoord.cx, param.chipCoord.cy};
            d.posData->chipX = param.chipCoord.chipX;
            d.posData->chipY = param.chipCoord.chipY;
            d.posData->chipNumber = param.chipCoord.globalChip;
            sharedData->dPoint = param.omParam.dPoint;
            sharedData->pNumber = maxAP_P_Number;
            d.detailParamData = sharedData;
            ++ begin;
            ++maxAP_P_Number;
        }
        if (param.semParam.valid) {
            QSharedPointer<IdpApParamListData> sharedData(new IdpApParamListData);
            sharedData->apTemplate.templateParam.patternRegParam = *param.semParam.param;
            if (param.semFocusParam && param.semFocusParam->enable) {
                sharedData->apTemplate.templateParam.autoFoucsParam.enable = true;
                sharedData->apTemplate.templateParam.autoFoucsParam.focusParam = *param.semFocusParam;
            }
            IdpApTableModeData& d = apTableMode->getDataList()[begin];
            if (d.posData.isNull()) {
                d.posData.reset(new IdpApPosListData);
            }
            d.posData->oPoint = IDW_PointF {param.chipCoord.cx, param.chipCoord.cy};
            d.posData->chipX = param.chipCoord.chipX;
            d.posData->chipY = param.chipCoord.chipY;
            d.posData->chipNumber = param.chipCoord.globalChip;
            d.detailParamData = sharedData;
            sharedData->dPoint = param.semParam.dPoint;
            sharedData->pNumber = maxAP_P_Number;
            ++maxAP_P_Number;
        }
        apTableMode->notifyDataChanged(oldSize, apTableMode->getDataList().size() - 1);
        ui->apTable->recalcColumnSize();
    }

};

IdpDataEditorController::IdpDataEditorController(QWidget* parent): RelationIdpEditController(parent) {
    connect(
        this,
        &IdpDataEditorController::asyncCallerChanged,
        this,
        [this]() {
            create();
            createAction();
        },
        Qt::SingleShotConnection);
}
IdpDataEditorController::~IdpDataEditorController() noexcept {
    if (dataHelper) {
        if (dataHelper->ui) {
            delete dataHelper->ui;
        }

        //为了确保group能够正确清理，这里必须手动调用各个与group相关的资源释放。
        if (dataHelper->apWizardWindow) delete dataHelper->apWizardWindow;
        if (dataHelper->apDetailEditor) delete dataHelper->apDetailEditor;
        if (dataHelper->mpDetailEditor) delete dataHelper->mpDetailEditor;
        delete dataHelper;
    }
    cdsem::removeNotExistsGroupFile();
}

bool IdpDataEditorController::eventFilter(QObject* watch, QEvent* event) {
    if (watch != this)
        return RelationIdpEditController::eventFilter(watch, event);
    if (event->type() == QEvent::Show) {
        if (dataHelper->windowGeometry.isValid())
            setGeometry(dataHelper->windowGeometry);
    } else if (event->type() == QEvent::Hide) {
        dataHelper->windowGeometry = geometry();
    } else {
        return RelationIdpEditController::eventFilter(watch, event);
    }
    return true;
}

void IdpDataEditorController::create() {
    dataHelper = new IdpDataEditorDataHelper;
    dataHelper->ui = new cdsem::UiIdpDataEditorController;
    dataHelper->d = this;
    setContent(new QWidget);
    dataHelper->ui->init(getContent());
}
void IdpDataEditorController::createAction() {
    installEventFilter(this);
    dataHelper->apTableMode = new IDP_ApDataListMode(dataHelper->ui->apTable);
    dataHelper->apTableMode->insertHead("No.", 0, 0);
    dataHelper->apTableMode->insertHead("Chip No.", 1, 3);
    dataHelper->apTableMode->insertHead("Coordinate X,Y", 4, 6);
    dataHelper->apTableMode->insertHead("P No.", 7, 7);
    dataHelper->apTableMode->insertHead("Image", 8, 8);
    dataHelper->ui->apTable->replaceDataModel(dataHelper->apTableMode);

    dataHelper->mpTableMode = new IdpMpDataTableModel(dataHelper->ui->mpTable);
    QList<int> columnsSize;
    columnsSize << 40 << 30 << 5 << 30 << 80 << 5 << 80 << 50 << 50;
    dataHelper->ui->apTable->setMinColumnsSize(columnsSize);
    dataHelper->ui->apTable->recalcColumnSize();

    dataHelper->ui->apNewAction->setEnabled(idp_template::apTemplateComponentAvailable());
    dataHelper->ui->apEditAction->setEnabled(dataHelper->ui->apNewAction->isEnabled());

    dataHelper->ui->mpNewAction->setEnabled(idp_template::multiMsTemplateWindowAvailable() && idp_template::mpTemplateComponentAvailable());
    dataHelper->ui->mpEditAction->setEnabled(dataHelper->ui->mpNewAction->isEnabled());

    connect(dataHelper->ui->apNewAction, &QAction::triggered, this, [this]() {
        if (dataHelper->baseData.idwId > 0) {
            showIdpApChipSelector();
        } else {
            if (!createIdpApDetailEditor()) {
                simpleMessageBox(this, "Error", "Error to open AP Editor");
                return;
            }
            IdpApEditModel param;
            param.param.pNumber = dataHelper->maxAP_P_Number;
            param.id = dataHelper->baseData.idpId;
            dataHelper->apDetailEditor->setParam(param);
            dataHelper->apDetailEditor->show();
            this->hide();
        }
    });
    connect(dataHelper->ui->apEditAction, &QAction::triggered, this, [this]() {
        int index = dataHelper->ui->apTable->getLimitSelectedRow();
        if (index < 0) {
            return;
        }
        IdpApTableModeData& modeData = dataHelper->apTableMode->getDataList()[index];
        if (modeData.detailParamData.isNull()) {
            return;
        }
        if (!createIdpApDetailEditor()) {
            simpleMessageBox(this, "Error", "Error to open AP Editor");
            return;
        }
        if (modeData.posData.isNull()) {
            modeData.posData.reset(new IdpApPosListData);
        }
        IdpApEditModel param;
        param.id = dataHelper->baseData.idpId;
        param.param = *modeData.detailParamData;
        param.chipNumber = modeData.posData->chipNumber;
        param.chipX = modeData.posData->chipX;
        param.chipY = modeData.posData->chipY;
        param.index = index;
        param.oPoint = modeData.posData->oPoint;
        dataHelper->apDetailEditor->setParam(param);
        dataHelper->apDetailEditor->show();
        this->hide();
    });

    connect(dataHelper->ui->mpChipBtn->getQPushButton(), &CDE90Button::clicked, this, [this]() {
        dataHelper->chipSelectorController = new IdpChipSelectorController(this);
        dataHelper->chipSelectorController->initAsyncCaller(getAsyncCaller());
        dataHelper->chipSelectorController->setWindowTitle("Measurement Chip Editor");
        dataHelper->chipSelectorController->setAttribute(Qt::WA_DeleteOnClose);
        dataHelper->chipSelectorController->setOkCallback([this](const QList<IdpMpTableModeData>& list) {
            dataHelper->mpTableMode->clear();
            dataHelper->mpTableMode->getDataList() = list;
            dataHelper->mpTableMode->resizeViewList(list.size());
            return true;
        });
        dataHelper->chipSelectorController->show();
        MainThreadInvoker::getInvoker().moveToScreenCenter(dataHelper->chipSelectorController);
        dataHelper->chipSelectorController->copyDataToSeqTable(dataHelper->mpTableMode->getDataList());
        dataHelper->chipSelectorController->initIdwTopId(dataHelper->baseData.idwTopId, QList<qint32>());
    });

    connect(dataHelper->ui->mpNewAction, &QAction::triggered, this, [this]() {
        if (dataHelper->baseData.idwId > 0) {
            showIdpMpChipSelector();
        } else {
            //New IDP without IDW
            if (!createIdpMpDetailEditor()) {
                simpleMessageBox(this, "Error", "Error to open MP/IP Editor");
                return;
            }
            IdpMpIpEditModel param;
            param.param.pNumber = dataHelper->maxMP_P_Number;
            param.id = dataHelper->baseData.idpId;
            param.oPoint = {0, 0};
            dataHelper->mpDetailEditor->setParam(param);
            dataHelper->mpDetailEditor->show();
            this->hide();
        }
    });

    connect(dataHelper->ui->mpEditAction, &QAction::triggered, this, [this]() {
        int index = dataHelper->ui->mpTable->getLimitSelectedRow();
        if (index < 0) {
            return;
        }
        QList<IdpMpTableModeData>& list = dataHelper->mpTableMode->getDataList();
        IdpMpTableModeData& modeData = list[index];
        if (modeData.detailParamData.isNull()) {
            return;
        }
        if (!createIdpMpDetailEditor()) {
            simpleMessageBox(this, "Error", "Error to open MP/IP Editor");
            return;
        }
        if (modeData.posData.isNull()) {
            modeData.posData.reset(new IdpMpPosListData);
        }
        IdpMpIpEditModel param;
        param.id = dataHelper->baseData.idpId;
        param.index = index;
        param.param = *modeData.detailParamData;
        param.chipNumber = modeData.posData->chipNumber;
        param.chipX = modeData.posData->chipX;
        param.chipY = modeData.posData->chipY;
        param.oPoint = modeData.posData->oPoint;
        param.prevCoordinate = dataHelper->getPrevPNoCoordinate(modeData.detailParamData->pNumber);
        QList<IdpMExeParamDetail>& mList = param.param.mpTemplate.templateParam.exeParam.list;
        if (mList.size() > 0) {
            QList<QList<int>>& dNums = modeData.posData->dNumbers;
            for (int i = 0; i < mList.size(); ++ i) {
                if (i >= dNums.size()) break;
                mList[i].dNumbers = dNums[i];
            }
        }
        dataHelper->mpDetailEditor->setParam(param);
        dataHelper->mpDetailEditor->show();
        this->hide();
    });

    connect(dataHelper->ui->mpDuplicateAction, &QAction::triggered, this, [this]() {
        int index = dataHelper->ui->mpTable->getLimitSelectedRow();
        if (index == -1) {
            return;
        }
        IdpMpTableModeData& data = dataHelper->mpTableMode->getDataList()[index];
        createMeasurementDuplicateEditor();
        dataHelper->measurementDuplicateController->show();
        dataHelper->measurementDuplicateController->resetToComponent(data.posData->duplicatePos);
        MainThreadInvoker::getInvoker().moveToParentCenter(this, dataHelper->measurementDuplicateController);
    });

    connect(dataHelper->ui->mpShiftCoordinateAction, &QAction::triggered, this, [this]() {
        if (dataHelper->ui->mpTable->selectionModel()->selectedRows(0).isEmpty()) {
            return;
        }
        if (!dataHelper->shiftCoordinateController) {
            dataHelper->shiftCoordinateController = new IdpShiftCoordinateSettingController(this);
            dataHelper->shiftCoordinateController->initAsyncCaller(getAsyncCaller());
            dataHelper->shiftCoordinateController->setWindowTitle("Shift Coordinate");
            dataHelper->shiftCoordinateController->setResizable(false);
            dataHelper->shiftCoordinateController->setOkCallback([this](const IDW_PointF& param) {
                QModelIndexList rows = dataHelper->ui->mpTable->selectionModel()->selectedRows(0);
                if (rows.isEmpty()) {
                    return true;
                }
                QList<qint32> indices;
                indices.reserve(rows.size());
                for (QModelIndex& index : rows) {
                    indices.append(index.row());
                }
                dataHelper->mpTableMode->updateDatas(indices, [&param](qint32 index, IdpMpTableModeData& data) {
                    Q_UNUSED(index)
                    if (data.posData.isNull()) {
                        return;
                    }
                    data.posData->oPoint.x += param.x;
                    data.posData->oPoint.y += param.y;
                });
                dataHelper->ui->mpTable->recalcColumnSize();
                return true;
            });
        }
        dataHelper->shiftCoordinateController->setParam(IDW_PointF{0,0});
        dataHelper->shiftCoordinateController->show();
        MainThreadInvoker::getInvoker().moveToParentCenter(this, dataHelper->shiftCoordinateController);
    });

    connect(dataHelper->ui->saveAct, &QAction::triggered, this, [this](bool) {
        saveData({param.idpName, dataHelper->baseData.idpId}, nullptr);
    });
    connect(dataHelper->ui->saveAsAct, &QAction::triggered, this, [this](bool) {
        if (!dataHelper->saveAsBox) {
            dataHelper->saveAsBox = new CDE90InputBox(this);
            dataHelper->saveAsBox->setWindowTitle("Save as...");
            dataHelper->saveAsBox->setInputLength(FILE_NAME_LENGTH);
            dataHelper->saveAsBox->setEditTitle("Create IDP");
            dataHelper->saveAsBox->setInputCallback([this](const QString& text) {
                if (text.isEmpty()) {
                    simpleMessageBox(this, "Message", "Please input IDP name.");
                    return false;
                }
                saveData({text, 0}, nullptr, true);
                return false;
            });
            dataHelper->saveAsBox->resize(300, 200);
        }
        dataHelper->saveAsBox->setInitText(param.idpName);
        dataHelper->saveAsBox->show();
    });
    connect(dataHelper->ui->exitAct, &QAction::triggered, this, [this](bool) {
        changeCloseStatusOnce(HW_CLOSE);
        this->close();
    });
    connect(dataHelper->ui->apCutAction, &QAction::triggered, this, [this]() {
        int row = helpToCutData(dataHelper->apTableMode, dataHelper->ui->apTable, dataHelper->apCutDatas);
        dataHelper->ui->apPasteMenu->setDisabled(row == 0);
    });
    connect(dataHelper->ui->apCopyAction, &QAction::triggered, this, [this]() {
        int row = helpToCutData(dataHelper->apTableMode, dataHelper->ui->apTable, dataHelper->apCutDatas, false);
        dataHelper->ui->apPasteMenu->setDisabled(row == 0);
    });
    connect(dataHelper->ui->apPasteBeforeAct, &QAction::triggered, this, [this]() {
        helpToPaste(dataHelper->apTableMode, dataHelper->ui->apTable, dataHelper->apCutDatas, true);
        dataHelper->ui->apPasteMenu->setDisabled(true);
    });
    connect(dataHelper->ui->apPasteAfterAct, &QAction::triggered, this, [this]() {
        helpToPaste(dataHelper->apTableMode, dataHelper->ui->apTable, dataHelper->apCutDatas, false);
        dataHelper->ui->apPasteMenu->setDisabled(true);
    });
    connect(dataHelper->ui->apModifyChipAction, &QAction::triggered, this, [this]() {
        if (!dataHelper->selectedApParam.isNull()) {
            dataHelper->selectedApParam = nullptr;
        }
        if (dataHelper->baseData.idwId == 0) {
            return;
        }
        int index = dataHelper->ui->apTable->getLimitSelectedRow();
        if (index < 0) {
            return;
        }
        QList<qint32> numbers;
        numbers.reserve(10);
        IdpApTableModeData& data = dataHelper->apTableMode->getDataList()[index];
        dataHelper->selectedApParam = data.detailParamData;
        QList<IdpApTableModeData>& list = dataHelper->apTableMode->getDataList();
        for (IdpApTableModeData& d : list) {
            if (d.detailParamData == data.detailParamData) {
                numbers.append(d.posData->chipNumber);
            }
        }
        showIdpApChipSelector(numbers);
    });

    connect(dataHelper->ui->mpCutAction, &QAction::triggered, this, [this]() {
        int row = helpToCutData(dataHelper->mpTableMode, dataHelper->ui->mpTable, dataHelper->mpCutDatas);
        dataHelper->ui->mpPasteMenu->setDisabled(row == 0);
    });
    connect(dataHelper->ui->mpCopyAction, &QAction::triggered, this, [this]() {
        int row = helpToCutData(dataHelper->mpTableMode, dataHelper->ui->mpTable, dataHelper->mpCutDatas, false);
        dataHelper->ui->mpPasteMenu->setDisabled(row == 0);
    });
    connect(dataHelper->ui->mpPasteBeforeAct, &QAction::triggered, this, [this]() {
        helpToPaste(dataHelper->mpTableMode, dataHelper->ui->mpTable, dataHelper->mpCutDatas, true);
        dataHelper->ui->mpPasteMenu->setDisabled(true);
    });
    connect(dataHelper->ui->mpPasteAfterAct, &QAction::triggered, this, [this]() {
        helpToPaste(dataHelper->mpTableMode, dataHelper->ui->mpTable, dataHelper->mpCutDatas, false);
        dataHelper->ui->mpPasteMenu->setDisabled(true);
    });
    connect(dataHelper->ui->mpModifyChipD_MAction, &QAction::triggered, this, [this]() {
        int index = dataHelper->ui->mpTable->getLimitSelectedRow();
        if (index < 0) {
            return;
        }
        QList<IdpMpTableModeData>& list = dataHelper->mpTableMode->getDataList();
        IdpMpTableModeData& detail = list[index];
        if (detail.detailParamData->mpTemplate.templateParam.exeParam.list.isEmpty()) return;
        createDNoByMeasurementEditor();
        dataHelper->dNoByMeasurementController->setData(detail.posData, detail.detailParamData, index);
        dataHelper->dNoByMeasurementController->show();
    });
    connect(dataHelper->ui->mpModifyChipD_PAction, &QAction::triggered, this, [this]() {
        QList<IdpMpTableModeData>& list =dataHelper-> mpTableMode->getDataList();
        if (list.isEmpty()) {
            return;
        }
        createDNoByPEditor();
        IdpDNoByPDataTableModel* tableModel = dataHelper->dNoByPController->getTableModel();
        tableModel->clear();
        QList<IdpDByPData>& dataList = tableModel->getDataList();

        QHash<qint64, QList<QSharedPointer<IdpMpPosListData>>> p_dChecker;
        qint64 id;
        for (IdpMpTableModeData& d : list) {
            //没有实际测量数据的数据，不存在D No.。先这么定。
            if (d.detailParamData->mpTemplate.templateParam.exeParam.list.isEmpty()) continue;
            id = reinterpret_cast<qint64>(d.detailParamData.data());
            auto it = p_dChecker.find(id);
            if (it == p_dChecker.end()) {
                it = p_dChecker.insert(id, QList<QSharedPointer<IdpMpPosListData>>());
                IdpDByPData pData;
                pData.updated = false;
                pData.id = id;
                pData.pNo = d.detailParamData->pNumber;
                collectViewMpName(d.detailParamData->mpTemplate.templateParam.exeParam.list, pData.name);
                dataList.append(pData);
            }
            it->append(d.posData);
        }
        if (dataList.isEmpty()) {
            simpleMessageBox(this, "Info", "No available data found.");
            return;
        }
        for (IdpDByPData& d : dataList) {
            auto it = p_dChecker.find(d.id);
            if (it == p_dChecker.end()) continue;
            collectViewDName(*it, d.before);
            d.after = d.before;
        }
        tableModel->resizeViewList(dataList.size());
        dataHelper->dNoByPController->show();
    });
    connect(dataHelper->ui->mpModifyPAction, &QAction::triggered, this, [this]() {
        QList<IdpMpTableModeData>& list = dataHelper->mpTableMode->getDataList();
        if (list.isEmpty()) {
            return;
        }
        createPNoEditor();
        IdpPNoDataTableModel* tableModel = dataHelper->pNoController->getTableModel();
        tableModel->clear();
        QList<IdpPatternNumberData>& dataList = tableModel->getDataList();
        dataList.reserve(list.size());
        QSet<IdpMpParamListData*> repeatChecker;
        repeatChecker.reserve(list.size());
        for (IdpMpTableModeData& data : list) {
            if (repeatChecker.contains(data.detailParamData.data())) {
                continue;
            }
            repeatChecker.insert(data.detailParamData.data());
            IdpPatternNumberData pData;
            pData.before = data.detailParamData->pNumber;
            pData.after = pData.before;
            pData.id = reinterpret_cast<qint64>(data.detailParamData.data());
            QList<IdpMExeParamDetail>& exeList = data.detailParamData->mpTemplate.templateParam
                                                     .exeParam.list;
            if (exeList.size() > 0) {
                pData.name = exeList[0].mpName;
            } else {
                pData.name = "";
            }
            dataList.append(pData);
        }
        tableModel->resizeViewList(dataList.size());
        dataHelper->pNoController->show();
    });

    connect(dataHelper->ui->mpModifyChipAction, &QAction::triggered, this, [this]() {
        if (!dataHelper->selectedMpParam.isNull()) {
            dataHelper->selectedMpParam = nullptr;
        }
        if (dataHelper->baseData.idwId == 0) {
            return;
        }
        int index = dataHelper->ui->mpTable->getLimitSelectedRow();
        if (index < 0) {
            return;
        }
        QList<qint32> numbers;
        numbers.reserve(1);
        QList<qint32> grayNumbers;
        grayNumbers.reserve(10);
        QList<IdpMpTableModeData>& list = dataHelper->mpTableMode->getDataList();
        IdpMpTableModeData* data = &list[index];
        numbers.append(data->posData->chipNumber);
        dataHelper->selectedMpParam = data->detailParamData;
        for (int i = 0; i < list.size(); ++ i) {
            if (i == index) continue;
            data = &list[i];
            if (data->detailParamData != dataHelper->selectedMpParam) continue;
            grayNumbers.append(data->posData->chipNumber);
        }
        showIdpMpChipSelector(numbers, grayNumbers);
        dataHelper->mpWaferSelector->setSingleSelectMode(true);
    });
    connect(dataHelper->ui->measureTimesAct, &QAction::triggered, this, [this]() {
        createTimesEditor();
        dataHelper->timesController->show();
        QList<IdpMpTableModeData>& list = dataHelper->mpTableMode->getDataList();
        QList<QSharedPointer<IdpMpParamListData>> pList;
        pList.reserve(list.size());
        QSet<QSharedPointer<IdpMpParamListData>> repeatChecker;
        for (IdpMpTableModeData& d : list) {
            if (repeatChecker.contains(d.detailParamData)) {
                continue;
            }
            pList.append(d.detailParamData);
            repeatChecker.insert(d.detailParamData);
        }
        dataHelper->timesController->resetToComponent(pList);
    });

    connect(dataHelper->ui->imgEnvAct, &QAction::triggered, this, [this]() {
        createImageEnvEditor();
        dataHelper->imageEnvController->show();
        dataHelper->imageEnvController->setParam(param.mpData.imageEnvData);
    });

    connect(dataHelper->ui->axisAct, &QAction::triggered, this, [this]() {
        createAxisAlignmentEditor();
        dataHelper->axisAlignmentController->show();
        MainThreadInvoker::getInvoker().moveToScreenCenter(dataHelper->axisAlignmentController);
        dataHelper->axisAlignmentController->setParam(param.mpData.axisParam);
        this->hide();
    });

    connect(dataHelper->ui->mpModifyZSensorAction, &QAction::triggered, this, [this]() {
        createZSensorController();
        int index = dataHelper->ui->mpTable->getLimitSelectedRow();
        if (index < 0) {
            return;
        }
        IdpMpTableModeData& mpData = dataHelper->mpTableMode->getDataList()[index];
        dataHelper->zSensorController->setParam(
            mpData.detailParamData->addressTemplate.templateParam.zParam);
        dataHelper->zSensorController->show();
        return;
    });

    connect(cdsem::CDSEM_GlobalDataHelper::get(), &cdsem::CDSEM_GlobalData::globalIdwChanged, this, [this](){
        quint64 idwId = cdsem::CDSEM_GlobalDataHelper::get()->getGlobalIdwId();
        if (idwId != dataHelper->baseData.idwId) {
            if (!dataHelper->pyHandlerDisabled) {
                dataHelper->createOpenedDIffStageController();
                if (dataHelper->openedStageDiffController)
                    dataHelper->openedStageDiffController->show();
            }
        } else {
            if (dataHelper->openedStageDiffController) {
                dataHelper->openedStageDiffController->disableClose(false);
                dataHelper->openedStageDiffController->close();
            }
            if (dataHelper->initStageDiffController) {
                dataHelper->initStageDiffController->changeCloseStatusOnce(HW_USER);
                dataHelper->initStageDiffController->disableClose(false);
                dataHelper->initStageDiffController->close();
            }
            if (dataHelper->pyHandlerDisabled) {
                toDefaultWindowColor();
                dataHelper->disablePyHandlers(false);
            }
        }
    });

    connect(dataHelper->ui->apAlignmentBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        QString errMsg = collectAPData();
        if (errMsg.length() > 0) {
            simpleMessageBox(this, "Error", errMsg);
            return;
        }
        QList<double> waferTransformMatrix = {1.0, 0, 0, 1.0, 0, 0};
        CALIBRATION_ENV.setWaferTransformMatrix(waferTransformMatrix);
        CALIBRATION_ENV.setWaferUnTransformMatrix(waferTransformMatrix);
        RecipeProcedureConfig config;
        config.procedureType = RPT_AP_TEST;
        RecipeProcedureHelper::get()->startProcedure(QSharedPointer<const IdpDetailData>(
                                                    new IdpDetailData(param)),
                                                    config);
    });
    connect(dataHelper->ui->apStopBtn->getQPushButton(), &QPushButton::clicked, this, [](){
        RecipeProcedureHelper::get()->stopProcedure();
    });

    connect(dataHelper->ui->measurementBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        QString errMsg = collectMPData(param, !dataHelper->ui->allRadio->isChecked());
        if (errMsg.length() > 0) {
            simpleMessageBox(this, "Error", errMsg);
            return;
        }
        if (param.mpData.posList.isEmpty()) {
            simpleMessageBox(this, "Info", "No IDP data found.");
            return;
        }
        RecipeProcedureConfig config;
        config.procedureType = RPT_MP_TEST;
        config.classId = dataHelper->baseData.classId;
        config.idpId = dataHelper->baseData.idpId;
        config.idwId = dataHelper->baseData.idwId;
        RecipeProcedureHelper::get()->startProcedure(QSharedPointer<const IdpDetailData>(new IdpDetailData(param)), config);
    });
    connect(dataHelper->ui->mpStopBtn->getQPushButton(), &QPushButton::clicked, this, [](){
        RecipeProcedureHelper::get()->stopProcedure();
    });
    connect(dataHelper->ui->apWizardBtn->getQPushButton(), &QPushButton::clicked, this, [this]() {
        if (!dataHelper->apWizardWindow) {
            dataHelper->apWizardWindow = new IdpWizardWindow(this);
            dataHelper->apWizardWindow->setWindowTitle("AP Wizard");
            dataHelper->apWizardWindow->initAsyncCaller(ACaller);
            dataHelper->apWizardWindow->setCloseCall([this](CDE90WindowCloseStatus status) {
                show();
                if (status == HW_OK) {
                    dataHelper->appendApDataFromWizard(dataHelper->apWizardComponents->getResult());
                }
                return true;
            });
            dataHelper->apWizardWindow->setComponent(dataHelper->apWizardComponents = new ApWizardComponents);
            dataHelper->apWizardWindow->setGroup(QString("idp_ap_wizard_%1").arg(dataHelper->baseData.idpId == 0 ? "create" : QString::number(dataHelper->baseData.idpId)));
        } else {
            dataHelper->apWizardWindow->resetStep();
        }
        dataHelper->apWizardWindow->show();
        hide();
    });
    connect(cdsem::CDSEM_GlobalDataHelper::get(), &cdsem::CDSEM_GlobalData::procedureRunningStatusChanged, this, [this]() {
        dataHelper->updatePyDisabledStatus();
    });

    createApTableAction();
    createMpTableAction();
}

void IdpDataEditorController::createApTableAction() {
    QAction* action = new QAction(dataHelper->ui->apTable);
    action->setShortcut(QKeySequence::Cut);
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect(action, &QAction::triggered, dataHelper->ui->apCutAction, &QAction::triggered);
    dataHelper->ui->apTable->addAction(action);
    action = new QAction(dataHelper->ui->apTable);
    action->setShortcut(QKeySequence::Copy);
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect(action, &QAction::triggered, dataHelper->ui->apCopyAction, &QAction::triggered);
    dataHelper->ui->apTable->addAction(action);
    action = new QAction(dataHelper->ui->apTable);
    action->setShortcut(QKeySequence::Paste);
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect(action, &QAction::triggered, dataHelper->ui->apPasteAfterAct, &QAction::triggered);
    dataHelper->ui->apTable->addAction(action);
    action = new QAction(dataHelper->ui->apTable);
    action->setShortcut(QKeySequence("Ctrl+Shift+V"));
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect(action, &QAction::triggered, dataHelper->ui->apPasteBeforeAct, &QAction::triggered);
    dataHelper->ui->apTable->addAction(action);
}

void IdpDataEditorController::createMpTableAction() {
    QAction* action = new QAction(dataHelper->ui->mpTable);
    action->setShortcut(QKeySequence::Cut);
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect(action, &QAction::triggered, dataHelper->ui->mpCutAction, &QAction::triggered);
    dataHelper->ui->mpTable->addAction(action);
    action = new QAction(dataHelper->ui->mpTable);
    action->setShortcut(QKeySequence::Copy);
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect(action, &QAction::triggered, dataHelper->ui->mpCopyAction, &QAction::triggered);
    dataHelper->ui->mpTable->addAction(action);
    action = new QAction(dataHelper->ui->mpTable);
    action->setShortcut(QKeySequence::Paste);
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect(action, &QAction::triggered, dataHelper->ui->mpPasteAfterAct, &QAction::triggered);
    dataHelper->ui->mpTable->addAction(action);
    action = new QAction(dataHelper->ui->mpTable);
    action->setShortcut(QKeySequence("Ctrl+Shift+V"));
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect(action, &QAction::triggered, dataHelper->ui->mpPasteBeforeAct, &QAction::triggered);
    dataHelper->ui->mpTable->addAction(action);
}

void IdpDataEditorController::showIdpApChipSelector(
    const QList<qint32>& selectedNumbers) {
    dataHelper->apWaferSelector = new IdpApWaferSelectorController(this);
    dataHelper->apWaferSelector->setResizable(false);
    dataHelper->apWaferSelector->setWindowTitle("Ap Chip Select");
    dataHelper->apWaferSelector->initAsyncCaller(getAsyncCaller());
    dataHelper->apWaferSelector->initIdwTopId(dataHelper->baseData.idwTopId, selectedNumbers);
    dataHelper->apWaferSelector->setCloseCall([this](CDE90WindowCloseStatus status) {
        if (status == HW_OK || status == HW_NEXT) {
            return true;
        }
        if (dataHelper->apWaitAddChips.size() > 0) {
            dataHelper->apWaitAddChips.clear();
        }
        return true;
    });
    dataHelper->apWaferSelector->setOkCallback([this](const QList<IdpApSelectedChip>& list) {
        if (!dataHelper->selectedApParam.isNull()) {
            sem::AfterGc gc([this]() { dataHelper->selectedApParam.clear(); });
            int index = dataHelper->ui->apTable->getLimitSelectedRow();
            if (index < 0) {
                return;
            }
            QList<qint32> waitRemoveIndices;
            waitRemoveIndices.reserve(10);
            QList<IdpApTableModeData>& tableList = dataHelper->apTableMode->getDataList();
            for (int i = 0; i < tableList.size(); ++i) {
                IdpApTableModeData& d = tableList[i];
                if (dataHelper->selectedApParam != d.detailParamData.data()) {
                    continue;
                }
                waitRemoveIndices.append(i);
            }
            if (waitRemoveIndices.size() > 0) {
                dataHelper->apTableMode->removeDatas(waitRemoveIndices);
            }
            if (list.size() > 0) {
                int start = tableList.size();
                tableList.resize(tableList.size() + list.size());
                for (const IdpApSelectedChip& c : list) {
                    IdpApTableModeData& d = tableList[start];
                    if (d.posData.isNull()) {
                        d.posData.reset(new IdpApPosListData);
                    }
                    d.posData->oPoint = c.oPoint;
                    d.posData->chipX = c.x;
                    d.posData->chipY = c.y;
                    d.posData->chipNumber = c.number;
                    d.detailParamData = dataHelper->selectedApParam;
                    ++start;
                }
                dataHelper->apTableMode->resizeViewList(tableList.size());
            }
            dataHelper->ui->apTable->recalcColumnSize();
            return;
        }
        if (list.isEmpty()) {
            return;
        }
        if (!createIdpApDetailEditor()) {
            simpleMessageBox(this, "Error", "Error to open AP Editor");
            return;
        }
        dataHelper->apWaitAddChips = list;
        QList<IdpApTableModeData>& tableList = dataHelper->apTableMode->getDataList();
        int oldSize = tableList.size();
        IdpApEditModel param;
        param.id = dataHelper->baseData.idpId;
        const IdpApSelectedChip& chipInfo = list[0];
        param.chipX = chipInfo.x;
        param.chipY = chipInfo.y;
        param.param.pNumber = dataHelper->maxAP_P_Number;
        param.index = oldSize;
        param.chipNumber = chipInfo.number;
        param.oPoint = chipInfo.oPoint;
        // param.param.chipCount = list.size();
        dataHelper->apDetailEditor->setParam(param);
        dataHelper->apDetailEditor->show();
        this->hide();
    });
    dataHelper->apWaferSelector->setAttribute(Qt::WA_DeleteOnClose);
    dataHelper->apWaferSelector->show();
    MainThreadInvoker::getInvoker().moveToScreenCenter(dataHelper->apWaferSelector);
}

bool IdpDataEditorController::createIdpApDetailEditor() {
    if (!dataHelper->apDetailEditor) {
        if (!idp_template::apTemplateComponentAvailable()) return false;
        dataHelper->apDetailEditor = new IdpApDetailEditorController;
        dataHelper->apDetailEditor->setWindowModality(Qt::NonModal);
        dataHelper->apDetailEditor->setWindowTitle("AP Information");
        dataHelper->apDetailEditor->initAsyncCaller(getAsyncCaller(), this);
        dataHelper->apDetailEditor->disablePyHandlers(dataHelper->pyRealDisabled);
        dataHelper->apDetailEditor->setCloseCall([this](CDE90WindowCloseStatus status) {
            this->show();
            if (status == HW_OK) {
                return true;
            }
            if (dataHelper->apWaitAddChips.size() > 0) {
                dataHelper->apWaitAddChips.clear();
            }
            return true;
        });
        dataHelper->apDetailEditor->setOkCallback([this](const IdpApEditModel& data) {
            if (dataHelper->apDetailEditor->checkDynamicTemplateStatus()) {
                int r  = showYesNoBox(this, "Warning", "Dynamic template cannot be saved to IDP independently, use \"Save Template\" instead. \nDiscard these changes?");
                if (r) return false;
                dataHelper->apDetailEditor->recoverToDynamicTemplateData();
            }
            if (dataHelper->apWaitAddChips.size() > 0) {
                // new data with idw
                return dataHelper->apNewPDataWithIDW(data);
            } else if (data.index == -1) {
                // new data without idw.
                //早期版本允许不载入IDW情况下编辑IDP，但是觉得没有IDW那么IDP编辑起来非常费劲且不现实，因此改为强制载入IDW
                return dataHelper->apNewPDataWithoutIDW(data);
            } else {
                //edit
                return dataHelper->apEditPData(data);
            }
            dataHelper->ui->apTable->recalcColumnSize();
            return true;
        });
    }
    return true;
}

void IdpDataEditorController::showIdpMpChipSelector(
    const QList<qint32>& selectedNumbers, const QList<qint32>& grayNumbers) {
    dataHelper->mpWaferSelector = new IdpMpWaferSelectorController(this);
    dataHelper->mpWaferSelector->setResizable(false);
    dataHelper->mpWaferSelector->setWindowTitle("MP/IP Chip Select");
    dataHelper->mpWaferSelector->initAsyncCaller(getAsyncCaller());
    dataHelper->mpWaferSelector->initIdwTopId(dataHelper->baseData.idwTopId, selectedNumbers, grayNumbers);
    dataHelper->mpWaferSelector->setAttribute(Qt::WA_DeleteOnClose);
    dataHelper->mpWaferSelector->setStartSeq(dataHelper->mpTableMode->getDataList().size() + 1);
    dataHelper->mpWaferSelector->setCloseCall([this](CDE90WindowCloseStatus status) {
        if (status == HW_OK || status == HW_NEXT) {
            return true;
        }
        if (dataHelper->mpWaitAddChips.size() > 0) {
            dataHelper->mpWaitAddChips.clear();
        }
        return true;
    });
    dataHelper->mpWaferSelector->setOkCallback([this](const QList<IdpMpSelectedChip>& list) {
        if (list.isEmpty()) {
            return false;
        }
        if (!dataHelper->selectedMpParam.isNull()) {
            bool couldClear = true;
            sem::AfterGc gc([this, &couldClear]() {
                if (couldClear)
                    dataHelper->selectedMpParam.clear();
            });
            int index = dataHelper->ui->mpTable->getLimitSelectedRow();
            if (index < 0) {
                couldClear = false;
                return false;
            }
            QList<IdpMpTableModeData>& tableList = dataHelper->mpTableMode->getDataList();
            IdpMpTableModeData& curr = tableList[index];
            if (curr.detailParamData != dataHelper->selectedMpParam) {
                simpleMessageBox(this, "Error", "The data has been modified and cannot be edited.");
                return true;
            }
            const IdpMpSelectedChip& first = list.first();
            for (int i = 0; i < tableList.size(); ++ i) {
                if (i == index) continue;
                IdpMpTableModeData& data = tableList[i];
                //different p no.
                if (data.detailParamData != dataHelper->selectedMpParam) continue;
                if (data.posData->chipNumber == first.number) {
                    int r = showYesNoBox(this, "Warning", "The selected Chip already exists in the same Pattern Group. Do you want to overwrite it?");
                    if (r) {
                        couldClear = false;
                        return false;
                    }
                    tableList.remove(i);
                    break;
                }
            }
            curr.posData->chipNumber = first.number;
            curr.posData->chipX = first.x;
            curr.posData->chipY = first.y;
            curr.posData->oPoint = first.oPoint;
            dataHelper->mpTableMode->notifyDataChanged(index, index);
            return true;
        }
        if (!createIdpMpDetailEditor()) {
            simpleMessageBox(this, "Error", "Error to open MP/IP Editor");
            return false;
        }
        dataHelper->mpWaitAddChips = list;
        QList<IdpMpTableModeData>& tableList = dataHelper->mpTableMode->getDataList();
        int oldSize = tableList.size();
        IdpMpIpEditModel param;
        const IdpMpSelectedChip& chipInfo = list[0];
        param.chipX = chipInfo.x;
        param.chipY = chipInfo.y;
        param.index = oldSize;
        param.chipNumber = chipInfo.number;
        param.id = dataHelper->baseData.idpId;
        param.param.pNumber = dataHelper->maxMP_P_Number;
        param.oPoint = chipInfo.oPoint;
        param.prevCoordinate = dataHelper->getPrevPNoCoordinate(param.param.pNumber);
        // param.param.chipCount = list.size();
        dataHelper->mpDetailEditor->setParam(param);
        dataHelper->mpDetailEditor->show();
        this->hide();
        return true;
    });
    dataHelper->mpWaferSelector->show();
    MainThreadInvoker::getInvoker().moveToScreenCenter(dataHelper->mpWaferSelector);
}

bool IdpDataEditorController::createIdpMpDetailEditor() {
    if (!dataHelper->mpDetailEditor) {
        if (!idp_template::multiMsTemplateWindowAvailable() || !idp_template::mpTemplateComponentAvailable()) return false;
        dataHelper->mpDetailEditor = new IdpMpDetailEditorController;
        dataHelper->mpDetailEditor->setResizable(false);
        dataHelper->mpDetailEditor->setWindowModality(Qt::NonModal);
        dataHelper->mpDetailEditor->setWindowTitle("Sequence Editor");
        dataHelper->mpDetailEditor->initAsyncCaller(getAsyncCaller(), this);
        dataHelper->mpDetailEditor->disablePyHandlers(dataHelper->pyRealDisabled);
        dataHelper->mpDetailEditor->setCloseCall([this](CDE90WindowCloseStatus status) {
            this->show();
            if (status == HW_OK) {
                return true;
            }
            if (dataHelper->mpWaitAddChips.size() > 0) {
                dataHelper->mpWaitAddChips.clear();
            }
            return true;
        });

        dataHelper->mpDetailEditor->setOkCallback([this](const IdpMpIpEditModel& data) {
            if (dataHelper->mpDetailEditor->checkDynamicTemplateStatus()) {
                int r  = showYesNoBox(this, "Warning", "Dynamic template cannot be saved to IDP independently, use \"Save Template\" instead. \nDiscard these changes?");
                if (r) return false;
                dataHelper->mpDetailEditor->recoverToDynamicTemplateData();
            }
            const QList<IdpMExeParamDetail>& subList = data.param.mpTemplate.templateParam.exeParam.list;
            if (dataHelper->mpWaitAddChips.size() > 0) {
                int oldSize = dataHelper->mpTableMode->getDataList().size();
                ++dataHelper->maxMP_P_Number;
                int start = oldSize;
                dataHelper->mpTableMode->resizeDataList(oldSize + dataHelper->mpWaitAddChips.size());
                QSharedPointer<IdpMpParamListData> sharedData = QSharedPointer<IdpMpParamListData>::create(data.param);
                QString dName;
                collectViewMpName(subList, sharedData->viewMpName);
                toViewDNumber(subList, dName);
                QList<QList<int>> dList;
                collectDList(subList, dList);


                for (IdpMpSelectedChip& c : dataHelper->mpWaitAddChips) {
                    IdpMpTableModeData& d = dataHelper->mpTableMode->getDataList()[start];
                    d.dNumbers = dName;
                    if (d.posData.isNull()) {
                        d.posData.reset(new IdpMpPosListData);
                    }
                    d.posData->oPoint = c.oPoint;
                    d.posData->chipX = c.x;
                    d.posData->chipY = c.y;
                    d.posData->chipNumber = c.number;
                    d.posData->dNumbers = dList;
                    d.detailParamData = sharedData;
                    ++ start;
                }
                dataHelper->mpTableMode->notifyDataChanged(oldSize, dataHelper->mpTableMode->getDataList().size() - 1);
                dataHelper->ui->mpTable->recalcColumnSize();
                dataHelper->mpWaitAddChips.clear();
            } else if (data.index == -1) {
                //这部分内容是不存在IDP时的编辑
                int oldSize = dataHelper->mpTableMode->getDataList().size();
                ++dataHelper->maxMP_P_Number;
                dataHelper->mpTableMode->resizeDataList(oldSize + 1);
                QSharedPointer<IdpMpParamListData> sharedData = QSharedPointer<IdpMpParamListData>::create(data.param);
                QString dName;
                collectViewMpName(subList, sharedData->viewMpName);
                toViewDNumber(subList, dName);
                QList<QList<int>> dList;
                collectDList(subList, dList);
                for (int i = oldSize; i < dataHelper->mpTableMode->getDataList().size(); ++ i) {
                    IdpMpTableModeData& d = dataHelper->mpTableMode->getDataList()[i];
                    d.dNumbers =dName;
                    if (d.posData.isNull()) {
                        d.posData.reset(new IdpMpPosListData);
                    }
                    d.posData->chipNumber = -1;
                    d.posData->dNumbers = dList;
                    d.detailParamData = sharedData;
                }

                dataHelper->mpTableMode->notifyDataChanged(oldSize, oldSize);
                dataHelper->ui->mpTable->recalcColumnSize();
            } else {
                //TODO 需要处理如果未对measure exe做任何改动的情况下，不要覆盖和处理dNo.
                QList<IdpMpTableModeData>& list = dataHelper->mpTableMode->getDataList();
                QSharedPointer<IdpMpParamListData> sharedData = list[data.index].detailParamData;
                QString dName;
                toViewDNumber(subList, dName);
                QList<QList<int>> dList;
                collectDList(subList, dList);

                *sharedData = data.param;
                collectViewMpName(subList, sharedData->viewMpName);
                for (int i = 0; i <list.size(); ++ i) {
                    IdpMpTableModeData& d = list[i];
                    if (sharedData != d.detailParamData) {
                        continue;
                    }
                    d.dNumbers = dName;
                    d.posData->dNumbers = dList;
                    dataHelper->mpTableMode->notifyDataChanged(i, i);
                }
                dataHelper->ui->mpTable->recalcColumnSize();
            }
            return true;
        });
    }
    return true;
}

void IdpDataEditorController::createMeasurementDuplicateEditor() {
    if (!dataHelper->measurementDuplicateController) {
        dataHelper->measurementDuplicateController = new IdpMeasurementArrangeSettingController(this);
        dataHelper->measurementDuplicateController->setWindowModality(Qt::WindowModal);
        dataHelper->measurementDuplicateController->setWindowTitle("Duplicate");
        dataHelper->measurementDuplicateController->setResizable(false);
        dataHelper->measurementDuplicateController->setOkCallback([this]() {
            int index = dataHelper->ui->mpTable->getLimitSelectedRow();
            if (index == -1) {
                simpleMessageBox(this, "Error", "Lost selected row.");
                return false;
            }
            IdpDuplicatePos pos;
            QString errMsg = dataHelper->measurementDuplicateController->collectData(pos);
            if (errMsg.length() > 0) {
                simpleMessageBox(this, "Error", errMsg);
                return false;
            }
            IdpMpTableModeData& data = dataHelper->mpTableMode->getDataList()[index];
            data.posData->duplicatePos = pos;
            return true;
        });
    }
}

void IdpDataEditorController::createDNoByPEditor() {
    if (dataHelper->dNoByPController) {
        return;
    }
    dataHelper->dNoByPController = new IdpDNoByPController(this);
    dataHelper->dNoByPController->setWindowModality(Qt::WindowModal);
    dataHelper->dNoByPController->setWindowTitle("Modify D No.");
    dataHelper->dNoByPController->setResizable(false);
    dataHelper->dNoByPController->setOkCallback([this]() {
        QList<IdpDByPData>& list = dataHelper->dNoByPController->getTableModel()->getDataList();
        QHash<IdpMpParamListData*, D_By_P_Data> hash;
        for (IdpDByPData& d : list) {
            if (!d.updated) continue;
            hash.insert(reinterpret_cast<IdpMpParamListData*>(d.id), D_By_P_Data{d.after, strToNumList<int>(d.after)});
        }
        QList<IdpMpTableModeData>& mpList = dataHelper->mpTableMode->getDataList();
        for (IdpMpTableModeData& m : mpList) {
            QList<IdpMExeParamDetail>& subList = m.detailParamData->mpTemplate.templateParam.exeParam.list;
            if (subList.isEmpty()) continue;
            auto it = hash.find(m.detailParamData.data());
            if (it == hash.end()) {
                continue;
            }
            m.dNumbers = it->name;
            m.posData->dNumbers.resize(subList.size());
            for (int i = 0; i < subList.size(); ++ i) {
                m.posData->dNumbers[i] = it->list;
            }
        }
        dataHelper->mpTableMode->notifyDataChanged(0, mpList.size() - 1);
        return true;
    });
}
void IdpDataEditorController::createDNoByMeasurementEditor() {
    if (dataHelper->dNoByMeasurementController) {
        return;
    }
    dataHelper->dNoByMeasurementController = new IdpDNoMeasurementController(this);
    dataHelper->dNoByMeasurementController->setWindowModality(Qt::WindowModal);
    dataHelper->dNoByMeasurementController->setWindowTitle("Data Group Number");
    dataHelper->dNoByMeasurementController->setResizable(false);
    dataHelper->dNoByMeasurementController->setOkCallback([this]() {
        int index = dataHelper->dNoByMeasurementController->getIndex();
        if (index < 0) {
            simpleMessageBox(this, "Error", "No data selected");
            return false;
        }
        QList<IdpMpTableModeData>& list = dataHelper->mpTableMode->getDataList();
        if (index >= list.size()) {
            simpleMessageBox(this, "Error", "Data changed while saving.");
            return false;
        }
        IdpMpTableModeData& tData = list[index];
        if (!dataHelper->dNoByMeasurementController->checkSameData(tData.posData)) {
            simpleMessageBox(this, "Error", "Data changed while saving.");
            return false;
        }
        if (tData.detailParamData->mpTemplate.templateParam.exeParam.list.isEmpty()) {
            simpleMessageBox(this, "Error", "Cannot modify D no. on none-measuremnt data.");
            return false;
        }
        QString errMsg = dataHelper->dNoByMeasurementController->modifyData();
        if (errMsg.length() > 0) {
            simpleMessageBox(this, "Error", errMsg);
            return false;
        }
        collectViewDName(tData.posData->dNumbers, tData.dNumbers);
        dataHelper->mpTableMode->notifyDataChanged(index, index);
        return true;
    });
}
void IdpDataEditorController::createPNoEditor() {
    if (dataHelper->pNoController) {
        return;
    }
    dataHelper->pNoController = new IdpPNoController(this);
    dataHelper->pNoController->setWindowModality(Qt::WindowModal);
    dataHelper->pNoController->setWindowTitle("Modify P No.");
    dataHelper->pNoController->setResizable(false);
    dataHelper->pNoController->setOkCallback([this]() {
        QList<IdpMpTableModeData>& list = dataHelper->mpTableMode->getDataList();
        if (list.isEmpty()) {
            return false;
        }
        QSet<IdpMpParamListData*> repeatChecker;
        repeatChecker.reserve(list.size());
        QList<IdpPatternNumberData>& pList = dataHelper->pNoController->getTableModel()->getDataList();
        QHash<qint64, IdpPatternNumberData*> pHash;
        for (IdpPatternNumberData& p : pList) {
            pHash.insert(p.id, &p);
        }

        for (IdpMpTableModeData& d : list) {
            if (repeatChecker.contains(d.detailParamData.data())) {
                continue;
            }
            repeatChecker.insert(d.detailParamData.data());
            auto it = pHash.find(reinterpret_cast<qint64>(d.detailParamData.data()));
            if (it == pHash.end()) {
                simpleMessageBox(this, "Error", "The MP list is changed during modify P No.");
                return false;
            }
            if (d.detailParamData->pNumber != (*it)->before) {
                simpleMessageBox(this, "Error", "The MP list is changed during modify P No.");
                return false;
            }
        }
        repeatChecker.clear();
        for (IdpMpTableModeData& d : list) {
            if (repeatChecker.contains(d.detailParamData.data())) {
                continue;
            }
            repeatChecker.insert(d.detailParamData.data());
            d.detailParamData->pNumber = pHash[reinterpret_cast<qint64>(d.detailParamData.data())]
                                             ->after;
        }
        dataHelper->mpTableMode->notifyDataChanged(0, list.size() - 1);
        return true;
    });
}

void IdpDataEditorController::createTimesEditor() {
    if (dataHelper->timesController) {
        return;
    }
    dataHelper->timesController = new IdpPMeasureTimesController(this);
    dataHelper->timesController->setResizable(false);
    dataHelper->timesController->setWindowModality(Qt::WindowModal);
    dataHelper->timesController->setWindowTitle("Measurement Times");
    dataHelper->timesController->setOkCallback([this]() {
        dataHelper->timesController->collectData();
        return true;
    });
}

void IdpDataEditorController::createImageEnvEditor() {
    if (dataHelper->imageEnvController) {
        return;
    }
    dataHelper->imageEnvController = new ImageEnvironmentController(this);
    dataHelper->imageEnvController->setWindowModality(Qt::WindowModal);
    dataHelper->imageEnvController->setWindowTitle("Image Environment");
    dataHelper->imageEnvController->setResizable(false);
    dataHelper->imageEnvController->setOkCallback([this](const IdpMpImageEnvData& param) {
        this->param.mpData.imageEnvData = param;
        return true;
    });
}

void IdpDataEditorController::createAxisAlignmentEditor() {
    if (dataHelper->axisAlignmentController) {
        return;
    }
    dataHelper->axisAlignmentController = new AxisAlignmentWaferController(this);
    dataHelper->axisAlignmentController->setWindowModality(Qt::WindowModal);
    dataHelper->axisAlignmentController->setWindowTitle("Axis Alignment on Wafer");
    dataHelper->axisAlignmentController->setResizable(false);
    dataHelper->axisAlignmentController->initAsyncCaller(ACaller);
    dataHelper->axisAlignmentController->setCloseCall([this](CDE90WindowCloseStatus){
        this->show();
        return true;
    });
    dataHelper->axisAlignmentController->setGroup(
        QString("mp_%1").arg(dataHelper->baseData.idpId > 0 ? QString::number(dataHelper->baseData.idpId) : "create"));
    dataHelper->axisAlignmentController->setOkCallback([this](const AxisAlignModel& data) {
        this->param.mpData.axisParam = data;
        return true;
    });
}

void IdpDataEditorController::createZSensorController() {
    if (!dataHelper->zSensorController) {
        dataHelper->zSensorController = new ZSensorSettingController(this);
        dataHelper->zSensorController->setWindowModality(Qt::WindowModal);
        dataHelper->zSensorController->setResizable(false);
        dataHelper->zSensorController->setWindowTitle("Z Sensor");
        dataHelper->zSensorController->hideBackBtn();
        dataHelper->zSensorController->hideNextBtn();
        dataHelper->zSensorController->setOkCallback([this](const ZSensorSettingParam& data) -> bool {
            int index = dataHelper->ui->mpTable->getLimitSelectedRow();
            if (index < 0) {
                return false;
            }
            IdpMpTableModeData& mpData = dataHelper->mpTableMode->getDataList()[index];
            ZSensorSettingParam& zParam = mpData.detailParamData->addressTemplate.templateParam
                                              .zParam;
            zParam.enable = data.enable;
            zParam.offset = data.offset;
            return true;
        });
    }
}

void IdpDataEditorController::resetToComponent() {
    QList<QSharedPointer<IdpApPosListData>>& apPosList = param.apData.posList;
    QList<QSharedPointer<IdpApParamListData>>& apParamList = param.apData.paramList;
    QList<IdpApTableModeData> apList;
    apList.reserve(apPosList.size());
    dataHelper->maxAP_P_Number = 1;
    for (QSharedPointer<IdpApPosListData>& data : apPosList) {
        IdpApTableModeData model;
        model.posData = data;
        if (data->paramIndex >= 0 && data->paramIndex < apParamList.size()) {
            model.detailParamData = apParamList[data->paramIndex];
        }
        if (model.detailParamData) {
            dataHelper->maxAP_P_Number = std::max(dataHelper->maxAP_P_Number, model.detailParamData->pNumber);
        }
        apList.append(model);
    }
    if (apPosList.size() > 0)
        ++dataHelper->maxAP_P_Number;
    dataHelper->apTableMode->insertDatas(apList);
    dataHelper->ui->apTable->recalcColumnSize();

    QList<QSharedPointer<IdpMpPosListData>>& mpPosList = param.mpData.posList;
    QList<IdpMpTableModeData> mpList;
    mpList.reserve(mpPosList.size());
    dataHelper->maxMP_P_Number = 1;
    QSet<IdpMpParamListData*> repeatChecker;

    int index;
    bool ignoreError = false;
    QSharedPointer<IdpMpParamListData> paramData;
    for (int i = 0; i < mpPosList.size(); ++ i) {
        QSharedPointer<IdpMpPosListData>& posData = mpPosList[i];
        index = posData->paramIndex;
        if (index < 0 || index >= param.mpData.paramList.size()) {
            if (!ignoreError) {
                int r = showYesNoBox(this, "Error", "Data integrity check failed. Continue loading and skip erroneous entries?");
                if (r) {
                    mpList.clear();
                    return;
                }
                ignoreError = true;
            }
            continue;
        }
        paramData = param.mpData.paramList[index];
        QList<IdpMExeParamDetail>& subList = paramData->mpTemplate.templateParam.exeParam.list;
        if (!repeatChecker.contains(paramData.data())) {
            repeatChecker.insert(paramData.data());
            collectViewMpName(subList, paramData->viewMpName);
        }
        dataHelper->maxMP_P_Number = std::max(dataHelper->maxMP_P_Number, paramData->pNumber);

        IdpMpTableModeData model;
        model.detailParamData = paramData;
        model.posData = posData;
        collectViewDName(posData->dNumbers, model.dNumbers);
        mpList.append(model);
    }
    if (mpPosList.size() > 0)
        ++dataHelper->maxMP_P_Number;
    dataHelper->mpTableMode->insertDatas(mpList);
    dataHelper->ui->mpTable->recalcColumnSize();
    dataHelper->ui->idwTextField->setText(dataHelper->baseData.waferName);
    dataHelper->ui->classTextField->setText(dataHelper->baseData.className);
    dataHelper->ui->idpTextField->setText(param.idpName);
}

QString IdpDataEditorController::collectAPData() {
    QList<QSharedPointer<IdpApPosListData>>& apPosList = param.apData.posList;
    QList<QSharedPointer<IdpApParamListData>>& apParamList = param.apData.paramList;
    apPosList.clear();
    apParamList.clear();
    const QList<IdpApTableModeData> apList = dataHelper->apTableMode->getDataList();
    if (apList.size() > 0) {
        apPosList.reserve(apList.size());
        QHash<IdpApParamListData*, int> repeatChecker;
        int paramSize = 0;
        IdpApParamListData* ptr;
        for (const IdpApTableModeData& data : apList) {
            ptr = data.detailParamData.data();
            if (!repeatChecker.contains(ptr)) {
                repeatChecker.insert(ptr, paramSize);
                ++paramSize;
            }
        }
        apParamList.resize(paramSize);
        for (const IdpApTableModeData& data : apList) {
            data.posData->paramIndex = repeatChecker[data.detailParamData.data()];
            apParamList.replace(data.posData->paramIndex, data.detailParamData);
            apPosList.append(data.posData);
        }
    }
    return "";
}

QString IdpDataEditorController::collectMPData(IdpDetailData& param, bool focusOnSelected) {
    QList<QSharedPointer<IdpMpPosListData>>& mpPosList = param.mpData.posList;
    QList<QSharedPointer<IdpMpParamListData>>& mpParamList = param.mpData.paramList;
    mpPosList.clear();
    mpParamList.clear();
    const QList<IdpMpTableModeData> mpList = dataHelper->mpTableMode->getDataList();
    if (mpList.size() > 0) {
        int size = mpList.size();
        if (focusOnSelected) {
            size = dataHelper->ui->mpTable->getSelectedRowCount();
        }
        if (size == 0) return "";

        mpPosList.reserve(size);
        QHash<IdpMpParamListData*, int> dParamRepeatChecker;
        QHash<IdpMpPosListData*, int> posRepeatChecker;
        int paramSize = 0;
        IdpMpParamListData* ptr;
        if (focusOnSelected) {
            QList<int> rows = dataHelper->ui->mpTable->getOrderedSelectedRows();
            for (int& r : rows) {
                ptr = mpList[r].detailParamData.data();
                if (!dParamRepeatChecker.contains(ptr)) {
                    dParamRepeatChecker.insert(ptr, paramSize);
                    ++paramSize;
                }
            }
            mpParamList.resize(paramSize);
            for (int& r : rows) {
                const IdpMpTableModeData& data = mpList[r];
                auto it = posRepeatChecker.find(data.posData.data());
                if (it == posRepeatChecker.end()) {
                    it = posRepeatChecker.insert(data.posData.data(), mpPosList.size());
                    data.posData->paramIndex = dParamRepeatChecker[data.detailParamData.data()];
                    mpParamList.replace(data.posData->paramIndex, data.detailParamData);
                    mpPosList.append(data.posData);
                }
            }
        } else {
            for (const IdpMpTableModeData& data : mpList) {
                ptr = data.detailParamData.data();
                if (!dParamRepeatChecker.contains(ptr)) {
                    dParamRepeatChecker.insert(ptr, paramSize);
                    ++paramSize;
                }
            }
            mpParamList.resize(paramSize);
            for (int i = 0; i < mpList.size(); ++ i) {
                const IdpMpTableModeData& data = mpList[i];
                auto it = posRepeatChecker.find(data.posData.data());
                if (it == posRepeatChecker.end()) {
                    it = posRepeatChecker.insert(data.posData.data(), mpPosList.size());
                    data.posData->paramIndex = dParamRepeatChecker[data.detailParamData.data()];
                    mpParamList.replace(data.posData->paramIndex, data.detailParamData);
                    mpPosList.append(data.posData);
                }
            }
        }
    }
    return "";
}

QString IdpDataEditorController::collectData() {
    QString errMsg = collectAPData();
    if (errMsg.length() > 0) return errMsg;
    return collectMPData(param);
}

void IdpDataEditorController::saveData(
    const IDP_SaveDataParam& saveParam, const sem::NO_PARAM_ARG_FUNC& callback, bool saveAs) {
    collectData();
    TotalSaveImgTmpData newImgIds;
    QList<quint64> allImageIds;
    collectAllImages(newImgIds, allImageIds);
    IDP_BaseData& baseData = dataHelper->baseData;
    bool s = ACaller->pushFocus(this, [this, saveParam, baseData, newImgIds, saveAs, callback, allImageIds](TSC) mutable {
        TransactionObject tdObj = TransactionManagerService::beginDefaultTransaction();
        TransactionObject tfObj = TransactionManagerService::beginFileTransaction();
        bool succ = false;
        if (!tdObj.isOpenSucc() || !tfObj.isOpenSucc()) {
            simpleMessageBox(this, "Error", "Error to open transaction for saving IDP.");
            return;
        }
        sem::AfterGc gc([&succ, &tdObj, &tfObj](){
            if (!succ) {
                tdObj.rollback();
                tfObj.rollback();
                return;
            }
        });
        IdpDataService* service = ServiceManager::instance()->getDefaultService<IdpDataService>();
        int errCode;
        quint64 existsId = service->getIdpIdByName(baseData.classId, saveParam.name, &errCode);
        if (errCode) {
            simpleMessageBox(this, "Error", "Error to check IDP name.");
            return;
        }
        if ((saveParam.id == 0 && existsId) || (saveParam.id > 0 && existsId > 0 && saveParam.id != existsId)) {
            if (saveAs) {
                simpleMessageBox(this, "Error", "IDP name already exists.");
            } else {
                simpleMessageBox(this, "Error", "IDP name already exists, please use \"Save as\" instead.");
            }
            return;
        }
        succ = localSaveAllImages(newImgIds, allImageIds);
        if (!succ) {
            simpleMessageBox(this, "Error", "Error to save images.");
            return;
        }
        MainThreadInvoker::getInvoker().blockRun([this, &newImgIds, &succ]() {
            OperationResult result = updateAllImageIds(newImgIds);
            if (result.errCode) {
                rollbackAllImagesIdOnly(newImgIds);
                simpleMessageBox(this, "Error", result.errMsg);
            } else {
                succ = true;
            }
        });
        if (!succ) {
            return;
        }
        if (succ) {
            succ = false;
            IdpDataEntity entity;
            MainThreadInvoker::getInvoker().blockRun(
                [&entity, &saveParam, &baseData, this, &succ]() {
                    idpApModelToJson(entity.getIdpApDetial(), param.apData);
                    idpMpModelToJson(entity.getIdpMpDetial(), param.mpData);
                    entity.setClassId(baseData.classId);
                    entity.setIdwId(baseData.idwId);
                    entity.setIdpName(saveParam.name);
                    succ = true;
                });
            if (!succ) {
                return;
            }
            succ = false;
            entity.setId(saveParam.id);
            errCode = service->saveWithTransaction(entity);
            if (errCode) {
                if (errCode == FILE_NAME_REPEAT_ERROR_CODE) {
                    if (saveAs) {
                        simpleMessageBox(this, "Error", "IDP name already exists.");
                    } else {
                        simpleMessageBox(this, "Error", "IDP name already exists, please use \"Save as\" instead.");
                    }
                } else {
                    simpleMessageBox(this, "Error", "Saved IDP Data Error.");
                }
                return;
            }
            ImageRelationRecordService* imageRelatioService = ServiceManager::defaultService<ImageRelationRecordService>();
            errCode = imageRelatioService->updateRelation(entity.getId(), IRDT_IDP, allImageIds);
            if (errCode) {
                simpleMessageBox(this, "Error", "Saved IDP Data Error.");
                return;
            }
            succ = tfObj.commit();
            if (!succ) {
                localRollbackAllImages(entity.getId(), newImgIds);
                simpleMessageBox(this, "Error", "Error to commit transaction for images.");
                return;
            }
            succ = tdObj.commit();
            if (!succ) {
                localRollbackAllImages(entity.getId(), newImgIds);

            } else {

            }
            MainThreadInvoker::getInvoker().blockRun([this, &entity, saveAs, callback, succ]() {
                if (succ) {
                    simpleMessageBox(this, "Info", "Saved successfully.");
                } else {
                    simpleMessageBox(this, "Error", "Error to commit transaction, failed to save IDP.");
                }
                dataHelper->baseData.idpId = entity.getId();
                this->param.idpName = entity.getIdpName();
                dataHelper->ui->idpTextField->setText(this->param.idpName);
                if (saveAs && dataHelper->saveAsBox) dataHelper->saveAsBox->close();
                if (saveAs) dataHelper->ui->saveAct->setVisible(true);
                if (callback) callback();
            });

        }
    });
    if (s) {
        loadingBox(this, "Process", "Working...");
    } else {
        simpleMessageBox(this, "Error", "Save IDP Data Error, no handler avaliable.");
    }
}

void IdpDataEditorController::prepareImage(
    ImageInfoEntity& imgEntity, QImage& image) {
    imgEntity.setWidth(image.width());
    imgEntity.setHeight(image.height());
    imgEntity.setImageFormat(image.format());
    imgEntity.setImageData(
        QByteArray::fromRawData(reinterpret_cast<char*>(image.bits()), image.sizeInBytes()));
}

void IdpDataEditorController::collectApImages(
    QList<IdpApSaveTmpData>& newImgIds, QList<quint64>& allImageIds) {
    newImgIds.reserve(param.apData.paramList.size());
    QList<QSharedPointer<IdpApParamListData>>& list = param.apData.paramList;
    quint64 imageId;
    for (QSharedPointer<IdpApParamListData>& data : list) {
        //动态关联模板，不做变更。
        if (data->apTemplate.templateId > 0 && data->apTemplate.templateRefType == 0) continue;
        imageId = data->apTemplate.templateParam.patternRegParam.imageId;
        if (imageId == 0) {
            if (data->apTemplate.templateParam.patternRegParam.imagePath.length() > 0)
                newImgIds.append({data->apTemplate.templateParam.patternRegParam.infoParam.imageAdditionData,
                              data.toWeakRef(), data->apTemplate.templateParam.patternRegParam.imagePath});
        } else {
            allImageIds.append(imageId);
        }
    }
}

void IdpDataEditorController::collectMpImages(
    QList<IdpMpSaveTmpData>& newImgIds, QList<quint64>& allImageIds) {
    newImgIds.reserve(param.mpData.paramList.size() * 3 + 2);
    QList<QSharedPointer<IdpMpParamListData>>& list = param.mpData.paramList;
    IdpMpSaveTmpData tmp;
    tmp.addressList.reserve(3);
    bool valid;
    quint64 imageId;
    for (QSharedPointer<IdpMpParamListData>& data : list) {
        //动态关联模板，不做变更。
        if (data->addressTemplate.templateId == 0 || data->addressTemplate.templateRefType != 0) {
            valid = false;
            AddressSaveTmpData tmpAddress;
            for (int i = 0; i < data->addressTemplate.templateParam.addressList.size(); ++ i) {
                MultiAddressDetailTemplateParam& address = data->addressTemplate.templateParam.addressList[i];
                imageId = address.patternRegParam.imageId;
                if (imageId == 0) {
                    if (address.patternRegParam.imagePath.length() > 0) {
                        if (!valid) valid = true;
                        tmpAddress = {address.patternRegParam.infoParam.imageAdditionData, address.patternRegParam.imagePath};
                        tmpAddress.index = i;
                        tmp.addressList.append(tmpAddress);
                    }
                } else {
                    allImageIds.append(imageId);
                }
            }
        }

        //动态关联模板，不做变更。
        if (data->mpTemplate.templateId > 0 && data->mpTemplate.templateRefType == 0) continue;
        imageId = data->mpTemplate.templateParam.patternRegParam.imageId;
        if (imageId == 0) {
            if (data->mpTemplate.templateParam.patternRegParam.imagePath.length() > 0) {
                tmp.mpImageDetail = data->mpTemplate.templateParam.patternRegParam.infoParam.imageAdditionData;
                tmp.mpPath = data->mpTemplate.templateParam.patternRegParam.imagePath;
                valid = true;
            }
        } else {
            allImageIds.append(imageId);
        }
        imageId = data->mpTemplate.templateParam.exeParam.imageId;
        if (imageId == 0) {
            if (data->mpTemplate.templateParam.exeParam.imagePath.length() > 0 && data->mpTemplate.templateParam.exeParam.imagePath != data->mpTemplate.templateParam.patternRegParam.imagePath) {
                tmp.exeImageDetail = data->mpTemplate.templateParam.exeParam.imageAddition;
                tmp.exePath = data->mpTemplate.templateParam.exeParam.imagePath;
                valid = true;
            }
        } else {
            allImageIds.append(imageId);
        }
        if (valid) {
            tmp.data = data.toWeakRef();
            newImgIds.append(tmp);
            tmp = {};
        }
    }
}

void IdpDataEditorController::collectAxisImages(
    IdpAxisSaveTmpData& newImgIds, QList<quint64>& allImageIds) {
    AxisAlignmentContentModel& axis = param.mpData.axisParam.alignContentParam;
    AxisStigmaContentModel& stigma = param.mpData.axisParam.stigmaContentParam;
    if (axis.imageId == 0) {
        if (axis.imagePath.length() > 0) {
            newImgIds.axisAlignPath = axis.imagePath;
            newImgIds.available = true;
            newImgIds.alignImageDetail = axis.patternParam.infoParam.imageAdditionData;
        }
    } else {
        allImageIds.append(axis.imageId);
    }
    if (stigma.imageId == 0) {
        if (stigma.imagePath.length() > 0) {
            newImgIds.axisStigmaPath = stigma.imagePath;
            newImgIds.available = true;
            newImgIds.stigmaImageDetail = stigma.patternParam.infoParam.imageAdditionData;
        }
    } else {
        allImageIds.append(stigma.imageId);
    }
}

void IdpDataEditorController::collectAllImages(
    TotalSaveImgTmpData& newImgIds, QList<quint64>& allImageIds) {
    collectApImages(newImgIds.apData, allImageIds);
    collectMpImages(newImgIds.mpData, allImageIds);
    collectAxisImages(newImgIds.axisData, allImageIds);
}

void coverImageId(
    int& i, QList<FileImageDuplicateSaveData>& list, quint64& id, const QString& path) {
    if (path.length() > 0) {
        id = list[i].entity.getId();
        ++i;
    }
}

bool IdpDataEditorController::localSaveAllImages(
    TotalSaveImgTmpData& newImgIds, QList<quint64>& allImageIds) {
    QList<FileImageDuplicateSaveData> list;
    list.reserve(newImgIds.apData.size() + newImgIds.mpData.size() * 3 + 2);
    QString json;
    FileImageDuplicateSaveData imgEntity;
    for (IdpApSaveTmpData& data : newImgIds.apData) {
        if (data.path.isEmpty()) continue;
        toImageDetailDataModelJson(json, data.apImageDetail);
        imgEntity.path = data.path;
        imgEntity.entity.setImageInfo(json);
        imgEntity.entity.setDataType(cdsem::IDT_AP_PATTERN);
        imgEntity.fileType = cdsem::FT_IMAGE_AP;
        list.append(imgEntity);
    }
    for (IdpMpSaveTmpData& data : newImgIds.mpData) {
        if (data.addressList.size() > 0) {
            for (AddressSaveTmpData& address : data.addressList) {
                imgEntity.path = address.path;
                toImageDetailDataModelJson(json, address.detail);
                imgEntity.entity.setImageInfo(json);
                imgEntity.entity.setDataType(cdsem::IDT_MS_PATTERN);
                imgEntity.fileType = cdsem::FT_IMAGE_MS;
                list.append(imgEntity);
            }
        }
        if (data.mpPath.length() > 0) {
            imgEntity.path = data.mpPath;
            toImageDetailDataModelJson(json, data.mpImageDetail);
            imgEntity.entity.setImageInfo(json);
            imgEntity.entity.setDataType(cdsem::IDT_MP_PATTERN);
            imgEntity.fileType = cdsem::FT_IMAGE_MP;
            list.append(imgEntity);
        }
        if (data.exePath.length() > 0) {
            imgEntity.path = data.exePath;
            toImageDetailDataModelJson(json, data.exeImageDetail);
            imgEntity.entity.setImageInfo(json);
            imgEntity.entity.setDataType(cdsem::IDT_MP_EXE_PATTERN);
            imgEntity.fileType = cdsem::FT_IMAGE_MEASURE;
            list.append(imgEntity);
        }
    }
    if (newImgIds.axisData.axisAlignPath.length() > 0) {
        imgEntity.path = newImgIds.axisData.axisAlignPath;
        toImageDetailDataModelJson(json, newImgIds.axisData.alignImageDetail);
        imgEntity.entity.setImageInfo(json);
        imgEntity.entity.setDataType(cdsem::IDT_AXIS_ALIGN_PATTERN);
        imgEntity.fileType = cdsem::FT_IMAGE_AXIS;
        list.append(imgEntity);
    }
    if (newImgIds.axisData.axisStigmaPath.length() > 0) {
        imgEntity.path = newImgIds.axisData.axisStigmaPath;
        toImageDetailDataModelJson(json, newImgIds.axisData.stigmaImageDetail);
        imgEntity.entity.setImageInfo(json);
        imgEntity.entity.setDataType(cdsem::IDT_AXIS_STIGMA_PATTERN);
        imgEntity.fileType = cdsem::FT_IMAGE_STIGMA;
        list.append(imgEntity);
    }
    if (list.isEmpty()) {
        return true;
    }
    ImageInfoService* imgService = ServiceManager::instance()->getDefaultService<ImageInfoService>();
    OperationResult result;
    imgService->save(list, &result);
    if (result.errCode) {
        simpleMessageBox(this, "Error", result.errMsg);
        return false;
    }
    for (FileImageDuplicateSaveData& data : list) {
        allImageIds.append(data.entity.getId());
    }
    int i = 0;
    for (IdpApSaveTmpData& data : newImgIds.apData) {
        coverImageId(i, list, data.id, data.path);
    }
    for (IdpMpSaveTmpData& data : newImgIds.mpData) {
        for (AddressSaveTmpData& address : data.addressList) {
            coverImageId(i, list, address.imageId, address.path);
        }
        coverImageId(i, list, data.mpId, data.mpPath);
        coverImageId(i, list, data.exeId, data.exePath);
    }
    coverImageId(i, list, newImgIds.axisData.axisAlignId, newImgIds.axisData.axisAlignPath);
    coverImageId(i, list, newImgIds.axisData.axisStigmaId, newImgIds.axisData.axisStigmaPath);
    return true;
}

OperationResult IdpDataEditorController::updateAllImageIds(
    const TotalSaveImgTmpData& newImgIds) {
    OperationResult result;
    result.errCode = DATA_PROCESS_CHANGED_ERR_CODE;
    for (const IdpApSaveTmpData& data : newImgIds.apData) {
        if (data.id == 0) {
            continue;
        }
        QSharedPointer<IdpApParamListData> apData = data.data.lock();
        if (apData.isNull() || apData->apTemplate.templateParam.patternRegParam.imagePath != data.path) {
            result.errMsg = "AP Data changed while saving.";
            return result;
        }
        apData->apTemplate.templateParam.patternRegParam.imageId = data.id;
    }
    for (const IdpMpSaveTmpData& data : newImgIds.mpData) {
        QSharedPointer<IdpMpParamListData> mpData = data.data.lock();
        if (mpData.isNull()) {
            result.errMsg = "MP Data changed while saving.";
            return result;
        }
        if (data.addressList.size() > 0) {
            if (data.addressList.size() > mpData->addressTemplate.templateParam.addressList.size()) {
                result.errMsg = "Address Data changed while saving.";
                return result;
            }
            QList<MultiAddressDetailTemplateParam>& origList = mpData->addressTemplate.templateParam.addressList;
            for (const AddressSaveTmpData& tmp : data.addressList) {
                if (tmp.index >= origList.size()) {
                    result.errMsg = "Address Data changed while saving.";
                    return result;
                }
                if (origList[tmp.index].patternRegParam.imagePath != tmp.path) {
                    result.errMsg = "Address Data changed while saving.";
                    return result;
                }
                origList[tmp.index].patternRegParam.imageId = tmp.imageId;
            }
        }
        if (data.mpId != 0) {
            if (mpData->mpTemplate.templateParam.patternRegParam.imagePath != data.mpPath) {
                result.errMsg = "MP/IP Data changed while saving.";
                return result;
            }
            mpData->mpTemplate.templateParam.patternRegParam.imageId = data.mpId;
            if (data.exeId == 0) {
                mpData->mpTemplate.templateParam.exeParam.imageId = data.mpId;
            }
        }
        if (data.exeId != 0) {
            if (mpData->mpTemplate.templateParam.exeParam.imagePath != data.exePath) {
                result.errMsg = "MP/IP Data changed while saving.";
                return result;
            }
            mpData->mpTemplate.templateParam.exeParam.imageId = data.exeId;
        }
    }
    if (newImgIds.axisData.axisAlignId > 0) {
        if (param.mpData.axisParam.alignContentParam.imagePath != newImgIds.axisData.axisAlignPath) {
            result.errMsg = "Axis Data changed while saving.";
            return result;
        }
        param.mpData.axisParam.alignContentParam.imageId = newImgIds.axisData.axisAlignId;
    }
    if (newImgIds.axisData.axisStigmaId > 0) {
        if (param.mpData.axisParam.stigmaContentParam.imagePath
            != newImgIds.axisData.axisStigmaPath) {
            result.errMsg = "Axis Stigma Data changed while saving.";
            return result;
        }
        param.mpData.axisParam.stigmaContentParam.imageId = newImgIds.axisData.axisStigmaId;
    }
    result.errCode = 0;
    return result;
}

void IdpDataEditorController::localRollbackAllImages(quint64 idpId,
    const TotalSaveImgTmpData& newImgIds) {
    MainThreadInvoker::getInvoker().blockRun([this, &newImgIds](){
        rollbackAllImagesIdOnly(newImgIds);
    });
    QList<quint64> imgIds;
    imgIds.reserve(newImgIds.apData.size() + newImgIds.mpData.size() * 3 + 2);
    for (const IdpApSaveTmpData& data : newImgIds.apData) {
        if (data.id > 0) {
            imgIds.append(data.id);
        }
    }
    for (const IdpMpSaveTmpData& data : newImgIds.mpData) {
        for (const AddressSaveTmpData& address : data.addressList) {
            if (address.imageId > 0)
                imgIds.append(address.imageId);
        }
        if (data.mpId > 0) {
            imgIds.append(data.mpId);
        }
    }
    if (newImgIds.axisData.axisAlignId > 0) {
        imgIds.append(newImgIds.axisData.axisAlignId);
    }
    if (newImgIds.axisData.axisStigmaId > 0) {
        imgIds.append(newImgIds.axisData.axisStigmaId);
    }
    if (imgIds.isEmpty()) {
        return;
    }
    qint32 errCode;
    {
        ImageInfoService* service = ServiceManager::instance()->getDefaultService<ImageInfoService>();
        errCode = service->realDeleteByIds(imgIds);
        if (errCode) {
            qCritical() << "数据保存错误回滚删除图像ID时失败: " << errCode;
        }
    }
    ImageRelationRecordService* service = ServiceManager::instance()->getDefaultService<ImageRelationRecordService>();
    errCode = service->realDeleteImageRelation(idpId, IRDT_IDP, imgIds);
    if (errCode) {
        qCritical() << "数据保存错误回滚删除图像数据关联ID时失败: " << errCode;
    }
}

void IdpDataEditorController::rollbackAllImagesIdOnly(const TotalSaveImgTmpData& newImgIds) {
    for (const IdpApSaveTmpData& data : newImgIds.apData) {
        if (data.id == 0) {
            continue;
        }
        QSharedPointer<IdpApParamListData> apData = data.data.lock();
        if (apData.isNull() || apData->apTemplate.templateParam.patternRegParam.imagePath != data.path) {
            continue;
        }
        apData->apTemplate.templateParam.patternRegParam.imageId = 0;
    }
    for (const IdpMpSaveTmpData& data : newImgIds.mpData) {
        QSharedPointer<IdpMpParamListData> mpData = data.data.lock();
        if (mpData.isNull()) {
            continue;
        }
        if (data.addressList.size() > 0) {
            QList<MultiAddressDetailTemplateParam>& origList = mpData->addressTemplate.templateParam.addressList;
            for (const AddressSaveTmpData& tmp : data.addressList) {
                if (tmp.index < origList.size() && tmp.path == origList[tmp.index].patternRegParam.imagePath) {
                    origList[tmp.index].patternRegParam.imageId = 0;
                }
            }
        }
        if (data.mpId != 0 && mpData->mpTemplate.templateParam.patternRegParam.imagePath == data.mpPath) {
            mpData->mpTemplate.templateParam.patternRegParam.imageId = 0;
            if (data.exeId == data.mpId) {
                mpData->mpTemplate.templateParam.exeParam.imageId = 0;
            }
        }
        if (data.exeId != 0 && mpData->mpTemplate.templateParam.exeParam.imagePath == data.exePath) {
            mpData->mpTemplate.templateParam.exeParam.imageId = 0;
        }
    }
    if (newImgIds.axisData.axisAlignId > 0
        && param.mpData.axisParam.alignContentParam.imagePath
               == newImgIds.axisData.axisAlignPath) {
        param.mpData.axisParam.alignContentParam.imageId = 0;
    }
    if (newImgIds.axisData.axisStigmaId > 0
        && param.mpData.axisParam.stigmaContentParam.imagePath
               == newImgIds.axisData.axisStigmaPath) {
        param.mpData.axisParam.stigmaContentParam.imageId = 0;
    }
}

void IdpDataEditorController::collectDynamicIds(
    const IdpDetailData& param, QList<quint64>& ids) {
    if (param.apData.paramList.size() > 0) {
        for (const QSharedPointer<IdpApParamListData>& d : param.apData.paramList) {
            if (d->apTemplate.templateId > 0 && d->apTemplate.templateRefType == 0) {
                ids.append(d->apTemplate.templateId);
            }
        }
    }
    if (param.mpData.paramList.size() > 0) {
        for (const QSharedPointer<IdpMpParamListData>& d : param.mpData.paramList) {
            //MP IP暂时不做动态导入功能
            // if (d->mpipTemplateId > 0 && d->mpipTemplateRefType == 0) {
            //     ids.append(d->mpipTemplateId);
            // }
            if (d->addressTemplate.templateId > 0 && d->addressTemplate.templateRefType == 0) {
                ids.append(d->addressTemplate.templateId);
            }
            if (d->mpTemplate.templateId > 0 && d->mpTemplate.templateRefType == 0) {
                ids.append(d->mpTemplate.templateId);
            }
        }
    }
}

void IdpDataEditorController::localQueryTemplate(
    const QList<quint64>& ids, const IdpDetailData& param) {
    sem::AfterGc gc(
        [this]() { MainThreadInvoker::getInvoker().runLater([this]() { resetToComponent(); }); });
    TemplateInfoService* service = ServiceManager::instance()
                                       ->getDefaultService<TemplateInfoService>();
    int errCode;
    QList<TemplateInfoEntity> mapList = service->getSimpleInfoByIds(ids, &errCode);
    if (errCode) {
        simpleMessageBox(this, "Error", "Load dynamic template data error.");
        return;
    }
    QList<TemplateInfoEntity> list = service->getDatasByIdsForNewVersion(ids, &errCode);
    if (errCode) {
        simpleMessageBox(this, "Error", "Load dynamic template data error.");
        return;
    }
    if (list.isEmpty() || mapList.isEmpty()) {
        return;
    }
    QHash<quint64, quint64> topIdToIdMap;
    for (TemplateInfoEntity& e : mapList) {
        topIdToIdMap.insert(e.getTopId(), e.getId());
    }
    QHash<quint64, TemplateInfoEntity> hash;
    quint64 topId;
    while (list.size() > 0) {
        topId = list.first().getTopId();
        auto it = topIdToIdMap.find(topId);
        if (it == topIdToIdMap.end()) {
            list.removeFirst();
            continue;
        }
        hash.insert(*it, list.takeFirst());
    }
    if (param.apData.paramList.size() > 0) {
        for (const QSharedPointer<IdpApParamListData>& d : param.apData.paramList) {
            if (d->apTemplate.templateId > 0 && d->apTemplate.templateRefType == 0) {
                auto it = hash.find(d->apTemplate.templateId);
                if (it == hash.end()) {
                    continue;
                }
                ApTemplateInfoModel model;
                errCode = templateInfoModelFromEntity(*it, model);
                if (errCode) {
                    simpleMessageBox(this, "Error", "Load AP dynamic template data error.");
                    return;
                }
                d->apTemplate.templateParam = model.data;
            }
        }
    }
    if (param.mpData.paramList.size() > 0) {
        for (const QSharedPointer<IdpMpParamListData>& d : param.mpData.paramList) {
            //MP IP暂时不做动态导入功能
            // if (d->mpipTemplateId > 0 && d->mpipTemplateRefType == 0) {
            //     auto it = hash.find(d->mpipTemplateId);
            //     if (it == hash.end()) {
            //         continue;
            //     }
            //     MPIPTemplateInfoModel model;
            //     errCode = templateInfoModelDataFromEntity(*it, model);
            //     if (errCode) {
            //         simpleMessageBox(this, "Error", "Load MPIP dynamic template data error.");
            //         return;
            //     }
            //     d->dPoint = IDW_PointF {model.dx,model.dy};
            //     d->dPointType = 0;
            // }
            if (d->addressTemplate.templateId > 0 && d->addressTemplate.templateRefType == 0) {
                auto it = hash.find(d->addressTemplate.templateId);
                if (it == hash.end()) {
                    continue;
                }
                MultiAddressTemplateInfoModel model;
                errCode = templateInfoModelFromEntity(*it, model);
                if (errCode) {
                    simpleMessageBox(this, "Error", "Load MS dynamic template data error.");
                    return;
                }
                d->addressTemplate.templateParam = model.data;
            }
            if (d->mpTemplate.templateId > 0 && d->mpTemplate.templateRefType == 0) {
                auto it = hash.find(d->mpTemplate.templateId);
                if (it == hash.end()) {
                    continue;
                }
                MpTemplateInfoModel model;
                errCode = templateInfoModelFromEntity(*it, model);
                if (errCode) {
                    simpleMessageBox(this, "Error", "Load MP dynamic template data error.");
                    return;
                }
                d->mpTemplate.templateParam = model.data;
            }
        }
    }
}

void IdpDataEditorController::localPrepareTitleTemplate(
    IdpDetailData& tmpData) {
    if (tmpData.mpData.paramList.size() > 0) {
        QList<quint64> staticMpipIds;
        QList<quint64> dynamicMpipIds;
        staticMpipIds.reserve(tmpData.mpData.paramList.size());
        dynamicMpipIds.reserve(tmpData.mpData.paramList.size());
        for (QSharedPointer<IdpMpParamListData>& d : tmpData.mpData.paramList) {
            if (d->mpipTemplateId > 0) {
                if (d->mpipTemplateRefType == 0) {
                    dynamicMpipIds.append(d->mpipTemplateId);
                } else {
                    staticMpipIds.append(d->mpipTemplateId);
                }
            }
        }
        if (dynamicMpipIds.isEmpty() && staticMpipIds.isEmpty()) {
            return;
        }
        int errCode;
        QHash<quint64, MPIPTemplateInfoModel> dynamicMap;
        QHash<quint64, MPIPTemplateInfoModel> staticMap;
        TemplateInfoService* templateService = ServiceManager::instance()
                                                   ->getDefaultService<TemplateInfoService>();
        QList<TemplateInfoEntity> mapList = templateService->getSimpleInfoByIds(dynamicMpipIds,
                                                                                &errCode);
        if (errCode) {
            simpleMessageBox(this, "Load Template Data Error", "Something goes wrong.");
            return;
        }
        if (mapList.size() > 0) {
            QHash<quint64, quint64> topToIds;
            for (TemplateInfoEntity& e : mapList) {
                topToIds.insert(e.getTopId(), e.getId());
            }
            QList<TemplateInfoEntity> list
                = templateService->getDatasByIdsForNewVersion(dynamicMpipIds, &errCode);
            if (errCode) {
                simpleMessageBox(this, "Load Template Data Error", "Something goes wrong.");
                return;
            }
            for (TemplateInfoEntity& e : list) {
                auto it = topToIds.find(e.getTopId());
                if (it == topToIds.end()) {
                    continue;
                }
                MPIPTemplateInfoModel m;
                errCode = templateInfoModelDataFromEntity(e, m);
                if (errCode) {
                    simpleMessageBox(this, "Load Template Data Error", "Something goes wrong.");
                    return;
                }
                dynamicMap.insert(*it, m);
            }
        }

        if (staticMpipIds.size() > 0) {
            QList<TemplateInfoEntity> list
                = templateService->getDatasByIdsForNewVersion(staticMpipIds, &errCode);
            if (errCode) {
                simpleMessageBox(this, "Load Template Data Error", "Something goes wrong.");
                return;
            }
            for (TemplateInfoEntity& e : list) {
                MPIPTemplateInfoModel m;
                errCode = templateInfoModelDataFromEntity(e, m);
                if (errCode) {
                    simpleMessageBox(this, "Load Template Data Error", "Something goes wrong.");
                    return;
                }
                staticMap.insert(e.getId(), m);
            }
        }

        quint64 msId, mpId;
        for (QSharedPointer<IdpMpParamListData>& d : tmpData.mpData.paramList) {
            if (d->mpipTemplateId > 0) {
                if (d->mpipTemplateRefType == 0) {
                    auto it = dynamicMap.find(d->mpipTemplateId);
                    if (it == dynamicMap.end()) {
                        msId = 0;
                        mpId = 0;
                    } else {
                        msId = it->msId;
                        mpId = it->mpId;
                    }
                    //目前的需求做为：如果MP/IP模板是动态载入的，MS/MP如果也是动态载入，则以MP/IP为准，否则以MS/MP为准
                    if (d->addressTemplate.templateRefType == 0) {
                        d->addressTemplate.templateId = msId;
                    }
                    if (d->mpTemplate.templateRefType == 0) {
                        d->mpTemplate.templateId = mpId;
                    }
                    d->mpipTemplateName = it->name;
                } else {
                    auto it = staticMap.find(d->mpipTemplateId);
                    if (it != staticMap.end()) {
                        d->mpipTemplateName = it->name;
                    }
                }
            }
        }
    }
}

void IdpDataEditorController::loadExistsData(const IDP_BaseData& base) {
    quint64 id = base.idpId;
    quint64 classId = base.classId;
    bool succ = ACaller->push(this, [this, id, classId](TSC) {
        IdpDataService* idpService = ServiceManager::instance()->getDefaultService<IdpDataService>();
        int errCode;
        IdpDataEntity entity = idpService->get(id, &errCode);
        if (errCode) {
            simpleMessageBox(this, "Load IDP Data Error", "Something goes wrong.");
            return;
        }
        if (entity.getId() <= 0) {
            simpleMessageBox(this, "Load IDP Data Error", "The IDP data is not exists.");
            return;
        }
        if (classId > 0 && entity.getClassId() != classId) {
            simpleMessageBox(this, "Load IDP Data Error", "The IDP data class analysis failed.");
            return;
        }
        ClassService* classService = ServiceManager::instance()->getDefaultService<ClassService>();
        ClassEntity classEntity = classService->getById(entity.getClassId(), &errCode);
        if (errCode) {
            simpleMessageBox(this,
                             "Load Class Data Error",
                             errCode == SQL_QUERY_DATA_NOT_EXISTS ? "Class Data is not exisits."
                                                                  : "Something goes wrong.");
            return;
        }

        QString waferName;
        if (entity.getIdwId() > 0) {
            IDW_WaferService* waferService = ServiceManager::instance()
                                                 ->getDefaultService<IDW_WaferService>();
            waferName = waferService->getWaferName(entity.getIdwId(), &errCode);
            if (errCode) {
                simpleMessageBox(this, "Load IDW Data Error", "Something goes wrong.");
                return;
            }
        }

        IdpDetailData tmpData;
        errCode = idpApModelFromJson(entity.getIdpApDetial(), tmpData.apData);
        if (errCode) {
            simpleMessageBox(this, "Error", "Error to load AP data.");
            return;
        }
        errCode = idpMpModelFromJson(entity.getIdpMpDetial(), tmpData.mpData);
        if (errCode) {
            simpleMessageBox(this, "Error", "Error to load MP data.");
            return;
        }
        localPrepareTitleTemplate(tmpData);
        QList<quint64> dynamicIds;
        dynamicIds.reserve(tmpData.apData.paramList.size() + tmpData.mpData.paramList.size() * 2);
        collectDynamicIds(tmpData, dynamicIds);
        if (dynamicIds.size() > 0) {
            localQueryTemplate(dynamicIds, tmpData);
        }

        MainThreadInvoker::getInvoker().blockRun(
            [this, &entity, &waferName, &classEntity, &tmpData]() {
                param = tmpData;
                dataHelper->baseData.idwId = entity.getIdwId();
                param.idpName = entity.getIdpName();
                dataHelper->baseData.waferName = waferName;
                dataHelper->baseData.className = classEntity.getClassName();
                dataHelper->baseData.classId = entity.getClassId();
                dataHelper->baseData.idpId = entity.getId();
                dataHelper->baseData.idwTopId = entity.getIdwTopId();
                resetToComponent();
                if (entity.getIdwId() > 0) {
                    quint64 idwTopId = cdsem::CDSEM_GlobalDataHelper::get()->getGlobalIdwTopId();
                    if (idwTopId != entity.getIdwTopId()) {
                        dataHelper->createInitDiffStageController();
                        dataHelper->initStageDiffController->show();
                    }
                }
            });
    });
    if (succ) {
        loadingBox(this, "Process", "Loading");
    }
}

void IdpDataEditorController::newData(quint64 classId, quint64 idwId, const QString& idpName) {
    if (classId <= 0) {
        simpleMessageBox(this, "Error", "No Class Specified.");
        return;
    }
    param.idpName = idpName;
    dataHelper->ui->idpTextField->setText(idpName);
    bool succ = ACaller->push(this, [this, classId, idwId](TSC) {
        QString className;
        QString idwName;
        quint64 idwTopId = 0;
        int errCode;
        {
            ClassService* classService = ServiceManager::instance()->getDefaultService<ClassService>();
            ClassEntity classEntity = classService->getById(classId, &errCode);
            if (errCode) {
                simpleMessageBox(this,
                                 "Load Class Data Error",
                                 errCode == SQL_QUERY_DATA_NOT_EXISTS ? "Class Data is not exists."
                                                                      : "Something goes wrong to load Class");
                return;
            }
            className = classEntity.getClassName();
        }
        {
            IDW_WaferService* service = ServiceManager::defaultService<IDW_WaferService>();
            IDW_WaferInfoEntity entity = service->getSimpleInfoById(idwId, &errCode);
            if (errCode) {
                simpleMessageBox(this,
                                 "Load IDW Data Error", "Something goes wrong to load IDW");
                return;
            }
            if (entity.getId() == 0) {
                simpleMessageBox(this,
                                 "Load IDW Data Error", "IDW Data is not exists.");
            }
            idwName = entity.getWaferName();
            idwTopId = entity.getTopId();
        }
        MainThreadInvoker::getInvoker().runLater([this, classId, className, idwId, idwTopId, idwName]() {
            dataHelper->maxAP_P_Number = 1;
            dataHelper->maxMP_P_Number = 1;
            dataHelper->apTableMode->clear();
            dataHelper->mpTableMode->clear();
            dataHelper->baseData.classId = classId;
            dataHelper->baseData.className = className;
            dataHelper->baseData.idwId = idwId;
            dataHelper->baseData.idwTopId = idwTopId;
            dataHelper->baseData.waferName = idwName;
            dataHelper->ui->classTextField->setText(dataHelper->baseData.className);
            dataHelper->ui->idwTextField->setText(dataHelper->baseData.waferName);

            if (idwId > 0) {
                quint64 gIdwTopId = cdsem::CDSEM_GlobalDataHelper::get()->getGlobalIdwTopId();
                if (gIdwTopId != idwTopId) {
                    dataHelper->createInitDiffStageController();
                    dataHelper->initStageDiffController->show();
                }
            }
        });
    });
    if (succ) {
        loadingBox(this, "Process", "Loading");
    }
}

void IdpDataEditorController::disableCoverSave(bool d) {
    dataHelper->ui->saveAct->setVisible(!d);
}

quint64 IdpDataEditorController::getIdpId() const {
    return dataHelper->baseData.idpId;
}

quint64 IdpDataEditorController::getIdwId() const {
    return dataHelper->baseData.idwId;
}

quint64 IdpDataEditorController::getIdwTopId() const {
    return dataHelper->baseData.idwTopId;
}
