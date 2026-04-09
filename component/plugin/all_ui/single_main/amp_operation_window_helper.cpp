#include "amp_operation_window_helper.h"
#include "signal_hub.h"
#include "signal_hub_ids.h"

AmpOperationWindowHelper::AmpOperationWindowHelper() {
    editor = new AmpEditController;
    editor->setWindowTitle("AMP");
    editor->initAsyncCaller(
        []() {
            SignalHub::instance()->unRegistAll(SIGNAL_HUB_OPEN_AMP);
        },
        true);
    SignalHub::instance()->registSlot<int, const QString&>(SIGNAL_HUB_OPEN_AMP,
                                                           HubAnyFunc<int, const QString&>(
                                                               [this](int id, const QString& json) {
                                                                   if (id >= 0 && json.size() > 0) {
                                                                       editor->loadAmp(cdsem::MeasureType(id), json);
                                                                   } else {
                                                                       editor->loadAmp();
                                                                   }
                                                                   if (editor->isHidden()) {
                                                                       editor->show();
                                                                   } else if (!editor->isActiveWindow()) {
                                                                       editor->activateWindow();
                                                                   }
                                                               }));
}

AmpOperationWindowHelper::~AmpOperationWindowHelper() noexcept {
    // editor->release();
}
