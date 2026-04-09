//
// Created by Shisan on 2026/2/5.
//

#ifndef HENGREN_CD_SEM_IMAGE_OPERATION_DATA_RELATION_H
#define HENGREN_CD_SEM_IMAGE_OPERATION_DATA_RELATION_H

#include "plugin_signal_hub.h"
#include "signal_hub_plugin_ids.h"
#include "cdsem_data_common.h"

namespace image_operation_data {
    inline void setImageOperationType(int type) {
        PluginSignalHub::instance()->sendMsg<void, int>(SWITCH_IMAGE_OPT_TYPE, type);
    }
};

#endif //HENGREN_CD_SEM_IMAGE_OPERATION_DATA_RELATION_H