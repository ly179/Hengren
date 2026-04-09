#ifndef TEMPLATE_SELECTOR_CONTROLLER_H
#define TEMPLATE_SELECTOR_CONTROLLER_H
#include "cde90_dialog.h"
#include "common_act_param_helper.h"
#include "ui_common_global.h"

namespace cdsem {
class UiTemplateSelectorController;
};
class TemplateInfoSearchDTO;
class DataListViewModel;
class ImageOperationImageCanvas;
class ListViewData;

class UI_COMMON_EXPORT TemplateSelectorController : public CDE90Dialog {
    SEM_OK_CALL(
        ListViewData)

private:
    cdsem::UiTemplateSelectorController* ui = nullptr;
    DataListViewModel* listModel = nullptr;
    ImageOperationImageCanvas* canvasPainter = nullptr;
    quint64 dataId;
    int type = -1;

    void closeEvent(QCloseEvent* event) override;

    void create();
    void createAction();

    void loadData(quint64 id);

public:
    explicit TemplateSelectorController(QWidget* parent = nullptr);
    ~TemplateSelectorController() noexcept;

    void setTitle(const QString& text);

    void load(const TemplateInfoSearchDTO& param);
};
#endif
