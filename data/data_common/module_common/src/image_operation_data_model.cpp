#include "image_operation_data_model.h"
#include "cdsem_global_data.h"
#include "cas_lock.h"
#include "app_util.h"
#include "plugin_signal_hub.h"
#include "signal_hub_plugin_ids.h"
#include "image_type_defition.h"

struct ImageOperationDataModelDataHelper {

    CasLock lock;

    ImageOperationDataModel* d;

    //窗口图像类型
    int ot = IMG_OPT_TYPE_SEM;

    cdsem::PhotoImageType photoImageType = cdsem::PIT_SEM;

    //图像冻结状态
    ImageFreezenStatus semFreezeStatus = IFS_LIVE;

    float minWorkDistance = 0;

    float maxWorkDistance = 0;

    //电子束聚焦工作距离
    float workDistance = 0;

    //电子束偏转角度
    float beamRot;
    float omRot;

    //单位：伏特
    float eht = 0;

    //单位：皮安
    float iProbe = 0;

    //1像素代表几纳米
    float pixelSize = 1;

    //所选择的Aperture 编号。从0开始，0-6或0-7
    int aptNum = -1;

    //放大倍率
    int semMag = 0;
    int omMag = 0;

    //扫描速率，0 - 7 数字越大扫描越慢
    int scanRate = -1;

    //扫描帧率
    int frame = 0;

    //是否是iProbe，如果是则使用iProbe数据，否则使用aptNum。
    bool isIProbe = false;

    bool disabledFromGlobal = false;

    void copy() {
        d->setImageFreezeStatus(SEM_PY_ENV.getFreezeStatus());
        d->setMinWorkDistance(SEM_PY_ENV.getMinWorkDistance());
        d->setMaxWorkDistance(SEM_PY_ENV.getMaxWorkDistance());
        d->setWorkDistance(SEM_PY_ENV.getWorkDistance());
        d->setBeamRot(SEM_PY_ENV.getBeamRot());
        d->setSemMag(SEM_PY_ENV.getMag());
        d->setOmRot(OM_PY_ENV.getRotation());
        d->setImageOperationType(cdsem::CDSEM_GlobalDataHelper::get()->getImageOperationType());
        //这些参数被挪到tcp imageready处更新效果更好更稳定。
        // if (freezeStatus != IFS_PHOTO) {
        // d->setAptNum(SEM_PY_ENV.getAptNum());
        // d->setIsProbe(SEM_PY_ENV.isOptIProbe());
        // d->setIProbe(SEM_PY_ENV.getIProbe());
        // d->setEHT(SEM_PY_ENV.getEHT());
        // d->setScanRate(SEM_PY_ENV.getScanRate());
        // d->setFrame(SEM_PY_ENV.getFrame());
        // d->setPixelSize(SEM_PY_ENV.getPixelSize());
        // }
    }

    void init() {
        QObject::connect(&SEM_PY_ENV, &CDSEM_GlobalSemPyParamModel::freezenChanged, d, [this](){
            d->setImageFreezeStatus(SEM_PY_ENV.getFreezeStatus());
        });
        QObject::connect(&SEM_PY_ENV, &CDSEM_GlobalSemPyParamModel::minWorkDistanceChanged, d, [this](){
            d->setMinWorkDistance(SEM_PY_ENV.getMinWorkDistance());
        });
        QObject::connect(&SEM_PY_ENV, &CDSEM_GlobalSemPyParamModel::maxWorkDistanceChanged, d, [this](){
            d->setMaxWorkDistance(SEM_PY_ENV.getMaxWorkDistance());
        });
        QObject::connect(&SEM_PY_ENV, &CDSEM_GlobalSemPyParamModel::workDistanceChanged, d, [this](){
            d->setWorkDistance(SEM_PY_ENV.getWorkDistance());
        });
        QObject::connect(&SEM_PY_ENV, &CDSEM_GlobalSemPyParamModel::beamRotChanged, d, [this](){
            d->setBeamRot(SEM_PY_ENV.getBeamRot());
        });
        QObject::connect(&SEM_PY_ENV, &CDSEM_GlobalSemPyParamModel::magChanged, d, [this](){
            d->setSemMag(SEM_PY_ENV.getMag());
        });
        QObject::connect(&OM_PY_ENV, &CDSEM_GlobalOmPyParamModel::rotationChanged, d, [this](){
            d->setOmRot(OM_PY_ENV.getRotation());
        });
        QObject::connect(cdsem::CDSEM_GlobalDataHelper::get(), &cdsem::CDSEM_GlobalData::imageOperationTypeChanged, d, [this](){
            d->setImageOperationType(cdsem::CDSEM_GlobalDataHelper::get()->getImageOperationType());
        });

        // QObject::connect(&SEM_PY_ENV, &CDSEM_GlobalSemPyParamModel::scanRateChanged, d, [this](){
        //     // if (SEM_PY_ENV.getFreezeStatus() != IFS_PHOTO) {
        //     d->setScanRate(SEM_PY_ENV.getScanRate());
        //     // }
        // });
        // QObject::connect(&SEM_PY_ENV, &CDSEM_GlobalSemPyParamModel::frameChanged, d, [this](){
        //     // if (SEM_PY_ENV.getFreezeStatus() != IFS_PHOTO) {
        //     d->setFrame(SEM_PY_ENV.getFrame());
        //     // }
        // });
        // QObject::connect(&SEM_PY_ENV, &CDSEM_GlobalSemPyParamModel::optIProbeChanged, d, [this](){
        //     // if (freezeStatus != IFS_PHOTO) {
        //     d->setIsProbe(SEM_PY_ENV.isOptIProbe());
        //     // }
        // });
        // QObject::connect(&SEM_PY_ENV, &CDSEM_GlobalSemPyParamModel::aptNumChanged, d, [this](){
        //     // if (freezeStatus != IFS_PHOTO) {
        //     d->setAptNum(SEM_PY_ENV.getAptNum());
        //     // }
        // });
        // QObject::connect(&SEM_PY_ENV, &CDSEM_GlobalSemPyParamModel::ehtChanged, d, [this](){
        //     // if (freezeStatus != IFS_PHOTO) {
        //     d->setEHT(SEM_PY_ENV.getEHT());
        //     // }
        // });
        // QObject::connect(&SEM_PY_ENV, &CDSEM_GlobalSemPyParamModel::iProbeChanged, d, [this](){
        //     // if (freezeStatus != IFS_PHOTO) {
        //     d->setIProbe(SEM_PY_ENV.getIProbe());
        //     // }
        // });
        // QObject::connect(&SEM_PY_ENV, &CDSEM_GlobalSemPyParamModel::pixelSizeChanged, d, [this](){
        //     // if (freezeStatus != IFS_PHOTO) {
        //     d->setPixelSize(SEM_PY_ENV.getPixelSize());
        //     // }
        // });
    }

    void disable() {
        QObject::disconnect(&SEM_PY_ENV, &CDSEM_GlobalSemPyParamModel::freezenChanged, d, nullptr);
        QObject::disconnect(&SEM_PY_ENV, &CDSEM_GlobalSemPyParamModel::minWorkDistanceChanged, d,  nullptr);
        QObject::disconnect(&SEM_PY_ENV, &CDSEM_GlobalSemPyParamModel::maxWorkDistanceChanged, d,  nullptr);
        QObject::disconnect(&SEM_PY_ENV, &CDSEM_GlobalSemPyParamModel::workDistanceChanged, d,  nullptr);
        QObject::disconnect(&SEM_PY_ENV, &CDSEM_GlobalSemPyParamModel::beamRotChanged, d,  nullptr);
        QObject::disconnect(&SEM_PY_ENV, &CDSEM_GlobalSemPyParamModel::magChanged, d, nullptr);
        QObject::disconnect(&OM_PY_ENV, &CDSEM_GlobalOmPyParamModel::magChanged, d, nullptr);
        QObject::disconnect(&OM_PY_ENV, &CDSEM_GlobalOmPyParamModel::rotationChanged, d, nullptr);

        // QObject::disconnect(&SEM_PY_ENV, &CDSEM_GlobalSemPyParamModel::ehtChanged, d,  nullptr);
        // QObject::disconnect(&SEM_PY_ENV, &CDSEM_GlobalSemPyParamModel::iProbeChanged, d, nullptr);
        // QObject::disconnect(&SEM_PY_ENV, &CDSEM_GlobalSemPyParamModel::pixelSizeChanged, d,  nullptr);
        // QObject::disconnect(&SEM_PY_ENV, &CDSEM_GlobalSemPyParamModel::aptNumChanged, d,  nullptr);
        // QObject::disconnect(&SEM_PY_ENV, &CDSEM_GlobalSemPyParamModel::scanRateChanged, d, nullptr);
        // QObject::disconnect(&SEM_PY_ENV, &CDSEM_GlobalSemPyParamModel::frameChanged, d, nullptr);
        // QObject::disconnect(&SEM_PY_ENV, &CDSEM_GlobalSemPyParamModel::optIProbeChanged, d, nullptr);
    }
};

ImageOperationDataModel::ImageOperationDataModel(QObject* parent) : QObject(parent) {
    dataHelper = new ImageOperationDataModelDataHelper;
    dataHelper->d = this;
    dataHelper->init();
    dataHelper->copy();
}
ImageOperationDataModel::~ImageOperationDataModel() noexcept {
    delete dataHelper;
}

void ImageOperationDataModel::setImageOperationType(int type) {
    CasLocker locker(dataHelper->lock);
    if (type == dataHelper->ot) return;
    dataHelper->ot = type;
    if (type == IMG_OPT_TYPE_IM) {
        if (!dataHelper->disabledFromGlobal) {
            dataHelper->disabledFromGlobal = true;
            dataHelper->disable();
        }
    } else {
        if (dataHelper->disabledFromGlobal) {
            dataHelper->disabledFromGlobal = false;
            dataHelper->init();
            dataHelper->copy();
        }
    }
    if (type == IMG_OPT_TYPE_OM) {
        setPhotoImageType(cdsem::PIT_OM);
    } else if (type == IMG_OPT_TYPE_SEM) {
        setPhotoImageType(cdsem::PIT_SEM);
    }
    emit imageOperationTypeChanged();
}
int ImageOperationDataModel::getImageOperationType() const {
    CasLocker locker(dataHelper->lock);
    return dataHelper->ot;
}

void ImageOperationDataModel::setImageFreezeStatus(ImageFreezenStatus status) {
    if (status == dataHelper->semFreezeStatus) return;
    dataHelper->semFreezeStatus = status;
    emit freezeStatusChanged();
}

ImageFreezenStatus ImageOperationDataModel::getImageFreezeStatus() const {
    return dataHelper->semFreezeStatus;
}

bool ImageOperationDataModel::imageIsFreezen() const {
    return dataHelper->semFreezeStatus == IFS_PHOTO || dataHelper->semFreezeStatus == IFS_TAKING;
}

void ImageOperationDataModel::setMinWorkDistance(float v) {
    if (v == dataHelper->minWorkDistance) return;
    dataHelper->minWorkDistance = v;
    emit minWorkDistanceChanged();
}

float ImageOperationDataModel::getMinWorkDistance() const {
    return dataHelper->minWorkDistance;
}

void ImageOperationDataModel::setMaxWorkDistance(float v) {
    if (v == dataHelper->maxWorkDistance) return;
    dataHelper->maxWorkDistance = v;
    emit maxWorkDistanceChanged();
}

float ImageOperationDataModel::getMaxWorkDistance() const {
    return dataHelper->maxWorkDistance;
}

void ImageOperationDataModel::setWorkDistance(float v) {
    if (v == dataHelper->workDistance) return;
    dataHelper->workDistance = v;
    emit workDistanceChanged();
}

float ImageOperationDataModel::getWorkDistance() const {
    return dataHelper->workDistance;
}

void ImageOperationDataModel::setBeamRot(float v) {
    if (v == dataHelper->beamRot) return;
    dataHelper->beamRot = v;
    emit beamRotChanged();
}

float ImageOperationDataModel::getBeamRot() const {
    return dataHelper->beamRot;
}

void ImageOperationDataModel::setOmRot(float v) {
    if (v == dataHelper->omRot) return;
    dataHelper->omRot = v;
    emit omRotChanged();
}
float ImageOperationDataModel::getOmRot() const {
    return dataHelper->omRot;
}

void ImageOperationDataModel::setEHT(float v) {
    if (dataHelper->eht == v) return;
    dataHelper->eht = v;
    emit ehtChanged();
}

float ImageOperationDataModel::getEHT() const {
    return dataHelper->eht;
}

void ImageOperationDataModel::setIProbe(float v) {
    if (dataHelper->iProbe == v) return;
    dataHelper->iProbe = v;
    emit iProbeChanged();
}

float ImageOperationDataModel::getIProbe() const {
    return dataHelper->iProbe;
}

void ImageOperationDataModel::setAptNum(int n) {
    if (dataHelper->aptNum == n) return;
    dataHelper->aptNum = n;
    emit aptNumChanged();
}

int ImageOperationDataModel::getAptNum() const {
    return dataHelper->aptNum;
}

void ImageOperationDataModel::setSemMag(int m) {
    if (dataHelper->semMag == m) return;
    dataHelper->semMag = m;
    emit semMagChanged();
}

int ImageOperationDataModel::getSemMag() const {
    return dataHelper->semMag;
}

void ImageOperationDataModel::setScanRate(int r) {
    if (dataHelper->scanRate == r) return;
    dataHelper->scanRate = r;
    emit scanRateChanged();
}

int ImageOperationDataModel::getScanRate() const {
    return dataHelper->scanRate;
}

void ImageOperationDataModel::setFrame(int f) {
    if (dataHelper->frame == f) return;
    dataHelper->frame = f;
    emit frameChanged();
}

int ImageOperationDataModel::getFrame() const {
    return dataHelper->frame;
}

void ImageOperationDataModel::setIsProbe(bool b) {
    if (dataHelper->isIProbe == b) return;
    dataHelper->isIProbe = b;
    emit isIProbeChanged();
}

bool ImageOperationDataModel::isProbe() const {
    return dataHelper->isIProbe;
}

void ImageOperationDataModel::setPixelSize(float v) {
    if (dataHelper->pixelSize == v) return;
    dataHelper->pixelSize = v;
    emit pixelSizeChanged();
}

float ImageOperationDataModel::getPixelSize() const {
    return dataHelper->pixelSize;
}

void ImageOperationDataModel::setPhotoImageType(cdsem::PhotoImageType type) {
    if (dataHelper->photoImageType == type) return;
    dataHelper->photoImageType = type;
    emit photoImageTypeChanged();
}
cdsem::PhotoImageType ImageOperationDataModel::getPhotoImageType() const {
    return dataHelper->photoImageType;
}

ImageOperationDataModelHelper::ImageOperationDataModelHelper() {
    dataModel = new ImageOperationDataModel;
    if (getMainThread() != QThread::currentThread())
        dataModel->moveToThread(getMainThread());

    PluginSignalHub::instance()->registSlot(SWITCH_IMAGE_OPT_TYPE, PluginHubAnyFunc<void, int>([](int type) {
        IOPT->setImageOperationType(type);
    }));
}

ImageOperationDataModelHelper::~ImageOperationDataModelHelper() noexcept {
    delete dataModel;
}

ImageOperationDataModel* ImageOperationDataModelHelper::get() {
    return instance()->dataModel;
}
