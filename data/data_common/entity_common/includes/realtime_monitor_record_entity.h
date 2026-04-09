//
// Created by Shisan on 2026/4/2.
//

#ifndef HENGREN_CD_SEM_REALTIME_MONITOR_RECORD_H
#define HENGREN_CD_SEM_REALTIME_MONITOR_RECORD_H

#include <QString>
#include <QDateTime>
#include "model_setget.h"

class RealTimeMonitorRecordEntity {
    ENTITY_CREATE_TIME
    CDSEMPSTRING(GroupId, groupId)
    CDSEMPSTRING(DataValue, dataValue)
    ENTITY_ID
    CDSEMPUINT64(DataIndex, dataIndex)
    CDSEMPBOOL(DataSplit, dataSplit)
};

#endif //HENGREN_CD_SEM_REALTIME_MONITOR_RECORD_H