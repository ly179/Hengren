#ifndef APP_INIT_PLUGINS_H
#define APP_INIT_PLUGINS_H

#include "app_init_interface.h"

class AppInitPlugins : public AppInitLoadingData{
public:
    explicit AppInitPlugins(QObject* parent = nullptr);
    void load() override;
};

#endif // APP_INIT_PLUGINS_H
