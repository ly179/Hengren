#ifndef IMAGE_VIEW_CONTROLLER_H
#define IMAGE_VIEW_CONTROLLER_H
#include "ui_image_manager_global.h"
#include "relation_image_view_controller.h"

struct ImageAddtionDataModel;

namespace cdsem {
class UiImageViewController;
struct TmpImageData;
struct ImageViewListData;
}

struct ImageViewControllerDataHelper;
class UI_IMG_MANAGER_EXPORT ImageViewController : public RelationImageViewController {
    friend struct ImageViewControllerDataHelper;
private:
    cdsem::UiImageViewController* ui = nullptr;
    ImageViewControllerDataHelper* dataHelper;

    void create();
    void createAction();

protected:
    void closeEvent(QCloseEvent*) override;

public:
    explicit ImageViewController(QWidget* parent = nullptr);
    ~ImageViewController() noexcept;
    void loadImage(quint64 imageId, int selectedData);
    //imagePath指向cdsem::TmpImageData的数据结构磁盘化数据
    void loadImage(const QString& imagePath, int selectedData);
    //imagePath指向cdsem::TmpImageData的数据结构磁盘化数据
    void loadImage(const QString& imagePath, const ImageAddtionDataModel&, int selectedData);
    void loadImage(const cdsem::TmpImageData& imageData, const QString& name, int selectedData);
    void loadImage(const cdsem::TmpImageData& imageData, const ImageAddtionDataModel&, const QString& name, int selectedData);
    void clearImage();
    void clearList();

    //设置完list后返回一个version版本，如果需要下次指定跳转到某一张图，指定index和version可操作跳转。
    //如果version不正确则返回错误，index错误则不会返回错误。
    quint64 setImageList(const QList<cdsem::ImageViewListData>& list);
    //从0开始，传0显示1
    //返回0 - 设置失败
    //返回1 - 需要刷新 - 刷新图
    //返回2 - 无需刷新 - 没有变化
    char setCurrentNo(int, int, quint64);

    void refreshCurrentImage();
};
#endif
