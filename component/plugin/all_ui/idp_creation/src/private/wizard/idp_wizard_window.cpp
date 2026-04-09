#include "idp_wizard_window.h"
#include "ui_wizard_window.h"
#include "view_data_mode.h"
#include "cde90_message_box.h"
#include "wizard_component.h"
#include "wizard_list_data.h"
#include <QVBoxLayout>
#include <QResizeEvent>
#include "image_file_temp_handler.h"

class StepTableColumnResizeHandler : public QObject {
public:
    explicit StepTableColumnResizeHandler(QObject* parent = nullptr) : QObject(parent) {}
    ~StepTableColumnResizeHandler() override {}

    bool eventFilter(QObject *watched, QEvent *event) override {
        if (watched != parent()) return QObject::eventFilter(watched, event);
        if (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease) {
            return true;
        }
        if (event->type() != QEvent::Resize) return QObject::eventFilter(watched, event);
        QResizeEvent* resizeEvent = static_cast<QResizeEvent*>(event);
        QTableView* table = qobject_cast<QTableView*>(watched);
        if (table) {
            QHeaderView* hHeader = table->horizontalHeader();
            QSize size = resizeEvent->size();
            hHeader->resizeSection(1, size.width() - hHeader->sectionSize(0) - 4);
        }
        return QObject::eventFilter(watched, event);
    }
};

class StepTableModel : public QAbstractTableModel {
    QList<WizardStepData> list;
    int oldSize = 0;
private:
    int rowCount(
        const QModelIndex& parent = QModelIndex()) const override {
        Q_UNUSED(parent)
        return list.size();
    }

    QVariant headerData(
        int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override {
        Q_UNUSED(section)
        Q_UNUSED(role)
        return QVariant();
    }
    virtual QVariant data(
        const QModelIndex& index, int role = Qt::DisplayRole) const override {
        if (index.isValid() && (role == Qt::DisplayRole || role == Qt::EditRole)) {
            int row = index.row();
            int column = index.column();
            if (row >= 0 && row < list.size() && column >= 0 && column < columnCount()) {
                const WizardStepData& data = list[row];
                if (column == 0) return data.finished ? "@" : "";
                return data.text;
            }
        }
        return QVariant();
    }

    bool setData(
        const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override {
        Q_UNUSED(index)
        Q_UNUSED(value)
        Q_UNUSED(role)
        return false;
    }

    int columnCount(
        const QModelIndex& parent = QModelIndex()) const override {
        Q_UNUSED(parent)
        return 2;
    }

public:
    explicit StepTableModel(QObject* parent = nullptr) : QAbstractTableModel(parent) {}
    ~StepTableModel() override {}

    inline QList<WizardStepData>& getList() {return list;}

    //全部在UI线程操作。
    //append 增量更新或尾部移除更新
    void notifyUpdated() {
        if (oldSize < list.size()) {
            beginInsertRows(QModelIndex(), oldSize, list.size() - 1);
            endInsertRows();
        } else if (oldSize > list.size()) {
            beginRemoveRows(QModelIndex(), list.size(), oldSize - 1);
            endRemoveRows();
        } else {
            emit dataChanged(createIndex(0, 0), createIndex(oldSize - 1, columnCount() - 1));
        }
        if (oldSize != list.size())
            oldSize = list.size();
    }

    //insert或中间remove移除更新
    void notifyUpdated(int index, int size) {
        if (oldSize < list.size()) {
            beginInsertRows(QModelIndex(), index, index + size - 1);
            endInsertRows();
        } else if (oldSize > list.size()) {
            beginRemoveRows(QModelIndex(), index, index + size - 1);
            endRemoveRows();
        }
        if (oldSize != list.size())
            oldSize = list.size();
    }

    //list不变但list某元素内容变化更新
    void notifyUpdated(int row) {
        emit dataChanged(createIndex(row, 0), createIndex(row, columnCount() - 1));
    }
};

IdpWizardWindow::IdpWizardWindow(QWidget* parent) : CDE90Window(parent) {
    connect(this, &CDE90Window::asyncCallerChanged, this, [this]() {
        create();
        createAction();
    });
}

IdpWizardWindow::~IdpWizardWindow() noexcept {
    if (ui) {
        delete ui;
    }
    cdsem::removeGroup(this->group);
}

QList<WizardStepData>& IdpWizardWindow::getList() {
    return tableModel->getList();
}
//全部在UI线程操作。
//append 增量更新或尾部移除更新
void IdpWizardWindow::notifyUpdated() {
    tableModel->notifyUpdated();
}

//insert或中间remove移除更新
void IdpWizardWindow::notifyUpdated(int index, int size) {
    tableModel->notifyUpdated(index, size);
}

//list不变但list某元素内容变化更新
void IdpWizardWindow::notifyUpdated(int row) {
    tableModel->notifyUpdated(row);
}

void IdpWizardWindow::select(int index) {
    ui->stepTable->selectRow(index);
}

QString IdpWizardWindow::genGroupPath() const {
    return cdsem::genCommonPath(this->group);
}

void IdpWizardWindow::finishedWizard() {
    disableClose(false);
    changeCloseStatusOnce(HW_OK);
    this->close();
}

void IdpWizardWindow::create() {
    QWidget* content = new QWidget;
    setContent(content);
    ui = new cdsem::UiWizardWindow;
    ui->init(content);


    QHeaderView* headerView = ui->stepTable->verticalHeader();
    headerView->setMinimumSectionSize(0);
    headerView->setMaximumSectionSize(20);
    headerView->setDefaultSectionSize(20);
    headerView->setSectionResizeMode(QHeaderView::Fixed);
    headerView = ui->stepTable->horizontalHeader();
    headerView->setMinimumSectionSize(0);
    headerView->setDefaultSectionSize(20);
    headerView->setSectionResizeMode(QHeaderView::Interactive);
}

void IdpWizardWindow::createAction() {
    disableClose();
    tableModel = new StepTableModel(ui->stepTable);
    ui->stepTable->replaceDataModel(tableModel);
    ui->stepTable->horizontalHeader()->resizeSection(0, 20);
    ui->stepTable->installEventFilter(new StepTableColumnResizeHandler(ui->stepTable));
    ui->stepTable->horizontalHeader()->setVisible(false);
    ui->stepTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->stepTable->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->stepTable->setAttribute(Qt::WA_ForceDisabled);
    connect(ui->cancelBtn->getQPushButton(), &QPushButton::clicked, this, [this]() {
        int r = showYesNoBox(this, "Confirm", "Quit Wizard ?");
        if (r) return;
        disableClose(false);
        close();
    });
    connect(ui->nextBtn->getQPushButton(), &QPushButton::clicked, this, &IdpWizardWindow::toNext);
    connect(ui->backBtn->getQPushButton(), &QPushButton::clicked, this, &IdpWizardWindow::toPrev);
    connect(ui->skipBtn->getQPushButton(), &QPushButton::clicked, this, &IdpWizardWindow::toSkip);
}

bool IdpWizardWindow::setComponent(WizardComponent* w) {
    if (component) {
        return false;
    }
    w->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    component = w;
    w->setParent(ui->contentContext);
    itsChildAndBeManaged(w, this);
    w->setWizardWindow(this);

    QVBoxLayout* vLayout = dynamic_cast<QVBoxLayout*>(ui->contentContext->layout());
    if (vLayout) {
        vLayout->addWidget(component);
        vLayout->setStretchFactor(w, 1);
    }

    getList().clear();
    notifyUpdated();

    connect(this, &IdpWizardWindow::toNext, w, &WizardComponent::toNext);
    connect(this, &IdpWizardWindow::toPrev, w, &WizardComponent::toPrev);
    connect(this, &IdpWizardWindow::toSkip, w, &WizardComponent::toSkip);
    connect(w, &WizardComponent::titleChanged, this, &IdpWizardWindow::setNote);
    connect(w, &WizardComponent::prevStatusChanged, this, &IdpWizardWindow::prevBtnEnable);
    connect(w, &WizardComponent::nextStatusChanged, this, &IdpWizardWindow::nextBtnEnable);
    connect(w, &WizardComponent::skipStatusChanged, this, &IdpWizardWindow::skipBtnStatus);
    connect(w, &WizardComponent::lastStepActivated, this, &IdpWizardWindow::setLastStepStatus);

    w->initSteps();
    return true;
}

void IdpWizardWindow::setGroup(const QString& group) {
    this->group = group;
    cdsem::removeGroup(this->group);
    cdsem::insertGroup(group);
}

void IdpWizardWindow::resetStep() {
    if (component) component->initSteps();
}

void IdpWizardWindow::nextBtnEnable(bool b) {
    ui->nextBtn->setEnabled(b);
}

void IdpWizardWindow::prevBtnEnable(bool b) {
    ui->backBtn->setEnabled(b);
}

void IdpWizardWindow::skipBtnStatus(char c) {
    if (c == 0) ui->skipBtn->setVisible(false);
    else if (c == 1) {
        ui->skipBtn->setVisible(true);
        ui->skipBtn->setEnabled(true);
    } else if (c == 2) {
        ui->skipBtn->setVisible(true);
        ui->skipBtn->setEnabled(false);
    }
}

void IdpWizardWindow::setNote(const QString& text) {
    ui->noteLabel->setText(text);
}

void IdpWizardWindow::setLastStepStatus(bool b) {
    ui->nextBtn->getQPushButton()->setText(b ? "OK" : "Next>>");
}