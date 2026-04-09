//
// Created by Shisan on 2026/4/2.
//

#ifndef HENGREN_CD_SEM_REALTIME_MONITOR_RECORD_SERVICE_H
#define HENGREN_CD_SEM_REALTIME_MONITOR_RECORD_SERVICE_H

#include "general_database_service_global.h"
#include "realtime_monitor_record_entity.h"
#include "iservice.h"

class RealTimeMonitorRecordDao;
#include "float_number.h"
class SEM_SERVICE_EXPORT RealTimeMonitorRecordService : public IService {
private:
    RealTimeMonitorRecordDao* dao;

public:
    RealTimeMonitorRecordService();
    ~RealTimeMonitorRecordService() override;

    int appendDataValue(quint64 idpId, const QString& groupId, const FloatNumber2& value, bool firstData = false);
    int appendDataValues(quint64 idpId, const QList<std::pair<QString, FloatNumber2>>& values, bool firstData = false);

    int clearAllData();

    QList<RealTimeMonitorRecordEntity> searchData(const QString& groupId, quint64 startIndex, int limit, int* errCode = nullptr);
    QList<RealTimeMonitorRecordEntity> searchData(const QString& groupId, const QList<quint64>& indices, int* errCode = nullptr);
};


#endif //HENGREN_CD_SEM_REALTIME_MONITOR_RECORD_SERVICE_H