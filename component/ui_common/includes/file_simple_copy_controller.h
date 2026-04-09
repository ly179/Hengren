#ifndef FILE_SIMPLE_COPY_CONTROLLER_H
#define FILE_SIMPLE_COPY_CONTROLLER_H
#include "cde90_window.h"
#include "file_copy_data_model.h"
#include "ui_common_global.h"

namespace cdsem {
class UiFileSimpleCopyController;
}

class UI_COMMON_EXPORT FileSimpleCopyController : public CDE90Window{

private:
    SimpleDataCopyParam param;
    cdsem::UiFileSimpleCopyController* ui = nullptr;

    void create();
    void createAction();

protected:

    virtual QString initData(const SimpleDataCopyParam& param) = 0;

    virtual QString beginCopy(const SimpleDataCopyParam& param) = 0;

    void setFromName(const QString&);
    void setToName(const QString&);

public:
     explicit FileSimpleCopyController(QWidget* parent = nullptr);
     ~FileSimpleCopyController() noexcept;

     void setCopyParam(const SimpleDataCopyParam&);
     void setCopyTitle(const QString&);

     void startCopy();
};
#endif
