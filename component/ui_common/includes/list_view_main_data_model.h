#ifndef LIST_VIEW_MAIN_DATA_MODEL_H
#define LIST_VIEW_MAIN_DATA_MODEL_H

#include "view_data_mode.h"
#include "ui_common_global.h"

class UI_COMMON_EXPORT ListViewMainData : public ListViewData {
private:
    quint64 topId = 0;

public:
    ListViewMainData();
    ListViewMainData(const ListViewMainData&) = default;
    ListViewMainData(ListViewMainData&&) = default;
    inline ListViewMainData(const ListViewData& other) {
        *this = other;
    }
    inline ListViewMainData(ListViewData&& other) {
        *this = std::move(other);
    }

    void setTopId(quint64 id);

    quint64 getTopId() const;

    ListViewMainData& operator=(const ListViewMainData& other) = default;
    ListViewMainData& operator=(ListViewMainData&& other) = default;
    ListViewMainData& operator=(const ListViewData& other);
    ListViewMainData& operator=(ListViewData&& other);
};

class UI_COMMON_EXPORT ListViewMainModel : public SimpleRowItemModel<ListViewMainData> {
public:
    explicit ListViewMainModel(
        QObject* parent = nullptr);
    virtual ~ListViewMainModel() noexcept;

private:
    virtual QVariant getItemData(
        qint32 row) const override;
};

#endif // LIST_VIEW_MAIN_DATA_MODEL_H
