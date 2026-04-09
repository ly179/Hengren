#ifndef RECIPE_IDP_DATA_SELECT_CONTROLLER_H
#define RECIPE_IDP_DATA_SELECT_CONTROLLER_H

#include "recipe_ref_data_select_controller.h"

class RecipeIdpDataSelectController : public RecipeRefDataSelectController {
public:
    explicit RecipeIdpDataSelectController(QWidget* parent = nullptr);
    ~RecipeIdpDataSelectController() noexcept;

    void initIdpList(quint64 classId, quint64 idwId = 0);
};

#endif // RECIPE_IDP_DATA_SELECT_CONTROLLER_H
