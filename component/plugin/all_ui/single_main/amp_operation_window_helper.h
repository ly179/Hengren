#ifndef AMP_OPERATION_WINDOW_HELPER_H
#define AMP_OPERATION_WINDOW_HELPER_H

#include "amp_edit_controller.h"
#include "sem_singleton_template.h"
#include "ui_single_main_global.h"

class UI_SINGLE_MAIN_EXPORT AmpOperationWindowHelper : public Singleton<AmpOperationWindowHelper> {
    friend class Singleton<AmpOperationWindowHelper>;

private:
    AmpEditController* editor;

    AmpOperationWindowHelper();

public:
    ~AmpOperationWindowHelper() noexcept;

    inline static AmpEditController* ampEditor() { return instance()->editor; }

    inline AmpEditController* operator->() { return editor; }
};

#endif // AMP_OPERATION_WINDOW_HELPER_H
