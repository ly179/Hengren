#include "cdsem_class_editor.h"

#include <QHBoxLayout>
#include "cde90_label.h"
#include <QSpacerItem>
#include <QVBoxLayout>
#include "cde90_button.h"
#include "cde90_message_box.h"
#include "cde90_spliter_line.h"
#include "cde90_text_field.h"
#include "cdsem_textfield_string_validator.h"
#include "class_service.h"
#include "sem_global.h"
#include "service_manager.h"

CDSemClassEditor::CDSemClassEditor(QWidget* parent): CDE90Dialog(parent) {
    create();
}
CDSemClassEditor::~CDSemClassEditor() noexcept {
}

void CDSemClassEditor::save() {
    QString className = textField->getQLineEdit()->text();
    if (className.isEmpty()) {
        simpleMessageBox(this, "Message", "Please input class name then click \"OK\" button to create Class.");
        return;
    }
    quint64 dataId = this->dataId;
    disableClose();
    getAsyncCaller()->push(this, [this, className, dataId](const TaskStopCmd&) mutable{
        MainThreadInvoker::getInvoker().blockRun([this](){
            loadingStatus(true);
        });
        bool couldClose = false;
        sem::AfterGc gc([this, &couldClose](){
            MainThreadInvoker::getInvoker().blockRun([this, couldClose](){
                loadingStatus(false);
                disableClose(false);
                if (couldClose) close();
            });
        });
        ClassService* service = ServiceManager::instance()->getDefaultService<ClassService>();
        qint32 errCode;
        quint64 id = service->getByName(className, &errCode);
        if (errCode) {
            simpleMessageBox(this, "Error", "Error to check class name.");
            return;
        }
        if ((dataId == 0 && id) || (dataId > 0 && id > 0 && dataId != id)) {
            simpleMessageBox(this, "Error", "Class name already exists.");
            return;
        }
        //没有变化不做操作
        if (dataId > 0 && dataId == id) {
            couldClose = true;
            return;
        }
        ClassEntity entity;
        entity.setClassName(className);
        if (dataId > 0) {
            entity.setId(dataId);
            service->update(entity, &errCode);
        } else {
            errCode = service->create(entity);
            dataId = entity.getId();
        }
        if (errCode) {
            simpleMessageBox(this, "Error Occern", dataId > 0 ? "Update Class Error." : "Create Class Error.");
            return;
        }
        couldClose = true;
        MainThreadInvoker::getInvoker().runLater([this, dataId](){
            if (this->okCallback) {
                this->okCallback(dataId);
            }
        });
    });
}

void CDSemClassEditor::create() {
    content = new QWidget;
    contentLayout = new QVBoxLayout(content);
    contentLayout->setContentsMargins(0,0,0,0);
    contentLayout->setSpacing(0);
    content->setLayout(contentLayout);

    centerContent = new QWidget(content);
    centerContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    centerLayout = new QVBoxLayout(centerContent);
    centerLayout->setContentsMargins(10,10,10,10);
    centerLayout->setSpacing(5);
    centerLayout->setAlignment(Qt::AlignCenter);
    centerContent->setLayout(centerLayout);
    contentLayout->addWidget(centerContent);

    titleLabel = new CDE90Label("Create Class", centerContent);
    textField = new CDE90TextFieldBox(centerContent);
    textField->getQLineEdit()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    centerLayout->addWidget(titleLabel);
    centerLayout->addItem(new QSpacerItem(10,10, QSizePolicy::Preferred, QSizePolicy::Expanding));
    centerLayout->addWidget(textField);
    centerLayout->addItem(new QSpacerItem(10,10, QSizePolicy::Preferred, QSizePolicy::Expanding));

    contentLayout->addWidget(new CDE90SpliterHLine(content));


    bottomContent = new QWidget(content);
    bottomContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    bottomLayout = new QHBoxLayout(bottomContent);
    bottomLayout->setContentsMargins(5,5,5,5);
    bottomLayout->setSpacing(5);
    bottomContent->setLayout(bottomLayout);
    contentLayout->addWidget(bottomContent);

    okBtn = new CDE90ButtonBox("OK", bottomContent);
    okBtn->mark(true);
    cancelBtn = new CDE90ButtonBox("Cancel", bottomContent);
    bottomLayout->addWidget(okBtn);
    bottomLayout->addItem(new QSpacerItem(10,10, QSizePolicy::Expanding, QSizePolicy::Preferred));
    bottomLayout->addWidget(cancelBtn);
    setContent(content);

    textField->getQLineEdit()->setFocus();
    connect(okBtn->getQPushButton(), &CDE90Button::clicked, this, [this](){
        save();
    });

    connect(cancelBtn->getQPushButton(), &CDE90Button::clicked, this, [this](){
        close();
    });

    CDSemTextFieldStringValidator* validator = new CDSemTextFieldStringValidator(
        textField->getQLineEdit());
    validator->setLow(0);
    validator->setHigh(FILE_NAME_LENGTH);
    textField->getQLineEdit()->setValidator(validator);
}

void CDSemClassEditor::setOkCallback(const CDE90ClassOKCallback& c) {
    this->okCallback = c;
}

void CDSemClassEditor::loadingStatus(bool s) {
    if (s) {
        okBtn->loading();
        cancelBtn->getQPushButton()->setDisabled(true);
        textField->getQLineEdit()->setDisabled(true);
    } else {
        okBtn->overLoading();
        cancelBtn->getQPushButton()->setDisabled(false);
        textField->getQLineEdit()->setDisabled(false);
    }
}

void CDSemClassEditor::setInitData(quint64 id, const QString& str) {
    textField->getQLineEdit()->setText(str);
    textField->getQLineEdit()->selectAll();
    this->dataId = id;
    if (id > 0) {
        titleLabel->setText("Edit Class");
    } else {
        titleLabel->setText("Create Class");
    }
}
