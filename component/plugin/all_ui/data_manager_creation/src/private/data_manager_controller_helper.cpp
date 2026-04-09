#include "data_manager_controller_helper.h"

#include <QGridLayout>
#include <QSpacerItem>
#include "cde90_label.h"
#include "cde90_menu.h"
#include "cde90_menu_bar.h"
#include "cde90_message_box.h"
#include "cde90_spliter_line.h"

#include "cde90_button.h"
#include "cde90_list_view.h"
#include "cde90_text_field.h"
#include "private/cdsem_class_editor.h"
#include "cdsem_device_selector.h"
#include "list_view_main_data_model.h"

void DataManagerControllerHelper::create() {
    content = new QWidget;
    contentLayout = new QVBoxLayout(content);
    contentLayout->setContentsMargins(0,0,0,0);
    contentLayout->setSpacing(0);
    content->setLayout(contentLayout);

    menuBarContent = new QWidget(content);
    menuBarLayout = new QHBoxLayout(menuBarContent);
    menuBarLayout->setContentsMargins(0,0,0,0);
    menuBarLayout->setSpacing(0);
    menuBarContent->setLayout(menuBarLayout);
    contentLayout->addWidget(menuBarContent);

    menuBar = new CDE90MenuBar(content);
    menuBar->setStyleSheet("QMenuBar {border-right: 2px solid transparent;}");
    menuBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    fileMenu = new CDE90Menu("File", menuBar);
    deviceMenu = new CDE90Menu("Device", menuBar);
    showMenu = new CDE90Menu("Show", menuBar);
    menuBar->addMenu(fileMenu);
    menuBar->addMenu(deviceMenu);
    menuBar->addMenu(showMenu);

    helpMenuBar = new CDE90MenuBar(menuBarContent);
    helpMenuBar->setStyleSheet("QMenuBar {border-left: 2px solid transparent;}");
    helpMenuBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    helpeMenu = new CDE90Menu("Help", helpMenuBar);
    helpMenuBar->addMenu(helpeMenu);

    menuBarLayout->addWidget(menuBar);
    // menuBarLayout->addItem(new QSpacerItem(10,10, QSizePolicy::Expanding, QSizePolicy::Preferred));
    menuBarLayout->addWidget(helpMenuBar);

    centerWidget = new QWidget(content);
    centerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    centerLayout = new QVBoxLayout(centerWidget);
    centerLayout->setContentsMargins(0,0,0,0);
    centerLayout->setSpacing(0);
    centerLayout->setAlignment(Qt::AlignTop);
    centerWidget->setLayout(centerLayout);
    contentLayout->addWidget(centerWidget);

    deviceContent = new QWidget(centerWidget);
    deviceContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    deviceLayout = new QHBoxLayout(deviceContent);
    deviceLayout->setContentsMargins(5,5,5,5);
    deviceLayout->setSpacing(0);
    deviceContent->setLayout(deviceLayout);
    centerLayout->addWidget(deviceContent);

    btn_device = new CDE90ButtonBox("Device", deviceContent);
    QObject::connect(btn_device->getQPushButton(), &CDE90Button::clicked, btn_device->getQPushButton(), [this](){openDeviceSelector();});
    textfield_device = new CDE90TextFieldBox(deviceContent);
    textfield_device->getQLineEdit()->setDisabled(true);
    freeMemLabel = new CDE90Label(deviceContent);
    deviceLayout->addWidget(btn_device);
    deviceLayout->addWidget(textfield_device);
    deviceLayout->addItem(new QSpacerItem(0,10, QSizePolicy::Expanding, QSizePolicy::Preferred));
    deviceLayout->addWidget(freeMemLabel);

    detailContent = new QWidget(centerWidget);
    detailContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    detailLayout = new QGridLayout(detailContent);
    detailLayout->setSpacing(0);
    detailLayout->setContentsMargins(0,0,0,0);
    detailContent->setLayout(detailLayout);
    centerLayout->addWidget(detailContent);

    createGrid(0,0);
    createGrid(1,1);
    createGrid(2,2);
    createGrid(3,4);
    QWidget* emptyWidget = new QWidget(detailContent);
    emptyWidget->setFixedSize(20,10);
    detailLayout->addWidget(emptyWidget, 0, 3);
    detailLayout->addWidget(new CDE90SpliterHLine(detailContent), 3, 0, 1, 5);
    //Test Data
    freeMemLabel->setText("1111MB free");
    textfield_device->getQLineEdit()->setText("H/D");
}

void DataManagerControllerHelper::createGrid(qint32 index, qint32 column) {

    DetailWidget& widgets = details[index];

    widgets.widget_action = new QWidget(detailContent);
    widgets.widget_action->setMinimumWidth(180);
    widgets.layout_action = new QHBoxLayout(widgets.widget_action);
    widgets.layout_action->setSpacing(0);
    widgets.layout_action->setContentsMargins(5,5,5,5);
    widgets.widget_action->setLayout(widgets.layout_action);
    detailLayout->addWidget(widgets.widget_action, 0, column);

    widgets.label_action = new CDE90Label(widgets.widget_action);
    widgets.btn_action = new CDE90ButtonBox("Action", widgets.widget_action);
    widgets.btn_action->getQPushButton()->setStyleSheet("QPushButton::menu-indicator {image: none; width: 0px; height: 0px;}");
    widgets.layout_action->addWidget(widgets.label_action);
    widgets.layout_action->addItem(new QSpacerItem(0,10, QSizePolicy::Expanding, QSizePolicy::Preferred));
    widgets.layout_action->addWidget(widgets.btn_action);

    widgets.widget_listView = new QWidget(detailContent);
    widgets.widget_listView->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    widgets.layout_listView = new QHBoxLayout(widgets.widget_listView);
    widgets.layout_listView->setSpacing(0);
    widgets.layout_listView->setContentsMargins(5,5,5,5);
    widgets.widget_listView->setLayout(widgets.layout_listView);
    detailLayout->addWidget(widgets.widget_listView, 1, column);
    widgets.listView = new CDE90ListView(new ListViewMainModel, widgets.widget_listView);
    widgets.layout_listView->addWidget(widgets.listView);

    widgets.widget_item = new QWidget(detailContent);
    widgets.layout_item = new QHBoxLayout(widgets.widget_item);
    widgets.layout_item->setSpacing(0);
    widgets.layout_item->setContentsMargins(5,5,5,5);
    widgets.widget_item->setLayout(widgets.layout_item);
    detailLayout->addWidget(widgets.widget_item, 2, column);
    widgets.label_item = new CDE90Label(widgets.widget_item);
    widgets.textfield_item = new CDE90TextFieldBox(widgets.widget_item);
    widgets.textfield_item->getQLineEdit()->setDisabled(true);
    widgets.textfield_item->setFixedWidth(110);
    widgets.textfield_item->getQLineEdit()->setAlignment(Qt::AlignRight);
    widgets.layout_item->addWidget(widgets.label_item);
    widgets.layout_item->addItem(new QSpacerItem(0,10, QSizePolicy::Expanding, QSizePolicy::Preferred));
    widgets.layout_item->addWidget(widgets.textfield_item);

    widgets.widget_selected = new QWidget(detailContent);
    widgets.layout_selected = new QVBoxLayout(widgets.widget_selected);
    widgets.layout_selected->setSpacing(0);
    widgets.layout_selected->setContentsMargins(5,5,5,5);
    widgets.layout_selected->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    widgets.widget_selected->setLayout(widgets.layout_selected);
    detailLayout->addWidget(widgets.widget_selected, 4, column);

    widgets.label_selected = new CDE90Label(widgets.widget_selected);
    widgets.textfield_selected = new CDE90TextFieldBox(widgets.widget_selected);
    widgets.textfield_selected->getQLineEdit()->setDisabled(true);
    widgets.textfield_selected->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    widgets.layout_selected->addWidget(widgets.label_selected);
    widgets.layout_selected->addWidget(widgets.textfield_selected);

}

ListViewMainData* DataManagerControllerHelper::getSelectedData(int index) {
    QModelIndexList list = details[index].listView->selectionModel()->selectedRows();
    if (list.size() == 1) {
        return static_cast<ListViewMainData*>(list[0].internalPointer());
    }
    return nullptr;
}

bool DataManagerControllerHelper::checkIsOverTheItemCountAndNotify(
    int index, int max, const QString& msg) {
    if (dynamic_cast<ListViewMainModel*>(details[index].listView->model())->getDataList().size()
        >= max) {
        simpleMessageBox(
            content,
            "Error",
            msg.isEmpty() ? "The maximum number of items that can be created is 5000. You are not "
                            "allowed to create more items."
                          : msg);
        return true;
    }
    return false;
}

void DataManagerControllerHelper::openDeviceSelector() {
    if (!deviceSelector) {
        deviceSelector = new CDSemDeviceSelector(content);
        deviceSelector->setWindowModality(Qt::WindowModal);
        deviceSelector->setWindowTitle("Device Selector");
        deviceSelector->setResizable(false);
        deviceSelector->setTitle("Device");
    }
    deviceSelector->show();
    MainThreadInvoker::getInvoker().moveToScreenCenter(deviceSelector);
}
