#ifndef RECIPE_REF_CHECK_CONTROLLER_H
#define RECIPE_REF_CHECK_CONTROLLER_H
#include "cde90_window.h"

namespace cdsem {
    class UiRecipeRefCheckController;
}

class RecipeRefCheckController : public CDE90Window{
private:
    cdsem::UiRecipeRefCheckController* ui = nullptr;

    void create();
    void createAction();
public:
     explicit RecipeRefCheckController(QWidget* parent = nullptr);
     ~RecipeRefCheckController() noexcept;

     void checkIdwRef(quint64 classId);
     void checkIdpRef(quint64 classId);
};
#endif
