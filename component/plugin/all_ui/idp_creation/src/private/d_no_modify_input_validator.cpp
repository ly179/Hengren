#include "d_no_modify_input_validator.h"
#include <QRegularExpression>

DNoModifyInputValidator::DNoModifyInputValidator(
    QObject *parent)
    : QValidator(parent) {}

DNoModifyInputValidator::~DNoModifyInputValidator() noexcept {}

QValidator::State DNoModifyInputValidator::validate(
    QString &str, int &pos) const {
    Q_UNUSED(pos);
    if (str.isEmpty()) {
        return QValidator::Intermediate;
    }
    static QRegularExpression reg("^\\d+,?\\d*$");
    QRegularExpressionMatch match = reg.match(str);
    return match.hasMatch() ? QValidator::Acceptable : QValidator::Invalid;
}
void DNoModifyInputValidator::fixup(
    QString &) const {}
