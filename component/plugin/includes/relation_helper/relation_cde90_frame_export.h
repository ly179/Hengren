#ifndef RELATION_CDE90_FRAME_EXPORT_H
#define RELATION_CDE90_FRAME_EXPORT_H
#include "cde90_frame.h"
#include "relation_cde90_export_helper.h"

#define __R_FRAME(NAME) ___R_WINDOW(NAME, CDE90Frame)

#define _R_FRAME(NAME) \
__R_FRAME(NAME) \
__R_WINDOW_END

//防报警
#ifdef CDE90_FRAME_H
CDE90_FRAME_H
#endif
#endif // RELATION_CDE90_FRAME_EXPORT_H
