#ifndef FILE_IDP_COPY_CONTROLLER_H
#define FILE_IDP_COPY_CONTROLLER_H
#include "cde90_window.h"
#include "idp_data_copy_module.h"
#include "ui_common_global.h"

namespace cdsem {
class UiFileIdpCopyController;
}

class ClassListSelectorController;
class IdwListSelectorController;

class UI_COMMON_EXPORT FileIdpCopyController : public CDE90Window{

private:

    IDP_CopyDataParam param;

    cdsem::UiFileIdpCopyController* ui = nullptr;
    ClassListSelectorController* classSelector = nullptr;
    IdwListSelectorController* idwSelector = nullptr;

    quint64 afterCopyClassId = 0;
    quint64 afterCopyId = 0;

    void create();
    void createAction();

    void createClassSelector();
    void createIDWSelector();
public:
     explicit FileIdpCopyController(QWidget* parent = nullptr);
     ~FileIdpCopyController() noexcept;

     void setParam(const IDP_CopyDataParam& param);

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
