//
// Created by Shisan on 2026/4/2.
//

#ifndef HENGREN_CD_SEM_REALTIME_MONITOR_RECORD_DAO_H
#define HENGREN_CD_SEM_REALTIME_MONITOR_RECORD_DAO_H

#include <float_number.h>

#include "abstract_dao.h"
#include "realtime_monitor_record_entity.h"
#include "general_database_dao_global.h"

class SqlConnectionFactory;
struct RealTimeMonitorRecordDaoDataHelper;
class SEM_DAO_EXPORT RealTimeMonitorRecordDao : public AbstractDao {
    friend struct RealTimeMonitorRecordDaoDataHelper;
private:
    QScopedPointer<RealTimeMonitorRecordDaoDataHelper> helper;
    QSharedPointer<SqlConnectionFactory> getConnectionFactory() override;

public:
    RealTimeMonitorRecordDao();
    ~RealTimeMonitorRecordDao() override;
    int appendDataValue(quint64 idpId, const QString& groupId, const FloatNumber2& value, bool firstData = false);
    int appendDataValues(quint64 idpId, const QList<std::pair<QString, FloatNumber2>>& values, bool firstData = false);

    int clearAllData();

    QList<RealTimeMonitorRecordEntity> searchData(const QString& groupId, quint64 startIndex, int limit, int* errCode = nullptr);
    QList<RealTimeMonitorRecordEntity> searchData(const QString& groupId, const QList<quint64>& indices, int* errCode = nullptr);

};


#endif //HENGREN_CD_SEM_REALTIME_MONITOR_RECORD_DAO_H