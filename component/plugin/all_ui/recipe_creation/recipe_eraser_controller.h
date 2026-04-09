#ifndef RECIPE_ERASER_CONTROLLER_H
#define RECIPE_ERASER_CONTROLLER_H
#include "cde90_window.h"
#include "ui_recipe_creation_global.h"

namespace cdsem {
class UiRecipeEraserController;
}
class RecipeEraseViewModel;
class RecipeRefCheckController;

class UI_RECIPE_LIB_DEP RecipeEraserController : public CDE90Window {
private:
    cdsem::UiRecipeEraserController* ui = nullptr;
    RecipeEraseViewModel* tableViewModel = nullptr;
    RecipeRefCheckController* refCheckController = nullptr;
    quint64 classId = 0;

    void create();
    void createAction();

    void deleteRecipe(quint64 id);
    void createRefCheckController();

public:
     explicit RecipeEraserController(QWidget* parent = nullptr);
     ~RecipeEraserController() noexcept;

     void refreshDataList(quint64 classId);
};
#endif
