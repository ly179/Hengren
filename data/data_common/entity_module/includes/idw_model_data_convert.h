#ifndef IDW_MODEL_DATA_CONVERT_H
#define IDW_MODEL_DATA_CONVERT_H
#include "entity_module_global.h"

class IDW_WaferInfoEntity;
class IDW_WaferData;
class IDW_ShotChipData;
void ENTITY_MODULE_EXPORT waferDataToEntity(IDW_WaferInfoEntity& entity,
                                            const IDW_WaferData& wafer,
                                            const IDW_ShotChipData& shot,
                                            const IDW_ShotChipData& chip);

int ENTITY_MODULE_EXPORT waferDataFromEntity(const IDW_WaferInfoEntity& entity,
                                             IDW_WaferData& wafer,
                                             IDW_ShotChipData& shot,
                                             IDW_ShotChipData& chip);

#endif // IDW_MODEL_DATA_CONVERT_H
