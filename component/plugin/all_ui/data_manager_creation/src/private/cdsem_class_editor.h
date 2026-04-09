#ifndef CDSEM_CLASS_EDITOR_H
#define CDSEM_CLASS_EDITOR_H
#include "cde90_dialog.h"

class QVBoxLayout;
class QHBoxLayout;
class CDE90Label;
class CDE90TextFieldBox;
class CDE90ButtonBox;

using CDE90ClassOKCallback = sem::ONE_PARAM_ARG_FUNC<quint64>;
class CDSemClassEditor : public CDE90Dialog {
    Q_OBJECT
private:

    CDE90ClassOKCallback okCallback;

    QVBoxLayout* contentLayout;
    QWidget* content;

    QVBoxLayout* centerLayout;
    QWidget* centerContent;
    CDE90Label* titleLabel;
    CDE90TextFieldBox* textField;

    QHBoxLayout* bottomLayout;
    QWidget* bottomContent;

    CDE90ButtonBox* okBtn;
    CDE90ButtonBox* cancelBtn;

    quint64 dataId = 0;

    void save();
    void create();
public:
    explicit CDSemClassEditor(QWidget* parent = nullptr);
    virtual ~CDSemClassEditor() noexcept;

    void setOkCallback(const CDE90ClassOKCallback&);
    void loadingStatus(bool);

    void setInitData(quint64 id, const QString&);

};

#endif // CDSEM_CLASS_EDITOR_H
