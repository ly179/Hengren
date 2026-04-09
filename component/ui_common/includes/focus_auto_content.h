#ifndef FOCUS_AUTO_CONTENT_H
#define FOCUS_AUTO_CONTENT_H
#include "cde90_frame.h"
#include "ui_common_global.h"
#include "float_number.h"

namespace cdsem {
class UiFocusAutoContent;
}
struct AutoFocusSettingParam;
class AsyncCaller;

struct FocusAutoContentDataHelper;
class UI_COMMON_EXPORT FocusAutoContent : public CDE90Frame {
private:
    cdsem::UiFocusAutoContent* ui = nullptr;
    FocusAutoContentDataHelper* dataHelper = nullptr;

    void create();
    void createAction();

    void createOffsetSettingController(int mode);
    void createThresholdSettingController();

    void reloadItemBox();

public:
     explicit FocusAutoContent(QWidget* parent = nullptr);
     explicit FocusAutoContent(bool asAddress, QWidget* parent = nullptr);
     ~FocusAutoContent() noexcept;

     void initParam(AutoFocusSettingParam* param);
     void initAsyncCaller(AsyncCaller* caller);
     void resetToComponent();
     QString collectData();

     //禁用一切与机台互动相关的操作
     void disablePyHandlers(bool b);
     void setAPos(const IDW_PointF&);

     void enableCheckParamValid(bool);
};
#endif
