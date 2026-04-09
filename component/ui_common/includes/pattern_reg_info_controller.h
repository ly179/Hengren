#ifndef PATTERN_REG_INFO_CONTROLLER_H
#define PATTERN_REG_INFO_CONTROLLER_H
#include <QHash>
#include <QList>
#include "cde90_window.h"
#include "template_data_model.h"
#include "ui_common_global.h"

namespace cdsem {
class UiPatternRegInfoController;
};

class CBoxMixKey;
struct PatternRegInfoControllerDataHelper;
class UI_COMMON_EXPORT PatternRegInfoController : public CDE90Window {
    friend struct PatternRegInfoControllerDataHelper;
private:
    cdsem::UiPatternRegInfoController* ui = nullptr;
    PatternRegInfoControllerDataHelper* dataHelper = nullptr;

    void create();
    void createAction();

public:
    explicit PatternRegInfoController(QWidget* parent = nullptr);
    ~PatternRegInfoController() noexcept;

    void setParam(const PatternRegInfoParam&, const IDW_PointF& relativePos = IDW_PointF{});
};
#endif
