#ifndef TEMPLATE_SAVE_BOX_CONTROLLER_H
#define TEMPLATE_SAVE_BOX_CONTROLLER_H
#include "cde90_window.h"
#include "common_act_param_helper.h"

namespace cdsem {
class UiTemplateSaveBoxController;
}
struct TemplateSaveBoxData {
    QString text;
    bool checked = false;
};

class TemplateSaveBoxController : public CDE90Window {

    SEM_OK_CALL(
        TemplateSaveBoxData)
private:
    cdsem::UiTemplateSaveBoxController* ui = nullptr;

    void create();
    void createAction();

public:
    explicit TemplateSaveBoxController(QWidget* parent = nullptr);
    ~TemplateSaveBoxController() noexcept;

    void resetComponent(const TemplateSaveBoxData& param);

    void disableCheckBox(bool = true);

    void setCheckBoxText(const QString&);
};
#endif
