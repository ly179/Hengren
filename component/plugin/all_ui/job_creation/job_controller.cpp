#include "job_controller.h"
#include <QLayout>
#include "cde90_spliter_line.h"
#include "job_item_controller.h"
#include "ui_job_controller.h"

JobController::JobController(QWidget* parent) : CDE90Window(parent) {
    //如果你没有异步任务需求，请将connect清理掉，保留create和createAction函数
    connect(this, &JobController::asyncCallerChanged, this, [this](){
        create();
        createAction();
    }, Qt::SingleShotConnection);
}

JobController::~JobController() noexcept {
    if (jobItemControllers) {
        delete[] jobItemControllers;
    }
    if (ui) {
        delete ui;
    }
}

void JobController::create() {
    ui = new cdsem::UiJobController;
    setContent(new QWidget);
    ui->init(getContent());

    createJobItemController();
}

void JobController::createAction() {
    bindCloseBtn(ui->closeBtn->getQPushButton());
}

void JobController::createJobItemController() {
    if (jobItemControllers) {
        return;
    }
    jobItemControllers = new JobItemController*[4];
    for (int i = 0; i < 4; ++i) {
        jobItemControllers[i] = new JobItemController;
        ui->jobItemContent->layout()->addWidget(jobItemControllers[i]);
        if (i > 0 && i < 3) {
            ui->jobItemContent->layout()->addWidget(new CDE90SpliterHLine(ui->jobItemContent));
        }
        itsChildAndBeManaged(jobItemControllers[i], this);
        jobItemControllers[i]->initAsyncCaller(ACaller);
        if (i == 3) {
            jobItemControllers[i]->setSetupLabelText("Mount Cassette");
        }
    }
    jobItemControllers[0]->setPortLabelText("PORT-A");
    jobItemControllers[1]->setPortLabelText("PORT-B");
    jobItemControllers[2]->setPortLabelText("PORT-C");
    jobItemControllers[3]->setPortLabelText("PORT-D");

    jobItemControllers[0]->setPort('A');
    jobItemControllers[1]->setPort('B');
    jobItemControllers[2]->setPort('C');
    jobItemControllers[3]->setPort('D');
}
