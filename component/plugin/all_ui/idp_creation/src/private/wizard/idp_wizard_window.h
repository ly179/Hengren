#ifndef HENGREN_WIZARD_WINDOW_H
#define HENGREN_WIZARD_WINDOW_H
#include "cde90_window.h"
#include "ui_idp_creation_global.h"

namespace cdsem {
class UiWizardWindow;
}

class WizardComponent;
class StepTableModel;
struct WizardStepData;
class UI_IDP_CREATION_EXPORT IdpWizardWindow : public CDE90Window{
    friend class WizardComponent;
    Q_OBJECT
private:
    QString group;
    cdsem::UiWizardWindow* ui = nullptr;
    StepTableModel* tableModel = nullptr;
    WizardComponent* component = nullptr;

    QList<WizardStepData>& getList();
    //全部在UI线程操作。
    //append 增量更新或尾部移除更新
    void notifyUpdated();

    //insert或中间remove移除更新
    void notifyUpdated(int index, int size);

    //list不变但list某元素内容变化更新
    void notifyUpdated(int row);

    void select(int);

    QString genGroupPath() const;

    void finishedWizard();

    void create();
    void createAction();
public:
     explicit IdpWizardWindow(QWidget* parent = nullptr);
     ~IdpWizardWindow() noexcept override;

    //只能设置一次，即初始化，否则返回 false.
    bool setComponent(WizardComponent*);

    void setGroup(const QString& group);

    void resetStep();

public slots:
    void nextBtnEnable(bool);
    void prevBtnEnable(bool);
    void skipBtnStatus(char);
    void setNote(const QString&);
    void setLastStepStatus(bool);
signals:
    void toNext();
    void toPrev();
    void toSkip();
};
#endif //HENGREN_WIZARD_WINDOW_H
