#ifndef IMAGE_OPERATION_DATA_MODEL_H
#define IMAGE_OPERATION_DATA_MODEL_H

#include <QObject>
#include "cdsem_global_env_data_model.h"
#include "sem_singleton_template.h"
#include "module_common_global.h"
#include "cdsem_data_common.h"

#define IOPT ImageOperationDataModelHelper::get()

//! 这是一个用于驱动ImageOperationWindow窗口显示数据的模型类
//! 可以灵活设置驱动方式，确保IM/OM/SEM/Freeze场景可灵活切换。
//! 该类必须在UI线程上使用
struct ImageOperationDataModelDataHelper;
class MODEL_COMMON_EXPORT ImageOperationDataModel : public QObject{
    Q_OBJECT
    friend struct ImageOperationDataModelDataHelper;
private:
    ImageOperationDataModelDataHelper* dataHelper;
public:
    explicit ImageOperationDataModel(QObject* parent = nullptr);
    ~ImageOperationDataModel() noexcept;

    void setImageOperationType(int);
    int getImageOperationType() const;
    void setImageFreezeStatus(ImageFreezenStatus);
    ImageFreezenStatus getImageFreezeStatus() const;
    bool imageIsFreezen() const;
    void setMinWorkDistance(float);
    float getMinWorkDistance() const;
    void setMaxWorkDistance(float);
    float getMaxWorkDistance() const;
    void setWorkDistance(float);
    float getWorkDistance() const;
    void setBeamRot(float);
    float getBeamRot() const;
    void setOmRot(float);
    float getOmRot() const;
    void setEHT(float);
    float getEHT() const;
    void setIProbe(float);
    float getIProbe() const;
    void setAptNum(int);
    int getAptNum() const;
    void setSemMag(int);
    int getSemMag() const;
    void setScanRate(int);
    int getScanRate() const;
    void setFrame(int);
    int getFrame() const;
    void setIsProbe(bool);
    bool isProbe() const;
    void setPixelSize(float);
    float getPixelSize() const;
    void setPhotoImageType(cdsem::PhotoImageType);
    cdsem::PhotoImageType getPhotoImageType() const;


signals:

    void imageOperationTypeChanged();
    void freezeStatusChanged();
    void minWorkDistanceChanged();
    void maxWorkDistanceChanged();
    void workDistanceChanged();
    void beamRotChanged();
    void omRotChanged();
    void ehtChanged();
    void iProbeChanged();
    void pixelSizeChanged();
    void aptNumChanged();
    void semMagChanged();
    void omMagChanged();
    void scanRateChanged();
    void frameChanged();
    void isIProbeChanged();
    void photoImageTypeChanged();
};

class MODEL_COMMON_EXPORT ImageOperationDataModelHelper : public Singleton<ImageOperationDataModelHelper> {
    friend class Singleton<ImageOperationDataModelHelper>;
private:
    ImageOperationDataModel* dataModel;
    ImageOperationDataModelHelper();

public:
    ~ImageOperationDataModelHelper() noexcept;
    static ImageOperationDataModel* get();
};

#endif // IMAGE_OPERATION_DATA_MODEL_H
