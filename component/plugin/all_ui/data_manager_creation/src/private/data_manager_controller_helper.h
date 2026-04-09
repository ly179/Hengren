#ifndef DATA_MANAGER_CONTROLLER_HELPER_H
#define DATA_MANAGER_CONTROLLER_HELPER_H
#include <QObject>

class QHBoxLayout;
class CDE90Label;
class CDE90ButtonBox;
class CDE90ListView;
class CDE90TextFieldBox;
class QVBoxLayout;
class CDE90MenuBar;
class CDE90Menu;
class QGridLayout;
class CDSemDeviceSelector;
class CDSemClassEditor;
class ListViewMainData;
class QWidget;

struct DetailWidget {
    QWidget* widget_action;
    QHBoxLayout* layout_action;
    CDE90Label* label_action;
    CDE90ButtonBox* btn_action;
    QWidget* widget_listView;
    QHBoxLayout* layout_listView;
    CDE90ListView* listView;
    QWidget* widget_item;
    QHBoxLayout* layout_item;
    CDE90Label* label_item;
    CDE90TextFieldBox* textfield_item;
    QWidget* widget_selected;
    QVBoxLayout* layout_selected;
    CDE90Label* label_selected;
    CDE90TextFieldBox* textfield_selected;
};

struct DataManagerControllerHelper {

    QVBoxLayout* contentLayout;
    QWidget* content;

    QHBoxLayout* menuBarLayout;
    QWidget* menuBarContent;
    CDE90MenuBar* menuBar;
    CDE90Menu* fileMenu;
    CDE90Menu* deviceMenu;
    CDE90Menu* showMenu;

    CDE90MenuBar* helpMenuBar;
    CDE90Menu* helpeMenu;

    QVBoxLayout* centerLayout;
    QWidget* centerWidget;

    QHBoxLayout* deviceLayout;
    QWidget* deviceContent;
    CDE90ButtonBox* btn_device;
    CDE90TextFieldBox* textfield_device;
    CDE90Label* freeMemLabel;

    QWidget* detailContent;
    QGridLayout* detailLayout;

    CDSemDeviceSelector* deviceSelector = nullptr;
    CDSemClassEditor* classEditor = nullptr;

    DetailWidget details[4];
    void create();

    void createGrid(qint32 index, qint32 column);
    ListViewMainData* getSelectedData(int index);

    bool checkIsOverTheItemCountAndNotify(int index, int max, const QString& msg = "");
    void openDeviceSelector();
};

#endif // DATA_MANAGER_CONTROLLER_HELPER_H
