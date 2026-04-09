#ifndef IMAGE_INFO_CONTROLLER_H
#define IMAGE_INFO_CONTROLLER_H
#include "cde90_window.h"

namespace cdsem {
class UiImageInfoController;
}

struct ImageAddtionDataModel;
class ImageInfoController : public CDE90Window{

private:
    cdsem::UiImageInfoController* ui = nullptr;

    void create();
    void createAction();
public:
     explicit ImageInfoController(QWidget* parent = nullptr);
     ~ImageInfoController() noexcept;

     void setImageInfo(const ImageAddtionDataModel&);
     void setImageInfo(const QString& html);
     void setName(const QString&);
};
#endif
