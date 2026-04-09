#ifndef RELATION_AMP_EDIT_CONTROLLER_H
#define RELATION_AMP_EDIT_CONTROLLER_H

#include "relation_cde90_window_export.h"
#include "cdsem_data_common.h"

__R_WINDOW(RelatoinAmpEditController)
virtual void loadAmp(cdsem::MeasureType, const QString& json) = 0;
// void loadAmp(const AmpCommonData&);
//如果AMS启用，则使用TMP进行load
//如果AMS没启用，则使用global进行load
virtual void loadAmp() = 0;

__R_WINDOW_END

#endif // RELATION_AMP_EDIT_CONTROLLER_H
