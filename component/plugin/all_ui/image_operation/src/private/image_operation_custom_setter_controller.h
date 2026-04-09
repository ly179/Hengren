#ifndef IMAGE_OPERATION_CUSTOM_SETTER_CONTROLLER_H
#define IMAGE_OPERATION_CUSTOM_SETTER_CONTROLLER_H
#include "cde90_dialog.h"

class QVBoxLayout;
class CDE90Label;
class CDE90Frame;
class QGridLayout;
class CDE90RadioButton;
class DatabaseComboBox;
class QHBoxLayout;
class CDE90ButtonBox;

class ImageOperationCustomSetterController : public CDE90Dialog{
private:
    QWidget* content;
    QVBoxLayout* contentLayout;

    QWidget* mainContent;
    QVBoxLayout* mainLayout;

    QWidget* cursorModeTitleContent;
    CDE90Label* cursorModeTitleLabel;

    CDE90Frame* cursorModeContent;
    QGridLayout* cursorModeLayout;
    CDE90RadioButton* advanceBtn;
    CDE90RadioButton* conventionalBtn;
    CDE90Label* pixelSizeLabel;
    CDE90Label* axLabel;
    CDE90Label* ayLabel;
    CDE90Label* cxLabel;
    CDE90Label* cyLabel;
    DatabaseComboBox* axBtn;
    DatabaseComboBox* ayBtn;
    DatabaseComboBox* cxBtn;
    DatabaseComboBox* cyBtn;

    CDE90Label* scanParamTitleLabel;
    QWidget* scanParamContent;
    QGridLayout* scanParamLayout;
    CDE90Label* scanTitleLabel;
    DatabaseComboBox* scanRateBtn;
    CDE90Label* frameTitleLabel;
    DatabaseComboBox* frameBtn;
    CDE90Label* filterTitleLabel;
    DatabaseComboBox* filterBtn;
    CDE90Label* flatTitleLabel;
    DatabaseComboBox* flatBtn;
    CDE90Label* dcTitleLabel;
    DatabaseComboBox* dcBtn;

    QVBoxLayout* operationLayout;
    QWidget* operationContent;
    CDE90Label* operationTitleLabel;
    QWidget* operationBtnContent;
    QHBoxLayout* operationBtnLayout;
    CDE90ButtonBox* defMagBtn;
    CDE90ButtonBox* roiSelectionBtn;
    CDE90ButtonBox* centerBtn;
    CDE90ButtonBox* scanBtn;

    QWidget* bottomContent;
    QHBoxLayout* bottomLayout;
    CDE90ButtonBox* closeBtn;

    void create();
    void createAction();

    void init();

public:
    ImageOperationCustomSetterController(QWidget* parent = nullptr);
    ~ImageOperationCustomSetterController() noexcept;
    void refreshView();
};

#endif // IMAGE_OPERATION_CUSTOM_SETTER_CONTROLLER_H
