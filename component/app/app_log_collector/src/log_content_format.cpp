#include "log_content_format.h"
#include <QRegularExpressionMatch>

constexpr char16_t DATE_FORMAT[] = u"HH:mm:ss.zzz";
#define MAX_CAT_LOG 255

LogContentStruct LogContentParser::parseLogLine(const QString& content, bool *ok) {
    //[#{time}][#{cat}][#{class}:#{line}]: #{content}
    static QRegularExpression dateTimeReg(R"(\[ *(\d{2}:\d{2}:\d{2}\.\d{3}) *\])");
    static QRegularExpression catlogReg(R"(\[([^\]]*)\])");
    static QRegularExpression classLineReg(R"(\[ *([a-zA-Z0-9/\\_$]*) *\: *(\S*) *\])");
    QRegularExpressionMatch match = dateTimeReg.matchView(content, 0, QRegularExpression::PartialPreferFirstMatch);
    QStringView dateStr;
    QStringView catlogStr;
    QStringView classStr;
    QStringView lineStr;
    if (ok) *ok = true;
    int start = 0;
    if (match.hasMatch()) {
        dateStr = match.capturedView(1);
        start = match.capturedEnd(0);
    }
    match = catlogReg.matchView(content, start, QRegularExpression::PartialPreferFirstMatch);
    if (match.hasMatch()) {
        catlogStr = match.capturedView(1);
        start = match.capturedEnd(0);
    }
    match = classLineReg.matchView(content, start, QRegularExpression::PartialPreferFirstMatch);
    if (match.hasMatch()) {
        classStr = match.capturedView(1);
        lineStr = match.capturedView(2);
        start = match.capturedEnd(0);
    }
    LogContentStruct result;
    result.line = -1;
    if (dateStr.length() > 0) {
        result.time = QTime::fromString(dateStr, QStringView(DATE_FORMAT));
    } else if (ok) *ok = false;
    if (catlogStr.length() > 0) {
        result.cat = catlogStr.trimmed().toString();
    } else if (ok) *ok = false;
    if (classStr.length() > 0) {
        if (classStr.length() > MAX_CAT_LOG) {
            QString str = classStr.toString().replace("\\\\", "/");
            int index = 0;
            while (1) {
                index = str.indexOf("/", index);
                if (index < 0) break;
                ++ index;
                if (str.length() - index > MAX_CAT_LOG) continue;
                str = str.sliced(index);
                break;
            }
            if (str.length() > MAX_CAT_LOG) {
                str = str.right(MAX_CAT_LOG);
            }
            result.classFile = str;
        } else {
            result.classFile = classStr.toString();
        }
    } else if (ok) *ok = false;
    if (lineStr.length() > 0) {
        bool _ok;
        result.line = lineStr.toInt(&_ok);
        if (!ok) result.line = -1;
        if (ok) *ok = _ok;
    } else if (ok) *ok = false;

    if (start < content.length()) {
        QStringView view = QStringView(content).sliced(start).trimmed();
        if (view.startsWith(':')) {
            view = view.sliced(1).trimmed();
        }
        result.content = view.toString();
    }
    if (result.cat.isEmpty())
        result.cat = "unknow";
    if (result.classFile.isEmpty())
        result.classFile = "unknow";
    return result;
}

//正则表达式版本，正则表达式的问题是必须格式全匹配，如果第一个出现日期，期望值是最大可能的采集到，而不是有一个不匹配就不做采集。
// LogContentStruct LogContentParser::parseLogLine(const QString& content, bool *ok) {
//     //[#{time}][#{cat}][#{class}:#{line}]: #{content}
//     static QRegularExpression reg(R"((?i)^\[(\d{2}:\d{2}:\d{2}\.\d{3})\](\[\S*\])?\[[a-zA-Z0-9/\\_$]*:(\S*)\]:(.+)$)");
//     QRegularExpressionMatch match = reg.match(content);
//     QStringView dateStr;
//     // QString levelStr;
//     QStringView classFileStr;
//     int line;
//     QStringView contentStr;
//     QStringView cat;
//     bool lineParseOk;
//     if (match.hasMatch()) {
//         dateStr = match.capturedView(1).trimmed();
//         cat = match.capturedView(2).trimmed();
//         classFileStr = match.capturedView(3).trimmed();
//         line = match.capturedView(4).trimmed().toInt(&lineParseOk);
//         contentStr = match.capturedView(5).trimmed();
//         QTime time = QTime::fromString(dateStr, QStringView(DATE_FORMAT));
//         if (!time.isValid()) {
//             if (ok) {
//                 *ok = false;
//             }
//             return {};
//         }
//         // if (levelStr.isEmpty()) {
//         //     if (ok) {
//         //         *ok = false;
//         //     }
//         //     return {};
//         // }
//         // if (contentStr.isEmpty()) {
//         //     if (ok) {
//         //         *ok = false;
//         //     }
//         //     return {};
//         // }
//         // _msg = _msg.replace("\\", "\\\\")
//         // .replace("\n", "\\n");
//         LogContentStruct result;
//         if (contentStr.length() > 0)
//             result.content = contentStr.trimmed().toString().replace("\\\\", "\\").replace("\\n", "\n");
//         if (ok) {
//             *ok = true;
//         }
//         if (cat.length() > 0) {
//             //剔除[]
//             cat = cat.sliced(1, cat.length() - 2).trimmed();
//         }
//
//         result.time = time;
//         result.classFile = classFileStr.toString();
//         result.line = lineParseOk? line : -1;
//         result.cat = cat.toString();
//         return result;
//     }
//     if (ok) {
//         *ok = false;
//     }
//     return {};
// }
