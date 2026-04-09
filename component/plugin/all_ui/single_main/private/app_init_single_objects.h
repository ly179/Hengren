#ifndef APP_INIT_SINGLE_OBJECTS_H
#define APP_INIT_SINGLE_OBJECTS_H

#include "app_init_interface.h"

class AppInitSingleObjects : public AppInitLoadingData {
public:
    explicit AppInitSingleObjects(QObject* parent = nullptr);
    void load() override;
};

#endif // APP_INIT_SINGLE_OBJECTS_H
