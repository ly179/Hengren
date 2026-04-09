#include "idp_mp_coordinate_setting_controller.h"
#include "cde90_message_box.h"
#include "cdsem_number_validator_helper.h"
#include "ui_idp_mp_coordinate_setting_controller.h"
#include "multi_shot_chip_select_controller.h"
#include "cdsem_global_data.h"
#include "cdsem_control_helper.h"
#include "cdsem_global_env_data_model.h"

struct IdpMpCoordinateSettingDataHelper{
    IdpMpCoordinateSettingController* d;
    MultiShotChipSelectController* multiShotChipSelector = nullptr;
    //0 - 默认，点击Reg 按钮回调
    //1 - 点击1 Move按钮回调
    int multiSelectedChipType = 0;

    void createMultiChipSelector() {
        if (multiShotChipSelector) return;
        multiShotChipSelector = new MultiShotChipSelectController(true, d);
        multiShotChipSelector->setWindowTitle("Overlapping Chips Selector");
        multiShotChipSelector->setWindowModality(Qt::WindowModal);
        multiShotChipSelector->setCloseCall([this](CDE90WindowCloseStatus status){
            if (status == HW_OK) {
                cdsem::PyShotChipCoordinate* data = multiShotChipSelector->getSelectedData();
                if (!data) {
                    simpleMessageBox(multiShotChipSelector, "Error", "Please select one chip.");
                    return false;
                }
                if (multiSelectedChipType == 0) {
                    dRegCoordinateToParam(data->globalChip);
                } else {
                    dMovePos(data->globalChip);
                }
            }
            multiShotChipSelector->clearListData();
            return true;
        });
    }

    void updateDPointXFromTextField() {
        int val = d->ui->posItemBox->currentValue();
        QString text = d->ui->dxTextField->text();
        if (val == 0) {
            d->param.dPoint.x = text;
        } else if (d->param.prevCoordinate.x != nullptr){
            if (text.isEmpty()) {
                text = "0";
            }
            d->param.dPoint.x = d->param.prevCoordinate.x + text;
        }
    }

    void updateDPointYFromTextField() {
        int val = d->ui->posItemBox->currentValue();
        QString text = d->ui->dyTextField->text();
        if (val == 0) {
            d->param.dPoint.y = text;
        } else if (d->param.prevCoordinate.y != nullptr){
            if (text.isEmpty()) {
                text = "0";
            }
            d->param.dPoint.y = d->param.prevCoordinate.y + text;
        }
    }

    void setDTextFieldValue() {
        int val = d->ui->posItemBox->currentValue();
        if (val == 0) {
            d->ui->dxTextField->setText(d->param.dPoint.x.toQString());
            d->ui->dyTextField->setText(d->param.dPoint.y.toQString());
        } else {
            if (d->param.dPoint.x == nullptr) {
                d->ui->dxTextField->setText((-d->param.prevCoordinate.x).toQString());
            } else {
                d->ui->dxTextField->setText((d->param.dPoint.x - d->param.prevCoordinate.x).toQString());
            }
            if (d->param.dPoint.y == nullptr) {
                d->ui->dyTextField->setText((-d->param.prevCoordinate.y).toQString());
            } else {
                d->ui->dyTextField->setText((d->param.dPoint.y - d->param.prevCoordinate.y).toQString());
            }
        }
    }

    void dRegCoordinateToParam(int chip) {
        bool succ;
        cdsem::PyShotChipCoordinate coordinate = cdsem::WaferStageCoordinateConvert::instance()->getShotChipUm(chip, &succ);
        if (!succ) {
            simpleMessageBox(d, "Error", "Error to check shot and chip info.");
            return;
        }
        IDW_PointF pos = cdsem::WaferStageCoordinateConvert::instance()->waferPosUm();
        d->param.dPoint.x = pos.x - coordinate.cx;
        d->param.dPoint.y = pos.y - coordinate.cy;
        setDTextFieldValue();
    }


    bool confirmChip(int& chip, int multiType) {
        if (d->param.chipNumber < 0) {
            simpleMessageBox(d, "Error", "Empty chip data.");
            return false;
        }
        chip = -1;
        cdsem::PyStageShotChipCoordinate currentChips = cdsem::WaferStageCoordinateConvert::instance()->getShotChipWaferUm(false);
        QList<cdsem::PyShotChipCoordinate> chips;
        if (currentChips.list.size() > 0) {
            chips.reserve(currentChips.list.size());
            for (cdsem::PyShotChipCoordinate& p : currentChips.list) {
                if (p.globalChip < 0) continue;
                chips.append(p);
                if (p.globalChip == d->param.chipNumber) {
                    chip = p.globalChip;
                }
            }
        }
        if (chip == -1) {
            if (chips.isEmpty()) {
                //使用指定的Chip来注册坐标
                simpleMessageBox(d, "Info", "No chip data detected. Defaulting to assigned registration coordinates.");
                chip = d->param.chipNumber;
            } else {
                //询问使用机台当前chip还是指定的chip来注册坐标
                int r = showYesNoBox(d, "Info", "Chip mismatch detected. Use registration coordinates from:", false, false, "Current Machine", "Specified");
                if (r) {
                    chip = d->param.chipNumber;
                } else {
                    if (chips.size() == 1) {
                        chip = chips.first().globalChip;
                    } else {
                        createMultiChipSelector();
                        multiSelectedChipType = multiType;
                        multiShotChipSelector->setListData(chips);
                        multiShotChipSelector->show();
                        return false;
                    }
                }
            }
        }
        return true;
    }

    void dRegCoordinate() {
        int chip;
        if (confirmChip(chip, 0)) {
            dRegCoordinateToParam(chip);
        }
    }

    void dMovePos(int chip) {
        if (chip < 0) return;
        bool succ;
        cdsem::PyShotChipCoordinate coordinate = cdsem::WaferStageCoordinateConvert::instance()->getShotChipUm(chip, &succ);
        if (!succ) {
            simpleMessageBox(d, "Error", "Error to check shot and chip info.");
            return;
        }
        IDW_PointF pos = d->param.dPoint;
        if (pos.x == nullptr) pos.x = coordinate.cx;
        else pos.x += coordinate.cx;
        if (pos.y == nullptr) pos.y = coordinate.cy;
        else pos.y += coordinate.cy;
        MoveStageParam stageParam;
        cdsem::WaferStageCoordinateConvert::instance()->umToStagePos(pos, stageParam);
        CDSEM_ControlHelper::instance()->moveStage(d, stageParam);
    }

    void dMovePos() {
        if (d->param.chipNumber < 0) {
            simpleMessageBox(d, "Error", "Empty chip data.");
            return;
        }
        int chip;
        if (confirmChip(chip, 1)) {
            dMovePos(chip);
        }
    }
};

IdpMpCoordinateSettingController::IdpMpCoordinateSettingController(QWidget* parent) : CDE90Dialog(parent) {
    //如果你没有异步任务需求，请将connect清理掉，保留create和createAction函数
    connect(this, &IdpMpCoordinateSettingController::asyncCallerChanged, this, [this](){
        create();
        createAction();
    }, Qt::SingleShotConnection);
}

IdpMpCoordinateSettingController::~IdpMpCoordinateSettingController() noexcept {
    if (ui) {
        delete ui;
    }
    if (dataHelper) delete dataHelper;
}

void IdpMpCoordinateSettingController::create() {
    dataHelper = new IdpMpCoordinateSettingDataHelper;
    dataHelper->d = this;
    ui = new cdsem::UiIdpMpCoordinateSettingController;
    setContent(new QWidget);
    ui->init(getContent());
}

void IdpMpCoordinateSettingController::createAction() {
    CDSemNumberValidatorHelper::doubleFree<2>()
        .put(ui->dxTextField->getQLineEdit())
        .put(ui->dyTextField->getQLineEdit())
        .autoFixMode();
    bindCloseBtn(ui->cancelBtn->getQPushButton());
    SEM_OK_ACTION(ui->okBtn->getQPushButton())
    SEM_STEP_NEXT_ACTION(ui->nextBtn->getQPushButton())

    connect(ui->posItemBox->getQComboBox(), &QComboBox::currentIndexChanged, this, [this](int){
        int val = ui->posItemBox->currentValue();
        if (val == 1 && (param.prevCoordinate.x == nullptr || param.prevCoordinate.y == nullptr)) {
            simpleMessageBox(this, "Error", "Previous Pattern Group No. not found. Relative Position unavailable.");
            ui->posItemBox->setCurrentData(0);
            return;
        }
        dataHelper->setDTextFieldValue();
    });
    connect(ui->dxTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        dataHelper->updateDPointXFromTextField();
    });
    connect(ui->dyTextField->getQLineEdit(), &QLineEdit::editingFinished, this, [this](){
        dataHelper->updateDPointYFromTextField();
    });
    connect(ui->posRegBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        dataHelper->dRegCoordinate();
    });
    connect(ui->moveBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        dataHelper->dMovePos();
    });
}

void IdpMpCoordinateSettingController::resetToComponent() {
    if (param.dPointType == 1 && (param.prevCoordinate.x == nullptr || param.prevCoordinate.y == nullptr)) {
        simpleMessageBox(this, "Error", "The previous Pattern Group No. is missing. The displayed coordinates will be automatically corrected.");
        param.dPointType = 0;
    }
    ui->chipNoXTextField->setText(param.chipX.toString());
    ui->chipNoYTextField->setText(param.chipY.toString());
    ui->diffCBox->setChecked(param.usePrevPoint);
    ui->posItemBox->setCurrentData(param.dPointType);
    dataHelper->setDTextFieldValue();
}
QString IdpMpCoordinateSettingController::collectData() {
    param.dPointType = ui->posItemBox->currentValue();
    if (param.dPointType == 1 && (param.prevCoordinate.x == nullptr || param.prevCoordinate.y == nullptr)) {
        return "Previous Pattern Group No. not found. Relative Position unavailable.";
    }
    QString val = ui->dxTextField->text();
    if (val.isEmpty()) {
        val = "0";
    }
    param.dPoint.x = val;
    val = ui->dyTextField->text();
    if (val.isEmpty()) {
        val = "0";
    }
    param.dPoint.y = val;
    if (param.dPointType == 1) {
        param.dPoint.x += param.prevCoordinate.x;
        param.dPoint.y += param.prevCoordinate.y;
    }
    param.usePrevPoint = ui->diffCBox->isChecked();
    param.dPointType = ui->posItemBox->currentValue();
    return "";
}

//禁用一切与机台互动相关的操作
void IdpMpCoordinateSettingController::disablePyHandlers(bool b) {
    ui->moveBtn->getQPushButton()->setDisabled(b);
    ui->posRegBtn->getQPushButton()->setDisabled(b);
    ui->chipRegBtn->getQPushButton()->setDisabled(b);
}
