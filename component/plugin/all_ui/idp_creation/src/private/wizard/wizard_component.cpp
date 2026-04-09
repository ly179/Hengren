//
// Created by Shisan on 2026/1/22.
//

#include "wizard_component.h"

#include <cde90_message_box.h>
#include <QStackedLayout>
#include "wizard_list_data.h"
#include "idp_wizard_window.h"

WizardComponent::WizardComponent(QWidget* parent) : CDE90Widget(parent) {
}

WizardComponent::~WizardComponent() noexcept {

}

QList<WizardStepData>& WizardComponent::list() {
    return window->getList();
}
//全部在UI线程操作。
//append 增量更新或尾部移除更新
void WizardComponent::notifyUpdated() {
    window->notifyUpdated();
}
//insert或中间remove移除更新
void WizardComponent::notifyUpdated(int index, int size) {
    window->notifyUpdated(index, size);
}
//list不变但list某元素内容变化更新
void WizardComponent::notifyUpdated(int row) {
    window->notifyUpdated(row);
}

void WizardComponent::select(int index) {
    window->select(index);
}

QString WizardComponent::genGroupPath() const {
    return window->genGroupPath();
}

void WizardComponent::toFinished(int index, bool b) {
    QList<WizardStepData>& list = window->getList();
    if (index < 0 || index >= list.size()) return;
    list[index].finished = b;
    window->notifyUpdated(index);
}

void WizardComponent::finishedWizard() {
    window->finishedWizard();
}

AsyncCaller* WizardComponent::caller() {
    return window->getAsyncCaller();
}

void WizardComponent::getSkip(int&) {
    //DO Nothing
}

void WizardComponent::getPrev(int& step) {
    //DO Nothing
}

void WizardComponent::stepCodeRefresh(int) {

}

void WizardComponent::toNext() {
    const PLinkedHashNode<int, sem::NO_PARAM_ARG_FUNC>* stepNode = nullptr;
    if (currentStep == nullptr) {
        stepNode = allSteps.first();
    } else {
        stepNode = allSteps.dataNode(currentStep.value());
        if (!stepNode) {
            simpleMessageBox(this, "Error", "No step found.");
            return;
        }
        if (!stepNode->next) {
            stepFinished();
            return;
        }
        stepNode = stepNode->next;
    }
    if (!stepNode) {
        simpleMessageBox(this, "Error", "No step found.");
        return;
    }
    currentStep = stepNode->key;
    if (!stepNode->next) {
        emit lastStepActivated();
    }
    if (stepNode->prev) emit prevStatusChanged(true);
    stepCodeRefresh(currentStep.value());
    stepNode->value();
}

void WizardComponent::toPrev() {
    if (currentStep == nullptr) return;
    int v = currentStep.value();
    getPrev(v);
    const PLinkedHashNode<int, sem::NO_PARAM_ARG_FUNC>* stepNode = allSteps.dataNode(v);
    if (!stepNode) {
        simpleMessageBox(this, "Error", "No step found.");
        return;
    }
    if (currentStep.value() == v) {
        if (!stepNode->prev) {
            simpleMessageBox(this, "Error", "No step found.");
            return;
        }
        stepNode = stepNode->prev;
    }
    currentStep = stepNode->key;
    stepCodeRefresh(currentStep.value());
    stepNode->value();
    if (!stepNode->prev) emit prevStatusChanged(false);
    emit lastStepActivated(false);
}
void WizardComponent::toSkip() {
    if (currentStep == nullptr) return;
    int v = currentStep.value();
    getSkip(v);
    if (v == currentStep.value()) return;
    const PLinkedHashNode<int, sem::NO_PARAM_ARG_FUNC>* stepNode = allSteps.dataNode(v);
    if (!stepNode) {
        simpleMessageBox(this, "Error", "No step found.");
        return;
    }
    currentStep = stepNode->key;
    if (!stepNode->prev) emit prevStatusChanged(false);
    emit nextStatusChanged(true);
    if (!stepNode->next) {
        emit lastStepActivated();
    } else {
        emit lastStepActivated(false);
    }
    stepCodeRefresh(currentStep.value());
    stepNode->value();
}

void WizardComponent::setWizardWindow(IdpWizardWindow* w) {
    window = w;
}

void WizardComponent::initSteps() {
    currentStep = nullptr;
    emit lastStepActivated(false);
    emit prevStatusChanged(false);
}