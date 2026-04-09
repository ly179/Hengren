#include "main_controller_helper.h"
#include "main_controller.h"
#include "cde90_message_box.h"
#include "sem_global.h"
#include "signal_hub_single_object_manager.h"
#include "relation_worksheet_controller.h"
#include "worksheet_relation.h"
#include "data_manager_relation.h"
#include "ui_control_pane_controller.h"
#include "amp_editor_relation.h"
#include "image_operation_relation.h"
#include <QPointer>
#include <QStyle>
#include "../../image_operation/includes/image_operation_controller.h"
struct MSR_Chip;

MainControllerHelper::MainControllerHelper(QObject* parent) : QObject(parent) {

}

MainControllerHelper::~MainControllerHelper() noexcept {
}

void MainControllerHelper::setMainWidget(MainController* parent) {
    this->parent = parent;
    parent->ui->btn_worksheet->getQPushButton()->setEnabled(worksheet::worksheetAvaliable());
    parent->ui->btn_fileManager->getQPushButton()->setEnabled(data_manager::fileManagerAvailable());
    parent->ui->btn_image->getQPushButton()->setEnabled(data_manager::imageManagerAvailable());
    parent->ui->btn_templateManager->getQPushButton()->setEnabled(data_manager::templateManagerAvailable());
    parent->ui->btn_msr->getQPushButton()->setEnabled(data_manager::msrManagerAvailable());
    parent->ui->btn_amp->getQPushButton()->setEnabled(amp_editor::ampEditorAlivable());
    connect(SignalHubSingleObjectManager::instance()->getSignalObject(), &SignalHubObjectSMObject::objectChanged, this, [this](const QString& id, bool opened){
        QPushButton* btn = nullptr;
        if (worksheet::isSingleWorksheetController(id)) {
            btn = this->parent->ui->btn_worksheet->getQPushButton();
        } else if (data_manager::isSingleFileManager(id)) {
            btn = this->parent->ui->btn_fileManager->getQPushButton();
        } else if (data_manager::isSingleImageManager(id)) {
            btn = this->parent->ui->btn_image->getQPushButton();
        } else if (data_manager::isSingleTemplateManager(id)) {
            btn = this->parent->ui->btn_templateManager->getQPushButton();
        } else if (data_manager::isSingleMSRManager(id)) {
            btn = this->parent->ui->btn_msr->getQPushButton();
        } else if (amp_editor::isSingleAMP(id)) {
            btn = this->parent->ui->btn_amp->getQPushButton();
        }
        if (!btn) return;
        if (opened) addCssClass(*btn, "GreenBg");
        else removeCssClass(*btn, "GreenBg");
        btn->style()->unpolish(btn);
        btn->style()->polish(btn);
    }, Qt::QueuedConnection);

}

void MainControllerHelper::openFileController() {
    MsgReceiveStatus<RelationFileManager*> status = data_manager::openFileManagerController();
    if (!status.ok) {
        simpleMessageBox(parent, "Error", "Error to open File Manager Window.");
    }
}

void MainControllerHelper::openTemplateController() {
    MsgReceiveStatus<RelationTemplateManager*> status = data_manager::openTemplateManagerController();
    if (!status.ok) {
        simpleMessageBox(parent, "Error", "Error to open Template Manager Window.");
    }
}

void MainControllerHelper::openImageOperationController() {
    if (parent->getImageOpWidget()) {
        parent->getImageOpWidget()->setVisible(true);
        parent->getImageOpWidget()->raise();
        parent->getImageOpWidget()->reloadImageFromGlobal();
    }
}

void MainControllerHelper::openWorkSheetController() {
    MsgReceiveStatus<RelationWorksheetController*> status = worksheet::openSingleWorksheetController();
    if (!status.ok) {
        simpleMessageBox(parent, "Error", "Error to open Worksheet.");
    }
}

void MainControllerHelper::openAmpController() {
    MsgReceiveStatus<RelatoinAmpEditController*> status = amp_editor::openSingleAMPEditor();
    if (!status.ok) {
        simpleMessageBox(parent, "Error", "Error to open AMP window.");
    }
}


void MainControllerHelper::openMSRController() {
    MsgReceiveStatus<RelationMSRManager*> status = data_manager::openMSRManagerController();
    if (!status.ok) {
        simpleMessageBox(parent, "Error", "Error to open MSR Manager Window.");
    }
}

void MainControllerHelper::openImageController() {
    MsgReceiveStatus<RelationImageManager*> status = data_manager::openImageManagerController();
    if (!status.ok) {
        simpleMessageBox(parent, "Error", "Error to open Image Manager Window.");
    }
}