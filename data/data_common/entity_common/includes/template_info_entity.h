#ifndef TEMPLATE_INFO_ENTITY_H
#define TEMPLATE_INFO_ENTITY_H

#include <QDateTime>
#include <QString>
#include "cdsem_pack_base.h"
#include "model_setget.h"

// 0 - alignment 1 - address 2 - MP 3 - MP/IP
enum TemplateInfoType { TEMP_ALIGN, TEMP_MS, TEMP_MP, TEMP_MPIP };

class TemplateInfoSearchDTO {
    CDSEMPC(
        QList<quint64>, Ids, ids)
    CDSEMPSTRING(
        TemplateName, templateName)
    CDSEMPSTRING(
        TemplateComment, templateComment)
    CDSEMPC(
        ULong64, TopId, topId)
    CDSEMPC(
        Char8, TemplateType, templateType)
    CDSEMPC(
        Char8, Invalid, invalid)
};

class TemplateInfoEntity {
    CDSEMPDATETIME(
        CreateTime, createTime)
    CDSEMPDATETIME(
        EditTime, editTime)
    CDSEMPSTRING(
        TemplateName, templateName)
    CDSEMPSTRING(
        TemplateData, templateData)
    CDSEMPSTRING(
        TemplateComment, templateComment)
    CDSEMPUINT64(
        Id, id, 0)
    CDSEMPUINT64(
        TopId, topId, 0)
    CDSEMPINT32(
        Version, version, 0)
    //类型，0 - alignment 1 - address 2 - MP 3 - MP/IP
    CDSEMPCHAR(
        TemplateType, templateType, 0)
    CDSEMPCHAR(
        Invalid, invalid, 0)
    CDSEMPBOOL(
        Deleted, deleted, false)
};

#endif // TEMPLATE_INFO_ENTITY_H
