#include "amp_data_select_controller.h"
#include "amp_data_service.h"
#include "cde90_message_box.h"
#include "cde90_process_box.h"
#include "sem_global.h"
#include "service_manager.h"
#include "list_view_main_data_model.h"

AmpDataSelectController::AmpDataSelectController(QWidget* parent)
    : DataSelectController(parent, true) {}

AmpDataSelectController::~AmpDataSelectController() noexcept {}

void AmpDataSelectController::refreshList() {
    getDataModel().clear();
    bool succ = ACaller->push(this, [this](TSC) {
        AmpDataService* service = ServiceManager::instance()
                                      ->getDefaultService<AmpDataService>();
        int errCode;
        AmpDataSearchDTO param;
        param.setInvalid(0);
        QList<AmpDataEntity> list = service->searchList(param, &errCode);
        if (errCode) {
            simpleMessageBox(this, "Error", "Load AMP data error.");
            return;
        }
        if (list.isEmpty()) {
            return;
        }
        MainThreadInvoker::getInvoker().blockRun([this, &list]() {
            ListViewMainModel& model = getDataModel();
            QList<ListViewMainData>& modeList = model.getDataList();
            modeList.resize(list.size());
            for (int i = 0; i < list.size(); ++i) {
                ListViewMainData& d = modeList[i];
                AmpDataEntity& e = list[i];
                d.setId(e.getId());
                d.setText(e.getAmpName());
                d.setTopId(e.getTopId());
            }
            model.resizeViewList(list.size());
        });
    });
    if (succ) {
        loadingBox(this, "Process", "Working");
    }
}
