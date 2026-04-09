#ifndef JOB_RECIPE_SETTING_CONTROLLER_H
#define JOB_RECIPE_SETTING_CONTROLLER_H
#include <QList>
#include "cde90_window.h"

namespace cdsem {
class UiJobRecipeSettingController;
}
class ListViewMainModel;
class JobClassSelectController;
struct JobBaseData {
    quint64 classId = 0;
    quint64 recipeId = 0;
    char port = '\0';
};
struct JobRecipeheckedSlots;

class JobRecipeSettingController : public CDE90Window{
private:
    JobBaseData baseData;
    QList<JobRecipeheckedSlots> checkedSlots;
    cdsem::UiJobRecipeSettingController* ui = nullptr;
    ListViewMainModel* listViewModel = nullptr;
    JobClassSelectController* classSelector = nullptr;

    void create();
    void createAction();

    void createClassSelector();
    void refreshRecipeList(quint64 idwId = 0);
    void refreshRecipeData(quint64);
    void resetCassetteSlots();

public:
     explicit JobRecipeSettingController(QWidget* parent = nullptr);
     ~JobRecipeSettingController() noexcept;

     void setPort(char);
};
#endif
