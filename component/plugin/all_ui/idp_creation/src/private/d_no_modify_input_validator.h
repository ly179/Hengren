#ifndef D_NO_MODIFY_INPUT_VALIDATOR_H
#define D_NO_MODIFY_INPUT_VALIDATOR_H

#include <QValidator>

class DNoModifyInputValidator : public QValidator {
private:
    QValidator::State validate(QString &, int &) const override;
    void fixup(QString &) const override;

public:
    explicit DNoModifyInputValidator(QObject *parent = nullptr);
    ~DNoModifyInputValidator() noexcept;
};

#endif // D_NO_MODIFY_INPUT_VALIDATOR_H
