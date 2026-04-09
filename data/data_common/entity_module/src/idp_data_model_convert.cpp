#include "idp_data_model_convert.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include "axis_align_model_convert.h"
#include "image_save_model_convert.h"
#include "model_data_convert_helper.h"
#include "exception_code.h"
#include "template_data_model_convert.h"
#include "idp_data_model.h"

template<class T>
bool putLinekdParamData(
    QJsonObject& obj, const TemplateLinkedModel<T>& param) {
    if (param.templateId > 0) {
        obj.insert("template_id", qint64(param.templateId));
        obj.insert("template_ref", param.templateRefType);
        //动态连接的模板内容不保存
        return param.templateRefType != 0;
    }
    return true;
}

void putIdpApParamListData(QJsonObject& obj, const IdpApParamListData& param) {
    obj.insert("d_point_x", param.dPoint.x.toQStringAndKeepFullDecimal());
    obj.insert("d_point_y", param.dPoint.y.toQStringAndKeepFullDecimal());
    obj.insert("p_number", param.pNumber);
    // obj.insert("chip_count", param.chipCount);
    QJsonObject apTempObj;
    if (putLinekdParamData(apTempObj, param.apTemplate)) {
        QJsonObject tObj;
        idpApTemplateToJsonObject(tObj, param.apTemplate.templateParam);
        apTempObj.insert("template_param", tObj);
    }
    obj.insert("ap_template", apTempObj);
}

template<class T>
bool fromLinekdParamData(
    const QJsonValue& val, TemplateLinkedModel<T>& param, int& errCode) {
    QJsonObject obj;
    errCode = jValToObject(obj, val);
    if (errCode) {
        return false;
    }
    if (obj.size() > 0) {
        errCode = jValToInt(param.templateId, obj["template_id"]);
        if (errCode) {
            return false;
        }
        errCode = jValToInt(param.templateRefType, obj["template_ref"]);
        if (errCode) {
            return false;
        }
        return param.templateId == 0 || param.templateRefType != 0;
    }
    return false;
}

int fromIdpApParamListData(const QJsonValue& val, IdpApParamListData& param) {
    int errCode;
    QJsonObject obj;
    errCode = jValToObject(obj, val);
    if (errCode) {
        return errCode;
    }
    if (obj.isEmpty()) {
        return 0;
    }
    QJsonValue templateVal = obj["ap_template"];
    if (fromLinekdParamData(templateVal, param.apTemplate, errCode)) {
        errCode = idpApTemplateFromJsonValue(templateVal["template_param"],
                                             param.apTemplate.templateParam);
        if (errCode) {
            return errCode;
        }
    } else if (errCode) {
        return errCode;
    }

    errCode = jValToNumber(param.dPoint.x, obj["d_point_x"], "0");
    if (errCode) {
        return errCode;
    }
    errCode = jValToNumber(param.dPoint.y, obj["d_point_y"], "0");
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.pNumber, obj["p_number"]);
    if (errCode) {
        return errCode;
    }
    // return jValToInt(param.chipCount, obj["chip_count"]);
    return 0;
}

void putIdpMpParamListData(QJsonObject& obj, const IdpMpParamListData& param) {
    //TODO 根据模板load情况不同采取不同的保存策略，同时param中缺少template绑定信息
    QJsonObject msTempObj;
    if (putLinekdParamData(msTempObj, param.addressTemplate)) {
        QJsonObject tObj;
        idpAddressTemplateToJsonObject(tObj, param.addressTemplate.templateParam);
        msTempObj.insert("template_param", tObj);
    }
    obj.insert("ms_template", msTempObj);
    QJsonObject mpTempObj;
    if (putLinekdParamData(mpTempObj, param.mpTemplate)) {
        QJsonObject tObj;
        idpMpTemplateToJsonObject(tObj, param.mpTemplate.templateParam);
        mpTempObj.insert("template_param", tObj);
    }
    obj.insert("mp_template", mpTempObj);
    obj.insert("mpip_template_id", qint64(param.mpipTemplateId));
    obj.insert("mpip_template_ref", param.mpipTemplateRefType);
    obj.insert("d_point_x", param.dPoint.x.toQStringAndKeepFullDecimal());
    obj.insert("d_point_y", param.dPoint.y.toQStringAndKeepFullDecimal());
    obj.insert("address_rotate", param.addressRotate.toQStringAndKeepFullDecimal());
    obj.insert("measurement_rotate", param.measurementRotate.toQStringAndKeepFullDecimal());
    // obj.insert("name", param.mpipTemplateName);
    obj.insert("p_number", param.pNumber);
    // obj.insert("chip_count", param.chipCount);
    obj.insert("d_point_type", param.dPointType);
    obj.insert("seq_type", param.seqType);
    obj.insert("use_prev_point", param.usePrevPoint);
    obj.insert("times", param.times);
}

int fromIdpMpParamListData(const QJsonValue& val, IdpMpParamListData& param) {
    int errCode;
    QJsonObject obj;
    errCode = jValToObject(obj, val);
    if (errCode) {
        return errCode;
    }
    if (obj.isEmpty()) {
        return 0;
    }

    QJsonValue templateVal = obj["ms_template"];
    if (fromLinekdParamData(templateVal, param.addressTemplate, errCode)) {
        errCode = idpAddressTemplateFromJsonValue(templateVal["template_param"],
                                                  param.addressTemplate.templateParam);
        if (errCode) {
            return errCode;
        }
    } else if (errCode) {
        return errCode;
    }
    templateVal = obj["mp_template"];
    if (fromLinekdParamData(templateVal, param.mpTemplate, errCode)) {
        errCode = idpMpTemplateFromJsonValue(templateVal["template_param"],
                                             param.mpTemplate.templateParam);
        if (errCode) {
            return errCode;
        }
    } else if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.mpipTemplateId, obj["mpip_template_id"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.mpipTemplateRefType, obj["mpip_template_ref"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToNumber(param.dPoint.x, obj["d_point_x"], "0");
    if (errCode) {
        return errCode;
    }
    errCode = jValToNumber(param.dPoint.y, obj["d_point_y"], "0");
    if (errCode) {
        return errCode;
    }
    errCode = jValToNumber(param.addressRotate, obj["address_rotate"], "0");
    if (errCode) {
        return errCode;
    }
    errCode = jValToNumber(param.measurementRotate, obj["measurement_rotate"], "0");
    if (errCode) {
        return errCode;
    }
    // jValToString(param.name, obj["name"]);
    errCode = jValToInt(param.pNumber, obj["p_number"]);
    if (errCode) {
        return errCode;
    }
    // errCode = jValToInt(param.chipCount, obj["chip_count"]);
    // if (errCode) {
    //     return errCode;
    // }
    errCode = jValToInt(param.dPointType, obj["d_point_type"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.seqType, obj["seq_type"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.times, obj["times"]);
    if (errCode) {
        return errCode;
    }
    return jValToBool(param.usePrevPoint, obj["use_prev_point"]);
}

void idpApModelToJson(QString& json, const IdpApDetailData& apData) {
    QJsonDocument doc;
    QJsonObject obj;

    if (apData.posList.size() > 0) {
        {
            QJsonArray posArray;
            for (const QSharedPointer<IdpApPosListData>& p : apData.posList) {
                QJsonObject pObj;
                pObj.insert("o_point_x", p->oPoint.x.toQStringAndKeepFullDecimal());
                pObj.insert("o_point_y", p->oPoint.y.toQStringAndKeepFullDecimal());
                pObj.insert("number", p->chipNumber);
                if (p->chipX != nullptr) {
                    pObj.insert("chip_x", p->chipX.value());
                }
                if (p->chipY != nullptr) {
                    pObj.insert("chip_y", p->chipY.value());
                }
                pObj.insert("param_index", p->paramIndex);
                posArray.append(pObj);
            }
            obj.insert("pos_list", posArray);
        }

        QJsonArray paramList;
        for (const QSharedPointer<IdpApParamListData>& p : apData.paramList) {
            QJsonObject pObj;
            putIdpApParamListData(pObj, *p);
            paramList.append(pObj);
        }
        obj.insert("param_list", paramList);
    }
    doc.setObject(obj);
    json = doc.toJson(QJsonDocument::Compact);
}
int idpApModelFromJson(const QString& json, IdpApDetailData& apData) {
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8(), &err);
    if (err.error != QJsonParseError::NoError) {
        qCritical() << "covert idp_ap data from json err: " << err.error << ", "
                    << err.errorString();
        return JSON_CONVERT_ERR_CODE;
    }
    QJsonValue val = doc["pos_list"];
    if (!val.isArray()) {
        if (!val.isNull() && !val.isUndefined()) {
            return JSON_CONVERT_ERR_CODE;
        }
    } else {
        QJsonArray posArray = val.toArray();
        val = doc["param_list"];
        if (!val.isArray()) {
            if (!val.isNull() && !val.isUndefined()) {
                return JSON_CONVERT_ERR_CODE;
            }
        } else {
            apData.posList.clear();
            apData.posList.reserve(posArray.size());
            int errCode;
            for (const QJsonValue& val : posArray) {
                QJsonObject obj;
                errCode = jValToObject(obj, val);
                if (errCode) {
                    return errCode;
                }
                if (obj.isEmpty()) {
                    continue;
                }
                QSharedPointer<IdpApPosListData> posData(new IdpApPosListData);
                errCode = jValToNumber(posData->oPoint.x, obj["o_point_x"], "0");
                if (errCode) {
                    return errCode;
                }
                errCode = jValToNumber(posData->oPoint.y, obj["o_point_y"], "0");
                if (errCode) {
                    return errCode;
                }
                errCode = jValToInt(posData->chipNumber, obj["number"]);
                if (errCode) {
                    return errCode;
                }
                errCode = jValToPack(posData->chipX, obj["chip_x"]);
                if (errCode) {
                    return errCode;
                }
                errCode = jValToPack(posData->chipY, obj["chip_y"]);
                if (errCode) {
                    return errCode;
                }
                errCode = jValToInt(posData->paramIndex, obj["param_index"]);
                if (errCode) {
                    return errCode;
                }
                apData.posList.append(posData);
            }
            QJsonArray paramArray = val.toArray();
            apData.paramList.clear();
            apData.paramList.reserve(paramArray.size());
            for (const QJsonValue& val : paramArray) {
                QSharedPointer<IdpApParamListData> paramData(new IdpApParamListData);
                errCode = fromIdpApParamListData(val, *paramData);
                if (errCode) {
                    return errCode;
                }
                apData.paramList.append(paramData);
            }
        }
    }
    return 0;
}

void putIdpMpDuplicate(
    QJsonObject& obj, const IdpDuplicatePos& param) {
    if (param.type < 0) {
        return;
    }
    obj.insert("type", param.type);
    if (param.pitchX != nullptr) {
        obj.insert("pitch_x", param.pitchX.toQStringAndKeepFullDecimal());
    }

    if (param.pitchY != nullptr) {
        obj.insert("pitch_y", param.pitchY.toQStringAndKeepFullDecimal());
    }

    if (param.r1 != nullptr) {
        obj.insert("r1", param.r1.value());
    }
    if (param.r2 != nullptr) {
        obj.insert("r2", param.r2.value());
    }
    if (param.r3 != nullptr) {
        obj.insert("r3", param.r3.value());
    }
    if (param.r4 != nullptr) {
        obj.insert("r4", param.r4.value());
    }
    obj.insert("direction", param.direction);
}

int fromIdpMpDuplicate(
    const QJsonValue& val, IdpDuplicatePos& param) {
    QJsonObject obj;
    int errCode = jValToObject(obj, val);
    if (errCode) {
        return errCode;
    }
    errCode = jValToInt(param.type, obj["type"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToNumber(param.pitchX, obj["pitch_x"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToNumber(param.pitchY, obj["pitch_y"]);
    if (errCode) {
        return errCode;
    }

    errCode = jValToPack(param.r1, obj["r1"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToPack(param.r2, obj["r2"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToPack(param.r3, obj["r3"]);
    if (errCode) {
        return errCode;
    }
    errCode = jValToPack(param.r4, obj["r4"]);
    if (errCode) {
        return errCode;
    }
    return jValToInt(param.direction, obj["direction"]);
}

void putIdpMpImageEnv(
    QJsonObject& obj, const IdpMpImageEnvData& param) {
    obj.insert("use_sys_param", param.useSysParam);
    QJsonObject alignObj;
    QJsonObject addressObj;
    QJsonObject measureObj;
    putImageEnvModelToJson(alignObj, param.alignment);
    putImageEnvModelToJson(addressObj, param.addressing);
    putImageEnvModelToJson(measureObj, param.measurement);
    obj.insert("alignment", alignObj);
    obj.insert("address", addressObj);
    obj.insert("measurement", measureObj);
}
int fromIdpMpImageEnv(
    const QJsonValue& val, IdpMpImageEnvData& param) {
    QJsonObject obj;
    int errCode = jValToObject(obj, val);
    if (errCode) {
        return errCode;
    }
    if (obj.isEmpty()) {
        return 0;
    }
    errCode = jValToBool(param.useSysParam, obj["use_sys_param"]);
    if (errCode) {
        return errCode;
    }
    errCode = fromImageEnvModelToJson(obj["alignment"], param.alignment);
    if (errCode) {
        return errCode;
    }
    errCode = fromImageEnvModelToJson(obj["address"], param.addressing);
    if (errCode) {
        return errCode;
    }
    return fromImageEnvModelToJson(obj["measurement"], param.measurement);
}

void idpMpModelToJson(QString& json, const IdpMpDetailData& mpData) {
    QJsonDocument doc;
    QJsonObject obj;
    QJsonObject iObj;
    QJsonObject aObj;
    putIdpMpImageEnv(iObj, mpData.imageEnvData);
    putAxisAlignData(aObj, mpData.axisParam);

    obj.insert("img_env", iObj);
    obj.insert("axis_alignment", aObj);
    if (mpData.posList.size() > 0) {
        {
            QJsonArray posArray;
            for (const QSharedPointer<IdpMpPosListData>& p : mpData.posList) {
                QJsonObject pObj;
                pObj.insert("o_point_x", p->oPoint.x.toQStringAndKeepFullDecimal());
                pObj.insert("o_point_y", p->oPoint.y.toQStringAndKeepFullDecimal());
                pObj.insert("number", p->chipNumber);
                if (p->chipX != nullptr) {
                    pObj.insert("chip_x", p->chipX.value());
                }
                if (p->chipY != nullptr) {
                    pObj.insert("chip_y", p->chipY.value());
                }
                pObj.insert("param_index", p->paramIndex);
                if (p->dNumbers.size() > 0) {
                    QJsonArray mArray;
                    for (const QList<int>& dNums : p->dNumbers) {
                        QJsonArray dArray;
                        for (const int& d : dNums) {
                            dArray.append(d);
                        }
                        mArray.append(dArray);
                    }
                    pObj.insert("d_numbers", mArray);
                }
                QJsonObject dObj;
                putIdpMpDuplicate(dObj, p->duplicatePos);
                if (dObj.size() > 0) {
                    pObj.insert("duplicate_pos", dObj);
                }
                posArray.append(pObj);
            }
            obj.insert("pos_list", posArray);
        }

        QJsonArray paramList;
        for (const QSharedPointer<IdpMpParamListData>& p : mpData.paramList) {
            QJsonObject pObj;
            putIdpMpParamListData(pObj, *p);
            paramList.append(pObj);
        }
        obj.insert("param_list", paramList);
    }
    doc.setObject(obj);
    json = doc.toJson(QJsonDocument::Compact);
}

int idpMpModelFromJson(const QString& json, IdpMpDetailData& mpData) {
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8(), &err);
    if (err.error != QJsonParseError::NoError) {
        qCritical() << "covert idp_mp data from json err: " << err.error << ", "
                    << err.errorString();
        return JSON_CONVERT_ERR_CODE;
    }
    int errCode = fromIdpMpImageEnv(doc["img_env"], mpData.imageEnvData);
    if (errCode) {
        return errCode;
    }
    errCode = fromAxisAlignData(doc["axis_alignment"], mpData.axisParam);
    if (errCode) {
        return errCode;
    }

    QJsonValue val = doc["pos_list"];
    if (!val.isArray()) {
        if (!val.isNull() && !val.isUndefined()) {
            return JSON_CONVERT_ERR_CODE;
        }
    } else {
        QJsonArray posArray = val.toArray();
        val = doc["param_list"];
        if (!val.isArray()) {
            if (!val.isNull() && !val.isUndefined()) {
                return JSON_CONVERT_ERR_CODE;
            }
        } else {
            mpData.posList.clear();
            mpData.posList.reserve(posArray.size());

            for (const QJsonValue& val : posArray) {
                QJsonObject obj;
                errCode = jValToObject(obj, val);
                if (errCode) {
                    return errCode;
                }
                if (obj.isEmpty()) {
                    continue;
                }
                QSharedPointer<IdpMpPosListData> posData(new IdpMpPosListData);
                errCode = jValToNumber(posData->oPoint.x, obj["o_point_x"], "0");
                if (errCode) {
                    return errCode;
                }
                errCode = jValToNumber(posData->oPoint.y, obj["o_point_y"], "0");
                if (errCode) {
                    return errCode;
                }
                errCode = jValToInt(posData->chipNumber, obj["number"]);
                if (errCode) {
                    return errCode;
                }
                errCode = jValToPack(posData->chipX, obj["chip_x"]);
                if (errCode) {
                    return errCode;
                }
                errCode = jValToPack(posData->chipY, obj["chip_y"]);
                if (errCode) {
                    return errCode;
                }
                errCode = jValToInt(posData->paramIndex, obj["param_index"]);
                if (errCode) {
                    return errCode;
                }
                errCode = fromIdpMpDuplicate(obj["duplicate_pos"], posData->duplicatePos);
                if (errCode) {
                    return errCode;
                }

                QJsonValueRef mValue = obj["d_numbers"];
                if (mValue.isArray()) {
                    QList<int> list;
                    list.reserve(2);
                    int num;
                    QJsonArray mArray = mValue.toArray();
                    for (const QJsonValue& m : mArray) {
                        if (!m.isArray()) {
                            return JSON_CONVERT_TYPE_ERR_CODE;
                        }
                        QJsonArray dArray = m.toArray();
                        for (const QJsonValue& v : dArray) {
                            errCode = jValToInt(num, v);
                            if (errCode) return errCode;
                            list.append(num);
                        }
                        posData->dNumbers.append(list);
                        list.clear();
                    }
                }

                mpData.posList.append(posData);
            }
            QJsonArray paramArray = val.toArray();
            mpData.paramList.clear();
            mpData.paramList.reserve(paramArray.size());
            for (const QJsonValue& val : paramArray) {
                QSharedPointer<IdpMpParamListData> paramData(new IdpMpParamListData);
                errCode = fromIdpMpParamListData(val, *paramData);
                if (errCode) {
                    return errCode;
                }
                mpData.paramList.append(paramData);
            }
        }
    }
    return 0;
}
