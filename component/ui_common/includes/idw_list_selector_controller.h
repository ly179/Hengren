#ifndef IDW_LIST_SELECTOR_CONTROLLER_H
#define IDW_LIST_SELECTOR_CONTROLLER_H
#include "data_select_controller.h"
#include "ui_common_global.h"

class UI_COMMON_EXPORT IdwListSelectorController : public DataSelectController {
private:
    quint64 classId = 0;

    void localSearchIdByName(ListViewMainData& text, int* errCode = nullptr) override;

public:
    IdwListSelectorController(QWidget* parent = nullptr);
    ~IdwListSelectorController() noexcept;

    void initIdwList(quint64 classId);
};

#endif // IDW_LIST_SELECTOR_CONTROLLER_H
