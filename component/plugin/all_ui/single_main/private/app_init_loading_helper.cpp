#include "app_init_loading_helper.h"
#include <QGridLayout>
#include "cde90_spliter_line.h"
#include "cde90_check_box.h"
#include "cde90_button.h"
#include "sem_global.h"

AppInitLoadingHelper::AppInitLoadingHelper(AppInitLoadingData* data, QObject* parent) : QObject(parent), loadingData(data) {
    data->setParent(this);
    connect(data, &AppInitLoadingData::overLoad, this, [this](const OperationResult& result, bool failedCouldSkip){
        if (result.errCode) {
            emit statusChanged(AppInitLoadingHelper::Failed, failedCouldSkip);
            emit errorOccern(result.errMsg);
            return;
        }
        emit statusChanged(AppInitLoadingHelper::Finished);
    });
}
AppInitLoadingHelper::~AppInitLoadingHelper() noexcept {
    delete loadingData;
}

SerialLoadingManager::SerialLoadingManager(QObject* parent) : QObject(parent) {}

void SerialLoadingManager::setASyncCaller(AsyncCaller* caller) {
    this->caller = caller;
}

void SerialLoadingManager::setContent(QWidget* w) {
    this->content = w;
}

void SerialLoadingManager::append(AppInitLoadingData* data) {
    AppInitLoadingHelper* helper = new AppInitLoadingHelper(data, content);
    QGridLayout* layout = dynamic_cast<QGridLayout*>(content->layout());

    int i = loadingSteps.size() * 2;
    if (i > 0) {
        CDE90SpliterHLine* hLine = new CDE90SpliterHLine(content);
        layout->addWidget(hLine, i - 1, 0, 1, 2);
    }

    CDE90CheckBox* box = new CDE90CheckBox(content);
    box->setStyleSheet("QCheckBox::disabled {color: #000000;}");
    box->setEnabled(false);
    box->setText(data->getTitle());
    layout->addWidget(box, i, 0);

    CDE90ButtonBox* reloadBtn = new CDE90ButtonBox("Retry", content);
    reloadBtn->setVisible(false);
    reloadBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    layout->addWidget(reloadBtn, i, 1);
    QSizePolicy sz = reloadBtn->sizePolicy();
    sz.setRetainSizeWhenHidden(true);
    reloadBtn->setSizePolicy(sz);

    QObject::connect(reloadBtn->getQPushButton(), &QPushButton::clicked, this, [this, helper](){
        emit helper->retry();
        start();
    });

    QObject::connect(helper, &AppInitLoadingHelper::retry, helper, [box, data](){
        box->setText(data->getTitle());
    });

    QObject::connect(helper, &AppInitLoadingHelper::errorOccern, helper, [box](const QString& text){
        box->setText(text);
    });

    QPointer pointer(this);
    QObject::connect(helper, &AppInitLoadingHelper::statusChanged, helper, [box, reloadBtn, pointer](AppInitLoadingHelper::LoadingStatus s, bool failedCouldSkip){
        if (!pointer) return;
        if (pointer->isExit) {
            return;
        }
        if (s == AppInitLoadingHelper::StandBy) {
            box->setChecked(false);
            reloadBtn->getQPushButton()->setVisible(false);
        } else if (s == AppInitLoadingHelper::Processing) {
            box->setProperty("checked-color", QString::number(qRgb(255, 255, 0)));
            box->setChecked(true);
            reloadBtn->setVisible(true);
            reloadBtn->loading();
        } else if (s == AppInitLoadingHelper::Finished) {
            box->setProperty("checked-color", "");
            box->setChecked(true);
            reloadBtn->overLoading();
            reloadBtn->getQPushButton()->setVisible(false);
            ++ pointer->processIndex;
            pointer->start();
        } else if (s == AppInitLoadingHelper::Failed) {
            box->setProperty("checked-color", QString::number(qRgb(255, 0, 0)));
            box->setChecked(true);
            reloadBtn->overLoading();
            reloadBtn->setVisible(!failedCouldSkip);
            if (failedCouldSkip) {
                ++ pointer->processIndex;
                pointer->start();
            }
        }
        box->update();
    }, Qt::QueuedConnection);
    loadingSteps.append(helper);
}

void SerialLoadingManager::exit() {
    isExit = true;
}

void SerialLoadingManager::start() {
    if (processIndex >= loadingSteps.size()) {
        emit finished();
        return;
    }
    AppInitLoadingHelper* helper = loadingSteps[processIndex];
    if (helper->isThreadTask()) {
        caller->push(this, [helper](TSC){
            emit helper->statusChanged(AppInitLoadingHelper::Processing);
            helper->doLoad();
        });
        return;
    }
    emit helper->statusChanged(AppInitLoadingHelper::Processing);
    helper->doLoad();
}
