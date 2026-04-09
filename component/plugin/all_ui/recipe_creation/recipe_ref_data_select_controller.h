#ifndef RECIPE_REF_DATA_SELECT_CONTROLLER_H
#define RECIPE_REF_DATA_SELECT_CONTROLLER_H
#include "cde90_window.h"

namespace cdsem {
class UiRecipeRefDataSelectController;
}
class ListViewMainData;
class ListViewMainModel;
class QItemSelectionModel;

class RecipeRefDataSelectController : public CDE90Window{
private:
    sem::NO_PARAM_ARG_FUNC okCallback;
    cdsem::UiRecipeRefDataSelectController* ui = nullptr;
    ListViewMainData* selectedData = nullptr;
    ListViewMainModel* listModel = nullptr;

    void create();
    void createAction();
public:
     explicit RecipeRefDataSelectController(QWidget* parent = nullptr);
     virtual ~RecipeRefDataSelectController() noexcept;

     void setOkCallback(const sem::NO_PARAM_ARG_FUNC&);

     const ListViewMainData* getSelectedData() const;

     ListViewMainModel& getDataModel();

     QItemSelectionModel& getSelectModel();

     void setSubTitle(const QString&);

     void setClassName(const QString&);
};
#endif
