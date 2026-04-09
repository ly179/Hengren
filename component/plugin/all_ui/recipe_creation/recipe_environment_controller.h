#ifndef RECIPE_ENVIRONMENT_CONTROLLER_H
#define RECIPE_ENVIRONMENT_CONTROLLER_H
#include "cde90_window.h"
#include "common_act_param_helper.h"

namespace cdsem {
class UiRecipeEnvironmentController;
}
struct RecipeEnvironmentModel;

class RecipeEnvironmentController : public CDE90Window{
    SEM_OK_CALL(
        RecipeEnvironmentModel)
private:
    cdsem::UiRecipeEnvironmentController* ui = nullptr;

    void create();
    void createAction();

    QString collectData(RecipeEnvironmentModel&);
    void saveDefault();

public:
     explicit RecipeEnvironmentController(QWidget* parent = nullptr);
     ~RecipeEnvironmentController() noexcept;

     void resetToComponent(const RecipeEnvironmentModel&);
};
#endif
