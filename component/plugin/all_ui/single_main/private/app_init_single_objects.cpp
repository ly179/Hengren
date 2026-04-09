#include "app_init_single_objects.h"
#include <QObject>
#include "image_operation_data_model.h"
#include "amp_operation_window_helper.h"
#include "image_operation_window_helper.h"
#include "image_scan_live_mode_helper.h"
#include "cdsem_control_helper.h"
#include "image_view_controller.h"
#include "cdsem_global_data.h"
#include "app_thread_invoker.h"
#include "app_exit_thread_manager.h"
#include "recipe_procedure.h"
#include "zeiss_tcp_client.h"
#include "om_camera_manager.h"
#include "image_list_controller.h"

AppInitSingleObjects::AppInitSingleObjects(QObject* parent) : AppInitLoadingData(parent) {
    setLoadOnTaskThread(false);
    setTitle("Init Component");
}

void AppInitSingleObjects::load() {
    //TODO 改为全局的，同时如果它销毁记得移除flusher.
    // LogViewController* logView = new LogViewController;
    // logView->initAsyncCaller();
    // logView->hideMinHideBtn(false);
    // logView->show();
    ImageOperationDataModelHelper::instance();
    //初始化像素与物理距离自动化更新操作
    //注册AMP窗口，以便需要打开AMP的地方可以正确运行
    AmpOperationWindowHelper::instance();
    //初始化图像操作窗口
    ImageOperationWindowHelper::instance();
    //初始化电镜扫描参数
    ImageScanLiveModeHelper::instance();

    CDSEM_ControlHelper::instance();

    GlobalImageViewHelper::instance();

    SingletonImageListHelper::instance();

    QThread* t = QThread::currentThread();
    QObject::connect(&STAGE_ENV, &CDSEM_GlobalStagePyParamModel::movingStageRotationChanged, t, []() {
        CDSEM_ControlHelper::instance()->rotation(nullptr, SEM_PY_ENV.getBeamRot());
    });

    AppThreadInvoker* invoker = cdsem::CDSEM_GlobalDataHelper::get()->imageInvoker();
    AppExitThreadManager::instance()->putQThread(invoker->thread(), [invoker](QThread* t){
        QObject::connect(t, &QThread::finished, QThread::currentThread(), [t](){
            AppExitThreadManager::instance()->finished(t);
        });
        invoker->quit();
    });

    AppExitThreadManager::instance()->putQThread(RecipeProcedureHelper::get()->innerThread(), [](QThread* t){
        QObject::connect(t, &QThread::finished, QThread::currentThread(), [t](){
            AppExitThreadManager::instance()->finished(t);
        });
        RecipeProcedureHelper::get()->exit();
    });

    AppExitThreadManager::instance()->putQThread(ZeissTcpClientHelper::get()->thread(), [](QThread* t){
        QObject::connect(t, &QThread::finished, QThread::currentThread(), [t](){
            AppExitThreadManager::instance()->finished(t);
        });
        ZeissTcpClientHelper::get()->exit();
    });

    invoker = HardWareDelayHelper::get()->getInvoker();
    AppExitThreadManager::instance()->putQThread(invoker->thread(), [](QThread* t){
        QObject::connect(t, &QThread::finished, QThread::currentThread(), [t](){
            AppExitThreadManager::instance()->finished(t);
        });
        HardWareDelayHelper::get()->exit();
    });

    //初始化OM camera的线程
    invoker = OmCameraManager::getDefaultCamera()->getInvoker();
    AppExitThreadManager::instance()->putQThread(invoker->thread(), [](QThread* t){
        QObject::connect(t, &QThread::finished, QThread::currentThread(), [t](){
            AppExitThreadManager::instance()->finished(t);
        });
        OmCameraManager::getDefaultCamera()->exit();
    });

    //CDSEM_ControlHelper属于TCP子线程对象，imageOperationController属于UI线程对象。
    //writeImageFromSupplier函数的设计用意就是允许在单一子线程上高速刷新图像，通知UI线程绘图的。
    //CDSEM_ControlHelper的线程已经注册到AppThreadManager中了，当前的条件是只有注册到里面的所有线程都停止了，才会退出QApplication。
    //而imageOperationController不是closeOnDelete，而且还是全局对象，也就QApplication退出后才会销毁
    //如此保证两个不同的线程上的对象的生命周期是安全的。
    QObject::connect(CDSEM_ControlHelper::instance()->getControl(),
                     &CDSEM_Control::imageReady, CDSEM_ControlHelper::instance()->getControl(), [](const QByteArray& data, const QSize& imageSize, const QSize& originSize, cdsem::ScanImageType type){
                         cdsem::CDSEM_GlobalDataHelper::get()->invokeImage([&originSize, &type, &imageSize, &data](){
                             ImageAddtionDataModel addition;
                             addition = SEM_PY_ENV;
                             addition = STAGE_ENV;
                             addition.originSize = originSize;
                             addition.imageType = type;
                             addition.realSize = imageSize;
                             addition.photoImagetype = cdsem::PIT_SEM;
                             addition.waferPos = cdsem::WaferStageCoordinateConvert::instance()->waferPosM(true);
                             cdsem::CDSEM_GlobalDataHelper::get()->setSemImage(data, imageSize.width(), imageSize.height(), int(QImage::Format_Grayscale8), addition);
                         }, true);
                     }, Qt::DirectConnection);

    QObject::connect(OmCameraManager::getDefaultCamera(),
                     &OmCamera::imageDataReady, OmCameraManager::getDefaultCamera(), [](const QByteArray& data, int w, int h){
                         cdsem::CDSEM_GlobalDataHelper::get()->invokeImage([&data, w, h](){
                             ImageAddtionDataModel addition;
                             addition.originSize = QSize(w, h);
                             addition = STAGE_ENV;
                             addition.imageType = cdsem::SIT_LIVE;
                             addition.photoImagetype = cdsem::PIT_OM;
                             addition.realSize = addition.originSize;
                             addition.imageRot = OM_PY_ENV.getRotation();
                             addition.brightness = OM_PY_ENV.getBrightness();
                             addition.mag = OM_PY_ENV.getMag();
                             addition.waferPos = cdsem::WaferStageCoordinateConvert::instance()->waferPosM(true);
                             cdsem::CDSEM_GlobalDataHelper::get()->setImage(IMG_OPT_TYPE_OM, data, w, h, int(QImage::Format_Grayscale8), addition);
                         }, true);
                     }, Qt::DirectConnection);

    QObject::connect(OmCameraManager::getDefaultCamera(), &OmCamera::exposureChanged, t, [](qreal e){
        OmCameraManager::getDefaultCamera()->invoke([e](){
            OperationResult result;
            qreal min = OmCameraManager::getDefaultCamera()->getMinExposure(result);
            qreal max = OmCameraManager::getDefaultCamera()->getMaxExposure(result);
            OM_PY_ENV.setBrightness(iraypleToBrightness(e, min, max));
        });
    });
    QObject::connect(OmCameraManager::getDefaultCamera(), &OmCamera::widthChanged, t, [](int w){
        OM_PY_ENV.setMag(iraypleSizeToMag(w));
    });
    QObject::connect(OmCameraManager::getDefaultCamera(), &OmCamera::rotationChanged, t, [](qreal r){
        OM_PY_ENV.setRotation(r);
    });
    emit overLoad(OperationResult {});
}
