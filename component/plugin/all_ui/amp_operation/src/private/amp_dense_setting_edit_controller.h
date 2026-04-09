#ifndef AMP_DENSE_SETTING_EDIT_CONTROLLER_H
#define AMP_DENSE_SETTING_EDIT_CONTROLLER_H
#include <QWidget>
#include "amp_data_model.h"

namespace cdsem {
class UiAmpDenseSettingEditController;
};
class CDE90TextFieldBox;

enum DenseSettingType {
    DST_LINE,
    DST_SPACE,
    DST_PITCH_L,
    DST_PITCH_R,
};

class AmpDenseSettingEditController : public QWidget {
    Q_OBJECT
private:
    cdsem::UiAmpDenseSettingEditController* ui = nullptr;
    AmpDenseObjectParam* param = nullptr;

    void create();
    void createAction();
    void initRealTimeEdit();

public:
    explicit AmpDenseSettingEditController(QWidget* parent = nullptr);
    ~AmpDenseSettingEditController() noexcept;

    inline void initParam(AmpDenseObjectParam* param) { this->param = param; }

    void resetToComponent();
    QString collectData(AmpDenseObjectParam& param);

    void setDenseSettingType(DenseSettingType);

signals:
    void dataChanged(QPointer<QObject> sender, void*);
};

void refreshTargetLimitTextField(bool isDense, CDE90TextFieldBox* target, CDE90TextFieldBox* limit);
#endif
