#include "log_content_view_controller.h"

#include <cde90_menu.h>
#include <cde90_message_box.h>
#include <qcompleter.h>

#include "ui_log_content_view_controller.h"
#include "highlight_log_textarea.h"
#include "log_flusher.h"
#include <QGridLayout>
#include <QLineEdit>

struct LogContentViewDataHelper {
    HighlightLogTextarea* textarea;
    bool searchMode = false;
    bool backwardMode = false;
};

LogContentViewController::LogContentViewController(QWidget* parent) : QWidget(parent), dataHelper(new LogContentViewDataHelper) {
    create();
    createAction();
}

LogContentViewController::~LogContentViewController() noexcept {
    if (ui) {
        delete ui;
    }
}

void LogContentViewController::create() {
    ui = new cdsem::UiLogContentViewController;
    ui->init(this);
}

void LogContentViewController::createAction() {
    installEventFilter(this);
    connect(ui->filterBtn->getQPushButton(), &QPushButton::clicked, this, [this]() {
        if (dataHelper->searchMode)
            emit doSearch(ui->filterBox->getQComboBox()->lineEdit()->text(), ui->enableMatchCase->isChecked(), ui->enableRegCBox->isChecked());
        else
            emit doFilter(ui->filterBox->getQComboBox()->lineEdit()->text(), ui->enableMatchCase->isChecked(), ui->enableRegCBox->isChecked());
    });
    connect(ui->filterBox->getQComboBox()->lineEdit(), &QLineEdit::returnPressed, this, [this]() {
        ui->filterBtn->getQPushButton()->click();
    });
    ui->filterBox->getQComboBox()->completer()->setCaseSensitivity(Qt::CaseSensitive);

    // CDE90Menu* searchMenu = new CDE90Menu(ui->filterBtn->getQPushButton());
    QAction* mAction = ui->filterBtn->getQPushButton()->addAction("Search");
    connect(mAction, &QAction::triggered, this, [this]() {
        ui->filterBtn->getQPushButton()->setText("Search");
        dataHelper->searchMode = true;
    });
    mAction = ui->filterBtn->getQPushButton()->addAction("Filter");
    connect(mAction, &QAction::triggered, this, [this]() {
        ui->filterBtn->getQPushButton()->setText("Filter");
        dataHelper->searchMode = false;
    });
    ui->filterBtn->getQPushButton()->setContextMenuPolicy(Qt::ActionsContextMenu);
}

bool LogContentViewController::eventFilter(QObject* object, QEvent* event) {
    if (object != this) return QWidget::eventFilter(object, event);
    if (event->type() != QEvent::KeyPress && event->type() != QEvent::KeyRelease)
        return QWidget::eventFilter(object, event);
    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
    if (keyEvent->key() != Qt::Key_Shift)
        return QWidget::eventFilter(object, event);
    if (event->type() == QEvent::KeyPress) {
        dataHelper->backwardMode = true;
    } else {
        dataHelper->backwardMode = false;
    }
    return true;
}

void LogContentViewController::setLogView(HighlightLogTextarea* area) {
    QGridLayout* layout = static_cast<QGridLayout *>(this->layout());
    // textarea->setWordWrapMode(QTextOption::NoWrap);
    // textarea->setReadOnly(true);
    area->setParent(this);
    layout->addWidget(area, 1, 0, 1, 3);
    dataHelper->textarea = area;
}

void LogContentViewController::pushFilterHistory(const QString& text, int i) {
    if (i > 0) {
        ui->filterBox->getQComboBox()->removeItem(i);
        ui->filterBox->getQComboBox()->insertItem(0, text);
        ui->filterBox->getQComboBox()->setCurrentIndex(0);
    } else if (i < 0) {
        QComboBox* combo = ui->filterBox->getQComboBox();
        int count = combo->count();
        for (int i = 0; i < count; ++i) {
            if (combo->itemText(i) == text) {
                ui->filterBox->getQComboBox()->removeItem(i);
                ui->filterBox->getQComboBox()->insertItem(0, text);
                ui->filterBox->getQComboBox()->setCurrentIndex(0);
                return;
            }
        }
        while (combo->count() >= 20) {
            combo->removeItem(combo->count() - 1);
        }
        ui->filterBox->getQComboBox()->insertItem(0, text);
        ui->filterBox->getQComboBox()->setCurrentIndex(0);
    }
}

void LogContentViewController::searchText(const QString& text, bool enableCase, bool enableRegular) {
    QTextCursor cursor = dataHelper->textarea->textCursor();
    if (text.isEmpty()) {
        cursor.clearSelection();
        return;
    }
    int flags = 0;
    if (enableCase)
        flags |= QTextDocument::FindCaseSensitively;
    if (dataHelper->backwardMode)
        flags |= QTextDocument::FindBackward;
    if (enableRegular) {
        QRegularExpression reg(text);
        if (!reg.isValid()) {
            simpleMessageBox(this, "Error", "Regular is invalid");
            return;
        }
        cursor = dataHelper->textarea->document()->find(QRegularExpression(text), cursor, QTextDocument::FindFlag(flags));
    } else {
        cursor = dataHelper->textarea->document()->find(text, cursor, QTextDocument::FindFlag(flags));
    }
    if (cursor.isNull()) {
        cursor.clearSelection();
        return;
    }
    dataHelper->textarea->setTextCursor(cursor);
}