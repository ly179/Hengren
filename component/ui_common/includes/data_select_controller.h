#ifndef DATA_SELECT_CONTROLLER_H
#define DATA_SELECT_CONTROLLER_H
#include "cde90_dialog.h"
#include "ui_common_global.h"

namespace cdsem {
class UiDataSelectController;
};
class ListViewMainData;
class ListViewMainModel;
class QItemSelectionModel;

using DataSelectOkCallback = std::function<bool()>;
class UI_COMMON_EXPORT DataSelectController : public CDE90Dialog {

private:
    cdsem::UiDataSelectController* ui = nullptr;
    const ListViewMainData* selectedData = nullptr;
    DataSelectOkCallback okCallback;
    ListViewMainModel* listModel;

    void create();
    void createAction();

protected:
    //子类应该实现该函数来完成通过name反向查找ID的能力
    //该函数会自动在异步线程中调用
    virtual void localSearchIdByName(ListViewMainData& data, int* errCode = nullptr);

public:
    explicit DataSelectController(QWidget* parent = nullptr, bool useAsyncCaller = true);
    virtual ~DataSelectController() noexcept;

    inline void setOkCallback(const DataSelectOkCallback& callback) { okCallback = callback; }

    void setViewMode(bool useKindMode);

    void setTitle(const QString&);
    void setSubTitle(const QString&);

    void closeAsOk();

    QString getDetail() const;

    const ListViewMainData* getSelectedData() const;

    ListViewMainModel& getDataModel();

    QItemSelectionModel& getSelectModel();

    QString getInputText() const;

    //获取确切的输入数据
    //该函数仅识别选中一个的场景，如果未选中或选中多个，则不会触发回调，同时返回值返回false。
    //如果在选中多个的情况下，输入框内有输入的值，则以输入框为准。
    //该函数在某种程度上属于部分抽象函数（调用模板函数），子类需要实现localSearchIdByName函数才能使该函数正确工作
    bool simpleGetSelectedData(const std::function<bool(const ListViewMainData&)>&);
};
#endif
