#ifndef FILE_IDW_COPY_CONTROLLER_H
#define FILE_IDW_COPY_CONTROLLER_H
#include "cde90_window.h"
#include "idw_wafer_copy_model.h"
#include "ui_common_global.h"

namespace cdsem {
class UiFileIdwCopyController;
}

class ClassListSelectorController;

class UI_COMMON_EXPORT FileIdwCopyController : public CDE90Window{
    Q_OBJECT
private:
    IDW_CopyDataParam param;
    cdsem::UiFileIdwCopyController* ui = nullptr;
    ClassListSelectorController* classSelector = nullptr;

    quint64 afterCopyId = 0;
    quint64 afterCopyClassId = 0;

    void create();
    void createAction();

    void createClassSelector();
public:
     explicit FileIdwCopyController(QWidget* parent = nullptr);
     ~FileIdwCopyController() noexcept;

     void setParam(const IDW_CopyDataParam& param);

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
