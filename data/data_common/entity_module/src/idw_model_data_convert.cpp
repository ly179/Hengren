#include "idw_model_data_convert.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include "model_data_convert_helper.h"
#include "idw_wafer_data_model.h"
#include "idw_wafer_entity.h"

int shotChipFromJsonDocument(const QJsonObject& obj, IDW_ShotChipData& data) {
    QJsonValue value = obj["array_x"];
    int val;
    int errCode = jValToInt(val, value);
    if (errCode) {
        return errCode;
    }
    data.setArrayX(val);
    value = obj["array_y"];
    errCode = jValToInt(val, value);
    if (errCode) {
        return errCode;
    }
    data.setArrayY(val);
    value = obj["direction"];
    errCode = jValToInt(val, value);
    if (errCode) {
        return errCode;
    }
    data.setDirection(val);
    value = obj["size_x"];
    errCode = jValToNumber(data.getSizeX(), value, "0");
    if (errCode) {
        return errCode;
    }
    value = obj["size_y"];
    errCode = jValToNumber(data.getSizeY(), value, "0");
    if (errCode) {
        return errCode;
    }
    value = obj["pitch_x"];
    errCode = jValToNumber(data.getPitchX(), value, "0");
    if (errCode) {
        return errCode;
    }
    value = obj["pitch_y"];
    errCode = jValToNumber(data.getPitchY(), value, "0");
    if (errCode) {
        return errCode;
    }
    value = obj["items"];
    if (!value.isArray()) {
        return JSON_CONVERT_TYPE_ERR_CODE;
    }
    QJsonArray array = value.toArray();
    if (array.size() > 0) {
        if (!data.getDataList()) {
            data.setDataList(new QList<IDW_Data_Item>(array.size()));
        }
        QList<IDW_Data_Item>& list = *data.getDataList();
        list.resize(array.size());
        for (int i = 0; i < array.size(); ++i) {
            QJsonValue valItem = array[i];
            if (!valItem.isObject()) {
                return JSON_CONVERT_TYPE_ERR_CODE;
            }
            QJsonObject obj = valItem.toObject();
            IDW_Data_Item& item = list[i];
            value = obj["number"];
            errCode = jValToInt(val, value);
            if (errCode) {
                return errCode;
            }
            item.setNumber(val);
            value = obj["offset_x"];
            errCode = jValToNumber(item.getOffsetX(), value, "0");
            if (errCode) {
                return errCode;
            }
            value = obj["offset_y"];
            errCode = jValToNumber(item.getOffsetY(), value, "0");
            if (errCode) {
                return errCode;
            }
            value = obj["size_x"];
            errCode = jValToNumber(item.getSizeX(), value, "0");
            if (errCode) {
                return errCode;
            }
            value = obj["size_y"];
            errCode = jValToNumber(item.getSizeY(), value, "0");
            if (errCode) {
                return errCode;
            }
        }
    }
    return 0;
}

int waferDataFromEntity(const IDW_WaferInfoEntity& entity,
                        IDW_WaferData& wafer,
                        IDW_ShotChipData& shot,
                        IDW_ShotChipData& chip) {
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(entity.getDetail().toUtf8(), &err);
    if (err.error != QJsonParseError::NoError) {
        qCritical() << "covert wafer data from entity err at line 1, err: " << err.error << ", "
                    << err.errorString();
        return JSON_CONVERT_ERR_CODE;
    }
    int errCode;
    errCode = jValToNumber(wafer.getMapOffset().x, doc["map_x"], "0");
    if (errCode) {
        return errCode;
    }
    errCode = jValToNumber(wafer.getMapOffset().y, doc["map_y"], "0");
    if (errCode) {
        return errCode;
    }
    errCode = jValToNumber(wafer.getInvalidArea(), doc["invalid_area"], "0");
    if (errCode) {
        return errCode;
    }
    errCode = jValToNumber(wafer.getNotchDirection(), doc["notch_direction"], "0");
    if (errCode) {
        return errCode;
    }
    wafer.setWaferDiameter(entity.getWaferDiameter());
    wafer.setWaferName(entity.getWaferName());
    int val;
    errCode = jValToInt(val, doc["origin"]);
    if (errCode) {
        return errCode;
    }
    wafer.setOriginalNumber(val);
    errCode = jValToInt(val, doc["invalid_type"]);
    if (errCode) {
        return errCode;
    }
    wafer.setInvalidType(val);
    errCode = jValToInt(val, doc["teg_type"]);
    if (errCode) {
        return errCode;
    }
    wafer.setTegType(val);
    QJsonValue value = doc["invalid_numbers"];
    if (!value.isNull() && !value.isUndefined() && !value.isArray()) {
        return JSON_CONVERT_TYPE_ERR_CODE;
    }
    if (value.isArray()) {
        QJsonArray array = value.toArray();
        if (array.size() > 0) {
            QSet<qint32>& numbers = wafer.getInvalidNumbers();
            numbers.clear();
            for (auto it = array.begin(); it != array.end(); ++it) {
                errCode = jValToInt(val, *it);
                if (errCode) {
                    return errCode;
                }
                numbers.insert(val);
            }
        }
    }
    value = doc["teg_numbers"];
    if (!value.isNull() && !value.isUndefined() && !value.isArray()) {
        return JSON_CONVERT_TYPE_ERR_CODE;
    }
    if (value.isArray()) {
        QJsonArray array = value.toArray();
        if (array.size() > 0) {
            QSet<qint32>& numbers = wafer.getTegNumbers();
            numbers.clear();
            for (auto it = array.begin(); it != array.end(); ++it) {
                errCode = jValToInt(val, *it);
                if (errCode) {
                    return errCode;
                }
                numbers.insert(val);
            }
        }
    }

    doc = QJsonDocument::fromJson(entity.getItems().toUtf8(), &err);
    if (err.error != QJsonParseError::NoError) {
        qCritical() << "covert wafer data from entity err at line 2, err: " << err.error << ", "
                    << err.errorString();
        return JSON_CONVERT_ERR_CODE;
    }
    value = doc["shot"];
    if (!value.isObject()) {
        return JSON_CONVERT_TYPE_ERR_CODE;
    }
    errCode = shotChipFromJsonDocument(value.toObject(), shot);
    if (errCode) {
        return errCode;
    }
    value = doc["chip"];
    if (!value.isObject()) {
        return JSON_CONVERT_TYPE_ERR_CODE;
    }
    errCode = shotChipFromJsonDocument(value.toObject(), chip);
    if (errCode) {
        return errCode;
    }

    int itemSize = shot.getArrayX() * shot.getArrayY();
    if ((itemSize > 0 && !shot.getDataList())
        || (shot.getDataList() && itemSize != shot.getDataList()->size())) {
        return IDW_ITEMS_NOT_EQUAL_ERR_CODE;
    }
    itemSize = chip.getArrayX() * chip.getArrayY();
    if ((itemSize > 0 && !chip.getDataList())
        || (chip.getDataList() && itemSize != chip.getDataList()->size())) {
        return IDW_ITEMS_NOT_EQUAL_ERR_CODE;
    }
    return 0;
}

void shotChipToJson(QJsonObject& obj, const IDW_ShotChipData& data) {
    obj.insert("array_x", data.getArrayX());
    obj.insert("array_y", data.getArrayY());
    obj.insert("direction", data.getDirection());
    obj.insert("size_x", data.getSizeX().toQString());
    obj.insert("size_y", data.getSizeY().toQString());
    obj.insert("pitch_x", data.getPitchX().toQString());
    obj.insert("pitch_y", data.getPitchY().toQString());
    if (data.getDataList() && data.getDataList()->size() > 0) {
        QList<IDW_Data_Item>& items = *data.getDataList();
        QJsonArray array;
        for (IDW_Data_Item& item : items) {
            QJsonObject itemObj;
            itemObj.insert("number", item.getNumber());
            itemObj.insert("offset_x", item.getOffsetX().toQString());
            itemObj.insert("offset_y", item.getOffsetY().toQString());
            itemObj.insert("size_x", item.getSizeX().toQString());
            itemObj.insert("size_y", item.getSizeY().toQString());
            array.append(itemObj);
        }
        obj.insert("items", array);
    }
}

void waferDataToEntity(IDW_WaferInfoEntity& entity,
                       const IDW_WaferData& wafer,
                       const IDW_ShotChipData& shot,
                       const IDW_ShotChipData& chip) {
    entity.setWaferDiameter(wafer.getWaferDiameter());
    entity.setWaferName(wafer.getWaferName());
    QJsonDocument doc;
    QJsonObject obj;
    obj.insert("map_x", wafer.getMapOffset().x.toQString());
    obj.insert("map_y", wafer.getMapOffset().y.toQString());
    obj.insert("invalid_area", wafer.getInvalidArea().toQString());
    obj.insert("notch_direction", wafer.getNotchDirection().toQString());
    obj.insert("origin", wafer.getOriginalNumber());
    obj.insert("invalid_type", wafer.getInvalidType());
    obj.insert("teg_type", wafer.getTegType());
    if (wafer.getInvalidNumbers().size() > 0) {
        const QSet<qint32>& numbers = wafer.getInvalidNumbers();
        QJsonArray array;
        for (auto it = numbers.begin(); it != numbers.end(); ++it) {
            array.append(*it);
        }
        obj.insert("invalid_numbers", array);
    }
    if (wafer.getTegNumbers().size() > 0) {
        const QSet<qint32>& numbers = wafer.getTegNumbers();
        QJsonArray array;
        for (auto it = numbers.begin(); it != numbers.end(); ++it) {
            array.append(*it);
        }
        obj.insert("teg_numbers", array);
    }
    doc.setObject(obj);
    entity.setDetail(doc.toJson(QJsonDocument::Compact));
    QJsonObject shotObj;
    QJsonObject chipObj;
    QJsonObject shotChipObj;
    QJsonDocument shotChipDoc;
    shotChipToJson(shotObj, shot);
    shotChipToJson(chipObj, chip);
    shotChipObj.insert("shot", shotObj);
    shotChipObj.insert("chip", chipObj);
    shotChipDoc.setObject(shotChipObj);
    entity.setItems(shotChipDoc.toJson(QJsonDocument::Compact));
}
