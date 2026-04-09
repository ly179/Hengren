//
// Created by Shisan on 2026/1/19.
//

#include "pattern_alg_itembox_helper.h"
#include "database_combo_box.h"
#include "cbox_global_keys.h"

void algKindChanged(cdsem::PatternRegParamOpenMode* mode, int cData, DatabaseComboBox* kindItemBox, DatabaseComboBox* algorithmItemBox) {
    cData = cData < 0 ? algorithmItemBox->currentData().toInt() : cData;
    algorithmItemBox->loadItemFromDatabase(CBOX_TYPE_AP_MODEL_ALG, false, [kindItemBox, algorithmItemBox, cData, mode]() {
            int kind = kindItemBox->currentData().toInt();
            if (*mode == cdsem::PRPOM_AP_OM || *mode == cdsem::PRPOM_AP_SEM) {
                if (kind != cdsem::MODEL_PATTERN_AP_UNIQUE) {
                    algorithmItemBox->safeRemoveItemByData(cdsem::MODEL_PATTERN_ALGORITHM_HYBRID);
                    algorithmItemBox->safeRemoveItemByData(cdsem::MODEL_PATTERN_ALGORITHM_GRAPHICS);
                }
            } else if (*mode == cdsem::PRPOM_MP_ADD) {
                if (kind != cdsem::MODEL_PATTERN_ADDRESS_UNIQUE) {
                    algorithmItemBox->safeRemoveItemByData(cdsem::MODEL_PATTERN_ALGORITHM_HYBRID);
                    algorithmItemBox->safeRemoveItemByData(cdsem::MODEL_PATTERN_ALGORITHM_GRAPHICS);
                }
            } else {
                if (kind != cdsem::MODEL_PATTERN_MP_UNIQUE) {
                    algorithmItemBox->safeRemoveItemByData(cdsem::MODEL_PATTERN_ALGORITHM_HYBRID);
                    algorithmItemBox->safeRemoveItemByData(cdsem::MODEL_PATTERN_ALGORITHM_GRAPHICS);
                }
            }
            if (*mode != cdsem::PRPOM_MP_M || kind != cdsem::MODEL_PATTERN_MP_ENTIRE) {
                algorithmItemBox->safeRemoveItemByData(cdsem::MODEL_PATTERN_ALGORITHM_AUTO);
            }
            algorithmItemBox->safeSetCurrentData(cData);
        });
}