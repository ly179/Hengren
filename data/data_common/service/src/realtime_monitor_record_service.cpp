//
// Created by Shisan on 2026/4/2.
//

#include "realtime_monitor_record_service.h"
#include "realtime_monitor_record_dao.h"
#include "dao_manager.h"

RealTimeMonitorRecordService::RealTimeMonitorRecordService() {
    dao = DaoManager::instance()->getFileFactoryDao<RealTimeMonitorRecordDao>();
}
RealTimeMonitorRecordService::~RealTimeMonitorRecordService() {}

int RealTimeMonitorRecordService::appendDataValue(quint64 idpId, const QString& groupId, const FloatNumber2& value, bool firstData) {
    return dao->appendDataValue(idpId, groupId, value, firstData);
}
int RealTimeMonitorRecordService::appendDataValues(quint64 idpId, const QList<std::pair<QString, FloatNumber2>>& values, bool firstData) {
    return dao->appendDataValues(idpId, values, firstData);
}

int RealTimeMonitorRecordService::clearAllData() {
    return dao->clearAllData();
}

QList<RealTimeMonitorRecordEntity> RealTimeMonitorRecordService::searchData(const QString& groupId, quint64 startIndex, int limit, int* errCode) {
    return dao->searchData(groupId, startIndex, limit, errCode);
}
QList<RealTimeMonitorRecordEntity> RealTimeMonitorRecordService::searchData(const QString& groupId, const QList<quint64>& indices, int* errCode) {
    return dao->searchData(groupId, indices, errCode);
}