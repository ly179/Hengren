#ifndef APP_INIT_SCAN_PARAMS_H
#define APP_INIT_SCAN_PARAMS_H

#include "app_init_interface.h"

class AppInitScanParams : public AppInitLoadingData{
public:
    explicit AppInitScanParams(QObject* parent = nullptr);

    void load() override;
};


class AppInitMemoryCondition : public AppInitLoadingData {
public:
    explicit AppInitMemoryCondition(QObject* parent = nullptr);

    void load() override;
};

#endif // APP_INIT_SCAN_PARAMS_H
