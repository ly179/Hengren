#ifndef JOB_ITEM_CONTROLLER_H
#define JOB_ITEM_CONTROLLER_H
#include <QWidget>

namespace cdsem {
class UiJobItemController;
}
class AsyncCaller;
class JobRecipeSettingController;

class JobItemController : public QWidget {
private:
    cdsem::UiJobItemController* ui = nullptr;
    AsyncCaller* caller = nullptr;
    JobRecipeSettingController* recipeController = nullptr;
    char port = '\0';

    void create();
    void createAction();

    void createJobRecipeSettingEditor();

public:
     explicit JobItemController(QWidget* parent = nullptr);
     ~JobItemController() noexcept;

     void setPortLabelText(const QString&);
     void setSetupLabelText(const QString&);

     void initAsyncCaller(AsyncCaller*);

     void setPort(char);
};
#endif
