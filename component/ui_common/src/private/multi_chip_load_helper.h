#ifndef MULTI_CHIP_LOAD_HELPER_H
#define MULTI_CHIP_LOAD_HELPER_H

#include <QObject>
#include "sem_common_data_model.h"
#include "sem_common.h"
class MultiShotChipSelectController;

namespace cdsem {
struct PyShotChipCoordinate;
}

class MultiChipLoadHelper : public QObject{
private:
    sem::ONE_PARAM_ARG_FUNC<const cdsem::PyShotChipCoordinate&> callback;
    MultiShotChipSelectController* multiShotChipSelector;
    QWidget* content = nullptr;

    bool pyHandlerDisabled = true;

    void createMultiChipSelector();
public:
    explicit MultiChipLoadHelper(QObject* parent = nullptr);
    ~MultiChipLoadHelper() noexcept;

    void setPyHandlerDisable(bool);

    OperationResult openSelector(const sem::ONE_PARAM_ARG_FUNC<const cdsem::PyShotChipCoordinate&>&, int currChip = -1);
};

#endif // MULTI_CHIP_LOAD_HELPER_H
