#ifndef RECIPE_IDW_DATA_SELECT_CONTROLLER_H
#define RECIPE_IDW_DATA_SELECT_CONTROLLER_H
#include "recipe_ref_data_select_controller.h"

class RecipeIdwDataSelectController : public RecipeRefDataSelectController {
public:
    explicit RecipeIdwDataSelectController(QWidget* parent = nullptr);
    ~RecipeIdwDataSelectController() noexcept;

    void initIdwList(quint64 classId);
};

#endif // RECIPE_IDW_DATA_SELECT_CONTROLLER_H
