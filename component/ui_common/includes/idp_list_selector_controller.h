#ifndef IDP_LIST_SELECTOR_CONTROLLER_H
#define IDP_LIST_SELECTOR_CONTROLLER_H

#include "data_select_controller.h"
#include "ui_common_global.h"

class UI_COMMON_EXPORT IdpListSelectorController : public DataSelectController {
private:
    quint64 classId = 0;

    void localSearchIdByName(ListViewMainData& text, int* errCode = nullptr) override;

public:
    IdpListSelectorController(QWidget* parent = nullptr);
    ~IdpListSelectorController() noexcept;

    void initIdpList(quint64 classId, quint64 idwId = 0);
};

#endif // IDP_LIST_SELECTOR_CONTROLLER_H
