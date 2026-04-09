#ifndef RELATION_CDE90_WINDOW_EXPORT_H
#define RELATION_CDE90_WINDOW_EXPORT_H
#include "cde90_window.h"
#include "relation_cde90_export_helper.h"
#define __R_WINDOW(NAME) ___R_WINDOW(NAME, CDE90Window)

#define _R_WINDOW(NAME) \
__R_WINDOW(NAME) \
__R_WINDOW_END

//防报警
#ifdef CDE90_WINDOW_H
CDE90_WINDOW_H
#endif
#endif // RELATION_CDE90_WINDOW_EXPORT_H
