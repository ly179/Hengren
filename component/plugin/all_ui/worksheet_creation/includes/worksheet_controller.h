#ifndef WORKSHEET_CONTROLLER_H
#define WORKSHEET_CONTROLLER_H
#include "ui_worksheet_creation_global.h"
#include "relation_worksheet_controller.h"

namespace cdsem {
class UiWorksheetController;
}

struct MSR_Chip;
struct MSR_MP_Chip_MetaData;
struct MSR_MP_Data;
struct WorksheetControllerDataHelper;
class UI_WORKSHEET_EXPORT WorksheetController : public RelationWorksheetController {
    friend struct WorksheetControllerDataHelper;
private:
    WorksheetControllerDataHelper* dataHelper;
    cdsem::UiWorksheetController* ui = nullptr;

    void create();
    void createAction();
public:
     explicit WorksheetController(QWidget* parent = nullptr);
     ~WorksheetController() noexcept;

     void resetData(const MSR_Chip&);
     void resetData(quint64 msrTopId);

     //允许一条一条记录的往表格中添加数据
     //仅允许以一次测量单位进行添加。
     //callback在worksheet内部数据线程中回调。如果需要在UI响应，请自行切换
     void appendData(const MSR_MP_Chip_MetaData& metaData, const MSR_MP_Data& data, const sem::ONE_PARAM_ARG_FUNC<bool>& = nullptr);
};
#endif
