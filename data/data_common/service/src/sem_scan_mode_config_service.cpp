#include "sem_scan_mode_config_service.h"
#include "dao_manager.h"
#include "sem_scan_mode_config_dao.h"
#include "image_scan_live_mode_helper.h"
#include "scan_model_data_convert.h"
#include "cas_lock.h"

SemScanModeConfigService::SemScanModeConfigService() {
    semScanModeConfigDao = DaoManager::instance()->getDefaultFactoryDao<SemScanModeConfigDao>();
}

SemScanModeConfigService::~SemScanModeConfigService() noexcept {}

qint32 SemScanModeConfigService::saveWithTransaction(
    QList<ScanModeEntity>& list) {
    return semScanModeConfigDao->saveWithTransaction(list);
}

QList<ScanModeEntity> SemScanModeConfigService::get(
    qint32* errCode) {
    return semScanModeConfigDao->get(errCode);
}

int SemScanModeConfigService::initGlobal(bool reload) {
    CasLocker locker(ImageScanLiveModeHelper::instance()->getLock());
    if (!reload && ImageScanLiveModeHelper::get()->isReady()) {
        return 0;
    }
    //TODO 当发生错误时，如果是首次load即ImageScanLiveModeHelper::get()->isLoaded()为false
    //在程序中硬编码一个默认数据，以便后续的扫描可以正常运行
    int errCode;
    QList<ScanModeEntity> list = get(&errCode);
    if (errCode) {
        return errCode;
    }
    QList<ScanModeDataModel> modeList;
    modeList.resize(list.size());
    ScanModeDataModel* data;
    for (int i = 0; i < list.size(); ++ i) {
        data = &modeList[i];
        errCode = scanModeDataFromEntity(list[i].getDetail(), *data);
        if (errCode) {
            return errCode;
        }
        data->setId(list[i].getId());
        data->setDataMode(i < 2 ? SDM_VIEW : SDM_INTEG);
    }
    ImageScanLiveModeHelper::get()->setGlobal(modeList);
    ImageScanLiveModeHelper::get()->toReadyOnce();
    return 0;
}
