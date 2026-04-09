#include "cdsem_device_selector.h"
#include <QSpacerItem>

CDSemDeviceSelector::CDSemDeviceSelector(QWidget* parent)
    : DataSelectController(parent, false) {
    setViewMode(false);
}

CDSemDeviceSelector::~CDSemDeviceSelector() noexcept {}

void CDSemDeviceSelector::initDeviceList() {}
