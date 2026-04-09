//
// Created by Shisan on 2026/1/22.
//

#ifndef HENGREN_CD_SEM_WIZARD_COMPONENT_H
#define HENGREN_CD_SEM_WIZARD_COMPONENT_H

#include "cde90_widget.h"
#include "plinked_node_hash.h"
#include "sem_common.h"
#include "cdsem_pack_base.h"
struct WizardStepData;
class IdpWizardWindow;
class AsyncCaller;
class WizardComponent : public CDE90Widget {
    Q_OBJECT
private:
    IdpWizardWindow* window;
protected:

    PLinkedHashNodeHash<int, sem::NO_PARAM_ARG_FUNC> allSteps;

    QList<WizardStepData>& list();

    Integer32 currentStep;
    //全部在UI线程操作。
    //append 增量更新或尾部移除更新
    void notifyUpdated();
    //insert或中间remove移除更新
    void notifyUpdated(int index, int size);
    //list不变但list某元素内容变化更新
    void notifyUpdated(int row);

    void select(int);

    QString genGroupPath() const;

    void toFinished(int, bool = true);

    void finishedWizard();

    AsyncCaller* caller();

    //点击Skip按钮时触发的动作
    virtual void getSkip(int&);
    //点击back按钮时要返回到的步骤，传入的int为当前步骤的上一步，如果需要返回到更多，则修改该值
    virtual void getPrev(int&);

    virtual void stepFinished() = 0;
    virtual void stepCodeRefresh(int);

public:
    explicit WizardComponent(QWidget *parent = nullptr);
    ~WizardComponent() noexcept override;

    void setWizardWindow(IdpWizardWindow*);

    virtual void initSteps();

    //将按钮事件绑定到以下槽中
public slots:
    void toNext();
    void toPrev();
    void toSkip();

signals:
    void titleChanged(const QString& text);
    void nextStatusChanged(bool);
    void prevStatusChanged(bool);
    //0 - hide 1 - enable 2 - disable
    void skipStatusChanged(char);
    void lastStepActivated(bool = true);
};

#endif //HENGREN_CD_SEM_WIZARD_COMPONENT_H