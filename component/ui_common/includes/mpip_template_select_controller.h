#ifndef MPIP_TEMPLATE_SELECT_CONTROLLER_H
#define MPIP_TEMPLATE_SELECT_CONTROLLER_H

#include "cdsem_pack_base.h"
#include "data_select_controller.h"

class UI_COMMON_EXPORT MPIPTemplateSelectController : public DataSelectController {
private:
    Integer32 type;
    void localSearchIdByName(ListViewMainData& data, int* errCode = nullptr) override;

public:
    explicit MPIPTemplateSelectController(QWidget* parent = nullptr);
    ~MPIPTemplateSelectController() noexcept;
    //TEMP_MS和TEMP_MP
    void initMpIpList(int type);
};

#endif // MPIP_TEMPLATE_SELECT_CONTROLLER_H
