#include "worksheet_controller.h"
#include <QLayout>
#include "cde90_message_box.h"
#include "cde90_process_box.h"
#include "cdsem_data_common.h"
#include "cdsem_path_util.h"
#include "image_info_entity.h"
#include "image_file_temp_handler.h"
#include "image_info_service.h"
#include "image_relation_record_service.h"
#include "image_save_model.h"
#include "image_save_model_convert.h"
#include "msr_data_model.h"
#include "msr_service.h"
#include "sem_global.h"
#include "service_manager.h"
#include "plugin_signal_hub.h"
#include "transaction_manager_service.h"
#include "ui_worksheet_controller.h"
#include "work_sheet_result_table.h"
#include "cdsem_global_data.h"
#include "image_operation_data_model.h"
#include "plinked_node_hash.h"
#include "exception_code.h"
#include "image_manager_relation.h"
#include "relation_image_list_controller.h"
#include "relation_image_view_controller.h"
#include "xlsx_export_relation.h"
#include <QFileDialog>
#include "cdsem_number_validator_helper.h"
#include  "loading_helper.h"
#include "image_type_defition.h"

#define RESET_GID 1
#define APP_GID 2

QString openFileDialog(QWidget* parent) {
    QFileDialog dialog(parent);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setWindowTitle("Export to XLSX");
    dialog.setFilter(QDir::Files);
    dialog.setNameFilter("Strict Open XML (*.xlsx)");
    if (!dialog.exec()) {
        return "";
    }
    QStringList list = dialog.selectedFiles();
    if (list.isEmpty()) return "";
    return list.first();
}

void openImageListWindow(QWidget* parent, const QList<cdsem::ImageViewListData>& list) {
    if (!MainThreadInvoker::getInvoker().isMainThread()) {
        MainThreadInvoker::getInvoker().runLater([parent, list](){
            openImageListWindow(parent, list);
        });
        return;
    }
    MsgReceiveStatus<RelationImageListController*> status = image_manager::openImageListController(list);
    if (!status.ok) {
        simpleMessageBox(parent, "Error", "Error to open image list window.");
    }
}

struct Locator {
    int index1;
    int index2;
};

MSR_MP_ExeResult createDataNotFoundResult() {
    MSR_MP_ExeResult r;
    r.errCode = DATA_NOT_FOUND_ERROR_CODE;
    r.errMsg = "Data not found.";
    return r;
}

MSR_MP_Chip* getMsrMpChipPtr(MSR_Chip& msr, const Locator& l) {
    if (l.index1 < 0 || l.index1 >= msr.mpList.size() || l.index2 < 0) {
        return nullptr;
    }
    return &msr.mpList[l.index1];
}

const MSR_MP_Chip* getMsrMpChipPtr(const MSR_Chip& msr, const Locator& l) {
    if (l.index1 < 0 || l.index1 >= msr.mpList.size() || l.index2 < 0) {
        return nullptr;
    }
    return &msr.mpList[l.index1];
}

const MSR_MP_Data* getMsrMpDataPtr(const MSR_Chip& msr, const Locator& l) {
    const MSR_MP_Chip* chip = getMsrMpChipPtr(msr, l);
    if (!chip) return nullptr;
    if (l.index2 >= chip->metaData.metaDatas.size()) {
        return nullptr;
    }
    auto it = chip->validIndices.find(l.index2);
    if (it == chip->validIndices.end()) {
        return nullptr;
    }
    int mListIndex = it.value();
    if (mListIndex < 0 || mListIndex >= chip->list.size()) {
        return nullptr;
    }
    return &chip->list[mListIndex];
}

MSR_MP_Data* getMsrMpDataPtr(MSR_Chip& msr, const Locator& l) {
    MSR_MP_Chip* chip = getMsrMpChipPtr(msr, l);
    if (!chip) return nullptr;
    if (l.index2 >= chip->metaData.metaDatas.size()) {
        return nullptr;
    }
    auto it = chip->validIndices.find(l.index2);
    if (it == chip->validIndices.end()) {
        return nullptr;
    }
    int mListIndex = it.value();
    if (mListIndex < 0 || mListIndex >= chip->list.size()) {
        return nullptr;
    }
    return &chip->list[mListIndex];
}

MSR_MP_ExeResult* getExeResult(MSR_Chip& msr, const Locator& l) {
    MSR_MP_Data* mpData = getMsrMpDataPtr(msr, l);
    if (!mpData) {
        return nullptr;
    }
    QMap<int, MSR_MP_ExeResult>& rs = mpData->mpResults;
    auto it2 = rs.find(l.index2);
    if (it2 == rs.end()) {
        return nullptr;
    }
    return &(*it2);
}

const MSR_MP_ExeResult* getExeResult(const MSR_Chip& msr, const Locator& l) {
    const MSR_MP_Data* mpData = getMsrMpDataPtr(msr, l);
    if (!mpData) {
        return nullptr;
    }
    const QMap<int, MSR_MP_ExeResult>& rs = mpData->mpResults;
    auto it2 = rs.find(l.index2);
    if (it2 == rs.end()) {
        return nullptr;
    }
    return &(*it2);
}

void recalcStats(WorkSheetGroupResultData& data) {
    if (data.list.isEmpty()) {
        data.summaryData.max = 0;
        data.summaryData.min = 0;
        data.summaryData.mean = 0;
        data.summaryData.sigma3 = 0;
        return;
    }
    FloatNumber3 min;
    FloatNumber3 max;
    FloatNumber3 tmp = 0;
    int total = 0;
    WorkSheetResultData* firstOne = nullptr;
    for (WorkSheetResultData& d : data.list) {
        if (d.pNo == -1 || d.result == nullptr || d.result < 0) continue;
        ++ total;
        if (min == nullptr || d.result < min) min = d.result;
        if (max == nullptr || d.result > max) max = d.result;
        tmp += d.result;
        if (firstOne == nullptr) firstOne = &d;
    }
    if (total == 0) {
        data.summaryData.max = 0;
        data.summaryData.min = 0;
        data.summaryData.mean = 0;
        data.summaryData.sigma3 = 0;
        return;
    } else if (total == 1) {
        data.summaryData.max = firstOne->result;
        data.summaryData.min = firstOne->result;
        data.summaryData.mean = firstOne->result;
        data.summaryData.sigma3 = 0;
        return;
    }
    FloatNumber3 avg;
    FloatNumber3 sigma;
    avg = tmp / total;
    tmp = 0;
    FloatNumber3 tmp2;
    for (WorkSheetResultData& d : data.list) {
        if (d.pNo == -1 || d.result == nullptr || d.result < 0) continue;
        tmp2 = (d.result - avg);
        tmp +=  tmp2 * tmp2;
    }
    tmp /= total;
    tmp = tmp.sqrt<>() * 3;
    data.summaryData.min = min;
    data.summaryData.max = max;
    data.summaryData.mean = avg;
    data.summaryData.sigma3 = tmp;
}

struct RepeatCheckKey {
    QString name;
    int dNo = -1;

    bool operator==(const RepeatCheckKey& other) const {
        if (this == &other) return true;
        return name == other.name && dNo == other.dNo;
    }

    bool operator<(const RepeatCheckKey& other) const {
        if (dNo == other.dNo) {
            return name < other.name;
        }
        return dNo < other.dNo;
    }
};

struct ChipViewData {
    QPoint view;
    int chipNumber;

    bool operator==(const ChipViewData& other) const {
        if (this == &other) return true;
        return chipNumber == other.chipNumber;
    }
};

inline size_t qHash(const RepeatCheckKey &key, size_t seed = 0) {
    return qHashMulti(seed, key.name, key.dNo);
}

inline size_t qHash(const ChipViewData &key, size_t seed = 0) {
    return qHash(key.chipNumber, seed);
}

struct ColValueData {
    QHash<int, Locator> locator;
    int col = -1;
};

struct WorksheetControllerDataHelper {
    MSR_Chip msrData;
    //数据定位器。
    //QPointe x: col, y: row
    //Locator index1:外层list下标 index2:内层metaData下标
    QHash<QPoint, Locator> locatorHash;
    //chipNumber -> row
    QHash<int, int> rowHash;
    //chipNumber高4 pNo 低4作为key，int作为msrData的mpList的下标索引
    //用于快速断定MSR_Chip中是否存在MSR_MP_Chip。
    //MSR_MP_Chip唯一标志为chipNumber + pNo.
    QHash<qint64, int> mpHash;
    //chipNumber = tableData.list index
    PLinkedHashNodeHash<RepeatCheckKey, ColValueData> colHash;

    QSet<QPoint> appendUpdatedPoints;

    WorksheetController* d;
    WorkSheetResultContent* worksheetContent = nullptr;
    CDE90InputBox* saveNameBox = nullptr;
    bool saveAs = false;

    void sendImage() {
        QPoint index = worksheetContent->getSelected();
        if (index.x() == -1 || index.y() == -1) return;
        bool succ = d->getAsyncCaller()->push(d, [this, index](TSC){
            Locator l = locatorHash.value(index, Locator{-1, -1});
            MSR_MP_Data* mpData = getMsrMpDataPtr(msrData, l);
            if (!mpData) return;
            cdsem::TmpImageData imageData;
            if (mpData->measureImagePath.length() > 0) {
                QString& path = mpData->measureImagePath;
                if (!cdsem::readImage(imageData, path)) {
                    simpleMessageBox(d, "Error", "Failed to load image data.");
                    return;
                }
            } else if (mpData->measureImageId > 0){
                quint64 id = mpData->measureImageId;
                ImageInfoService* service = ServiceManager::defaultService<ImageInfoService>();
                int errCode;
                ImageInfoEntity entity = service->getById(id, &errCode);
                if (errCode) {
                    simpleMessageBox(d, "Error", "Error to load image data");
                    return;
                }
                if (entity.getId() == 0) {
                    simpleMessageBox(d, "Error", "Image data is not exists");
                    return;
                }
                imageData.data = std::move(entity.getImageData());
                imageData.addition = std::move(entity.getImageInfo());
                imageData.size = QSize(entity.getWidth(), entity.getHeight());
                imageData.format = entity.getImageFormat();
            }
            if (imageData.addition.isEmpty()) {
                simpleMessageBox(d, "Error", "Image detail information is lost.");
                return;
            }
            ImageAddtionDataModel addition;
            int errCode = fromImageDeitalDataModelJson(imageData.addition, addition);
            if (errCode) {
                simpleMessageBox(d, "Error", "Image detail information format error.");
                return;
            }
            cdsem::CDSEM_GlobalDataHelper::get()->invokeImage([imageData, addition](){
                if (imageData.data.isEmpty()) return;
                //性能提示：把setImImage放在setImageOperationType之后可以让图像只发生一次更新，写反了会发生两次。
                cdsem::CDSEM_GlobalDataHelper::get()->setImage(IMG_OPT_TYPE_IM, imageData.data, imageData.size.width(), imageData.size.height(), imageData.format, addition);
                cdsem::CDSEM_GlobalDataHelper::get()->setImageOperationType(IMG_OPT_TYPE_IM);
                MainThreadInvoker::getInvoker().runLater([addition](){
                    IOPT->setPhotoImageType(addition.photoImagetype);
                    IOPT->setSemMag(addition.mag);
                });
            });
        });
        if (succ) {
            loadingBoxHelper(d, "Process", "Loading Image...")
        }
    }

    void openSaveNameBox(const QString& title, const QString& editTitle, const QString& initText) {
        if (!saveNameBox) {
            saveNameBox = new CDE90InputBox(d);
            saveNameBox->setInputLength(FILE_NAME_LENGTH);
            saveNameBox->setInputCallback([this](const QString& msrName){
                if (msrName.isEmpty()) {
                    simpleMessageBox(d, "Message", "Please input MSR name.");
                    return false;
                }
                doSaveData(msrName);
                return false;
            });
            saveNameBox->resize(300,200);
        }
        saveNameBox->show();
        saveNameBox->setWindowTitle(title);
        saveNameBox->setEditTitle(editTitle);
        saveNameBox->setInitText(initText);
    }

    QList<cdsem::ImageViewListData> collectAllApImages() {
        QList<cdsem::ImageViewListData> list;
        if (msrData.apList.isEmpty()) return list;
        list.reserve(msrData.apList.size());
        for (MSR_AP_Chip& c : msrData.apList) {
            if (c.apPatternResult.imagePath.isEmpty() && c.apPatternResult.imageId == 0) continue;
            list.append(cdsem::ImageViewListData {c.apPatternResult.imagePath, c.apPatternResult.imageId});
        }
        return list;
    }

    QList<cdsem::ImageViewListData> collectAllMpImages() {
        QList<cdsem::ImageViewListData> list;
        if (msrData.mpList.isEmpty()) return list;
        list.reserve(msrData.mpList.size() * 2);
        QSet<int> repeatCheck;
        for (MSR_MP_Chip& c : msrData.mpList) {
            int oIndex;
            for (auto it = c.validIndices.begin(); it != c.validIndices.end(); ++ it) {
                oIndex = it.value();
                if (oIndex < 0 || oIndex >= c.list.size()) continue;
                if (!repeatCheck.contains(oIndex)) {
                    repeatCheck.insert(oIndex);
                    MSR_MP_Data& mpData = c.list[oIndex];
                    if (mpData.addressPatternResults.size() > 0) {
                        for (MSR_PatternResult& addResult : mpData.addressPatternResults) {
                            if (addResult.imagePath.length() > 0 || addResult.imageId > 0) {
                                list.append(cdsem::ImageViewListData {addResult.imagePath, addResult.imageId});
                            }
                        }
                    }
                    if (mpData.mpPatternResult.imagePath.length() > 0 || mpData.mpPatternResult.imageId > 0) {
                        list.append(cdsem::ImageViewListData {mpData.mpPatternResult.imagePath, mpData.mpPatternResult.imageId});
                    }
                    if (mpData.measureImagePath.length() > 0 || mpData.measureImageId > 0) {
                        list.append(cdsem::ImageViewListData {mpData.measureImagePath, mpData.measureImageId});
                    }
                }
            }
            repeatCheck.clear();
        }
        return list;
    }

    QList<cdsem::ImageViewListData> collectAllImages() {
        QList<cdsem::ImageViewListData> list1 = collectAllApImages();
        QList<cdsem::ImageViewListData> list2 = collectAllMpImages();
        list1.reserve(list2.size() + list1.size());
        list1.append(list2);
        return list1;
    }

    void setModifyDataToTextField() {
        if (!MainThreadInvoker::getInvoker().isMainThread()) {
            MainThreadInvoker::getInvoker().runLater([this](){
                setModifyDataToTextField();
            });
            return;
        }
        QPoint index = worksheetContent->getSelected();
        if (index.x() == -1 || index.y() == -1) return;
        d->getAsyncCaller()->push(d, [this, index](TSC){
            Locator l = locatorHash.value(index, Locator{-1, -1});
            if (l.index1 == -1 || l.index2 == -1) return;
            MSR_MP_ExeResult* exeResult = getExeResult(msrData, l);
            if (!exeResult) {
                MainThreadInvoker::getInvoker().runLater([this](){
                    d->ui->infoLabel->clear();
                    d->ui->modifyBtn->setDisabled(true);
                });
                return;
            }
            MSR_MP_ExeResult& r = *exeResult;
            MainThreadInvoker::getInvoker().runLater([this, r](){
                d->ui->modifyBtn->setDisabled(false);
                d->ui->modifyTextField->setText(r.result.toMinString());
                if (r.errCode) {
                    d->ui->infoLabel->setTextWithElide(QString("%1:%2").arg(r.errCode).arg(r.errMsg));
                } else {
                    if (r.result != nullptr) {
                        d->ui->infoLabel->setTextWithElide(QString("%1nm").arg(r.result.toMinString()));
                    } else {
                        d->ui->infoLabel->clear();
                    }
                }
            });
        });
    }

    void modifyCellData() {
        QPoint index = worksheetContent->getSelected();
        if (index.x() == -1 || index.y() == -1) return;
        FloatNumber1 v = d->ui->modifyTextField->text();
        if (v == nullptr) return;
        d->ACaller->push(d, [this, index, v](TSC){
            Locator l = locatorHash.value(index, Locator{-1, -1});
            if (l.index1 == -1 || l.index2 == -1) return;
            MSR_MP_Chip* chip = getMsrMpChipPtr(msrData, l);
            if (!chip) return;
            auto rowIt = rowHash.find(chip->metaData.chipNumber);
            if (rowIt == rowHash.end()) return;
            MSR_MP_ExeResult* r = getExeResult(msrData, l);
            if (!r) return;
            r->result = v;
            r->errCode = 0;
            r->errMsg.clear();
            int row = *rowIt;
            int col;
            WorkSheetData& data = worksheetContent->getWorkSheetData();
            MSR_MP_MetaData& metaData = chip->metaData.metaDatas[l.index2];
            for (int& d : metaData.dNumbers) {
                RepeatCheckKey key {metaData.mpName, d};
                auto colIt = colHash.dataNode(key);
                if (!colIt) continue;
                col = colIt->value.col;
                WorkSheetGroupResultData& w = data.resultList[col / 2];
                WorkSheetResultData& rData = w.list[row];
                rData.result = r->result;
                if (rData.hasError) rData.hasError = false;
                worksheetContent->update(row, col);
                recalcStats(w);
                worksheetContent->updateSummaryData(col / 2, w.summaryData);
            }
            MainThreadInvoker::getInvoker().runLater([this, v](){
                d->ui->infoLabel->setTextWithElide(QString("%1nm").arg(v.toMinString()));
            });
        });
    }

    void localResetData(const MSR_Chip& data) {
        worksheetContent->clearData();
        locatorHash.clear();
        rowHash.clear();
        colHash.clear();
        mpHash.clear();
        msrData = data;
        //dNumber + name = result
        //result: chipNumber -> mp result
        //chipNumber = tableData.list index
        RepeatCheckKey key;
        QHash<int, int>::Iterator rowIt;
        QList<ChipViewData> chipNumbers;
        chipNumbers.reserve(10);
        for (int i = 0; i < data.mpList.size(); ++ i) {
            const MSR_MP_Chip& c = data.mpList[i];
            if (c.metaData.chipNumber < 0 || c.metaData.metaDatas.isEmpty()) continue;
            mpHash.insert(qint64(c.metaData.chipNumber) << 32 | c.metaData.pNum, i);
            for (int j = 0; j < c.metaData.metaDatas.size(); ++ j) {
                const MSR_MP_MetaData& metaData = c.metaData.metaDatas[j];
                if (metaData.dNumbers.isEmpty()) continue;
                rowIt = rowHash.find(c.metaData.chipNumber);
                for (const int& d : metaData.dNumbers) {
                    key = {metaData.mpName, d};
                    auto node = colHash.dataNode(key);
                    if (!node) {
                        colHash.append(key, ColValueData {}, &node);
                        node->getValue().col = (colHash.size() - 1) * 2;
                    }
                    // if (node->getValue().locator.contains(c.metaData.chipNumber)) {
                    //     qWarning() << "Chip: " << c.metaData.chipNumber << ", MP Name: " << metaData.mpName
                    //                << ", D No.:" << d << " same key will be covered at cell: ("
                    //                << node->value.col << "," << (rowIt == rowHash.end() ? chipNumbers.size() : *rowIt) << ").";
                    // }
                    node->getValue().locator.insert(c.metaData.chipNumber, Locator{i, j});
                }
                if (rowIt == rowHash.end()) {
                    rowIt = rowHash.insert(c.metaData.chipNumber, chipNumbers.size());
                    chipNumbers.append(ChipViewData{c.chipView, c.metaData.chipNumber});
                }
            }
        }
        int col = colHash.size();
        if (col <= 0) return;
        WorkSheetData& tableData = worksheetContent->getWorkSheetData();
        worksheetContent->beginUpdateColumnForAllNews(col * 2);
        WorkSheetGroupResultData resultData;
        resultData.list.reserve(chipNumbers.size());
        WorkSheetResultData result;
        QHash<int, Locator>::Iterator rIt;
        int colIndex = 0;
        for (auto node = colHash.first(); node != nullptr; node = node->next) {
            const RepeatCheckKey& key = node->key;
            ColValueData& value = node->getValue();
            value.col = colIndex;
            resultData.title = QString("D No.%1\n%2").arg(key.dNo).arg(key.name);
            QPoint local;
            for (int i = 0; i < chipNumbers.size(); ++ i) {
                ChipViewData& chip = chipNumbers[i];
                rIt = value.locator.find(chip.chipNumber);
                WorkSheetResultData d;
                if (rIt == value.locator.end()) {
                    d.pNo = -1;
                    d.result = 0;
                } else {
                    Locator& l = *rIt;
                    const MSR_MP_Chip& c = data.mpList[l.index1];
                    d.pNo = c.metaData.pNum;
                    auto valid = c.validIndices.find(l.index2);
                    if (valid == c.validIndices.end() || valid.value() < 0 || valid.value() >= c.list.size()) {
                        d.hasError = false;
                        d.result.clear();
                        d.hasImage = false;
                    } else {
                        const MSR_MP_Data& mpData = c.list[valid.value()];
                        d.hasImage = mpData.measureImagePath.length() > 0 || mpData.measureImageId > 0;
                        const MSR_MP_ExeResult& exeResult = mpData.mpResults[valid.key()];
                        d.result = exeResult.result;
                        d.hasError = exeResult.errCode;
                    }
                    local = QPoint(colIndex, i);
                    locatorHash.insert(local, l);
                }
                resultData.list.append(d);
            }
            recalcStats(resultData);
            tableData.resultList.append(resultData);
            resultData.summaryData = WorkSheetSummaryData {};
            resultData.list.clear();
            //一条数据对应两列，因此这里加2.
            colIndex += 2;
        }
        worksheetContent->endUpdateColumn();
        int row = rowHash.size();
        if (row <= 0) {
            worksheetContent->clearData();
            locatorHash.clear();
            //这个就不擦了，方便定位
            // dataHash.clear();
            return;
        }
        worksheetContent->beginUpdateRowForAllNews(row);
        tableData.list.reserve(chipNumbers.size());
        for (ChipViewData& chip : chipNumbers) {
            tableData.list.append(chip.view);
        }
        worksheetContent->endUpdateRow();
        setModifyDataToTextField();
    }

    bool localAppendData(const MSR_MP_Chip_MetaData& metaData, const MSR_MP_Data& data) {
        if (metaData.chipNumber < 0) return false;
        auto dit = mpHash.find(qint64(metaData.chipNumber) << 32 | metaData.pNum);
        if (dit == mpHash.end()) return false;
        auto rit = rowHash.find(metaData.chipNumber);
        if (rit == rowHash.end()) return false;

        int mIndex = *dit;
        int row = *rit;
        if (mIndex < 0 || mIndex >= msrData.mpList.size()) return false;
        if (row < 0 || row >= rowHash.size()) return false;
        MSR_MP_Chip& originData = msrData.mpList[mIndex];
        if (originData.metaData != metaData) return false;
        if (data.mpResults.isEmpty()) {
            originData.validIndices.clear();
        } else {
            for (auto it = data.mpResults.begin(); it != data.mpResults.end(); ++ it) {
                if (it.key() < 0 || it.key() >= metaData.metaDatas.size()) return false;
                const MSR_MP_MetaData& meta = metaData.metaDatas[it.key()];
                for (int i = 0; i < meta.dNumbers.size(); ++ i) {
                    RepeatCheckKey key{meta.mpName, meta.dNumbers[i]};
                    auto cit = colHash.dataNode(key);
                    if (cit == nullptr) return false;
                }
            }
            int col;
            WorkSheetData& tData = worksheetContent->getWorkSheetData();
            QPoint point;
            for (auto it = data.mpResults.begin(); it != data.mpResults.end(); ++ it) {
                originData.validIndices.insert(it.key(), originData.list.size());

                const MSR_MP_MetaData& meta = metaData.metaDatas[it.key()];
                for (int i = 0; i < meta.dNumbers.size(); ++ i) {
                    RepeatCheckKey key{meta.mpName, meta.dNumbers[i]};
                    auto cit = colHash.dataNode(key);
                    col = cit->value.col;

                    //修改值
                    WorkSheetGroupResultData& w = tData.resultList[col / 2];
                    WorkSheetResultData& rData = w.list[row];
                    rData.result = it->result;
                    rData.pNo = metaData.pNum;
                    rData.hasError = it->errCode;
                    rData.hasImage = data.measureImagePath.length() > 0 || data.measureImageId > 0;
                    point = QPoint(col, row);
                    locatorHash.insert(point, Locator {mIndex, it.key()});
                    worksheetContent->update(point.y(), point.x());
                    recalcStats(w);
                    worksheetContent->updateSummaryData(point.x() / 2, w.summaryData);
                    appendUpdatedPoints.insert(point);
                }
            }
            originData.list.append(data);
        }
        MainThreadInvoker::getInvoker().blockRun([this](){
            QPoint index = worksheetContent->getSelected();
            if (index.x() == -1 || index.y() == -1) return;
            if (appendUpdatedPoints.contains(index))
                setModifyDataToTextField();
        });
        appendUpdatedPoints.clear();
        return true;
    }

    void saveData() {
        MSR_Chip& msrData = this->msrData;
        if (msrData.fileName.isEmpty()) {
            QString title = saveAs ? "Save as..." : "Save";
            QString editTitle = msrData.msrId > 0 ? "Edit MSR" : "Create MSR";
            QString initText = "";
            openSaveNameBox(title, editTitle, initText);
        } else if (saveAs) {
            QString title = "Save as...";
            QString editTitle = "Create MSR";
            QString initText = msrData.fileName;
            openSaveNameBox(title, editTitle, initText);
        } else {
            doSaveData("");
        }
    }

    void doSaveData(const QString& newName) {
        MSR_Chip& msrData = this->msrData;
        bool saveAs = this->saveAs;
        bool succ = d->getAsyncCaller()->push(d, [this, msrData, newName, saveAs](TSC) mutable {
            if (newName.length() > 0) {
                msrData.fileName = newName;
            }
            if (saveAs) msrData.msrId = 0;
            QHash<QString, int> imagePathChecker;
            QList<FileImageDuplicateSaveData> fileList;
            QString path;
            for (MSR_AP_Chip& ap : msrData.apList) {
                if (ap.apPatternResult.imagePath.size() > 0) {
                    path = fixFilePath(ap.apPatternResult.imagePath);
                    if (imagePathChecker.contains(path)) continue;
                    imagePathChecker.insert(path, fileList.size());
                    FileImageDuplicateSaveData key;
                    key.path = path;
                    key.entity.setDataType(cdsem::IDT_AP_PATTERN_MATCH);
                    key.fileType = cdsem::FT_MSR_IMAGE_AP;
                    fileList.append(key);
                }
            }
            for (MSR_MP_Chip& mp : msrData.mpList) {
                for (MSR_MP_Data& mpData : mp.list) {
                    for (MSR_PatternResult& addResult : mpData.addressPatternResults) {
                        if (addResult.imageId <= 0 && addResult.imagePath.size() > 0) {
                            path = fixFilePath(addResult.imagePath);
                            if (imagePathChecker.contains(path)) continue;
                            imagePathChecker.insert(path, fileList.size());
                            FileImageDuplicateSaveData key;
                            key.path = path;
                            key.entity.setDataType(cdsem::IDT_MS_PATTERN_MATCH);
                            key.fileType = cdsem::FT_MSR_IMAGE_MS;
                            fileList.append(key);
                        }
                    }
                    if (mpData.mpPatternResult.imageId <= 0 && mpData.mpPatternResult.imagePath.size() > 0) {
                        path = fixFilePath(mpData.mpPatternResult.imagePath);
                        if (imagePathChecker.contains(path)) continue;
                        imagePathChecker.insert(path, fileList.size());
                        FileImageDuplicateSaveData key;
                        key.path = path;
                        key.entity.setDataType(cdsem::IDT_MP_PATTERN_MATCH);
                        key.fileType = cdsem::FT_MSR_IMAGE_MP;
                        fileList.append(key);
                    }
                    if (mpData.measureImageId <= 0 && mpData.measureImagePath.size() > 0) {
                        path = fixFilePath(mpData.measureImagePath);
                        if (imagePathChecker.contains(path)) continue;
                        imagePathChecker.insert(path, fileList.size());
                        FileImageDuplicateSaveData key;
                        key.path = path;
                        key.entity.setDataType(cdsem::IDT_MP_MEASUREMENT);
                        key.fileType = cdsem::FT_MSR_IMAGE_MEASURE;
                        fileList.append(key);
                    }
                }
            }
            TransactionObject imageTObj;
            sem::AfterGc gc([&imageTObj](){
                if (imageTObj.isOpenSucc()) imageTObj.rollback();
            });
            OperationResult oResult;
            if (imagePathChecker.size() > 0) {
                imageTObj = TransactionManagerService::beginFileTransaction();
                if (!imageTObj.isOpenSucc()) {
                    simpleMessageBox(d, "Error", "Error to open transaction for Image Saving.");
                    return;
                }
                ImageInfoService* imageService = ServiceManager::defaultService<ImageInfoService>();
                imageService->save(fileList, &oResult);
                if (oResult.errCode) {
                    simpleMessageBox(d, "Error", oResult.errMsg);
                    return;
                }
            }

            for (MSR_AP_Chip& ap : msrData.apList) {
                if (ap.apPatternResult.imagePath.size() > 0) {
                    path = fixFilePath(ap.apPatternResult.imagePath);
                    ap.apPatternResult.imageId = fileList[imagePathChecker[path]].entity.getId();
                }
            }
            for (MSR_MP_Chip& mp : msrData.mpList) {
                for (MSR_MP_Data& mpData : mp.list) {
                    for (MSR_PatternResult& addResult : mpData.addressPatternResults) {
                        if (addResult.imageId <= 0 && addResult.imagePath.size() > 0) {
                            path = fixFilePath(addResult.imagePath);
                            addResult.imageId = fileList[imagePathChecker[path]].entity.getId();
                        }
                    }
                    if (mpData.mpPatternResult.imageId <= 0 && mpData.mpPatternResult.imagePath.size() > 0) {
                        path = fixFilePath(mpData.mpPatternResult.imagePath);
                        mpData.mpPatternResult.imageId = fileList[imagePathChecker[path]].entity.getId();
                    }
                    if (mpData.measureImageId <= 0 && mpData.measureImagePath.size() > 0) {
                        path = fixFilePath(mpData.measureImagePath);
                        mpData.measureImageId = fileList[imagePathChecker[path]].entity.getId();
                    }
                }
            }

            MsrService* msrService = ServiceManager::defaultService<MsrService>();
            oResult = msrService->save(msrData);
            if (oResult.errCode) {
                if (imageTObj.isOpenSucc() && !imageTObj.rollback()) {
                    simpleMessageBox(d, "Error", QString("Error to save MSR data, and error to rollback transaction for images.\n%1").arg(oResult.errMsg));
                } else {
                    simpleMessageBox(d, "Error", QString("Error to save MSR data: %1").arg(oResult.errMsg));
                }
                return;
            } else {
                if (fileList.size() > 0) {
                    QList<quint64> allImageIds;
                    allImageIds.reserve(fileList.size());
                    for (FileImageDuplicateSaveData& f : fileList) {
                        allImageIds.append(f.entity.getId());
                    }
                    ImageRelationRecordService* imageRelatioService = ServiceManager::defaultService<ImageRelationRecordService>();
                    oResult.errCode = imageRelatioService->updateRelation(msrData.msrId, IRDT_MSR, allImageIds);
                    if (oResult.errCode) {
                        simpleMessageBox(d, "Error", "Error to save image relation data.");
                    } else {
                        if (imageTObj.commit()) {
                            MainThreadInvoker::getInvoker().blockRun([this, &msrData](){
                                simpleMessageBox(d, "Info", "Save MSR data successfully.");
                                this->msrData = msrData;
                                if (saveNameBox && saveNameBox->isVisible()) {
                                    saveNameBox->close();
                                }
                            });
                            return;
                        } else {
                            simpleMessageBox(d, "Error", "Error to commit MSR image transcaction.");
                        }
                    }
                } else {
                    MainThreadInvoker::getInvoker().blockRun([this, &msrData](){
                        simpleMessageBox(d, "Info", "Save MSR data successfully.");
                        this->msrData = msrData;
                        if (saveNameBox && saveNameBox->isVisible()) {
                            saveNameBox->close();
                        }
                    });
                    return;
                }
                oResult.errCode = msrService->realDeleteDataById(msrData.msrId);
                if (oResult.errCode) {
                    simpleMessageBox(d, "Error", "Error to delete MSR data when failed to save image data.");
                }
            }
        });
        if (succ) {
            loadingBoxHelper(d, "Processing", "Saving...")
        }
    }
};

WorksheetController::WorksheetController(QWidget* parent) : RelationWorksheetController(parent) {
    //如果你没有异步任务需求，请将connect清理掉，保留create和createAction函数
    connect(this, &WorksheetController::asyncCallerChanged, this, [this](){
        dataHelper = new WorksheetControllerDataHelper;
        dataHelper->d = this;
        create();
        createAction();
    }, Qt::SingleShotConnection);
}

WorksheetController::~WorksheetController() noexcept {
    if (ui) {
        delete ui;
    }
}

void WorksheetController::create() {
    ui = new cdsem::UiWorksheetController;
    setContent(new QWidget);
    ui->init(getContent());

    dataHelper->worksheetContent = new WorkSheetResultContent(ui->worksheetContent);
    ui->worksheetContent->layout()->addWidget(dataHelper->worksheetContent);
}

void WorksheetController::createAction() {

    if (xlsx_export::xlsxExportAvailable()) {
        connect(ui->exportToXlsx, &QAction::triggered, this, [this](){
            QString path = openFileDialog(this);
            if (path.isEmpty()) return;
            bool succ = ACaller->push(this, [this, path](TSC){
                MsgReceiveStatus<void> r = xlsx_export::exportMSR(dataHelper->msrData, path);
                if (!r.ok) {
                    simpleMessageBox(this, "Error", "Failed to export XLSX");
                    return;
                }
                simpleMessageBox(this, "Information", "Export XLSX complete.");
            });
            if (succ) {
                loadingBox(this, "Please wait", "Export data to XLSX");
            }
        });
    } else {
        ui->fileMenu->removeAction(ui->exportToXlsx);
    }

    CDSemNumberValidatorHelper::doubleFree<1>()
        .put(ui->modifyTextField)
        .autoFixMode();

    connect(ui->saveAct, &QAction::triggered, this, [this](){
        dataHelper->saveAs = false;
        dataHelper->saveData();
    });
    connect(ui->saveAsAct, &QAction::triggered, this, [this](){
        dataHelper->saveAs = true;
        dataHelper->saveData();
    });

    connect(ui->exitAct, &QAction::triggered, this, [this]() {
        changeCloseStatusOnce(HW_CLOSE);
        close();
    });

    connect(dataHelper->worksheetContent, &WorkSheetResultContent::selectionChanged, this, [this](const QItemSelection &selected, const QItemSelection &deselected){
        Q_UNUSED(selected)
        Q_UNUSED(deselected)
        dataHelper->setModifyDataToTextField();
    });

    connect(ui->modifyBtn->getQPushButton(), &QPushButton::clicked, this, [this](){
        dataHelper->modifyCellData();
    });

    connect(dataHelper->worksheetContent, &WorkSheetResultContent::doubleClicked, this, [this](const QModelIndex& mIndex){
        QPoint index(mIndex.column(), mIndex.row());
        if (index.x() == -1 || index.y() == -1) return;
        ACaller->push(this, [this, index](TSC){
            Locator l = dataHelper->locatorHash.value(index, Locator{-1, -1});
            MSR_MP_Data* mpData = getMsrMpDataPtr(dataHelper->msrData, l);
            if (!mpData) return;
            if (mpData->measureImagePath.length() > 0) {
                QString& path = mpData->measureImagePath;
                MainThreadInvoker::getInvoker().runLater([this, path, l](){
                    MsgReceiveStatus<RelationImageViewController*> status = image_manager::openImageViewController(path, l.index2);
                    if (!status.ok) {
                        simpleMessageBox(this, "Error", "Error to open image display window.");
                    }
                });
            } else if (mpData->measureImageId > 0){
                quint64 id = mpData->measureImageId;
                MainThreadInvoker::getInvoker().runLater([this, id, l](){
                    MsgReceiveStatus<RelationImageViewController*> status = image_manager::openImageViewController(id, l.index2);
                    if (!status.ok) {
                        simpleMessageBox(this, "Error", "Error to open image display window.");
                    }
                });
            }
        });
    });

    connect(ui->showApImageAct, &QAction::triggered, this, [this](){
        ACaller->push(this, [this](TSC){
            QList<cdsem::ImageViewListData> list = dataHelper->collectAllApImages();
            openImageListWindow(this, list);
        });
    });

    connect(ui->showMpImageAct, &QAction::triggered, this, [this](){
        ACaller->push(this, [this](TSC){
            QList<cdsem::ImageViewListData> list = dataHelper->collectAllMpImages();
            openImageListWindow(this, list);
        });
    });

    connect(ui->displayAllMpImageAct, &QAction::triggered, this, [this](){
        ACaller->push(this, [this](TSC){
            QList<cdsem::ImageViewListData> list = dataHelper->collectAllImages();
            openImageListWindow(this, list);
        });
    });

    connect(ui->sendImageAct, &QAction::triggered, this, [this](){
        dataHelper->sendImage();
    });
}

void WorksheetController::resetData(const MSR_Chip& data) {
    ACaller->stop(APP_GID);
    ACaller->stop(RESET_GID);
    ACaller->push(this, RESET_GID, [this, data](TSC cmd){
        if (cmd.isStop()) return;
        sem::AfterGc gc([this](){
            MainThreadInvoker::getInvoker().runLater([this](){
                ui->loadingPane->hide();
                ui->loadingPane->getLoadingHelper().block();
            });
        });
        MainThreadInvoker::getInvoker().runLater([this](){
            ui->loadingPane->show();
            ui->loadingPane->getLoadingHelper().exec();
        });
        dataHelper->localResetData(data);
    });
}

void WorksheetController::resetData(quint64 msrTopId) {
    ACaller->stop(APP_GID);
    ACaller->stop(RESET_GID);
    ACaller->push(this, RESET_GID, [this, msrTopId](TSC cmd){
        if (cmd.isStop()) return;
        sem::AfterGc gc([this](){
            MainThreadInvoker::getInvoker().runLater([this](){
                ui->loadingPane->hide();
                ui->loadingPane->getLoadingHelper().block();
            });
        });
        MainThreadInvoker::getInvoker().runLater([this](){
            ui->loadingPane->show();
            ui->loadingPane->getLoadingHelper().exec();
        });
        MsrService* service = ServiceManager::defaultService<MsrService>();
        MSR_Chip data;
        OperationResult result = service->getByTopId(msrTopId, data);
        if (result.errCode) {
            simpleMessageBox(this, "Error", result.errMsg);
            return;
        }
        if (cmd.isStop()) return;
        dataHelper->localResetData(data);
    });
}

//发生数据移除时，全量更新数据、
//数据原样覆盖时，仅更新数据
//数据新增时，增量更新
//本函数并不保证appendData后保存再载入顺序是一致的。
void WorksheetController::appendData(const MSR_MP_Chip_MetaData& metaData, const MSR_MP_Data& data, const sem::ONE_PARAM_ARG_FUNC<bool>& callback) {
    ACaller->push(this, APP_GID, [this, metaData, data, callback](TSC cmd){
        if (cmd.isStop()) return;
        sem::AfterGc gc([this](){
            MainThreadInvoker::getInvoker().runLater([this](){
                ui->loadingPane->hide();
                ui->loadingPane->getLoadingHelper().block();
            });
        });
        MainThreadInvoker::getInvoker().runLater([this](){
            ui->loadingPane->show();
            ui->loadingPane->getLoadingHelper().exec();
        });
        bool succ = dataHelper->localAppendData(metaData, data);
        if (callback) callback(succ);
    });
}
