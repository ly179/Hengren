#include "amp_data_service.h"
#include "amp_data_dao.h"
#include "dao_manager.h"

AmpDataService::AmpDataService() {
    ampDataDao = DaoManager::instance()->getDefaultFactoryDao<AmpDataDao>();
}

AmpDataService::~AmpDataService() noexcept {}

qint32 AmpDataService::saveWithTransaction(
    AmpDataEntity &entity) {
    return ampDataDao->saveWithTransaction(entity);
}

AmpDataEntity AmpDataService::get(
    quint64 id, qint32 *errCode) {
    return ampDataDao->get(id, errCode);
}

quint64 AmpDataService::checkName(
    const QString &ampName, qint32 *errCode) {
    return ampDataDao->checkName(ampName, errCode);
}

QList<AmpDataEntity> AmpDataService::searchList(
    const AmpDataSearchDTO &param, qint32 *errCode) {
    return ampDataDao->searchList(param, errCode);
}
