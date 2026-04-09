#ifndef FILE_RECIPE_COPY_CONTROLLER_H
#define FILE_RECIPE_COPY_CONTROLLER_H
#include "cde90_window.h"
#include "recipe_data_model.h"
#include "ui_common_global.h"

namespace cdsem {
class UiFileRecipeCopyController;
}
class ClassListSelectorController;
class IdwListSelectorController;
class IdpListSelectorController;
class UI_COMMON_EXPORT FileRecipeCopyController : public CDE90Window{

private:

    Recipe_CopyDataParam param;

    cdsem::UiFileRecipeCopyController* ui = nullptr;
    ClassListSelectorController* classSelector = nullptr;
    IdwListSelectorController* idwSelector = nullptr;
    IdpListSelectorController* idpSelector = nullptr;

    bool openedRecipeSelfIdw = true;

    quint64 afterCopyClassId = 0;
    quint64 afterCopyId = 0;

    void create();
    void createAction();

    void createClassSelector();
    void createIDWSelector();
    void createIDPSelector();
public:
     explicit FileRecipeCopyController(QWidget* parent = nullptr);
     ~FileRecipeCopyController() noexcept;

     void setParam(const Recipe_CopyDataParam& param);

     inline quint64 getAfterCopyId() const {
         return afterCopyId;
     }
     inline quint64 getAfterCopyClassId() const {
         return afterCopyClassId;
     }

 public slots:
     void beginCopy();
};
#endif
