#include "list_view_main_data_model.h"


ListViewMainData::ListViewMainData() : ListViewData() {}

void ListViewMainData::setTopId(quint64 id) {
    topId = id;
}

quint64 ListViewMainData::getTopId() const {
    return topId;
}

ListViewMainData& ListViewMainData::operator=(const ListViewData& other) {
    if (this == &other) return *this;
    setId(other.getId());
    setText(other.getText());
    const ListViewMainData* d = dynamic_cast<const ListViewMainData*>(&other);
    if (d) {
        setTopId(d->getTopId());
    } else {
        setTopId(0);
    }
    return *this;
}

ListViewMainData& ListViewMainData::operator=(ListViewData&& other) {
    if (this == &other) return *this;
    setId(other.getId());
    setText(std::move(other.getText()));
    const ListViewMainData* d = dynamic_cast<const ListViewMainData*>(&other);
    if (d) {
        setTopId(d->getTopId());
    } else {
        setTopId(0);
    }
    return *this;
}

ListViewMainModel::ListViewMainModel(QObject* parent) : SimpleRowItemModel(parent){

}

ListViewMainModel::~ListViewMainModel() noexcept {}

QVariant ListViewMainModel::getItemData(
    qint32 row) const {
    return getDataList()[row].getText();
}
