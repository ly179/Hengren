#ifndef IMAGE_LIST_CONTROLLER_H
#define IMAGE_LIST_CONTROLLER_H
#include "ui_image_manager_global.h"
#include "relation_image_list_controller.h"

namespace cdsem {
class UiImageListController;
struct ImageViewListData;
}

struct ImageListControllerDataHelper;
class UI_IMG_MANAGER_EXPORT ImageListController : public RelationImageListController {
    friend struct ImageListControllerDataHelper;
private:
    cdsem::UiImageListController* ui = nullptr;
    ImageListControllerDataHelper* dataHelper = nullptr;

    void create();
    void createAction();

protected:
    void closeEvent(QCloseEvent*) override;
public:
     explicit ImageListController(QWidget* parent = nullptr);
     ~ImageListController() noexcept;

     void setImageList(const QList<cdsem::ImageViewListData>& list);
     void clear();
};

#endif
