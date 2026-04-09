#ifndef CDSEM_DEVICE_SELECTOR_H
#define CDSEM_DEVICE_SELECTOR_H
#include "data_select_controller.h"
#include "ui_common_global.h"

class UI_COMMON_EXPORT CDSemDeviceSelector : public DataSelectController {
private:
public:
    explicit CDSemDeviceSelector(QWidget* parent = nullptr);
    virtual ~CDSemDeviceSelector() noexcept;

    void initDeviceList();
};

#endif // CDSEM_DEVICE_SELECTOR_H
