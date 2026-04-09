#ifndef RELATION_PROCEDURE_MANUAL_NORMAL_H
#define RELATION_PROCEDURE_MANUAL_NORMAL_H

#include "relation_cde90_window_export.h"

enum ProcedureManualNormalStatus {
    PMNS_OK = 1,
    PMNS_PROCESS = 101,
    PMNS_RETRY = 102,
    PMNS_ENV = 103,
    PMNS_REJECT = 104,
    PMNS_FOCUS_CLOSE = 999
};

__R_WINDOW(RelatoinProcedureManualNormal)
    virtual void setMsgText(const QString&) = 0;
    virtual void setProcessButtonText(const QString&) = 0;
    virtual void showProcessButton(bool = true) = 0;
    virtual void showCmdArea(bool = true) = 0;
    virtual void showOKButton(bool b) = 0;
    virtual void setRetryButtonText(const QString& text) = 0;
    virtual void setEnvButtonText(const QString& text) = 0;
    virtual void setRejectButtonText(const QString& text) = 0;

    inline void setProcedureStatusId(quint64 id) {
        procedureStatusId = id;
    }
    inline quint64 getProcedureStatusId() const {
        return procedureStatusId;
    }
private:
    quint64 procedureStatusId = 0;
__R_WINDOW_END

#endif // RELATION_PROCEDURE_MANUAL_NORMAL_H
