#ifndef PROCEDURE_MANUAL_NORMAL_WINDOW_H
#define PROCEDURE_MANUAL_NORMAL_WINDOW_H
#include "relation_procedure_manual_normal.h"
#include "procedure_manual_global.h"

namespace cdsem {
class UiProcedureManualNormalWindow;
}

struct ProcedureManualNormalDataHelper;

class UI_PROCEDURE_MANUAL_EXPORT ProcedureManualNormalWindow : public RelatoinProcedureManualNormal{
    friend struct ProcedureManualNormalDataHelper;
private:
    ProcedureManualNormalDataHelper* dataHelper = nullptr;
    cdsem::UiProcedureManualNormalWindow* ui = nullptr;

    void create();
    void createAction();
protected:
    void closeEvent(QCloseEvent*) override;
public:
    explicit ProcedureManualNormalWindow(QWidget* parent = nullptr);
    ~ProcedureManualNormalWindow() noexcept;

    void setMsgText(const QString&) override;
    void setProcessButtonText(const QString&) override;
    void showProcessButton(bool = true) override;
    void showCmdArea(bool = true) override;
    void showOKButton(bool b) override;
    void setRetryButtonText(const QString& text) override;
    void setEnvButtonText(const QString& text) override;
    void setRejectButtonText(const QString& text) override;
};

#endif
