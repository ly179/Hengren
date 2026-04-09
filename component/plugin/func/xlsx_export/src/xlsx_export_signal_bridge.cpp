#include "xlsx_export_signal_bridge.h"
#ifdef _XLSX_EXP_TARGET
#include "plugin_version_util.h"
#include "exception_code.h"
#include "sem_common_data_model.h"
#include "plugin_signal_hub.h"
#include "signal_hub_plugin_ids.h"

#include "msr_data_model.h"
#include "xlsxdocument.h"

#include "service_manager.h"
#include "image_info_service.h"
#include "idw_wafer_service.h"
#include "idw_model_data_convert.h"
#include "idw_shot_chip_pos_helper.h"
#include "image_file_temp_handler.h"
#include "image_operation_image_canvas.h"
#include "image_save_model.h"
#include "image_save_model_convert.h"
#include "private/msr_export_image_drawer.h"

struct RepeatCheckKey {
    QString name;
    int dNo = -1;

    bool operator==(const RepeatCheckKey& other) const {
        if (this == &other) return true;
        return name == other.name && dNo == other.dNo;
    }

    bool operator!=(const RepeatCheckKey& other) const {
        if (this == &other) return false;
        return name != other.name || dNo != other.dNo;
    }
};

inline size_t qHash(const RepeatCheckKey &key, size_t seed = 0) {
    return qHashMulti(seed, key.name, key.dNo);
}

struct FullColData {
    QList<FloatNumber3> results;
    int col;
};

struct ImageColData {
    FloatNumber3 data;
    QString imagePath;
    quint64 imageId = 0;
    int metaIndex = -1;
    bool hasError = false;
};

struct StatisticsColData {
    FloatNumber3 total;
    FloatNumber3 min;
    FloatNumber3 max;
    FloatNumber3 mean;
    FloatNumber3 sigma3;
    int count;
};

const MSR_MP_Data* getValidData(const MSR_MP_Chip& chip, int metaIndex) {
    auto it = chip.validIndices.find(metaIndex);
    if (it == chip.validIndices.end() || *it < 0 || *it >= chip.list.size()) return nullptr;
    return &chip.list[*it];
}
const MSR_MP_ExeResult* getExeResult(const MSR_MP_Data* data, int metaIndex) {
    if (!data) return nullptr;
    auto it2 = data->mpResults.find(metaIndex);
    if (it2 == data->mpResults.end()) return nullptr;
    return &(*it2);
}
inline const MSR_MP_ExeResult* getExeResult(const MSR_MP_Chip& chip, int metaIndex) {
    return getExeResult(getValidData(chip, metaIndex), metaIndex);
}

bool preparedIDWData(quint64 idwId, IDW_ShotChipPosHelper& ruler) {
    ruler.getWaferData().reset(new IDW_WaferData);
    ruler.getShotData().reset(new IDW_ShotChipData);
    ruler.getChipData().reset(new IDW_ShotChipData);
    IDW_WaferService* service = ServiceManager::defaultService<IDW_WaferService>();
    int errCode;
    IDW_WaferInfoEntity entity = service->getWaferInfoById(idwId, &errCode);
    if (errCode || entity.getId() == 0) return false;
    errCode = waferDataFromEntity(entity, *ruler.getWaferData(), *ruler.getShotData(), *ruler.getChipData());
    if (errCode) return false;
    return true;
}

void prepareIDWRuler(IDW_ShotChipPosHelper& rulerHelper) {
    const QList<IDW_Data_Item>& shotItems = *rulerHelper.getShotData()->getDataList();
    const QList<IDW_Data_Item>& chipItems = *rulerHelper.getChipData()->getDataList();
    int chipIndex;
    int chipNumPerShot = rulerHelper.getChipData()->getArrayX() * rulerHelper.getChipData()->getArrayY();
    IDW_FloatNumber tmpChipX, tmpChipY;
    for (int i = 0; i < shotItems.size(); ++ i) {
        const IDW_Data_Item& shotItem = shotItems[i];
        for (int j = 0; j < chipItems.size(); ++ j) {
            const IDW_Data_Item& chipItem = chipItems[j];
            chipIndex = shotItem.getNumber() * chipNumPerShot + chipItem.getNumber();
            tmpChipX = shotItem.getOffsetX() + chipItem.getOffsetX();
            tmpChipY = shotItem.getOffsetY() + chipItem.getOffsetY();
            rulerHelper.setChipColumnRulerData(new ShotChipComparator(tmpChipY), chipIndex);
            rulerHelper.setChipRowRulerData(new ShotChipComparator(tmpChipX), chipIndex);
        }
    }
    rulerHelper.updateRuler(nullptr);
    rulerHelper.calcInvaldNumbers();
}

void prepareIDWExcelRowColumnSize(const IDW_ShotChipPosHelper& rulerHelper, QXlsx::Document& doc) {
    QXlsx::Format format;
    format.setBorderColor(Qt::black);
    format.setBorderStyle(QXlsx::Format::BorderMedium);
    format.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
    format.setVerticalAlignment(QXlsx::Format::AlignVCenter);
    format.setFontSize(16);
    format.setFontBold(true);
    QPoint oPoint = rulerHelper.getOriginChipNumberPoint();
    //ruler中的row对应表格的col
    const QList<ShotChipComparePack>& rowList = rulerHelper.getChipRowList();
    for (int i = 0; i < rowList.size(); ++ i) {
        doc.setColumnWidth(i + 2, 28.5);
        doc.write(1, i + 2, i - oPoint.x(), format);
    }
    const QList<ShotChipComparePack>& colList = rulerHelper.getChipColumnList();

    const QSet<qint32>& invalidChips = rulerHelper.getInvalidChipNumbers();

    QXlsx::Format tmpFormat;
    QVariant emptyData;
    //ruler中的col对应表格的row
    bool hasValidChip;
    for (int i = 0; i < colList.size(); ++ i) {
        doc.mergeCells(QXlsx::CellRange(i * 3 + 2, 1, i * 3 + 4, 1), format);
        doc.setRowHeight(i * 3 + 2, 159);
        doc.write(i* 3 + 2, 1, colList.size() - i - 1 - oPoint.y(), format);

        //表格内容线条加粗，如果不喜欢，这个for循环去掉即可。
        for (int j = 0; j < rowList.size(); ++ j) {
            const QList<qint32>& chipNs = rulerHelper.findChipNumberByRuler(QPoint(j, colList.size() - i - 1));
            if (chipNs.isEmpty()) continue;
            hasValidChip = false;
            for (const qint32& n : chipNs) {
                if (invalidChips.contains(n)) {
                    continue;
                }
                hasValidChip = true;
                break;
            }
            if (!hasValidChip) continue;

            tmpFormat = QXlsx::Format ();
            tmpFormat.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
            tmpFormat.setVerticalAlignment(QXlsx::Format::AlignVCenter);
            tmpFormat.setLeftBorderColor(Qt::black);
            tmpFormat.setLeftBorderStyle(QXlsx::Format::BorderMedium);
            tmpFormat.setTopBorderColor(Qt::black);
            tmpFormat.setTopBorderStyle(QXlsx::Format::BorderMedium);
            tmpFormat.setRightBorderColor(Qt::black);
            tmpFormat.setRightBorderStyle(QXlsx::Format::BorderMedium);
            doc.write(i * 3 + 2, j + 2, emptyData, tmpFormat);
            tmpFormat = QXlsx::Format ();
            tmpFormat.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
            tmpFormat.setVerticalAlignment(QXlsx::Format::AlignVCenter);
            tmpFormat.setLeftBorderColor(Qt::black);
            tmpFormat.setLeftBorderStyle(QXlsx::Format::BorderMedium);
            tmpFormat.setRightBorderColor(Qt::black);
            tmpFormat.setRightBorderStyle(QXlsx::Format::BorderMedium);
            doc.write(i * 3 + 3, j + 2, emptyData, tmpFormat);
            tmpFormat = QXlsx::Format ();
            tmpFormat.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
            tmpFormat.setVerticalAlignment(QXlsx::Format::AlignVCenter);
            tmpFormat.setLeftBorderColor(Qt::black);
            tmpFormat.setLeftBorderStyle(QXlsx::Format::BorderMedium);
            tmpFormat.setBottomBorderColor(Qt::black);
            tmpFormat.setBottomBorderStyle(QXlsx::Format::BorderMedium);
            tmpFormat.setRightBorderColor(Qt::black);
            tmpFormat.setRightBorderStyle(QXlsx::Format::BorderMedium);
            doc.write(i * 3 + 4, j + 2, emptyData, tmpFormat);
        }
    }
}

void exportImage(QXlsx::Document& doc, const QHash<int, ImageColData>& imageHash, const IDW_ShotChipPosHelper& rulerHelper) {
    ImageInfoService* service = ServiceManager::defaultService<ImageInfoService>();
    QPoint oPoint = rulerHelper.getOriginChipNumberPoint();
    int colSize = rulerHelper.getChipColumnList().size();
    cdsem::TmpImageData tmpData;
    ImageInfoEntity entity;
    ImageAddtionDataModel addition;
    bool succ;
    //带有覆盖图册的最终图
    QImage cache;
    //打底图，即原始拍摄图
    QImage image;
    QImage* outputImage;
    int errCode;
    QVariant emptyData;
    QXlsx::Format errFormat;
    errFormat.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
    errFormat.setVerticalAlignment(QXlsx::Format::AlignVCenter);
    errFormat.setRightBorderColor(Qt::black);
    errFormat.setRightBorderStyle(QXlsx::Format::BorderMedium);
    errFormat.setPatternBackgroundColor(Qt::red);

    ImageOperationImageCanvas* canvas = new ImageOperationImageCanvas;
    sem::AutoGc<ImageOperationImageCanvas*> gc(canvas);
    const QString* additionStr;
    for (auto it = imageHash.begin(); it != imageHash.end(); ++ it) {
        const QPoint& pos = rulerHelper.findChipRulerPos(it.key(), &succ);
        if (!succ) continue;
        if (it->imagePath.length() > 0) {
            succ = cdsem::readImage(tmpData, it->imagePath);
            if (!succ) continue;
            image = QImage(reinterpret_cast<const uchar*>(tmpData.data.constData()), tmpData.size.width(), tmpData.size.height(), QImage::Format(tmpData.format));
            additionStr = &tmpData.addition;
        } else if (it->imageId > 0) {
            entity = service->getById(it->imageId, &errCode);
            if (errCode) continue;
            if (entity.getId() == 0) continue;
            image = QImage(reinterpret_cast<const uchar*>(entity.getImageData().constData()), entity.getWidth(), entity.getHeight(), QImage::Format(entity.getImageFormat()));
            additionStr = &entity.getImageInfo();
        } else {
            image = QImage();
            additionStr = nullptr;
        }
        if (image.isNull()) continue;
        outputImage = &image;
        if (additionStr != nullptr) {
            addition = ImageAddtionDataModel{};
            if (!fromImageDeitalDataModelJson(*additionStr, addition)
                && drawMeasureOverlayCursors(addition, canvas, image, it->metaIndex, cache)) {
                outputImage = &cache;
                canvas->clearAndDeleteMeasurementTools();
            }
        }

        //图像统一以5cm导出。
        //5cm要放下图像的所有像素，因此每米放多少像素
        //1m是5cm的20倍，因此x20。
        //注意，insert Image是从0开始的。
        //这是QXlsx一个奇怪的问题，总之按照QXlsx的规则来
        outputImage->setDotsPerMeterX(outputImage->width() * 20);
        outputImage->setDotsPerMeterY(outputImage->height() * 20);
        doc.insertImage((colSize - pos.y() - 1) * 3 + 1, pos.x() + 1, *outputImage);
        if (it->hasError) {
            if (it->data != nullptr)
                doc.write((colSize - pos.y() - 1) * 3 + 3, pos.x() + 2, QString("%1nm").arg(it->data.toMinString()), errFormat);
            else
                doc.write((colSize - pos.y() - 1) * 3 + 3, pos.x() + 2, emptyData, errFormat);
        } else {
            if (it->data != nullptr)
                doc.write((colSize - pos.y() - 1) * 3 + 3, pos.x() + 2, QString("%1nm").arg(it->data.toMinString()));
        }
        doc.write((colSize - pos.y() - 1) * 3 + 4, pos.x() + 2, QString("%1, %2").arg(pos.x() - oPoint.x()).arg(pos.y() - oPoint.y()));
    }

}

void exportMSR_to_XLSX(const MSR_Chip& data, const QString& path) {

    //xlsx write 从1，1开始，insertImage从0，0开始。
    QXlsx::Document doc;
    doc.renameSheet(doc.currentSheet()->sheetName(), "Summary");
    //chipNumber -> row
    //row从1开始
    QHash<int, int> rowHash;
    //mp/d -> col
    //col从1开始
    QHash<RepeatCheckKey, FullColData> colHash;
    QHash<RepeatCheckKey, QHash<int, ImageColData>> colImageHash;
    RepeatCheckKey repeatKey;
    QHash<int, int>::Iterator rowIt;
    QHash<RepeatCheckKey, FullColData>::Iterator colIt;
    int row, col;
    QXlsx::Format errFormat;
    errFormat.setPatternBackgroundColor(Qt::red);

    QXlsx::Format centerFormat;
    centerFormat.setVerticalAlignment(QXlsx::Format::AlignVCenter);
    centerFormat.setHorizontalAlignment(QXlsx::Format::AlignHCenter);

    QXlsx::Format centerWithTopBorderFormat = centerFormat;
    centerWithTopBorderFormat.setTopBorderColor(Qt::black);
    centerWithTopBorderFormat.setTopBorderStyle(QXlsx::Format::BorderMedium);

    QXlsx::Format centerWithBottomBorderFormat = centerFormat;
    centerWithBottomBorderFormat.setBottomBorderColor(Qt::black);
    centerWithBottomBorderFormat.setBottomBorderStyle(QXlsx::Format::BorderMedium);

    QXlsx::Format topBorderFormat;
    topBorderFormat.setTopBorderColor(Qt::black);
    topBorderFormat.setTopBorderStyle(QXlsx::Format::BorderMedium);

    QXlsx::Format leftBorderFormat;
    leftBorderFormat.setLeftBorderColor(Qt::black);
    leftBorderFormat.setLeftBorderStyle(QXlsx::Format::BorderMedium);

    QXlsx::Format rightBorderFormat;
    rightBorderFormat.setRightBorderColor(Qt::black);
    rightBorderFormat.setRightBorderStyle(QXlsx::Format::BorderMedium);

    QXlsx::Format tmpFormat;
    ImageColData imageColData;

    for (const MSR_MP_Chip& chip : data.mpList) {
        rowIt = rowHash.find(chip.metaData.chipNumber);
        if (rowIt == rowHash.end()) {
            //row从1开始所以先要+1，但是第一行不放任何数据所以再+1，而后D.No/MpName/DataPno.标题各占一行所以一共加5
            rowIt = rowHash.insert(chip.metaData.chipNumber, rowHash.size() + 5);
            tmpFormat = leftBorderFormat;
            tmpFormat.setRightBorderColor(Qt::black);
            tmpFormat.setRightBorderStyle(QXlsx::Format::BorderMedium);
            doc.write(*rowIt, 2, rowHash.size(), tmpFormat);
            doc.write(*rowIt, 3, QString("%1, %2").arg(chip.chipView.x()).arg(chip.chipView.y()), rightBorderFormat);
        }
        row = *rowIt;
        const QList<MSR_MP_MetaData>& metaList = chip.metaData.metaDatas;
        for (int index = 0; index < metaList.size(); ++ index) {
            const MSR_MP_MetaData& m = metaList[index];
            const MSR_MP_Data* mpData = getValidData(chip, index);
            const MSR_MP_ExeResult* result = getExeResult(mpData, index);
            for (const int& d : m.dNumbers) {
                repeatKey.dNo = d;
                repeatKey.name = m.mpName;
                colIt = colHash.find(repeatKey);
                if (colIt == colHash.end()) {
                    //col从1开始所以先加1，但是第一列不放任何数据所以再+1
                    //No.和ChipNo.各占1列所以一共加4
                    colIt = colHash.insert(repeatKey, FullColData {QList<FloatNumber3>(), int(colHash.size() * 2 + 4)});
                    tmpFormat = centerWithTopBorderFormat;
                    tmpFormat.setRightBorderColor(Qt::black);
                    tmpFormat.setRightBorderStyle(QXlsx::Format::BorderMedium);
                    doc.mergeCells(QXlsx::CellRange(2, colIt->col, 2, colIt->col + 1), tmpFormat);
                    doc.mergeCells(QXlsx::CellRange(3, colIt->col, 3, colIt->col + 1), rightBorderFormat);
                    doc.write(2, colIt->col, QString("D No.%1").arg(d), tmpFormat);
                    doc.write(3, colIt->col, m.mpName, centerFormat);
                    doc.write(4, colIt->col, "Data", centerWithBottomBorderFormat);
                    tmpFormat = centerWithBottomBorderFormat;
                    tmpFormat.setRightBorderColor(Qt::black);
                    tmpFormat.setRightBorderStyle(QXlsx::Format::BorderMedium);
                    doc.write(4, colIt->col + 1, "P No.", tmpFormat);
                    doc.setColumnWidth(colIt->col + 1, 5);
                    colIt->results.reserve(std::min(rowHash.size(), qsizetype(10)));
                }
                if (!result) {
                    continue;
                }
                col = colIt->col;
                if (result->errCode) {
                    if (result->result != nullptr && result->result >= 0) {
                        doc.write(row, col, result->result.toMinString(), errFormat);
                        colIt->results.append(result->result);
                    } else {
                        doc.write(row, col, "Error", errFormat);
                    }
                } else {
                    doc.write(row, col, result->result.toMinString());
                    colIt->results.append(result->result);
                }
                doc.write(row, col + 1, chip.metaData.pNum, rightBorderFormat);
                if (mpData->measureImageId > 0 || mpData->measureImagePath.length() > 0) {
                    imageColData.imagePath = mpData->measureImagePath;
                    imageColData.imageId = mpData->measureImageId;
                    imageColData.hasError = result->errCode;
                    imageColData.metaIndex = index;
                    if (result->result != nullptr && result->result >= 0) {
                        imageColData.data = result->result;
                    } else if (imageColData.data != nullptr) {
                        imageColData.data.clear();
                    }
                    colImageHash[repeatKey].insert(chip.metaData.chipNumber, imageColData);
                }
            }
        }
    }
    if (rowHash.size() > 0) {

        tmpFormat = centerWithTopBorderFormat;
        tmpFormat.setLeftBorderColor(Qt::black);
        tmpFormat.setLeftBorderStyle(QXlsx::Format::BorderMedium);
        tmpFormat.setBottomBorderColor(Qt::black);
        tmpFormat.setBottomBorderStyle(QXlsx::Format::BorderMedium);
        tmpFormat.setRightBorderColor(Qt::black);
        tmpFormat.setRightBorderStyle(QXlsx::Format::BorderMedium);

        doc.mergeCells(QXlsx::CellRange(2, 2, 4, 2), tmpFormat);
        doc.write(2, 2, "No.", tmpFormat);

        tmpFormat = centerWithTopBorderFormat;
        tmpFormat.setBottomBorderColor(Qt::black);
        tmpFormat.setBottomBorderStyle(QXlsx::Format::BorderMedium);
        tmpFormat.setRightBorderColor(Qt::black);
        tmpFormat.setRightBorderStyle(QXlsx::Format::BorderMedium);
        doc.mergeCells(QXlsx::CellRange(2, 3, 4, 3), tmpFormat);
        doc.write(2, 3, "Chip No.", tmpFormat);
        doc.setColumnWidth(2, 5);

        QHash<int, StatisticsColData> totalHash;
        for (auto it = colHash.begin(); it != colHash.end(); ++ it) {
            for (FloatNumber3& f : it->results) {
                auto sit = totalHash.find(it->col);
                if (sit == totalHash.end()) {
                    StatisticsColData d;
                    d.total = f;
                    d.count = 1;
                    d.min = f;
                    d.max = f;
                    totalHash.insert(it->col, d);
                } else {
                    ++ sit->count;
                    sit->total += f;
                    if (f < sit->min) {
                        sit->min = f;
                    }
                    if (f > sit->max) {
                        sit->max = f;
                    }
                }
            }
        }




        for (auto it = totalHash.begin(); it != totalHash.end(); ++ it) {
            if (it->count <= 1) {
                it->mean = it->total;
                it->sigma3 = 0;
                continue;
            }
            it->mean = it->total / it->count;
        }

        FloatNumber3 tmp, tmp2;
        for (auto it = colHash.begin(); it != colHash.end(); ++ it) {
            if (it->results.size() < 1) continue;
            auto tit = totalHash.find(it->col);
            if (tit == totalHash.end() || tit->count <= 1) continue;
            tmp = 0;
            for (FloatNumber3& f : it->results) {
                tmp2 = f - tit->mean;
                tmp += tmp2 * tmp2;
            }
            tmp /= tit->count;
            tit->sigma3 = tmp.sqrt<>() * 3;
        }
        row = rowHash.size() + 5;
        {
            tmpFormat = topBorderFormat;
            tmpFormat.setLeftBorderColor(Qt::black);
            tmpFormat.setLeftBorderStyle(QXlsx::Format::BorderMedium);
            tmpFormat.setRightBorderColor(Qt::black);
            tmpFormat.setRightBorderStyle(QXlsx::Format::BorderMedium);
            tmpFormat.setVerticalAlignment(QXlsx::Format::AlignVCenter);
            tmpFormat.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
            doc.mergeCells(QXlsx::CellRange(row, 2, row, 3), tmpFormat);
            doc.write(row, 2, "Maximum", tmpFormat);
        }

        tmpFormat = centerFormat;
        tmpFormat.setLeftBorderColor(Qt::black);
        tmpFormat.setLeftBorderStyle(QXlsx::Format::BorderMedium);
        tmpFormat.setRightBorderColor(Qt::black);
        tmpFormat.setRightBorderStyle(QXlsx::Format::BorderMedium);

        QVariant emptyData;
        doc.mergeCells(QXlsx::CellRange(row + 1, 2, row + 1, 3), tmpFormat);
        doc.write(row + 1, 2, "Minimum", tmpFormat);
        doc.mergeCells(QXlsx::CellRange(row + 2, 2, row + 2, 3), tmpFormat);
        doc.write(row + 2, 2, "Mean", tmpFormat);
        {
            tmpFormat.setBottomBorderColor(Qt::black);
            tmpFormat.setBottomBorderStyle(QXlsx::Format::BorderMedium);
            doc.mergeCells(QXlsx::CellRange(row + 3, 2, row + 3, 3), tmpFormat);
            doc.write(row + 3, 2, "3 Sigma", tmpFormat);
        }
        for (auto it = colHash.begin(); it != colHash.end(); ++ it) {
            tmpFormat = topBorderFormat;
            tmpFormat.setRightBorderColor(Qt::black);
            tmpFormat.setRightBorderStyle(QXlsx::Format::BorderMedium);
            doc.mergeCells(QXlsx::CellRange(row, it->col, row, it->col + 1), tmpFormat);
            auto dit = totalHash.find(it->col);
            if (dit == totalHash.end() || dit->max == nullptr) {
                doc.write(row, it->col, emptyData, tmpFormat);
            } else {
                doc.write(row, dit.key(), dit->max.toMinString(), tmpFormat);
            }

            doc.mergeCells(QXlsx::CellRange(row + 1, it->col, row + 1, it->col + 1), rightBorderFormat);
            if (dit == totalHash.end() || dit->min == nullptr) {
                doc.write(row + 1, it->col, emptyData, rightBorderFormat);
            } else {
                doc.write(row + 1, dit.key(), dit->min.toMinString(), rightBorderFormat);
            }

            doc.mergeCells(QXlsx::CellRange(row + 2, it->col, row + 2, it->col + 1), rightBorderFormat);
            if (dit == totalHash.end() || dit->mean == nullptr) {
                doc.write(row + 2, it->col, emptyData, rightBorderFormat);
            } else {
                doc.write(row + 2, dit.key(), dit->mean.toMinString(), rightBorderFormat);
            }
            tmpFormat = QXlsx::Format ();
            tmpFormat.setBottomBorderColor(Qt::black);
            tmpFormat.setBottomBorderStyle(QXlsx::Format::BorderMedium);
            tmpFormat.setRightBorderColor(Qt::black);
            tmpFormat.setRightBorderStyle(QXlsx::Format::BorderMedium);
            doc.mergeCells(QXlsx::CellRange(row + 3, it->col, row + 3, it->col + 1), tmpFormat);
            if (dit == totalHash.end() || dit->sigma3 == nullptr) {
                doc.write(row + 3, it->col, emptyData, tmpFormat);
            } else {
                doc.write(row + 3, dit.key(), dit->sigma3.toMinString(), tmpFormat);
            }
        }
    }

    if (colImageHash.size() > 0) {
        IDW_ShotChipPosHelper ruler;
        if (preparedIDWData(data.idwId, ruler)) {
            prepareIDWRuler(ruler);
            QString sheetName;
            for (auto it = colImageHash.begin(); it != colImageHash.end(); ++ it) {
                sheetName = QString("D No.%1, %2").arg(it.key().dNo).arg(it.key().name);
                doc.addSheet(sheetName);
                doc.selectSheet(sheetName);
                prepareIDWExcelRowColumnSize(ruler, doc);
                exportImage(doc, *it, ruler);
            }
        }
    }

    doc.selectSheet(0);
    doc.saveAs(path);
}

SIGNAL_CHECK(_XLSX_EXP_TARGET) {
    Q_UNUSED(sysVersion)
#ifdef _XLSX_EXP_VERSION_
    bool succ = readVersion(_XLSX_EXP_VERSION_, *libVersion);
    if (succ) {
        result->errCode = 0;
        result->errMsg.clear();
        //TODO 未来可添加版本号处理机制
    } else {
        result->errCode = LIBRARY_VERSION_READ_ERROR_CODE;
        result->errMsg = "Xlsx Export version is not defined.";
    }
#else
#warning "XLSX Export component version is not defined."
    result->errCode = 0;
    result->errMsg.clear();
#endif
}

SIGNAL_REG(_XLSX_EXP_TARGET) {
    Q_UNUSED(sysVersion)
    PluginSignalHub::instance()->registSlot<void, const MSR_Chip&, const QString&>(MSR_XLSX_EXPORT,
        PluginHubAnyFunc<void, const MSR_Chip&, const QString&>(
            [](const MSR_Chip& data, const QString& path){
                exportMSR_to_XLSX(data, path);
            })
        );
    if (result) {
        result->errCode = 0;
        result->errMsg.clear();
    }
}

SIGNAL_UNREG(_XLSX_EXP_TARGET) {
    QString pid = QString::fromLatin1(pluginId);
    PluginSignalHub::instance()->unRegistAll(MSR_XLSX_EXPORT, [pid, callback](){
        callback(pid.toLatin1());
    });
}

XLSX_EXPORT_SIGNAL_BRIDGE_H
#endif //_XLSX_EXP_TARGET
