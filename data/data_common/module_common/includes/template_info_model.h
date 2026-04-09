#ifndef TEMPLATE_INFO_MODEL_H
#define TEMPLATE_INFO_MODEL_H

#include <QString>
#include "template_data_model.h"

template<class T>
struct TemplateInfoModel {
    T data;
    QString name;
    QString comment;
    quint64 id;
    char type;
};
struct MPIPTemplateInfoModel {
    FloatNumber<2> dx;
    FloatNumber<2> dy;
    QString name;
    //无该字段，只是为了统一做兼容
    QString comment;
    quint64 msId = 0;
    quint64 mpId = 0;
    quint64 id = 0;
};

using ApTemplateInfoModel = TemplateInfoModel<ApTemplateParam>;
using MultiAddressTemplateInfoModel = TemplateInfoModel<MultiAddressTemplateParam>;
using MpTemplateInfoModel = TemplateInfoModel<MpTemplateParam>;

#endif // TEMPLATE_INFO_MODEL_H
