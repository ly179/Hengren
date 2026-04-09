#ifndef IDP_DATA_EDITOR_CONTROLLER_H
#define IDP_DATA_EDITOR_CONTROLLER_H
#include <QList>
#include "common_act_param_helper.h"
#include "idp_data_model.h"
#include "ui_idp_creation_global.h"
#include "sem_common_data_model.h"
#include "relation_idp_data_edit_controller.h"

struct IdpApSaveTmpData;
struct IdpMpSaveTmpData;
class ImageInfoEntity;
struct IdpAxisSaveTmpData;
struct TotalSaveImgTmpData;
struct IDP_SaveDataParam;

namespace cdsem {
class UiIdpDataEditorController;
};

struct IdpDataEditorDataHelper;
/**
 * 本类是IDP的编辑首页面
 *
 * AP的Chip Reg按钮注册，识别坐标时不包含beamShift
 * AP的Dx,Dy Reg按钮注册，会叠加beamShift坐标
 * AP的Move按钮，目前不叠加beamShift坐标，且CDSEM_ControlHelper的stage move命令不会叠加beamShift，且会自动强制归零beamShift。
 * 因此Move按钮是基于这个自动的，不做干预的命令执行。
 * AP的image Reg会叠加beamShift
 *
 * MP的Address、MP与AP的情况相同
 * @brief The idp_data_editor class
 */
class UI_IDP_CREATION_EXPORT IdpDataEditorController : public RelationIdpEditController{
    friend struct IdpDataEditorDataHelper;
    SEM_SET_INIT_PARAM(IdpDetailData)
private:
    struct IdpDataEditorDataHelper* dataHelper = nullptr;

    bool eventFilter(QObject* watch, QEvent* event) override;

    void create();
    void createAction();
    void createApTableAction();
    void createMpTableAction();

    void showIdpApChipSelector(const QList<qint32>& = QList<qint32>());
    bool createIdpApDetailEditor();
    void showIdpMpChipSelector(const QList<qint32>& = QList<qint32>(), const QList<qint32>& = QList<qint32>());
    bool createIdpMpDetailEditor();
    void createMeasurementDuplicateEditor();
    void createDNoByPEditor();
    void createDNoByMeasurementEditor();
    void createPNoEditor();
    void createTimesEditor();
    void createImageEnvEditor();
    void createAxisAlignmentEditor();

    void createZSensorController();

    void resetToComponent();
    QString collectAPData();
    QString collectMPData(IdpDetailData&, bool focusOnSelected = false);
    QString collectData();

    void saveData(const IDP_SaveDataParam& baseData, const sem::NO_PARAM_ARG_FUNC&, bool saveAs = false);

    void prepareImage(ImageInfoEntity& imgEntity, QImage& image);

    void collectApImages(QList<IdpApSaveTmpData>& newImgIds, QList<quint64>& allImageIds);
    void collectMpImages(QList<IdpMpSaveTmpData>& newImgIds, QList<quint64>& allImageIds);
    void collectAxisImages(IdpAxisSaveTmpData& newImgIds, QList<quint64>& allImageIds);
    void collectAllImages(TotalSaveImgTmpData& newImgIds, QList<quint64>& allImageIds);
    bool localSaveAllImages(TotalSaveImgTmpData& newImgIds, QList<quint64>& allImageIds);
    OperationResult updateAllImageIds(const TotalSaveImgTmpData& newImgIds);
    void localRollbackAllImages(quint64 idpId, const TotalSaveImgTmpData& newImgIds);
    void rollbackAllImagesIdOnly(const TotalSaveImgTmpData& newImgIds);

    void collectDynamicIds(const IdpDetailData&, QList<quint64>& ids);
    void localQueryTemplate(const QList<quint64>& ids, const IdpDetailData&);

    void localPrepareTitleTemplate(IdpDetailData&);

public:
    explicit IdpDataEditorController(QWidget* parent = nullptr);
    ~IdpDataEditorController() noexcept;

    void loadExistsData(const IDP_BaseData&) override;
    void newData(quint64 classId, quint64 idwId, const QString& idpName) override;

    void disableCoverSave(bool = true) override;

    quint64 getIdpId() const override;
    quint64 getIdwId() const override;
    quint64 getIdwTopId() const override;
};

#endif // IDP_DATA_EDITOR_CONTROLLER_H
