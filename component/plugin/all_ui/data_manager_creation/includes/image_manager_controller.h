#ifndef IMAGE_MANAGER_CONTROLLER_H
#define IMAGE_MANAGER_CONTROLLER_H
#include "ui_data_manager_creation_global.h"
#include "image_file_temp_handler.h"
#include "relation_image_manager.h"

namespace cdsem {
class UiImageManagerController;
}

struct ImageManagerDataHelper;

class UI_DATA_MANAGER_EXPORT ImageManagerController : public RelationImageManager {
    friend struct ImageManagerDataHelper;
    Q_OBJECT
private:
    cdsem::UiImageManagerController* ui = nullptr;
    ImageManagerDataHelper* dataHelper = nullptr;

    void create();
    void createAction();
public:
     explicit ImageManagerController(QWidget* parent = nullptr);
     ~ImageManagerController() noexcept;

 signals:
     void openImageView(const QList<cdsem::ImageViewListData>&, int);
};
#endif
