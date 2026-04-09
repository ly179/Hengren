#include "image_overlay_controller.h"
#include "ui_image_overlay_controller.h"
#include "view_data_mode.h"
#include "result_data_save_model.h"
#include <QStyledItemDelegate>
#include <QPainter>

quint32 colors[5] = {qRgb(0, 132, 201), qRgb(200, 0, 0), qRgb(0, 201, 76), qRgb(200, 200, 0), qRgb(100, 100, 100)};

class ImageOverlayTableModel : public SimpleRowTableModel<ImageOverlayDataModel> {
private:
    QVariant getItemData(qint32 row, qint32 column) const {
        if (row < 0 || column < 0) return QVariant();
        const QList<ImageOverlayDataModel>& list = getDataList();
        if (row >= list.size()) {
            return QVariant();
        }
        const ImageOverlayDataModel& data = list[row];
        switch (column) {
        case 0:
            return data.data ? data.data->measureName : "";
        case 1:
            return data.data ? data.data->dNum : "";
        case 2:
            return int(data.profileLine);
        case 3:
            return int(data.edgeCrossMark);
        case 4:
            return int(data.designMark);
        case 5:
            return int(data.cursorMark);
        case 6:
            return int(data.viewResult);
        default:
            return QVariant();
        }
    }
public:
    explicit ImageOverlayTableModel(QObject* parent = nullptr) : SimpleRowTableModel(parent) {
    }
};

class ImageOverlayItemDelegate : public QStyledItemDelegate {
private:
    void paint(QPainter *painter,
        const QStyleOptionViewItem &option, const QModelIndex &index) const {
        if (index.column() < 2) {
            QStyledItemDelegate::paint(painter, option, index);
            return;
        }
        QVariant v = index.data();
        if (v.isNull()) return;
        OverlayStatus status = OverlayStatus(v.toInt());
        if (status != OVS_CHECKED && status != OVS_EXISTS) return;

        QColor color(colors[index.column() - 2]);
        painter->save();
        sem::AfterGc gc([painter](){
            painter->restore();
        });
        painter->setPen(QPen(color, 2));
        int minR = 16;
        QRect rect;
        if (option.rect.width() < minR || option.rect.height() < minR) {
            minR = std::min(option.rect.width(), option.rect.height());
        }
        rect = QRect(option.rect.x() + option.rect.width() / 2 - minR / 2, option.rect.y() + option.rect.height() / 2 - minR / 2, minR, minR);

        if (minR >= 16) {
            painter->drawRect(rect);
            rect = QRect(rect.x() + 4, rect.y() + 4, rect.width() - 8, rect.height() - 8);
            if (status == OVS_EXISTS) return;
            painter->setBrush(QBrush(color));
            painter->drawRect(rect);
        } else {
            if (status == OVS_CHECKED) {
                painter->setBrush(QBrush(color));
            }
            painter->drawRect(rect);
        }
        // painter->restore();
    }
public:
    explicit ImageOverlayItemDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {}
};

OverlayStatus& switchStatus(OverlayStatus& status) {
    if (status == OVS_EXISTS) status = OVS_CHECKED;
    else if (status == OVS_CHECKED) status =OVS_EXISTS;
    return status;
}

struct StatusCheckResult {
    OverlaySelectedType type = OST_UNKNOW;
    bool checked = false;
};

struct ImageOverlayControllerDataHelper {
    ImageOverlayController* d;
    ImageOverlayTableModel* overlayTableModel;

    StatusCheckResult updateCheckStatus(int row, int cell) {
        StatusCheckResult result;
        if (row < 0 || cell < 2) return result;
        QList<ImageOverlayDataModel>& list = overlayTableModel->getDataList();
        if (row >= list.size()) return result;
        ImageOverlayDataModel& data = list[row];
        cell -= 2;
        switch(cell) {
        case 0:
            result.type = OST_PROFILE_LINE;
            result.checked = switchStatus(data.profileLine) == OVS_CHECKED;
            break;
        case 1:
            result.type = OST_EDGE_CROSS_MARK;
            result.checked = switchStatus(data.edgeCrossMark) == OVS_CHECKED;
            break;
        case 2:
            result.type = OST_DESIGN_MARK;
            result.checked = switchStatus(data.designMark) == OVS_CHECKED;
            break;
        case 3:
            result.type = OST_CURSOR_MARK;
            result.checked = switchStatus(data.cursorMark) == OVS_CHECKED;
            break;
        case 4:
            result.type = OST_VIEW_RESULT;
            result.checked = switchStatus(data.viewResult) == OVS_CHECKED;
            break;
        default:
            return result;
        }
        overlayTableModel->notifyDataChanged(row, row);
        return result;
    }
};

ImageOverlayController::ImageOverlayController(QWidget* parent) : CDE90Window(parent) {
    create();
    createAction();
}

ImageOverlayController::~ImageOverlayController() noexcept {
    if (dataHelper) delete dataHelper;
    if (ui) {
        delete ui;
    }
}

void ImageOverlayController::setDataPtr(const QList<ImageOverlayDataModel>& list, int selectedRow, bool blockSignal ) {
    if (list.isEmpty()) {
        clear();
        return;
    }
    QList<ImageOverlayDataModel>& modelList = dataHelper->overlayTableModel->getDataList();
    modelList.clear();
    modelList.reserve(list.size());
    modelList.append(list);
    dataHelper->overlayTableModel->resizeViewList(list.size());
    if (selectedRow < 0 || selectedRow >= list.size()) selectedRow = 0;
    if (blockSignal)
        ui->controlList->blockSignals(true);
    ui->controlList->selectRow(selectedRow);
    if (blockSignal)
        ui->controlList->blockSignals(false);
}

void ImageOverlayController::selectRow(int r) {
    QList<ImageOverlayDataModel>& modelList = dataHelper->overlayTableModel->getDataList();
    if (r < 0 || r >= modelList.size()) return;
    if (r == ui->controlList->getSelectedRow()) return;
    ui->controlList->selectRow(r);
}

void ImageOverlayController::clear() {
    dataHelper->overlayTableModel->clear();
}

void ImageOverlayController::setDefaultSetting(OverlaySelectedType t, bool checked) {
    switch(t) {
    case OST_PROFILE_LINE:
        ui->profileCBox->blockSignals(true);
        ui->profileCBox->setChecked(checked);
        ui->profileCBox->blockSignals(false);
        break;
    case OST_EDGE_CROSS_MARK:
        ui->edgeCBox->blockSignals(true);
        ui->edgeCBox->setChecked(checked);
        ui->edgeCBox->blockSignals(false);
        break;
    case OST_DESIGN_MARK:
        ui->designCBox->blockSignals(true);
        ui->designCBox->setChecked(checked);
        ui->designCBox->blockSignals(false);
        break;
    case OST_CURSOR_MARK:
        ui->measureCBox->blockSignals(true);
        ui->measureCBox->setChecked(checked);
        ui->measureCBox->blockSignals(false);
        break;
    case OST_VIEW_RESULT:
        ui->resultCBox->blockSignals(true);
        ui->resultCBox->setChecked(checked);
        ui->resultCBox->blockSignals(false);
        break;
    default:
        return;
    }
}

void ImageOverlayController::create() {
    dataHelper = new ImageOverlayControllerDataHelper;
    dataHelper->d = this;
    QWidget* content = new QWidget;
    setContent(content);
    ui = new cdsem::UiImageOverlayController;
    ui->init(content);
}

void ImageOverlayController::createAction() {
    bindCloseBtn(ui->closeBtn->getQPushButton());

    ui->controlList->setStyleSheet(
        "QTableView::Item::Selected{"
            "color: rgb(40, 21, 254);"
            "background-color: rgb(230, 230, 230);"
        "}"
        "QHeaderView {"
            "font-weight: normal;"
        "}"
    );

    dataHelper->overlayTableModel = new ImageOverlayTableModel(ui->controlList);
    dataHelper->overlayTableModel->insertHead("MP Name");
    dataHelper->overlayTableModel->insertHead("D No.");
    dataHelper->overlayTableModel->insertHead("Profile");
    dataHelper->overlayTableModel->insertHead("Edge");
    dataHelper->overlayTableModel->insertHead("Design");
    dataHelper->overlayTableModel->insertHead("Measure");
    dataHelper->overlayTableModel->insertHead("Result");
    ui->controlList->replaceDataModel(dataHelper->overlayTableModel);
    ui->controlList->replaceItemDelegate(new ImageOverlayItemDelegate(ui->controlList));
    ui->controlList->setSelectionMode(QAbstractItemView::SingleSelection);

    {
        QList<int> resizeList;
        resizeList.reserve(7);
        resizeList << 143 << 50 << 55 << 50 << 55 << 65 << 55;
        ui->controlList->setMinColumnsSize(resizeList);
        ui->controlList->recalcColumnSize();
    }

    ui->controlList->setEnableCellClick(true);
    ui->controlList->setDisableDbClick();
    connect(ui->controlList, &CDE90TableView::cellClick, this, [this](const QModelIndex& index, QMouseEvent* event){
        Qt::MouseButton btn = event->button();
        if (!index.isValid() || btn != Qt::LeftButton) {
            return;
        }
        StatusCheckResult result = dataHelper->updateCheckStatus(index.row(), index.column());
        emit overlaySelectedType(index.row(), result.type, result.checked);
    });

    connect(ui->controlList->selectionModel(), &QItemSelectionModel::selectionChanged, this, [this](const QItemSelection &selected, const QItemSelection &deselected){
        Q_UNUSED(selected)
        Q_UNUSED(deselected)
        int row = ui->controlList->getSelectedRow();
        if (row < 0) return;
        emit overlaySelectedRow(row);
    });

    connect(ui->profileCBox, &QCheckBox::toggled, this, [this](bool checked){
        emit overlaySettingChanged(OST_PROFILE_LINE, checked);
    });
    connect(ui->edgeCBox, &QCheckBox::toggled, this, [this](bool checked){
        emit overlaySettingChanged(OST_EDGE_CROSS_MARK, checked);
    });
    connect(ui->designCBox, &QCheckBox::toggled, this, [this](bool checked){
        emit overlaySettingChanged(OST_DESIGN_MARK, checked);
    });
    connect(ui->measureCBox, &QCheckBox::toggled, this, [this](bool checked){
        emit overlaySettingChanged(OST_CURSOR_MARK, checked);
    });
    connect(ui->resultCBox, &QCheckBox::toggled, this, [this](bool checked){
        emit overlaySettingChanged(OST_VIEW_RESULT, checked);
    });

}
