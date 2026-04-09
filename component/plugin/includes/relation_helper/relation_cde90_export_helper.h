#ifndef RELATION_CDE90_EXPORT_HELPER_H
#define RELATION_CDE90_EXPORT_HELPER_H
#include "plugin_signal_hub_hook.h"

#define ___R_WINDOW(NAME, WINDOW) \
class NAME : public WINDOW, public PluginSignalHubHook { \
public: \
explicit NAME(QWidget* parent = nullptr): WINDOW(parent) {} \
virtual ~NAME() noexcept = default;

#define __R_WINDOW_END };
//防报警
#ifdef PLUGIN_SIGNAL_HUB_HOOK_H
PLUGIN_SIGNAL_HUB_HOOK_H
#endif
#endif // RELATION_CDE90_EXPORT_HELPER_H
