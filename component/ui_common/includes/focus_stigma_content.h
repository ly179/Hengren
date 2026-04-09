#ifndef FOCUS_STIGMA_CONTENT_H
#define FOCUS_STIGMA_CONTENT_H
#include "cde90_frame.h"
#include "ui_common_global.h"
#include "float_number.h"

namespace cdsem {
class UiFocusStigmaContent;
}
struct AutoStigmaSettingParam;
class AsyncCaller;

struct FocusStigmaContentHelper;
class UI_COMMON_EXPORT FocusStigmaContent : public CDE90Frame {
private:
    FocusStigmaContentHelper* dataHelper = nullptr;
    cdsem::UiFocusStigmaContent* ui = nullptr;

    void create();
    void createAction();

    void createOffsetSettingController(int mode);

    void reloadItemBox();

public:
     explicit FocusStigmaContent(QWidget* parent = nullptr);
     explicit FocusStigmaContent(bool asAddress, QWidget* parent = nullptr);
     ~FocusStigmaContent() noexcept;

     void initParam(AutoStigmaSettingParam* param);
     void initAsyncCaller(AsyncCaller* caller);
     void resetToComponent();
     QString collectData();

     //禁用一切与机台互动相关的操作
     void disablePyHandlers(bool b);

     void setAPos(const IDW_PointF& aPos);

     void enableCheckParamValid(bool b);
};
#endif
