#ifndef AMP_DATA_SELECT_CONTROLLER_H
#define AMP_DATA_SELECT_CONTROLLER_H
#include "data_select_controller.h"
class AmpDataSelectController : public DataSelectController {
public:
    explicit AmpDataSelectController(QWidget* parent = nullptr);
    ~AmpDataSelectController() noexcept;

    void refreshList();
};

#endif // AMP_DATA_SELECT_CONTROLLER_H
