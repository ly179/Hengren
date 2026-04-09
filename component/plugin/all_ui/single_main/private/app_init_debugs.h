#ifndef APP_INIT_DEBUGS_H
#define APP_INIT_DEBUGS_H

#include "app_init_interface.h"

class AppInitDebugCalibration : public AppInitLoadingData {
public:
    explicit AppInitDebugCalibration(QObject* parent = nullptr);
    void load() override;
};

#endif // APP_INIT_DEBUGS_H
