#ifndef RECIPE_EDITOR_CONTROLLER_H
#define RECIPE_EDITOR_CONTROLLER_H
#include "cde90_window.h"
#include "plinked_node_list.h"
#include "recipe_data_model.h"
#include "ui_recipe_creation_global.h"

namespace cdsem {
class UiRecipeEditorController;
}
class RecipeIdwDataSelectController;
class RecipeIdpDataSelectController;
class FemMeasurementController;
class MaintenanceController;
class RecipeEnvironmentController;
class CDE90InputBox;
struct RecipeDataModel;
struct RecipeSaveAdditionParam;

struct Recipe_BaseData {
    quint64 classId = 0;
    quint64 recipeId = 0;
};
using CassetteSlotList = PLinkedNodeList<int>;
using CassetteSlotNode = PLinkedNode<int>;

class UI_RECIPE_LIB_DEP RecipeEditorController : public CDE90Window {
private:
    RecipeDataModel param;
    Recipe_BaseData baseData;
    CassetteSlotList cassetteSlots;
    cdsem::UiRecipeEditorController* ui = nullptr;
    RecipeIdwDataSelectController* idwSelector = nullptr;
    RecipeIdpDataSelectController* idpSelector = nullptr;
    FemMeasurementController* femEditorController = nullptr;
    MaintenanceController* maintenanceController = nullptr;
    RecipeEnvironmentController* environmentController = nullptr;
    CDE90InputBox* saveAsBox = nullptr;

    void create();
    void createAction();

    void createIdwSelector();
    void createIdpSelector();
    void createFemMeasurementEditor();
    void createMaintenanceEditor();
    void createEnvironmentEditor();

    QString collectData(RecipeDataModel&);
    void saveData(const RecipeDataModel&, const RecipeSaveAdditionParam&, bool saveAs = false);

public:
     explicit RecipeEditorController(QWidget* parent = nullptr);
     ~RecipeEditorController() noexcept;

     void loadExistsData(const Recipe_BaseData&);
     void newData(quint64 classId, const QString& recipeName);

     void disableCoverSave(bool = true);

     quint64 getRecipeId() const;
};
#endif
