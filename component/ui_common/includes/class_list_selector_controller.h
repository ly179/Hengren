#ifndef CLASS_LIST_SELECTOR_CONTROLLER_H
#define CLASS_LIST_SELECTOR_CONTROLLER_H

#include "data_select_controller.h"
#include "ui_common_global.h"

class UI_COMMON_EXPORT ClassListSelectorController : public DataSelectController {
private:
    void localSearchIdByName(ListViewMainData& text, int* errCode = nullptr) override;
public:
    explicit ClassListSelectorController(QWidget* parent = nullptr);
    ~ClassListSelectorController() noexcept;

    void initClassList();
};

#endif // CLASS_LIST_SELECTOR_CONTROLLER_H
